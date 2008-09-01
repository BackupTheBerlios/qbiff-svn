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
extern QString PIXNEWMAIL;
extern QString PIXNOMAIL; 

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
	setFont (QFont ("FrutigerNextLT:style=Bold", 10, QFont::Bold));
	mLastNewCount = 0;
	mTimer = new QTimer ( this );
	connect (
		mTimer , SIGNAL (timeout   (void)),
		this   , SLOT   (timerDone (void))
	);
	installEventFilter (this);
}

//=========================================
// slotClicked
//-----------------------------------------
void Button::slotClicked (void) {
	clickedButton (this);
}

//=========================================
// timerDone
//-----------------------------------------
void Button::timerDone (void) {
	showTip(this);
	mTimer->stop();
}

//=========================================
// eventFilter
//-----------------------------------------
bool Button::eventFilter ( QObject*, QEvent* event ) {
	QMouseEvent* mouse = (QMouseEvent*)event;
	if (! mouse) {
		return (false);
	}
	//printf ("%d\n",mouse->type());
	switch (mouse->type()) {
		case QEvent::HoverEnter:
			mTimer->start(1000);
		break;
		case QEvent::HoverLeave:
			mTimer->stop();
		break;
		default:
		break;
	}
	return (false);
}

