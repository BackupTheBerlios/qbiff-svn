/**************
FILE          : clientfolder.cpp
***************
PROJECT       : QBiff - A qt based biff
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : new mail notification service 
              : 
              :
DESCRIPTION   : implementation class for a client folder
              : connected to a TCP port
              :
              :
STATUS        : Status: Beta
**************/
#include "clientfolder.h"

//=========================================
// Constructor
//-----------------------------------------
ClientFolder::ClientFolder (WFlags wflags) : QWidget (0,0,wflags)  {
	mButtonBar  = new QHBoxLayout ( this );
	mTimer      = new QTimer ( this );
	mClient     = new SSLClient;
	connect (
        mClient , SIGNAL (gotLine ( QString )),
        this    , SLOT   (gotLine ( QString ))
    );
	connect (
		mTimer , SIGNAL (timeout   (void)),
		this   , SLOT   (timerDone (void))
	);
	mClient -> writeClient ("INIT");
	mTimer  -> start ( 10, FALSE );
}

//=========================================
// gotLine
//-----------------------------------------
void ClientFolder::gotLine ( QString line ) {
	QStringList tokens = QStringList::split (":",
		QString::fromLocal8Bit(line)
	);
	QString folder = tokens[0];
	QString status = tokens[1];
	QString newmail= tokens[2];
	QString curmail= tokens[3];
	if (folder.isEmpty()) {
		return;
	}
	if (! mButton[folder]) {
		Button* btn = new Button (folder,this);
		ClientInfo* info = new ClientInfo (folder,btn);
		btn->setPaletteBackgroundColor ( QColor(0,0,128) );
		btn->setPaletteForegroundColor ( QColor(255,255,255));
		btn->setTip (newmail,curmail);
		QObject::connect (
			btn , SIGNAL ( clickedButton (QPushButton*) ),
			this, SLOT   ( folderEvent   (QPushButton*) )
		);
		mButtonBar -> addWidget ( btn );
		btn->setHidden (false);
		mButton.insert (folder,btn);
		mInfo.insert (folder,info);
		mFolderNames.append (folder);

		if ( status == "new" ) {
			btn->setPaletteBackgroundColor( QColor(0,128,0) );
		}
		if ( status == "empty" ) {
			btn->setHidden (true);
		}
	} else {
		if ( status == "new" ) {
			mButton[folder]->setHidden (false);
			mButton[folder]->setPaletteBackgroundColor( QColor(0,128,0) );
		}
		if ( status == "empty" ) {
			mButton[folder]->setHidden (true);
		}
		if ( status == "uptodate" ) {
			mButton[folder]->setHidden (false);
			mButton[folder]->setPaletteBackgroundColor( QColor(0,0,128) );
		}
		mButton[folder]->setTip (newmail,curmail);
		mInfo[folder]->setTip (newmail,curmail);
	}
	resize (sizeHint());
}

//=========================================
// folderEvent
//-----------------------------------------
void ClientFolder::folderEvent (QPushButton* btn) {
	btn->setDisabled (true);
	QString text = btn->text();
	if (mRemoteMail) {
		QString* servercmd = new QString;
		QTextOStream(servercmd) << "WRITE " << text;
		mClient -> writeClient (*servercmd);
	}
	QProcess* proc = new QProcess();
	proc -> addArgument (MY_MAILCLIENT);
	proc -> addArgument (MY_FOLDER + text);
	proc -> start();
	while (proc->isRunning()) {
		qApp->processEvents();
		usleep (1000);
	}
	if (btn) {
		btn->setDisabled (false);
	}
}

//=========================================
// setRemoteMail
//-----------------------------------------
void ClientFolder::setRemoteMail ( bool arg ) {
	mRemoteMail = arg;
}

//=========================================
// timerDone
//-----------------------------------------
void ClientFolder::timerDone (void) {
	mClient -> clientReadWrite ();
	resize (sizeHint());
}

//=========================================
// End Session
//-----------------------------------------
void ClientFolder::cleanup (void) {
	mTimer  -> stop();
	mClient -> writeClient ("QUIT");
}
