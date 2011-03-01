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

#include <KPasswordDialog>

//=========================================
// Globals...
//-----------------------------------------
extern QString DH1024;
extern QString DH512;

//=========================================
// Globals...
//-----------------------------------------
extern QString DH1024;
extern QString DH512;

//=========================================
// Globals...
//-----------------------------------------
extern KAboutData* about;
extern bool useGUI;

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
	if (! useGUI) {
		char password[80];
		char* p = 0;
		p = fgets (password, sizeof(password), stdin);
		password[strlen(password)-1]='\0';
		strncpy(buf, (char *)(password), size);
		buf[size - 1] = '\0';
		return (strlen(buf));
	} else {
		//=========================================
		// setup default password dialog
		//-----------------------------------------
		QString walletFolder = about->appName();
		QString password;

		//=========================================
		// open wallet and ask for the password
		//-----------------------------------------
		KWallet::Wallet *wallet = KWallet::Wallet::openWallet(
			KWallet::Wallet::NetworkWallet(), 0
		);
		if ( wallet && wallet->hasFolder(walletFolder) ) {
			wallet->setFolder(walletFolder);
			QString retrievedPass;
			wallet->readPassword("qbiffp", retrievedPass);
			if ( !retrievedPass.isEmpty() ) {
				password = retrievedPass;
			}
		}
		//=========================================
		// password empty ask for it
		//-----------------------------------------
		if ( password.isEmpty() ) {
			KPasswordDialog::KPasswordDialogFlag flag;
			if ( wallet ) {
				flag = KPasswordDialog::ShowKeepPassword;
			}
			KPasswordDialog kpd(0, flag);
			kpd.setPrompt(i18n("Please enter password"));
			kpd.setCaption(i18n("qbiff"));
			// ...
			// We don't want to dump core when the password dialog is
			// shown, becauseit could contain the entered password.
			// KPasswordDialog::disableCoreDumps()
			// seems to be gone in KDE 4 -- do it manually
			struct rlimit rlim;
			rlim.rlim_cur = rlim.rlim_max = 0;
			setrlimit(RLIMIT_CORE, &rlim);

			if ( kpd.exec() == KDialog::Accepted ) {
				password = kpd.password();
				//=========================================
				// store password in wallet if keep is set
				//-----------------------------------------
				if ( wallet && kpd.keepPassword() ) {
					if ( !wallet->hasFolder( walletFolder ) ) {
						wallet->createFolder(walletFolder);
					}
					wallet->setFolder(walletFolder);
					wallet->writePassword("qbiffp", password);
				}
			} else {
				//=========================================
				// password dialog canceled
				//-----------------------------------------
				return 1;
			}
		}
		//=========================================
		// delete wallet
		//-----------------------------------------
		delete wallet;
		//QTextStream out(stdout);
		//out << password << endl;
		strncpy(buf, password.toLatin1().data(), size);
		buf[size - 1] = '\0';
		return password.length();
	}
}

//=========================================
// init_dhparams...
//-----------------------------------------
void init_dhparams (void) {
	BIO *bio;
	bio = BIO_new_file(DH512.toLatin1().data(), "r");
	if (!bio) {
		qerror("Error opening file dh512.pem");
	}
	dh512 = PEM_read_bio_DHparams(bio, NULL, NULL, NULL);
	if (!dh512) {
		qerror("Error reading DH parameters from dh512.pem");
	}
	BIO_free(bio);

	bio = BIO_new_file(DH1024.toLatin1().data(), "r");
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
			const X509V3_EXT_METHOD    *meth;
			//v3_ext_method *meth;
			void                 *ext_str = NULL;

			if (!(meth = X509V3_EXT_get(ext))) {
				break;
			}
			data = ext->value->data;

			#if (OPENSSL_VERSION_NUMBER > 0x00907000L)
			#if (OPENSSL_VERSION_NUMBER > 0x0090705FL)
			if (meth->it) {
				ext_str = ASN1_item_d2i (
					NULL, (const unsigned char**)&data, ext->value->length,
					ASN1_ITEM_ptr(meth->it)
				);
			} else {
				ext_str = meth->d2i(
					NULL, (const unsigned char**)&data, ext->value->length
				);
			}
			#else
			if (meth->it) {
				ext_str = ASN1_item_d2i (
					NULL, (unsigned char**)&data, ext->value->length,
					ASN1_ITEM_ptr(meth->it)
				);
			} else {
				ext_str = meth->d2i(
					NULL, (unsigned char**)&data, ext->value->length
				);
            }
			#endif
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
