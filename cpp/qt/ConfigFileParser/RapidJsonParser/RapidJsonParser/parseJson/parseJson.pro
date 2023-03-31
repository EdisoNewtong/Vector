
TEMPLATE = app
QT       += core gui
CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

macx {
        QMAKE_CXXFLAGS += -Wno-inconsistent-missing-override
}


TARGET = parseJson
SOURCES += main.cpp \
    mainwindow.cpp \
    JSonModel/JsonStandardItem.cpp \
    SuperTextEdit.cpp



INCLUDEPATH += ../rapidjson/include \
               JsonModel

DEFINES += EDISON_LINE_INFO_TRACE
LIBS += -L../rapidjson -lrapidjson

win32:debug {
	LIBS += -L../rapidjson/debug
}

win32:release {
        LIBS += -L../rapidjson/release
}

RESOURCES += parsejson.qrc

FORMS += \
    mainwindow.ui

HEADERS += \
    mainwindow.h \
    JSonModel/JsonStandardItem.h \
    SuperTextEdit.h

RC_ICONS = RapidJSON.ico




