TEMPLATE  += app
SOURCES   += qbiff.cpp
SOURCES   += sslcommon.cpp
SOURCES   += sslserver.cpp
SOURCES   += sslinit.cpp
SOURCES   += sslconnection.cpp
SOURCES   += sslclient.cpp
SOURCES   += serverhandler.cpp
SOURCES   += clientfolder.cpp
SOURCES   += clientinfo.cpp
SOURCES   += button.cpp
SOURCES   += parser.cpp
SOURCES   += notify.cpp
HEADERS   += sslcommon.h
HEADERS   += sslserver.h
HEADERS   += sslinit.h
HEADERS   += sslconnection.h
HEADERS   += sslclient.h
HEADERS   += serverhandler.h
HEADERS   += clientfolder.h
HEADERS   += clientinfo.h
HEADERS   += button.h
HEADERS   += parser.h
HEADERS   += notify.h

CONFIG    += qt warn_on release
unix:LIBS += -lssl -lcrypto -lXtst
TARGET    += qbiff

RPM_OPT_FLAGS ?= -O2

QMAKE_CXXFLAGS = $(RPM_OPT_FLAGS) -fno-strict-aliasing
unix:INCLUDEPATH += /usr/X11R6/include
unix:INCLUDEPATH += /usr/include/X11
unix:INCLUDEPATH += /usr/include/xorg
