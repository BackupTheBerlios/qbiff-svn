/**************
FILE          : clientinfo.cpp
***************
PROJECT       : QBiff - A qt based biff
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : new mail notification service 
              : 
              :
DESCRIPTION   : client info window
              :
              :
STATUS        : Status: Beta
**************/
#include "clientinfo.h"

//============================================
// Constructor
//--------------------------------------------
ClientInfo::ClientInfo (
	QString &folder, QWidget* parent, const char* name, bool modal, WFlags f
) : QWidget ( 
	parent,name,
	(modal ? (f|WType_Modal) : f) | WType_TopLevel | WStyle_Dialog 
) {
	QBoxLayout* layer1 = new  QVBoxLayout ( this );
	mLabel  = new QLabel ( this );
	mLabel -> setFrameStyle (
		QFrame::Box|QFrame::Plain
	);
	mLabel -> setLineWidth ( 1 );
	layer1 -> addWidget ( mLabel );
	mFolder = folder;
	mTimer = new QTimer ( this );
	connect (
		mTimer , SIGNAL (timeout   (void)),
		this   , SLOT   (timerDone (void))
	);
	mNewMailCount = 0;
	adjustSize();
}

//============================================
// set label text
//--------------------------------------------
void ClientInfo::setTip (const QString& newmail,const QString& curmail) {
	bool showMe = true;
	int newcount = newmail.toInt();
	if (mNewMailCount >= newcount) {
		showMe = false;
	}
	mNewMailCount = newcount;
	if (showMe) {
		int curcount = curmail.toInt();
		int allcount = newcount + curcount;
		QString allmail;
		allmail.sprintf ("%d",allcount);
		QString text;
		QTextOStream (&text)
			<< "<table border=0 cellspacing=0><tr>"
			<< "<td><img src=\"" << PIXINFO << "\"></td>"
			<< "<td><nobr>Folder: <b>" << mFolder << " : "
			<< newmail << "</b> new Mail(s)</nobr><br><hr>"
			<< "Counting <b>" << allmail << "</b> mails</td>"
			<< "</tr></table>";
		mLabel -> setText ( text );
		QFontMetrics metrics ( font() );
		show();
		mTimer -> start ( 2000, true );
	}
}

//============================================
// Show event
//--------------------------------------------
void ClientInfo::showEvent ( QShowEvent* ) {
	QWidget* parent = parentWidget();
	int xo = 0;
	int yo = 0;
	if ( parent ) {
		xo = parent->x() + (parent->width()  / 2);
		yo = parent->y() - (parent->height() / 2) - (height() / 2) - 10;
	}
	if (qApp->mainWidget()) {
		move ( 
			qApp->mainWidget()->x() + xo, 
			qApp->mainWidget()->y() + yo
		);
	}
}

//============================================
// hide window
//--------------------------------------------
void ClientInfo::timerDone (void) {
	hide();
}
