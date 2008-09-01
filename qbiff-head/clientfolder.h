/**************
FILE          : clientfolder.h
***************
PROJECT       : QBiff - A qt based biff
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : new mail notification service 
              : 
              :
DESCRIPTION   : header file for client folder widget
              :
              :
STATUS        : Status: Beta
**************/
#ifndef CLIENTFOLDER_H
#define CLIENTFOLDER_H 1

#include <qapplication.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qprocess.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qhash.h>
#include <qthread.h>
#include <qtextstream.h>
#include <signal.h>

#include "button.h"
#include "sslclient.h"
#include "config.h"
#include "clientinfo.h"

//=========================================
// Class ClientFolder
//-----------------------------------------
class ClientFolder : public QWidget {
	Q_OBJECT

	public:
	ClientFolder ( Qt::WindowFlags = 0 );
	void setRemoteMail  (bool);
	void setToggle      (bool);
	void cleanup (void);

	private slots:
	void gotLine        (QString);
	void folderEvent    (QPushButton*);
	void showTip        (QPushButton*);
	void timerDone      (void);
	void timerProcDone  (void);
	void gotToggled     (bool);

	private:
	QPalette                   mPDefault;
	QPalette                   mPRed;
	QPalette                   mPBlue;
	QPalette                   mPGreen;
	QTimer*                    mTimer;
	QTimer*                    mTimerProc;
	SSLClient*                 mClient;
	QPushButton*               mPrivate;
	QPixmap                    mPublicsPixmap;
	QPixmap                    mPrivatePixmap;
	QBoxLayout*                mButtonBar;
	QHash<QString,Button*>     mButton;
	QHash<QString,ClientInfo*> mInfo;
	QList<char*>               mFolderNames;
	QList<QProcess*>           mProcessList;
	QList<Button*>             mButtonsList;
	bool                       mRemoteMail;
	bool                       mIsPrivate;
};

#endif
