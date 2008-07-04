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
// Globals...
//--------------------------------------------
extern QString PIXINFO; 
extern QString PIXPUBL;
extern QString PIXPRIV;

//============================================
// Constructor
//--------------------------------------------
ClientInfo::ClientInfo (
	QString &folder, QWidget* parent, int newcount,
	bool modal, Qt::WFlags f
) : QWidget ( 
	parent, (modal ? (f|(Qt::WFlags)Qt::WindowModal) : f)| Qt::Window|Qt::Dialog
) {
	QBoxLayout* layer1 = new  QVBoxLayout ( this );
	mLabel  = new QLabel ( this );
	mLabel -> setFrameStyle (
		QFrame::Box | QFrame::Plain
	);
	QPalette pal = mLabel -> palette();
	pal.setColor( QPalette::Window, QColor(250,250,210) );
	mLabel -> setPalette (pal);
	mLabel -> setLineWidth ( 1 );
	layer1 -> addWidget ( mLabel );
	layer1 -> setMargin (0);
	mFolder = folder;
	mTimer = new QTimer ( this );
	connect (
		mTimer , SIGNAL (timeout   (void)),
		this   , SLOT   (timerDone (void))
	);
	mNewMailCount = newcount;
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
		QTextStream (&text)
			<< "<table border=0 cellspacing=0>"
			<< "<tr>"
			<< "<th rowspan=2><img src=\"" << PIXINFO << "\"></th>"
			<< "<td><nobr>Folder: <b>" << mFolder
			<< " : " << newmail << "</b> new Mail(s)</nobr></td>"
			<< "</tr>"
			<< "<tr>"
			<< "<td><hr>Counting <b>" << allmail << "</b> mails</td>"
			<< "</tr>"
			<< "</table>";
		mLabel -> setText ( text );
		QFontMetrics metrics ( font() );
		show();
		mTimer -> start ( 2000 );
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
	QWidgetList list = QApplication::topLevelWidgets();
	QWidget* mainWidget = list.at(0);
	if (mainWidget) {
		move ( 
			mainWidget->x() + xo, 
			mainWidget->y() + yo
		);
	}
}

//============================================
// hide window
//--------------------------------------------
void ClientInfo::timerDone (void) {
	hide();
}
