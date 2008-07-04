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
// Globals
//-----------------------------------------
extern QString mailClient;
extern QString mailPrivate;
extern QString PIXINFO; 
extern QString PIXPUBL;
extern QString PIXPRIV;
extern QString myFolder;

//=========================================
// Constructor
//-----------------------------------------
ClientFolder::ClientFolder (Qt::WindowFlags wflags) : QWidget (0,wflags)  {
	mButtonBar  = new QHBoxLayout ( this );
	mPrivatePixmap = QPixmap ( PIXPRIV );
	mPublicsPixmap = QPixmap ( PIXPUBL );
	mPrivate = new QPushButton ( this );
	mPrivate -> setCheckable ( true );
	mPrivate -> setIcon(QIcon(mPublicsPixmap));
	mPrivate -> setIconSize(mPublicsPixmap.size());
	mPDefault = mPrivate -> palette();
	mPBlue = mPDefault;
	mPBlue.setColor( QPalette::Button, QColor(0,0,128));
	mPBlue.setColor( QPalette::ButtonText, QColor(255,255,255));
	mPGreen = mPDefault;
	mPGreen.setColor( QPalette::Button, QColor(0,128,0));
	mPGreen.setColor( QPalette::ButtonText, QColor(255,255,255));
	mPrivate -> setFocusPolicy (Qt::NoFocus);
	mButtonBar -> addWidget ( mPrivate );
	mButtonBar -> setSpacing (2);
	mButtonBar -> setMargin (0);
	mIsPrivate = false;
	connect (
		mPrivate , SIGNAL (toggled    ( bool )),
		this     , SLOT   (gotToggled ( bool ))
	);
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
	mTimer  -> start ( 10 );
}

//=========================================
// gotLine
//-----------------------------------------
void ClientFolder::gotLine ( QString line ) {
	QStringList tokens = QString::fromLocal8Bit(
		line.toLatin1().data()
	).split(":");
	QString folder = tokens[0];
	QString status = tokens[1];
	QString newmail= tokens[2];
	QString curmail= tokens[3];
	if (folder.isEmpty()) {
		return;
	}
	if (! mButton.contains(folder)) {
		Button* btn = new Button (folder,this);
		ClientInfo* info = new ClientInfo (folder,btn,newmail.toInt());
		btn->setPalette (mPBlue);
		btn->setFocusPolicy (Qt::NoFocus);
		btn->setTip (newmail,curmail);
		btn->setFixedHeight (mPrivate->height());
		QObject::connect (
			btn , SIGNAL ( clickedButton (QPushButton*) ),
			this, SLOT   ( folderEvent   (QPushButton*) )
		);
		mButtonBar -> addWidget ( btn );
		btn->setHidden (false);
		mButton.insert (folder,btn);
		mInfo.insert (folder,info);
		mFolderNames.append (folder.toLatin1().data());

		if ( status == "new" ) {
			btn->setPalette (mPGreen);
		}
		if ( status == "empty" ) {
			btn->setHidden (true);
		}
	} else {
		if ( status == "new" ) {
			mButton[folder]->setHidden (false);
			mButton[folder]->setPalette (mPDefault);
			mButton[folder]->setPalette (mPGreen);
		}
		if ( status == "empty" ) {
			mButton[folder]->setHidden (true);
		}
		if ( status == "uptodate" ) {
			mButton[folder]->setHidden (false);
			mButton[folder]->setPalette (mPDefault);
			mButton[folder]->setPalette (mPBlue);
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
		QString servercmd;
		QTextStream(&servercmd) << "WRITE " << text;
		mClient -> writeClient (servercmd);
	}
	QProcess* proc = new QProcess();
	QString program;
	QStringList arguments;
	if (mIsPrivate) {
		program = mailPrivate;
	} else {
		program = mailClient;
	}
	arguments <<  QString(myFolder + text);
	proc->execute(program,arguments);
	mProcessList.append (proc);
	mButtonsList.append ((Button*)btn);
}

//=========================================
// setRemoteMail
//-----------------------------------------
void ClientFolder::setRemoteMail ( bool arg ) {
	mRemoteMail = arg;
}

//=========================================
// setToggle
//-----------------------------------------
void ClientFolder::setToggle ( bool toggle ) {
	if (toggle) {
		mPrivate -> show();
	} else {
		mPrivate -> hide();
	}
}

//=========================================
// timerDone
//-----------------------------------------
void ClientFolder::timerDone (void) {
	mClient -> clientReadWrite ();
	resize (sizeHint());
	int pCount = 0;
	int pRemove[mProcessList.count()];
	QListIterator<QProcess*> it (mProcessList);
	while (it.hasNext()) {
		pRemove[pCount] = 0;
		QProcess *proc = it.next();
		if (proc->state() != QProcess::Running) {
			mButtonsList.at(pCount)->setDisabled (false);
			pRemove[pCount] = 1;
		}
		pCount++;
	}
	for (int n=0;n<mProcessList.count();n++) {
		if (pRemove[n] == 1) {
			mButtonsList.removeAt (n);
			mProcessList.removeAt (n);
		}
	}
}

//=========================================
// End Session
//-----------------------------------------
void ClientFolder::cleanup (void) {
	mTimer  -> stop();
	mClient -> writeClient ("QUIT");
}

//=========================================
// gotToggled
//-----------------------------------------
void ClientFolder::gotToggled (bool on) {
	if (! on) {
		mPrivate -> setIcon (QIcon(mPublicsPixmap));
		mPrivate -> setIconSize (mPublicsPixmap.size());
		mIsPrivate = false;
	} else {
		mPrivate -> setIcon(QIcon(mPrivatePixmap));
		mPrivate -> setIconSize(mPrivatePixmap.size());
		mIsPrivate = true;
	}
}
