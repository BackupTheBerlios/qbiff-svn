/**************
FILE          : parser.cpp
***************
PROJECT       : QBiff - A qt based biff
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : new mail notification service 
              : 
DESCRIPTION   : implementation for parsing .qbiffrc
              :
              :
STATUS        : Status: Beta
**************/
#include <stdlib.h>
#include <stdio.h>

#include "parser.h"

//=========================================
// Constructor
//-----------------------------------------
Parser::Parser ( const QString & file ) {
	mFile = new QFile (file);
	if (! mFile -> open(IO_ReadOnly)) {
		printf ("Parser::couldn't open file: %s\n",file.ascii());
		exit (1);
	}
	QString line;
	while ((mFile->readLine(line,MAX_LINE_LENGTH)) != 0) {
		if ((line[0] == '#') || (line.isEmpty())) {
			continue;
		}
		line.truncate(line.length()-1);
		QString *name = new QString (line);
		mList.append (name->ascii());
		if (mFile->atEnd()) {
			break;
		}
	}
}

//=========================================
// return file list
//-----------------------------------------
QList<char> Parser::folderList (void) {
	return mList;
}
