/**************
FILE          : button.h
***************
PROJECT       : QBiff - A qt based biff
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : new mail notification service 
              : 
              :
DESCRIPTION   : header file for button widget
              :
              :
STATUS        : Status: Beta
**************/
#ifndef BUTTON_H
#define BUTTON_H 1

#include <qpushbutton.h>
#include <qapplication.h>
#include <qwidget.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qstring.h>
#include <qevent.h>
#include <qtimer.h>
#include <qnamespace.h>
#include <X11/extensions/XTest.h>

#include "config.h"

#define UPTODATE 1
#define COLOR    2

//=========================================
// Class Button
//-----------------------------------------
class Button : public QPushButton {
	Q_OBJECT

	private:
	int      mLastNewCount;
	QString  mFolder;

	private:
	QString tipText (const QString &,const QString &);

	public:
	Button( const QString &, QWidget*, const char* =0 );
	void setTip (const QString &,const QString &);

	private slots:
	void slotClicked (void);

	signals:
	void clickedButton (QPushButton*);
};

#endif
