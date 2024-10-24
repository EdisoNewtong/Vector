
TEMPLATE = app
QT       += core gui
CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

macx {
        QMAKE_CXXFLAGS += -Wno-inconsistent-missing-override
}

TARGET = XmlDisplay
SOURCES += main.cpp \
           mainwindow.cpp \
           SuperTextEdit.cpp \
           XMLModel/xmlStandardItem.cpp \
           XMLModel/GraphicNode.cpp


INCLUDEPATH += ../XmlParser \
               XMLModel

LIBS += -L../XmlParser -lXmlParser

win32:debug {
    LIBS += -L../XmlParser/debug
}

win32:release {
    LIBS += -L../XmlParser/release
}


FORMS += mainwindow.ui

HEADERS += mainwindow.h \
           SuperTextEdit.h \
           XMLModel/xmlStandardItem.h \
           XMLModel/GraphicNode.h


RESOURCES += xmldisplay.qrc

RC_ICONS = RapidXML.ico
