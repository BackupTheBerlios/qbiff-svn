/**************
FILE          : sslclient.h
***************
PROJECT       : QBiff - A qt based biff
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : new mail notification service 
              : 
              :
DESCRIPTION   : header file for network socket stuff
              :
              :
STATUS        : Status: Beta
**************/
#ifndef SSL_CLIENT_H
#define SSL_CLIENT_H 1

#include <qapplication.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qdict.h>

#include "sslcommon.h"
#include "config.h"

//=========================================
// Class SSLClient
//-----------------------------------------
class SSLClient : public SSLCommon {
	Q_OBJECT

	public:
	SSLClient ( QObject* = 0 );

	private:
	bool     mDataToWrite;
	int      mSocket;
	fd_set   readFDs;
	fd_set   writeFDs;
	SSL_CTX* ctx;
	SSL*     ssl;

	private:
	void setupClientCTX (void);
	void connectTCP (void);

	public:
	void writeClient ( const QString & );
	void clientReadWrite (void);
	void SSLFree (void);

	signals:
	void gotLine ( QString );
};

#endif
