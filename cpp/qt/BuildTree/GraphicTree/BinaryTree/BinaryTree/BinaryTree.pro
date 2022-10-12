#-------------------------------------------------
#
# Project created by QtCreator 2020-12-14T19:42:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BinaryTree
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        binarytreeview.cpp \
        mysettingbtndelegate.cpp \
        globalSettings.cpp \
        nodestylesettingdlg.cpp \
        nodestylecfg.cpp \
        binarytreemodel.cpp \
		treenode.cpp

HEADERS += \
        mainwindow.h \
        binarytreeview.h \
        mysettingbtndelegate.h \
        globalSettings.h \
        nodestylesettingdlg.h \
        nodestylecfg.h \
        binarytreemodel.h \
		treenode.h \
        XmlParser/rapidxml.hpp \
        XmlParser/rapidxml_iterators.hpp \
        XmlParser/rapidxml_print.hpp \
        XmlParser/rapidxml_utils.hpp

FORMS += \
        mainwindow.ui \
        nodestylesettingdlg.ui

INCLUDEPATH += XmlParser

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
