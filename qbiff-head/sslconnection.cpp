/**************
FILE          : sslserver.cpp
***************
PROJECT       : QBiff - A qt based biff
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : new mail notification service 
              : 
              :
DESCRIPTION   : network related socket functions
              :
              :
STATUS        : Status: Beta
**************/
#include "sslserver.h"

//=========================================
// Globals...
//-----------------------------------------
extern QList<SSLServerConnection> mSSLConnections;

//=========================================
// Constructor...
//-----------------------------------------
SSLServerConnection::SSLServerConnection ( SSL* pSSL ) {
	ssl = pSSL;
}

//=========================================
// doConnection...
//-----------------------------------------
void SSLServerConnection::run ( void ) {
	long err;
	if (SSL_accept(ssl) <= 0) {
		qerror("Error accepting SSL connection");
	}
	if ((err = postConCheck(ssl, CLIENT)) != X509_V_OK) {
		fprintf(stderr, "-Error: peer certificate: %s\n",
			X509_verify_cert_error_string(err)
		);
		qerror("Error checking SSL object after connection");
	}
	printf ("SSL Connection opened\n");
	if (readClient()) {
		SSL_shutdown (ssl);
	} else {
		SSL_clear (ssl);
	}
	printf ("SSL Connection closed\n");
	mSSLConnections.remove (this);
	SSL_free (ssl);
	ERR_remove_state (0);
	exit();
}

//=========================================
// readClient...
//-----------------------------------------
int SSLServerConnection::readClient ( void ) {
	char buf[2];
	int len;
	QString line;

	while (1) {
	do {
		int r = SSL_read (ssl,buf,1);
		switch (SSL_get_error(ssl,r)) {
		case SSL_ERROR_NONE:
			len=r;
		break;
		case SSL_ERROR_WANT_READ:
		break;
		case SSL_ERROR_ZERO_RETURN:
		break;
		case SSL_ERROR_SYSCALL:
			fprintf ( stderr,"SSL Error: Premature close\n" );
			return (
				(SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) ? 1:0
			);
		break;
		default:
			break;
		}
		if (buf[0] == '\n') {
			line = line.simplifyWhiteSpace();
			QStringList tokens = QStringList::split (" ", line);
			if ( tokens[0] == "INIT" ) {
				clientInit();
			}
			if ( tokens[0] == "WRITE" ) {
			if (tokens[1]) {
				QString sshfile = "/tmp/isny_" + tokens[1];
				QFile sshflag (sshfile);
				if (sshflag.open( IO_WriteOnly ) ) {
					sshflag.close();
				}
			}
			}
			if  ( tokens[0] == "QUIT" ) {
			return (
				(SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) ? 1:0
			);
			}
			line = "";
		} else {
			line.append (buf[0]);
		}
	} while (SSL_pending(ssl));
	}
	return (
		(SSL_get_shutdown(ssl) & 
		SSL_RECEIVED_SHUTDOWN) ? 1 : 0
	);
}

//=========================================
// writeClient...
//-----------------------------------------
void SSLServerConnection::writeClient ( const QString & data ) {
	int err = 0;
	QString stream (data + "\n");
	char buf[stream.length()];
	memcpy (buf,stream.data(),stream.length());
	for (unsigned int nwritten = 0;nwritten < sizeof(buf);nwritten+=err) {
		err = SSL_write(ssl,buf + nwritten, sizeof(buf) - nwritten);
		switch (SSL_get_error(ssl,err)) {
		case SSL_ERROR_NONE:
			break;
		case SSL_ERROR_WANT_WRITE:
			continue;
		case SSL_ERROR_ZERO_RETURN:
			return;
		case SSL_ERROR_SYSCALL:
			return;
		default:
			return;
		}
	}
}
