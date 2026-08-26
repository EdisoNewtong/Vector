#-------------------------------------------------
#
# Project created by QtCreator 2019-10-31T11:15:34
#
#-------------------------------------------------

QT       += core gui
CONFIG += C++11
QMAKE_CXXFLAGS += -Wno-inconsistent-missing-override

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DHB
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

##################################################
# Windows Icon   file extension  is  .ico
##################################################
RC_ICONS = DHB.ico




##################################################
# MacOS Icon      file extension is   .icns
#
# Where to download  ???.icons ? 
#   url : https://macosicons.com
#
##################################################
# ICON = DHB.icns



