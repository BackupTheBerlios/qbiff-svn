/**************
FILE          : button.cpp
***************
PROJECT       : QBiff - A qt based biff
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : new mail notification service 
              : 
              :
DESCRIPTION   : implementation for button widget
              :
              :
STATUS        : Status: Beta
**************/
#include "button.h"

//=========================================
// Constructor
//-----------------------------------------
Button::Button(
	const QString &text, QWidget*parent, const char*name
): QPushButton (text,parent,name) {
	QObject::connect (
		this , SIGNAL ( clicked     (void) ),
		this,  SLOT   ( slotClicked (void) )
	);
	mFolder = text;
	setFont (QFont ("Sans Serif", 10, QFont::Normal));
	mLastNewCount = 0;
}

//=========================================
// create info text
//-----------------------------------------
QString Button::tipText (const QString& newmail,const QString& curmail) {
	int newcount = newmail.toInt();
	int curcount = curmail.toInt();
	int allcount = newcount + curcount;
	QString allmail;
	allmail.sprintf ("%d",allcount);
	QString text;
	QTextOStream (&text)
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
	return text;
}

//=========================================
// set tooltip with mail count
//-----------------------------------------
void Button::setTip (const QString& newmail,const QString& curmail) {
	QString text = tipText (newmail,curmail);
	QToolTip::add (this,text);
}

//=========================================
// slotClicked
//-----------------------------------------
void Button::slotClicked (void) {
	clickedButton (this);
}
