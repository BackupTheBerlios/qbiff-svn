/**************
FILE          : main.cpp
***************
PROJECT       : QBiff - A qt based biff
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : new mail notification service 
              : 
              :
DESCRIPTION   : ask for password using kwallet system
              :
              :
STATUS        : Status: Beta
**************/
#include <sys/resource.h>
#include <kwallet.h>
#include <KPasswordDialog>
#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocalizedString>
#include <QTextStream>

int main(int argc, char **argv) {
	KAboutData about (
		"qbiff-pwd",
		0,
		ki18n("qbiff-pwd"),
		"0.2.0",
		ki18n("KDE version of ssh-askpass"),
		KAboutData::License_GPL,
		ki18n("(c) 2009 Marcus Schaefer"),
		ki18n("qbiff-pwd is a helper program to ask for the password in a qbiff session"),
		"http://www.isny.homelinux.com",
		"ms@suse.de"
	);
	about.addAuthor(
		ki18n("Armin Berres"), ki18n("Current author"), "trigger@space-based.de", 0
	);
	about.addAuthor(
		ki18n("Hans van Leeuwen"), ki18n("Original author"), "hanz@hanz.nl", 0
	);
	about.addAuthor(
		ki18n("Marcus Schaefer"), ki18n("Adapted for qbiff"), "ms@suse.de", 0
	);
	KCmdLineOptions options;
	options.add("+[prompt]",ki18n("Prompt")); 
	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineArgs::addCmdLineOptions( options );
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	//=========================================
	// create application object
	//-----------------------------------------
	KApplication app;
	app.disableSessionManagement();
	//=========================================
	// setup default password dialog
	//-----------------------------------------
	QString walletFolder = about.appName();
	QString dialog = i18n("Please enter password");  // Default dialog text.
	QString keyFile;
	QString password;
	//=========================================
	// commandline options
	//-----------------------------------------
	if ( args->count() > 0 ) {
		dialog = args->arg(0);
		keyFile = dialog.section(" ", -2).remove(":");
	}
	args->clear();
	//=========================================
	// open wallet and ask for the password
	//-----------------------------------------
	KWallet::Wallet *wallet = KWallet::Wallet::openWallet( KWallet::Wallet::NetworkWallet(), 0 );
	if ( wallet && wallet->hasFolder(walletFolder) ) {
		wallet->setFolder(walletFolder);
		QString retrievedPass;
		wallet->readPassword(keyFile, retrievedPass);
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

		kpd.setPrompt(dialog);
		kpd.setCaption(i18n("qbiff-pwd"));
		// ...
		// We don't want to dump core when the password dialog is shown, because
		// it could contain the entered password. KPasswordDialog::disableCoreDumps()
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
				wallet->writePassword(keyFile, password);
			}
		} else {
			//=========================================
			// password dialog canceled
			//-----------------------------------------
			return 1;
		}
	}
	//=========================================
	// delete wallet print password
	//-----------------------------------------
	delete wallet;
	QTextStream out(stdout);
	out << password << endl; 
	return 0; 
}
