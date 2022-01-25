#-------------------------------------------------
#
# Project created by QtCreator 2019-12-04T22:08:28
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileSafeBox
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
		MyWidget.cpp

HEADERS  += mainwindow.h\
			MyWidget.h

FORMS    += mainwindow.ui
