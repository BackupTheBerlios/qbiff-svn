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
#include <qiodevice.h>

#include "config.h"
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
QString mailClient     = MY_MAILCLIENT;
QString mailPrivate    = MY_MAILCLPRIV;
QString baseDir        = BASEDIR;
QString myFolder       = MY_FOLDER;
QString myButtonFont   = "FrutigerNextLT:style=Bold";
int myButtonFontSize   = 10;

//=========================================
// Globals
//-----------------------------------------
QString CAFILE;
QString DH1024;
QString DH512;
QString SERVER_CERTFILE;
QString CLIENT_CERTFILE;
QString PIXSHAPE;
QString PIXSHAPEBG;
QString PIXNEWMAIL;
QString PIXNOMAIL;
QString PIXPUBL;
QString PIXPRIV;

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
	bool haveToggle     = false;

	//=========================================
	// get options
	//-----------------------------------------
	while (1) {
	int option_index = 0;
	static struct option long_options[] =
	{
		{"remote"        , 0 , 0 , 'r'},
		{"server"        , 1 , 0 , 's'},
		{"port"          , 1 , 0 , 'p'},
		{"mailfolder"    , 1 , 0 , 'f'},
		{"buttonfont"    , 1 , 0 , 'F'},
		{"buttonfontsize", 1 , 0 , 'Z'},
		{"readmail"      , 1 , 0 , 'm'},
		{"readpriv"      , 1 , 0 , 'i'},
		{"basedir"       , 1 , 0 , 'b'},
		{"toggle"        , 0 , 0 , 't'},
		{"help"          , 0 , 0 , 'h'},
		{0               , 0 , 0 , 0  }
	};
	int c = getopt_long (
		argc, argv, "rhs:p:tf:m:i:b:",long_options, &option_index
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

	case 't':
		haveToggle = true;
	break;

	case 'r':
		remoteMail = true;
	break;

	case 'f':
		myFolder = *(new QString (optarg));
		myFolder += "/";
	break;

	case 'Z':
		myButtonFontSize = (new QString (optarg))->toInt();
	break;

	case 'F':
		myButtonFont = *(new QString (optarg));
	break;

	case 's':
		serverName = *(new QString (optarg));
	break;

	case 'p':
		serverPort = (new QString (optarg))->toInt();
	break;

	case 'm':
		mailClient = *(new QString (optarg));
	break;

	case 'i':
		mailPrivate= *(new QString (optarg));
	break;

	case 'b':
		baseDir = *(new QString (optarg));
	break;

	case 'h':
		usage();
	break;
	default:
		exit (1);
	}
	}

	//=======================================
	// certification stuff
	//---------------------------------------
	CAFILE         = baseDir + "/cert-server/rootcert.pem";
	DH1024         = baseDir + "/cert-server/dh1024.pem";
	DH512          = baseDir + "/cert-server/dh512.pem";
	SERVER_CERTFILE= baseDir + "/cert-server/server.pem";
	CLIENT_CERTFILE= baseDir + "/cert-client/client.pem";

	// .../
	// shape pixmap used to create the bitmask for the non
	// rectangular info window
	// ----
	PIXSHAPE       = baseDir + "/pixmaps/shape.xpm";
	// .../
	// background image set as QFrame stylesheet. Alternative
	// shape.png.transparent can be used, which allows to set
	// the background color within the C++ code
	// ----
	PIXSHAPEBG     = baseDir + "/pixmaps/shape.xpm"; 

	PIXNEWMAIL     = baseDir + "/pixmaps/newmail.png";
	PIXNOMAIL      = baseDir + "/pixmaps/nomail.png";
	PIXPUBL        = baseDir + "/pixmaps/public.png";
	PIXPRIV        = baseDir + "/pixmaps/private.png";

	//=========================================
	// create entity Server or Client
	//-----------------------------------------
	if ( ! useGUI ) {
		QString pidfile;
		pidfile.sprintf ("/var/tmp/qbiff.%d.pid",serverPort);
		QFile run (pidfile);
		if (run.exists()) {
		if (run.open( QIODevice::ReadOnly )) {
			QTextStream stream ( &run );
			QString pid = stream.readLine();
			run.close();
			if (kill (pid.toInt(),0) == 0) {
				printf ("qbiff::already running: %s\n",pid.toLatin1().data());
				exit (0);
			} else {
				unlink (pidfile.toLatin1().data());
			}
		}
		}
		if ( ! run.open( QIODevice::WriteOnly ) ) {
			printf ("qbiff::couldn't open pid file\n");
			exit (1);
		}
		int pid = getpid();
		QTextStream stream( &run );
		stream << pid << "\n";
		run.close();
		pServer = new ServerHandler;
	} else {
		Qt::WFlags wflags = Qt::Window;
		wflags |= 
			Qt::FramelessWindowHint  |
			Qt::WindowStaysOnTopHint |
			Qt::X11BypassWindowManagerHint;
		pFolder = new ClientFolder ( wflags );
		pFolder -> setRemoteMail (remoteMail);
		pFolder -> setToggle (haveToggle);
		pFolder -> hide ();
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
	printf ("[ -s | --server <ip> ]\n");
	printf ("   in client mode: specify server to connect.\n");
	printf ("[ -p | --port <number> ]\n");
	printf ("   in client mode: specify server port to connect.\n");
	printf ("[ -t | --toggle ]\n");
	printf ("   show toggle button to be able to switch between\n");
	printf ("   readmail and readmail.private to be called.\n");
	printf ("[ -m | --readmail <program> ]\n");
	printf ("   call this program as standard mail reader\n");
	printf ("[ -i | --privmail <program> ]\n");
	printf ("   call this program as private mail reader (toggle)\n");
	printf ("[ -b | --basedir <directory> ]\n");
	printf ("   base directory for metadata, default: /usr/share/qbiff\n");
	printf ("   certification and pixmap information is stored here\n");
	printf ("[ -f | --mailfolder <directory> ]\n");
	printf ("   path to maildir organised mail files\n");
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
