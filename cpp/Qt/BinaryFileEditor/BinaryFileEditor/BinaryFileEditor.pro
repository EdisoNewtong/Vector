#-------------------------------------------------
#
# Project created by QtCreator 2021-01-21T15:35:29
#
#-------------------------------------------------

QT       += core gui

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BinaryFileEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        myhexeditor.cpp

HEADERS  += mainwindow.h \
            myhexeditor.h

FORMS    += mainwindow.ui

RC_ICONS = BinaryFileEditor.ico