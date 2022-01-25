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
        MyWidget.cpp \
        my_encrypt.cpp \
        my_decrypt.cpp

HEADERS  += mainwindow.h\
            MyWidget.h \
            my_encrypt.h \
            my_decrypt.h

FORMS    += mainwindow.ui
