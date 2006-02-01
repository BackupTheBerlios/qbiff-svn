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
#include <qhbox.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qdict.h>
#include <qthread.h>
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
	ClientFolder ( WFlags = 0 );
	void setRemoteMail  (bool);
	void setToggle      (bool);
	void cleanup (void);

	private slots:
	void gotLine        (QString);
	void folderEvent    (QPushButton*);
	void timerDone      (void);
	void gotToggled     (bool);

	private:
	QTimer*             mTimer;
	SSLClient*          mClient;
	QPushButton*        mPrivate;
	QPixmap             mPublicsPixmap;
	QPixmap             mPrivatePixmap;
	QBoxLayout*         mButtonBar;
	QDict<Button>       mButton;
	QDict<ClientInfo>   mInfo;
	QList<char>         mFolderNames;
	bool                mRemoteMail;
	bool                mIsPrivate;
};

#endif
