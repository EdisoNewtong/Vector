QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

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
    ##################################################
    core/buff.cpp \
    core/cmdOptions.cpp \
    core/dataValue.cpp \
    core/globalDirector.cpp \
    core/myException.cpp \
    ##################################################
    core/parser/parserBase.cpp \
    core/parser/blank/blankParser.cpp \
    core/parser/comment/singlelineCommentParser.cpp \
    core/parser/comment/multilineCommentParser.cpp \
    core/parser/operator/operatorParser.cpp \
    core/parser/semicolon/semicolonParser.cpp \
    core/parser/sequence/sequenceParser.cpp \
    ##################################################
    core/token/tokenBase.cpp \
    core/token/tokenMgr.cpp \
    ##################################################
    core/util/dataTypeUtil.cpp \
    core/util/enumUtil.cpp \
    core/util/keywordList.cpp \
    core/util/opUtil.cpp \
    core/util/stringNumber.cpp \
    ##################################################
    core/variblePool/variblePool.cpp


HEADERS += \
    mainwindow.h \
    ##################################################
    core/buff.h \
    core/cmdOptions.h \
    core/commonEnum.h \
    core/dataValue.h \
    core/globalDirector.h \
    core/myException.h \
    ##################################################
    core/parser/parserBase.h \
    core/parser/blank/blankParser.h \
    core/parser/comment/singlelineCommentParser.h \
    core/parser/comment/multilineCommentParser.h \
    core/parser/operator/operatorParser.h \
    core/parser/semicolon/semicolonParser.h \
    core/parser/sequence/sequenceParser.h \
    ##################################################
    core/token/tokenBase.h \
    core/token/tokenMgr.h \
    ##################################################
    core/util/charUtil.h \
    core/util/dataTypeUtil.h \
    core/util/enumUtil.h \
    core/util/keywordList.h \
    core/util/opUtil.h \
    core/util/stringNumber.h \
    ##################################################
    core/variblePool/variblePool.h

INCLUDEPATH += \
    core \
    ##################################################
    core/parser \
    core/parser/blank \
    core/parser/comment \
    core/parser/operator \
    core/parser/semicolon \
    core/parser/sequence \
    ##################################################
    core/token \
    ##################################################
    core/util \
    ##################################################
    core/variblePool

FORMS += \
    mainwindow.ui

RC_ICONS = Code-Execute.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
