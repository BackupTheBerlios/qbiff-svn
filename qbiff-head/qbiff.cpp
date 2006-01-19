/**************
FILE          : qbiff.cpp
***************
PROJECT       : QBiff - A qt based biff
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : new mail notification service 
              : 
              :
DESCRIPTION   : qbiff will create a buttonbar for different
              : folders and enables the button to call a 
              : mailreader for the selected folder
              :
              :
STATUS        : Status: Beta
**************/
#include <stdlib.h>
#include <signal.h>
#include <qapplication.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qfile.h>
#include <getopt.h>

#include "serverhandler.h"
#include "clientfolder.h"

//=========================================
// Globals
//-----------------------------------------
bool useGUI;
ClientFolder*  pFolder = NULL;
ServerHandler* pServer = NULL;
int  serverPort        = PORT;
QString serverName     = SERVER;

//=========================================
// Global functions
//-----------------------------------------
void quit  (int,siginfo_t*,void*);
void usage (void);

//=========================================
// The magic :-)
//-----------------------------------------
int main(int argc,char*argv[]) {
	//=========================================
	// set locale
	//-----------------------------------------
	setlocale (LC_ALL,"");

	//=========================================
	// setup signal handler
	//-----------------------------------------
	struct sigaction action;
	action.sa_sigaction = quit;
	sigaction (SIGHUP , &action , 0);
	sigaction (SIGINT , &action , 0);
	sigaction (SIGTERM, &action , 0);

	//=========================================
	// create Qt application
	//-----------------------------------------
	useGUI = getenv ( "DISPLAY" ) != 0;
	QApplication app ( argc,argv,useGUI );

	//=========================================
	// init variables...
	//-----------------------------------------
	bool remoteMail     = false;

	//=========================================
	// get options
	//-----------------------------------------
	while (1) {
	int option_index = 0;
	static struct option long_options[] =
	{
		{"remote"  , 0 , 0 , 'r'},
		{"server"  , 1 , 0 , 's'},
		{"port"    , 1 , 0 , 'p'},
		{"help"    , 0 , 0 , 'h'},
		{0         , 0 , 0 , 0  }
	};
	int c = getopt_long (
		argc, argv, "rhs:p:",long_options, &option_index
	);
	if (c == -1)
	break;

	switch (c) {
	case 0:
		fprintf (stderr,"qbiff: option %s", long_options[option_index].name);
		if (optarg) {
			fprintf (stderr," with arg %s", optarg);
		}
		fprintf (stderr,"\n");
	break;

	case 'r':
		remoteMail = true;
	break;

	case 's':
		serverName = *(new QString (optarg));
	break;

	case 'p':
		serverPort = (new QString (optarg))->toInt();
	break;

	case 'h':
		usage();
	break;
	default:
		exit (1);
	}
	}

	//=========================================
	// create entity Server or Client
	//-----------------------------------------
	if ( ! useGUI ) {
		QString pidfile;
		pidfile.sprintf ("/var/tmp/qbiff.%d.pid",serverPort);
		QFile run (pidfile);
		if (run.exists()) {
		if (run.open( IO_ReadOnly )) {
			QTextStream stream ( &run );
			QString pid = stream.readLine();
			run.close();
			if (kill (pid.toInt(),0) == 0) {
				printf ("qbiff::already running: %s\n",pid.ascii());
				exit (0);
			} else {
				unlink (pidfile);
			}
		}
		}
		if ( ! run.open( IO_WriteOnly ) ) {
			printf ("qbiff::couldn't open pid file\n");
			exit (1);
		}
		int pid = getpid();
		QTextStream stream( &run );
		stream << pid << "\n";
		run.close();
		pServer = new ServerHandler;
	} else {
		QWidget::WFlags wflags = Qt::WType_TopLevel;
		wflags |= Qt::WStyle_Customize | Qt::WStyle_NoBorder;
		pFolder = new ClientFolder ( wflags );
		pFolder -> setFixedHeight(0);
		pFolder -> setRemoteMail (remoteMail);
		app.setMainWidget ( pFolder );
		pFolder-> show ();
	}
	return app.exec();
}

//=========================================
// usage
//-----------------------------------------
void usage (void) {
	printf ("Linux QBiff Version 1.2 Server and Client\n");
	printf ("(C) Copyright 2004 SuSE GmbH\n");
	printf ("\n");
	printf ("usage: qbiff [ options ]\n");
	printf ("options:\n");
	printf ("[ -r | --remote ]\n");
	printf ("   in server mode: Enables the flag files to be written.\n");
	printf ("   flag files can be used to start a terminal based program\n");
	printf ("   on the remote side whereas the controling terminal\n");
	printf ("   remains local\n");
	printf ("[ -s | --server ]\n");
	printf ("   in client mode: specify server to connect.\n");
	printf ("[ -p | --port ]\n");
	printf ("   in client mode: specify server port to connect.\n");
	printf ("--\n");
	exit (1);
}

//=========================================
// clean sweep and exit
//-----------------------------------------
void quit (int code,siginfo_t*,void*) {
	if (pFolder) {
		printf ("End Client Session\n");
		pFolder->cleanup();
	}
	if (pServer) {
		printf ("End Server Session\n");
		pServer->cleanup();
	}
	if ( ! useGUI ) {
		QString runfile;
		runfile.sprintf ("/var/tmp/qbiff.%d.pid",serverPort);
		QFile run (runfile);
		run.remove();
	}
	exit (code);
}
