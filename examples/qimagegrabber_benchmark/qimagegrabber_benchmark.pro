#-------------------------------------------------
#
# Project created by QtCreator 2010-12-31T14:53:55
#
#-------------------------------------------------

QT       += core gui network opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qimagegrabber_test
TEMPLATE = app

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR = build

DESTDIR = bin

LIBS += -L../../qimagegrabber/bin -lqimagegrabber -lqwt
INCLUDEPATH += ../../qimagegrabber/

SOURCES += main.cpp\
        mainwindow.cpp \
    dialogfpsgraph.cpp \
    dialogdelaymeasurehelp.cpp \
    dialogcomparesettings.cpp \
    imageview.cpp \
    imageitem.cpp \
    dialogcolor.cpp \
    colordetector.cpp

HEADERS  += mainwindow.h \
    dialogfpsgraph.h \
    dialogdelaymeasurehelp.h \
    dialogcomparesettings.h \
    imageview.h \
    imageitem.h \
    dialogcolor.h \
    colordetector.h

FORMS    += mainwindow.ui \
    dialogfpsgraph.ui \
    dialogdelaymeasurehelp.ui \
    dialogcomparesettings.ui \
    dialogcolor.ui
