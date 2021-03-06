#-------------------------------------------------
#
# Project created by QtCreator 2010-12-31T14:53:55
#
#-------------------------------------------------

# TURN OFF SHADOW building!

CONFIG += qt
QT       += core gui network opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qimagegrabber_test
TEMPLATE = app

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR = build

DESTDIR = bin

LIBS += -L../../qimagegrabber/bin -lqimagegrabber
INCLUDEPATH += ../../qimagegrabber/

SOURCES += main.cpp\
        mainwindow.cpp \
    imageview.cpp \
    imageitem.cpp

HEADERS  += mainwindow.h \
    imageview.h \
    imageitem.h

FORMS    += mainwindow.ui
