/**************
FILE          : sslcommon.cpp
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
#include "sslcommon.h"

//=========================================
// Globals...
//-----------------------------------------
DH *dh512  = NULL;
DH *dh1024 = NULL;

//=========================================
// SSL callback Functions...
//-----------------------------------------
int verify_callback (int ok, X509_STORE_CTX *store) {
	char data[256];
	if (!ok) {
		X509 *cert = X509_STORE_CTX_get_current_cert(store);
		int  depth = X509_STORE_CTX_get_error_depth(store);
		int  err = X509_STORE_CTX_get_error(store);

		fprintf(stderr, "-Error with certificate at depth: %i\n", depth);
		X509_NAME_oneline(X509_get_issuer_name(cert), data, 256);
		fprintf(stderr, "  issuer   = %s\n", data);
		X509_NAME_oneline(X509_get_subject_name(cert), data, 256);
		fprintf(stderr, "  subject  = %s\n", data);
		fprintf(stderr, "  err %i:%s\n",
			err, X509_verify_cert_error_string(err)
		);
	}
	return ok;
}

//=========================================
// passwd_cb...
//-----------------------------------------
int passwd_cb (char* buf,int size,int,void*) {
	char password[80];
	fgets (password, sizeof(password), stdin);
	password[strlen(password)-1]='\0';
	strncpy(buf, (char *)(password), size);
	buf[size - 1] = '\0';
	return (strlen(buf));
}

//=========================================
// init_dhparams...
//-----------------------------------------
void init_dhparams (void) {
	BIO *bio;
	bio = BIO_new_file(DH512, "r");
	if (!bio) {
		qerror("Error opening file dh512.pem");
	}
	dh512 = PEM_read_bio_DHparams(bio, NULL, NULL, NULL);
	if (!dh512) {
		qerror("Error reading DH parameters from dh512.pem");
	}
	BIO_free(bio);

	bio = BIO_new_file(DH1024, "r");
	if (!bio) {
		qerror("Error opening file dh1024.pem");
	}
	dh1024 = PEM_read_bio_DHparams(bio, NULL, NULL, NULL);
	if (!dh1024) {
		qerror("Error reading DH parameters from dh1024.pem");
	}
	BIO_free(bio);
}

//=========================================
// tmp_dh_callback...
//-----------------------------------------
DH* tmp_dh_callback (SSL*,int, int keylength) {
	DH *ret;
	if (!dh512 || !dh1024) {
		init_dhparams(  );
	}
	switch (keylength) {
	case 512:
		ret = dh512;
		break;
	case 1024:
	default:
		ret = dh1024;
		break;
	}
	return ret;
}

//=========================================
// handle_error...
//-----------------------------------------
void handle_error (const char *file, int lineno, const char *msg) {
	fprintf(stderr, "** %s:%i %s\n", file, lineno, msg);
	ERR_print_errors_fp(stderr);
	exit (1);
}

//=========================================
// SSLCommon member  Functions...
//-----------------------------------------
//=========================================
// Constructor
//-----------------------------------------
SSLCommon::SSLCommon ( QObject* ) {
	// ...
}

//=========================================
// post_connection_check...
//-----------------------------------------
long SSLCommon::postConCheck (SSL *ssl, char *host) {
	X509      *cert;
	X509_NAME *subj;
	char      data[256];
	int       extcount;
	int       ok = 0;

	if (!(cert = SSL_get_peer_certificate(ssl)) || !host) {
		goto err_occured;
	}
	if ((extcount = X509_get_ext_count(cert)) > 0) {
	for (int i = 0;  i < extcount;  i++) {
		char              *extstr;
		X509_EXTENSION    *ext;

		ext = X509_get_ext(cert, i);
		extstr = (char*) OBJ_nid2sn (
			OBJ_obj2nid(X509_EXTENSION_get_object(ext))
		);
		if (!strcmp(extstr, "subjectAltName")) {
			int                  j;
			unsigned char        *data;
			STACK_OF(CONF_VALUE) *val;
			CONF_VALUE           *nval;
			X509V3_EXT_METHOD    *meth;
			void                 *ext_str = NULL;

			if (!(meth = X509V3_EXT_get(ext))) {
				break;
			}
			data = ext->value->data;

			#if (OPENSSL_VERSION_NUMBER > 0x00907000L)
			if (meth->it) {
				ext_str = ASN1_item_d2i (
					NULL, &data, ext->value->length,
					ASN1_ITEM_ptr(meth->it)
				);
			} else {
				ext_str = meth->d2i(NULL, &data, ext->value->length);
			}
			#else
			ext_str = meth->d2i(NULL, &data, ext->value->length);
			#endif

			val = meth->i2v(meth, ext_str, NULL);
			for (j = 0;  j < sk_CONF_VALUE_num(val);j++) {
				nval = sk_CONF_VALUE_value(val, j);
				if (
					!strcmp(nval->name, "DNS") && 
					!strcmp(nval->value, host)
				) {
					ok = 1; break;
				}
			}
		}
		if (ok) {
			break;
		}
	}
	}
	if (
		!ok && (subj = X509_get_subject_name(cert)) &&
        X509_NAME_get_text_by_NID(subj, NID_commonName, data, 256) > 0
	) {
		data[255] = 0;
		if (strcasecmp(data, host) != 0) {
			goto err_occured;
		}
	}
	X509_free(cert);
	return SSL_get_verify_result(ssl);

	err_occured:
	if (cert) {
		X509_free(cert);
	}
	return X509_V_ERR_APPLICATION_VERIFICATION;
}

//=========================================
// init_OpenSSL...
//-----------------------------------------
void SSLCommon::init_OpenSSL (void) {
	if ( ! SSL_library_init() ) {
		qerror ("OpenSSL initialization failed");
	}
	SSL_load_error_strings();
}

//=========================================
// seed_prng...
//-----------------------------------------
void SSLCommon::seed_prng (void) {
	RAND_load_file ("/dev/urandom", 1024);
}
