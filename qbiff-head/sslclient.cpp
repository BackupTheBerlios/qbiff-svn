/**************
FILE          : sslclient.cpp
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
#include "sslclient.h"

//=========================================
// Constructor
//-----------------------------------------
SSLClient::SSLClient ( QObject* parent ) : SSLCommon ( parent ) {
	BIO *sbio;
	printf ("Initialize openssl\n");
	init_OpenSSL ();
	seed_prng ();
	printf ("Setup client context\n");
	setupClientCTX ();
	printf ("Connecting TCP socket\n");
	connectTCP ();

	printf ("Creating new SSL object\n");
	ssl = SSL_new (ctx);
	printf ("Creating new SSL BIO socket\n");
	sbio= BIO_new_socket (mSocket,BIO_NOCLOSE);
	printf ("Setup SSL BIO socket\n");
	SSL_set_bio (ssl,sbio,sbio);
	printf ("Setup SSL and TCP socket\n");
	SSL_set_fd (ssl,mSocket);
	printf ("Sleeping 5 sec\n");
	sleep (5);
	printf ("Connecting SSL socket\n");
	if (SSL_connect(ssl) <=0) {
		qerror ("Error creating connection BIO");
	}
	int err = postConCheck (ssl, SERVER);
	if (err != X509_V_OK) {
		fprintf (stderr, "-Error: peer certificate: %s\n",
			X509_verify_cert_error_string(err)
		);
		qerror ("Error checking SSL object after connection");
	}
	int ofcmode = fcntl (mSocket,F_GETFL,0);
	if (fcntl (mSocket,F_SETFL,ofcmode | O_NDELAY)) {
		qerror ("Couldn't make socket nonblocking");
	}
	mDataToWrite = false;
	FD_ZERO (&readFDs);
	FD_ZERO (&writeFDs);
	printf ("SSL Connection created\n");
}

//=========================================
// clientReadWrite
//-----------------------------------------
void SSLClient::clientReadWrite ( void ) {
	if (mDataToWrite) {
		FD_SET (mSocket,&writeFDs);
	} else {
		FD_SET (mSocket,&readFDs);
	}
	struct timeval timeout;
	timeout.tv_sec  = 0;
	timeout.tv_usec = 1000;
	int width = mSocket + 1;
	int r = select (width,&readFDs,&writeFDs,0,&timeout);
	if (r == 0) {
		return;
	}
	char buf[2];
	QString line;
	int len = 0;
	if (FD_ISSET (mSocket,&readFDs)) {
	do {
		int r = SSL_read (ssl,buf,1);
		switch (SSL_get_error(ssl,r)) {
		case SSL_ERROR_NONE:
			len=r;
		break;
		case SSL_ERROR_WANT_READ:
		break;
		break;	
		case SSL_ERROR_ZERO_RETURN:
		break;
		case SSL_ERROR_SYSCALL:
			qerror ("SSL Error: Premature close");
		break;
		default:
			break;
		}
		if (buf[0] == '\n') {
			gotLine (line);
		} else {
			line.append(buf[0]);
		}
	} while (SSL_pending(ssl));
	}
}

//=========================================
// writeClient
//-----------------------------------------
void SSLClient::writeClient ( const QString & data ) {
	int err = 0;
	mDataToWrite = true;
	FD_SET (mSocket,&writeFDs);
	QString stream (data + "\n");
	char buf[stream.length()];
	memcpy (buf,stream.data(),stream.length());
	for (unsigned int nwritten = 0;nwritten < sizeof(buf); nwritten += err) {
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
	FD_SET (mSocket,&readFDs);
	mDataToWrite = false;
}

//=========================================
// connectTCP
//-----------------------------------------
void SSLClient::connectTCP (void) {
	struct hostent *hp;
	struct sockaddr_in addr;

	if (!(hp=gethostbyname(SERVER))) {
		qerror ("Couldn't resolve host");
	}
	bzero(&addr,sizeof(addr));
	bcopy (hp->h_addr,(char*)&addr.sin_addr,hp->h_length);
	addr.sin_addr=*(struct in_addr*)hp->h_addr_list[0];
	addr.sin_family=hp->h_addrtype;
	addr.sin_port=htons(PORT);

	if ((mSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {
		qerror ("Couldn't create socket");
	}
	if (::connect(mSocket,(struct sockaddr *)&addr,sizeof(addr))<0) {
		qerror ("Couldn't connect socket");
	}
}

//=========================================
// setupClientCTX
//-----------------------------------------
void SSLClient::setupClientCTX (void) {
	ctx = SSL_CTX_new(SSLv23_method(  ));
	SSL_CTX_set_default_passwd_cb (ctx, passwd_cb);
	if (SSL_CTX_load_verify_locations(ctx, CAFILE, CADIR) != 1) {
		qerror("Error loading CA file and/or directory");
	}
	if (SSL_CTX_set_default_verify_paths(ctx) != 1) {
		qerror("Error loading default CA file and/or directory");
	}
	if (SSL_CTX_use_certificate_chain_file(ctx, CLIENT_CERTFILE) != 1) {
		qerror("Error loading certificate from file");
	}
	if (
		SSL_CTX_use_PrivateKey_file(ctx,CLIENT_CERTFILE,SSL_FILETYPE_PEM) != 1
	) {
		qerror("Error loading private key from file");
	}
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);
	SSL_CTX_set_verify_depth(ctx, 4);
	SSL_CTX_set_options(ctx, SSL_OP_ALL|SSL_OP_NO_SSLv2);
	if (SSL_CTX_set_cipher_list(ctx, CIPHER_LIST) != 1) {
		qerror("Error setting cipher list (no valid ciphers)");
	}
}

//=========================================
// SSLFree
//-----------------------------------------
void SSLClient::SSLFree ( void ) {
	SSL_shutdown (ssl);
	SSL_free (ssl);
	SSL_CTX_free (ctx);
}
