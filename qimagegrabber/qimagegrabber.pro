
#-------------------------------------------------
#
# Project created by QtCreator 2010-02-04T12:05:27
#
#-------------------------------------------------

QT       += network gui

TARGET = qimagegrabber
TEMPLATE = lib
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
DESTDIR = ../../bin
CONFIG     += dll

DEFINES += QIMAGEGRABBER_LIBRARY

SOURCES +=     qimagegrabber.cpp \
	qimagegrabbermjpeg.cpp \
    qimagegrabberv4l2.cpp \
    qimagegrabberhttp.cpp \
    qimagegrabberfile.cpp \
    dialogqimagegrabbersettings.cpp \
    qimagegrabbericamview.cpp

HEADERS +=  qimagegrabber_global.h \
    qimagegrabber.h \
    qimagegrabbermjpeg.h \
    qimagegrabberhttp.h \
    qimagegrabberfile.h \
    dialogqimagegrabbersettings.h \
    qimagegrabbericamview.h

unix {
HEADERS += qimagegrabberv4l2.h
SOURCES += qimagegrabberv4l2.cpp
}

win32 {
#HEADERS += qimagegrabbervfw.h
#SOURCES += qimagegrabbervfw.cpp
#QT += gui
#LIBS += -L../bin/win32 \
#        -lvfw32
}

FORMS += \
    dialogqimagegrabbersettings.ui
