/**************
FILE          : notify.h
***************
PROJECT       : QBiff - A qt based biff
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : new mail notification service 
              : 
              :
DESCRIPTION   : header file for F_NOTIFY support
              :
              :
STATUS        : Status: Beta
**************/
#ifndef NOTIFY_H
#define NOTIFY_H 1

#include <qwidget.h>
#include <qintdict.h>
#include <stdio.h>
#include <stdlib.h>
#include <qlist.h>
#include <qdict.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/types.h>
#include <glob.h>

#include "parser.h"

//=========================================
// Defines...
//-----------------------------------------
#define QBIFF_CREATE  10
#define QBIFF_DELETE  11

//=========================================
// Functions...
//-----------------------------------------
void handleNotifyCreateEvent (int,siginfo_t*,void*);
void handleNotifyDeleteEvent (int,siginfo_t*,void*);
void handlePendingEvent (int,siginfo_t*,void*);

//=========================================
// Class NotifyCount...
//-----------------------------------------
class NotifyCount {
	private:
	QString* mFolder;
	QPoint*  mCount;

	public:
	QString* getFolder (void);
	QPoint*  getCount  (void);
	
	public:
	NotifyCount (const QString&, const QPoint&);
};

//=========================================
// Class Notify...
//-----------------------------------------
class Notify : public QObject {
	Q_OBJECT

	public:
	Notify ( Parser* );

	public:
	QList<NotifyCount> getInitialFolderList (void);
	bool sendSignal ( int,int );
	void init ( bool = false );

	private:
	int getFiles ( const QString& );
	void activateFolderNotification ( const QString&,const QString& );
	void cleanActiveFolderNotification ( void );

	private:
	QList<NotifyCount> mInitialFolderList;
	QList<int> mFDs;
	Parser* mParse;

	public:
	QIntDict<QString> mNotifyDirs;
	QIntDict<QPoint>  mNotifyCount;

	signals:
	void sigNotify ( QString*,QPoint* );
	void sigCreate ( QString*,QPoint* );
	void sigDelete ( QString*,QPoint* );
};

#endif
