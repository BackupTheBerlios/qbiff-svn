/**************
FILE          : xbox.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : xbox wrapper class for QMessageBox
              : to create a frame around the dialog because
              : YaST2 does not provide a Windowmanager
              :
STATUS        : Status: Up-to-date
**************/
#ifndef CLIENTINFO_H
#define CLIENTINFO_H 1

#include <qapplication.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtimer.h>

#include "config.h"

//=========================================
// Class ClientInfo
//-----------------------------------------
class ClientInfo : public QWidget {
	Q_OBJECT

	private:
	QLabel*  mLabel;
	QString  mFolder;
	QTimer*  mTimer;
	int      mNewMailCount;

	public:
	ClientInfo (
		QString&,QWidget*,int,const char* =0,
		bool =TRUE,WFlags = Qt::WStyle_Customize | Qt::WStyle_NoBorder
	);

	public:
	void setTip (const QString&,const QString&);

	protected:
	virtual void showEvent   ( QShowEvent * );

	private slots:
	void timerDone (void);
};

#endif
