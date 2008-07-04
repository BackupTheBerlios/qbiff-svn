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
// Globals...
//-----------------------------------------
extern QString PIXINFO; 

//=========================================
// Constructor
//-----------------------------------------
Button::Button(
	const QString &text, QWidget*parent 
): QPushButton (text,parent) {
	QObject::connect (
		this , SIGNAL ( clicked     (void) ),
		this,  SLOT   ( slotClicked (void) )
	);
	mFolder = text;
	//setFont (QFont ("Dejavu Sans", 10, QFont::Normal));
	setFont (QFont ("FrutigerNextLT:style=Bold", 8, QFont::Bold));
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
	return text;
}

//=========================================
// set tooltip with mail count
//-----------------------------------------
void Button::setTip (const QString& newmail,const QString& curmail) {
	QString text = tipText (newmail,curmail);
	setToolTip (text);
}

//=========================================
// slotClicked
//-----------------------------------------
void Button::slotClicked (void) {
	clickedButton (this);
}
