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
SSLServerInit::SSLServerInit ( QObject* parent ) : SSLCommon ( parent ) {
	QString* port = new QString;
	QTextOStream (port) << PORT;
	init_OpenSSL ();
	seed_prng ();
	setupServerCTX ();
	acc = BIO_new_accept ((char*)port->ascii());
	if (!acc) {
		qerror("Error creating server socket");
	}
	if (BIO_do_accept(acc) <= 0) {
		qerror("Error binding server socket");
	}
}

//=========================================
// Connection loop...
//-----------------------------------------
void SSLServerInit::openConnection (void) {
	while (1) {
		SSL* ssl;
		if (BIO_do_accept(acc) <= 0) {
			qerror("Error accepting connection");
		}
		BIO* client = BIO_pop(acc);
		if (!(ssl = SSL_new(ctx))) {
			qerror("Error creating SSL context");
		}
		SSL_set_accept_state(ssl);
		SSL_set_bio(ssl, client, client);
		SSLServerConnection* connection = new SSLServerConnection (ssl);
		mSSLConnections.append (connection);
		connect (
			connection , SIGNAL ( clientInit     (void) ),
			this       , SLOT   ( initClientInit (void) )
		);
		connection -> start();
	}
	SSLFree();
}

//=========================================
// Certificate setup...
//-----------------------------------------
void SSLServerInit::setupServerCTX ( void ) {
	ctx = SSL_CTX_new(SSLv23_method(  ));
	if (SSL_CTX_load_verify_locations(ctx, CAFILE, CADIR) != 1) {
		qerror("Error loading CA file and/or directory");
	}
	if (SSL_CTX_set_default_verify_paths(ctx) != 1) {
		qerror("Error loading default CA file and/or directory");
	}
	if (SSL_CTX_use_certificate_chain_file(ctx, SERVER_CERTFILE) != 1) {
		qerror("Error loading certificate from file");
	}
	if (
		SSL_CTX_use_PrivateKey_file (ctx,SERVER_CERTFILE,SSL_FILETYPE_PEM) != 1
	) {
		qerror("Error loading private key from file");
	}
	SSL_CTX_set_verify (
		ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
		verify_callback
	);
	SSL_CTX_set_verify_depth(ctx, 4); 
	SSL_CTX_set_options(
		ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE
	);
	SSL_CTX_set_tmp_dh_callback(ctx, tmp_dh_callback);
	if (SSL_CTX_set_cipher_list(ctx, CIPHER_LIST) != 1) {
		qerror("Error setting cipher list (no valid ciphers)");
	}
}

//=========================================
// Write data to all connections...
//-----------------------------------------
void SSLServerInit::writeClient ( const QString & data ) {
	QListIterator<SSLServerConnection> io (mSSLConnections);
	for (; io.current(); ++io) {
		SSLServerConnection* connection = io.current();
		connection -> writeClient ( data );
	}
}

//=========================================
// re-emit clientInit signal...
//-----------------------------------------
void SSLServerInit::initClientInit ( void ) {
	clientInit();
}

//=========================================
// free SSL context...
//-----------------------------------------
void SSLServerInit::SSLFree ( void ) {
	SSL_CTX_free (ctx);
	BIO_free (acc);
}
