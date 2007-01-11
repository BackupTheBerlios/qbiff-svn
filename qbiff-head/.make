#!/bin/sh
# Copyright (c) 2001 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <ms@suse.de>, 2001
#
# CVS ID:
# --------
# Status: Up-to-date
#
export QMAKESPEC=$QTDIR/mkspecs/linux-g++/
$QTDIR/bin/qmake -makefile -unix -o Makefile qbiff.pro
