/**************
FILE          : notify.c
***************
PROJECT       : QBiff - A qt based biff
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : new mail notification service 
              : 
              :
DESCRIPTION   : kernel F_NOTIFY support
              :
              :
STATUS        : Status: Beta
**************/
#include "notify.h"
#include "config.h"

//====================================
// Globals
//------------------------------------
void* self = NULL;
int FD[3] = {-1,-1,-1};

//====================================
// Constructor
//------------------------------------
NotifyCount::NotifyCount (const QString& folder, const QPoint& count) {
	mFolder = new QString ( folder );
	mCount  = new QPoint  ( count );
}

//====================================
// getFolder
//------------------------------------
QString* NotifyCount::getFolder (void) {
	return mFolder;
}

//====================================
// getCount
//------------------------------------
QPoint* NotifyCount::getCount (void) {
	return mCount;
}

//====================================
// Constructor
//------------------------------------
Notify::Notify ( Parser* parse ) {
	mParse = parse;
	init ();
}

void Notify::init ( void ) {
	QList<char> mFolderNames = mParse -> folderList();
	QList<char> subdir;
	subdir.append ("/new");
	subdir.append ("/cur");

	struct sigaction action;
	struct sigaction pending;

	action.sa_sigaction = handleNotifyCreateEvent;
	sigemptyset (&action.sa_mask);
	action.sa_flags = SA_SIGINFO;
	sigaction (SIGRTMIN + 0 , &action , 0);

	action.sa_sigaction = handleNotifyDeleteEvent;
	sigemptyset (&action.sa_mask);
	action.sa_flags = SA_SIGINFO;
	sigaction (SIGRTMIN + 1 , &action , 0);

	pending.sa_sigaction = handlePendingEvent;
	sigaction (SIGIO , &pending , 0);

	sigset_t block_set;
	sigemptyset (&block_set);
	for (int i=0;i<2;i++) { 
		sigaddset (&block_set,SIGRTMIN + i);
	}

	mInitialFolderList.clear();
	mNotifyDirs.clear();
	mNotifyCount.clear();
	sigprocmask(SIG_BLOCK, &block_set,0);
	QListIterator<char> it ( mFolderNames );
	for (; it.current(); ++it) {
		QPoint* dirCount = 0;
		int FDs[4];
		int FDcount = 0;
		for (int i=0;i<2;i++) {
			if (i == 0) {
				dirCount = new QPoint;
				FDcount  = 0;
			}
			int count = getFiles (
				MY_FOLDER+QString(it.current())+QString(subdir.at(i))+"/*"
			);
			if (i == 0) {
				dirCount->setX (count);
			}
			if (i == 1) {
				dirCount->setY (count);
			}
			for (int n=0;n<2;n++) {
				FD[n] = open (
					MY_FOLDER+QString(it.current())+ QString(subdir.at(i)),
					O_RDONLY
				);
				if (FD[n] == -1) {
					continue;
				}
				FDs[FDcount] = FD[n];
				fcntl (FD[n], F_SETSIG, SIGRTMIN + n);
				long flags = 0;
				switch (n) {
					case 0:
						flags = DN_MULTISHOT | DN_CREATE;
					break;
					case 1:
						flags = DN_MULTISHOT | DN_DELETE;
					break;
					default:
					break;
				}
				if (fcntl (FD[n],F_NOTIFY, flags) == -1) {
					continue;
				}
				QString* folder = new QString (
					it.current()+QString(subdir.at(i))
				);
				mNotifyDirs.insert (
					FD[n], folder
				);
				FDcount++;
			}
			if (i == 1) {
				for (int n=0;n<4;n++) {
					mNotifyCount.insert ( FDs[n], dirCount );
				}
				NotifyCount* initial = new NotifyCount (
					it.current(),*dirCount
				);
				mInitialFolderList.append (initial);
			}
		}
	}
	sigprocmask(SIG_UNBLOCK, &block_set,0);
	self = this;
}

//=========================================
// count files
//-----------------------------------------
int Notify::getFiles (const QString& pattern) {
	int count = 0;
	glob_t globbuf;
	if (glob (pattern.ascii(), GLOB_MARK, 0, &globbuf) == 0) {
		count = globbuf.gl_pathc;
		#if 0
		for (unsigned int n=0;n< globbuf.gl_pathc;n++) {
			printf ("%s\n",globbuf.gl_pathv[n]);
		}
		#endif
	}
	globfree (&globbuf);
	return count;
}

//=========================================
// Member call for handleNotificationEvent
//-----------------------------------------
bool Notify::sendSignal (int fd,int flag) {
	if ( mNotifyDirs[fd] ) {
		QString* pFolder = mNotifyDirs[fd];
		QPoint*  count   = mNotifyCount[fd];
		QStringList tokens = QStringList::split ( "/", *pFolder );
		QString folder  = tokens.first();
		QString dirname = tokens.last();
		switch (flag) {
			case QBIFF_CREATE:
				printf ("________create %s %p\n",pFolder->ascii(),count);
				if (dirname == "new") {
					count -> rx()++;
				} else {
					count -> ry()++;
				}
				sigCreate ( &folder,count );
			break;
			case QBIFF_DELETE:
				printf ("________delete %s %p\n",pFolder->ascii(),count);
				if (dirname == "new") {
					count -> rx()--;
				} else {
					count -> ry()--;
				}
				sigDelete ( &folder,count );
			break;
			default:
			break;
		}
		sigNotify ( &folder,count );
		return true;
	}
	return false;
}

//=========================================
// Member call for handlePendingEvent
//-----------------------------------------
void Notify::sendPoll (void) {
	sigPoll ();
}

//=========================================
// getInitialFolderList
//-----------------------------------------
QList<NotifyCount> Notify::getInitialFolderList (void) {
	return mInitialFolderList;
}

//=========================================
// Real time signal SIGRTMIN0 arrived
//-----------------------------------------
void handleNotifyCreateEvent ( int, siginfo_t* si , void* ) {
	Notify* obj = (Notify*)self;
	obj->sendSignal (si->si_fd,QBIFF_CREATE);
}

//=========================================
// Real time signal SIGRTMIN1 arrived
//-----------------------------------------
void handleNotifyDeleteEvent ( int, siginfo_t* si , void* ) {
	Notify* obj = (Notify*)self;
	obj->sendSignal (si->si_fd,QBIFF_DELETE);
}

//=========================================
// Pollable event occured (SIGIO)
//-----------------------------------------
void handlePendingEvent ( int, siginfo_t* , void* ) {
	// ... /
	// RT signal queue is full, the result is a SIGIO and we
	// need to check all notify-directories
	// ...
	Notify* obj = (Notify*)self;
	obj -> sendPoll ();
}
