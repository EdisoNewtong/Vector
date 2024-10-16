QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

macx {
    QMAKE_CXXFLAGS += -Wno-inconsistent-missing-override
}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    MVC/TreeNode.cpp \
    MVC/TreeView.cpp \
    MVC/TreeModel.cpp \
    SuperTextEdit.cpp \
    renderObject/myRectWithTextItem.cpp \
    renderObject/renderCfg.cpp

HEADERS += \
    mainwindow.h \
    MVC/TreeNode.h \
    MVC/TreeView.h \
    MVC/TreeModel.h \
    SuperTextEdit.h \
    renderObject/myRectWithTextItem.h \
    renderObject/renderCfg.h


FORMS += \
    mainwindow.ui


INCLUDEPATH += MVC \
               ../parser/rapidxml \
               ../../parser/rapidxml \
               ../parser/rapidjson \
               ../parser/lua/src


LIBS += -L../parser/rapidxml -lrapidxml \
        -L../parser/rapidjson -lrapidjson \
        -L../parser/lua       -llua

win32:debug {
    LIBS += -L../parser/rapidxml/debug \
            -L../parser/rapidjson/debug \
            -L../parser/lua/debug
}

win32:release {
    LIBS += -L../parser/rapidxml/release \
            -L../parser/rapidjson/release \
            -L../parser/lua/release
}

RC_ICONS = NormalTree.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
