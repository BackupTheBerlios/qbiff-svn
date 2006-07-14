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
#include "serverhandler.h"

//====================================
// Globals
//------------------------------------
sigset_t block_set;
int FD[3] = {-1,-1,-1};
void* self = NULL;

//====================================
// External Globals
//------------------------------------
extern ServerHandler* pServer;

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
	mParse = parse; init();
	struct sigaction pending;
	pending.sa_sigaction = handlePendingEvent; 
	sigaction (SIGIO , &pending , 0);

	struct sigaction action;
	action.sa_sigaction = handleNotifyEvent;
	sigemptyset (&action.sa_mask);
	action.sa_flags = SA_SIGINFO;
	sigaction (SIGRTMIN + 0 , &action , 0);
	sigaction (SIGRTMIN + 1 , &action , 0);

	sigemptyset (&block_set);
	sigaddset (&block_set,SIGIO);
	for (int i=0;i<2;i++) {
		sigaddset (&block_set,SIGRTMIN + i);
	}
	self = this;
}

//====================================
// init
//------------------------------------
void Notify::init ( bool clean ) {
	sigprocmask(SIG_BLOCK, &block_set,0);
	QList<char> mFolderNames = mParse -> folderList();
	if (clean) {
		printf ("________cleaning: pollable event occured\n");
		cleanActiveFolderNotification();
	}
	QList<char> subdir;
	subdir.append ("/new");
	subdir.append ("/cur");
	QListIterator<char> it ( mFolderNames );
	int FDcount = 0;
	for (; it.current(); ++it) {
		QPoint* dirCount = 0;
		for (unsigned int i=0;i<subdir.count();i++) {
			if (i == 0) {
				dirCount = new QPoint;
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
			activateFolderNotification (
				it.current(),subdir.at(i)
			);
			if (i == 1) {
				int start = FDcount;
				int ended = FDcount + 4;
				for (int n=start;n<ended;n++) {
					int fd = *mFDs.at(n);
					mNotifyCount.insert ( fd, dirCount );
				}
				FDcount = ended;
				NotifyCount* initial = new NotifyCount (
					it.current(),*dirCount
				);
				mInitialFolderList.append (initial);
			}
		}
	}
	sigprocmask(SIG_UNBLOCK, &block_set,0);
	mTimer = new QTimer ( this );
	connect ( 
		mTimer , SIGNAL (timeout   (void)),
		this   , SLOT   (timerDone (void))
	);
	mTimer -> start ( 10, FALSE );
}

//=========================================
// activateFolderNotification
//-----------------------------------------
void Notify::activateFolderNotification (
	const QString& folderName, const QString& subDir
) {
	for (int n=0;n<2;n++) {
		int fd = open (
			MY_FOLDER+folderName+subDir,
			O_RDONLY
		);
		if (fd == -1) {
			return;
		}
		fcntl (fd, F_SETSIG, SIGRTMIN + n);
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
		if (fcntl (fd,F_NOTIFY, flags) == -1) {
			return;
		}
		QString* folder = new QString (
			folderName+subDir
		);
		mNotifyDirs.insert (
			fd, folder
		);
		int* saveFD = (int*)malloc (sizeof (int));
		*saveFD = fd;
		mFDs.append (saveFD);
	}
}

//=========================================
// 
//-----------------------------------------
void Notify::cleanActiveFolderNotification (void) {
	QListIterator<int> fd ( mFDs );
	for (; fd.current(); ++fd) {
		fcntl (*fd.current(), F_NOTIFY, 0);
		fcntl (*fd.current(), F_SETSIG, 0);
		close (*fd.current());
	}
	mInitialFolderList.clear();
	mNotifyDirs.clear();
	mNotifyCount.clear();
	mFDs.clear();
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
	sigprocmask(SIG_BLOCK, &block_set,0);
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
		sigprocmask(SIG_UNBLOCK, &block_set,0);
		return true;
	}
	sigprocmask(SIG_UNBLOCK, &block_set,0);
	return false;
}

//=========================================
// getInitialFolderList
//-----------------------------------------
QList<NotifyCount> Notify::getInitialFolderList (void) {
	return mInitialFolderList;
}

//=========================================
// enqueue
//-----------------------------------------
void Notify::enqueue (int fd, int fl) {
	int* flag = (int*)malloc (sizeof(int));
	*flag = fl;
	mNotifyQueue.insert (fd,flag);
}

//=========================================
// Real time signal arrived
//-----------------------------------------
void handleNotifyEvent ( int s, siginfo_t* si , void* ) {
	Notify* obj = (Notify*)self;
	if (s == SIGRTMIN) {
		// Real time signal SIGRTMIN0 arrived
		obj -> enqueue (si->si_fd,QBIFF_CREATE);
	} else {
		// Real time signal SIGRTMIN1 arrived
		obj -> enqueue (si->si_fd,QBIFF_DELETE);
	}
}

//=========================================
// Pollable event occured (SIGIO)
//-----------------------------------------
void handlePendingEvent ( int, siginfo_t* , void* ) {
	// ... /
	// RT signal queue is full, the result is a SIGIO and we
	// need to check all notify-directories
	// ----
	if (pServer) {
		pServer -> poll();
	}
}

//=========================================
// timerDone
//-----------------------------------------
void Notify::timerDone (void) {
	// .../
	// every time the timer expires this method is called.
	// the function will send a signal including the file
	// descriptor of the touched file and the flag whether this
	// file was created or deleted
	// ----
	QIntDictIterator<int> it (mNotifyQueue);
	for (; it.current(); ++it) {
		int fd = it.currentKey();
		int flag = *it.current();
		sendSignal (fd,flag);
	}
	mNotifyQueue.clear();
}
