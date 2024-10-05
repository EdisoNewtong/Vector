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
    SuperTextEdit.cpp \
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
    core/token/expEvaluation.cpp \
    ##################################################
    core/util/dataTypeUtil.cpp \
    core/util/enumUtil.cpp \
    core/util/keywordList.cpp \
    core/util/opUtil.cpp \
    core/util/stringNumber.cpp \
    ##################################################
    core/variblePool/variblePool.cpp \
    ##################################################
    core/functionPool/functionBase.cpp \
    core/functionPool/functionMgr.cpp \
    core/functionPool/basic/MyAbs.cpp \
    core/functionPool/basic/MyFmod.cpp \
    core/functionPool/basic/MyFmax.cpp \
    core/functionPool/basic/MyFmin.cpp \
    core/functionPool/explog/MyExp.cpp \
    core/functionPool/explog/MyExp2.cpp \
    core/functionPool/explog/MyLog.cpp \
    core/functionPool/explog/MyLog2.cpp \
    core/functionPool/explog/MyLog10.cpp \
    core/functionPool/powSqrt/MyPow.cpp \
    core/functionPool/powSqrt/MySqrt.cpp \
    core/functionPool/powSqrt/MyCbrt.cpp \
    core/functionPool/powSqrt/MyHypot.cpp \
    core/functionPool/trigonometric/MyCos.cpp \
    core/functionPool/trigonometric/MySin.cpp \
    core/functionPool/trigonometric/MyTan.cpp \
    core/functionPool/trigonometric/MyAsin.cpp \
    core/functionPool/trigonometric/MyAcos.cpp \
    core/functionPool/trigonometric/MyAtan.cpp \
    core/functionPool/trigonometric/MyAtan2.cpp \
    core/functionPool/nearest/MyCeil.cpp \
    core/functionPool/nearest/MyFloor.cpp \
    core/functionPool/nearest/MyTrunc.cpp \
    core/functionPool/nearest/MyRound.cpp


HEADERS += \
    mainwindow.h \
    SuperTextEdit.h \
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
    core/token/expEvaluation.h \
    ##################################################
    core/util/charUtil.h \
    core/util/dataTypeUtil.h \
    core/util/enumUtil.h \
    core/util/keywordList.h \
    core/util/opUtil.h \
    core/util/stringNumber.h \
    ##################################################
    core/variblePool/variblePool.h \
    ##################################################
    core/functionPool/functionBase.h \
    core/functionPool/functionMgr.h \
    core/functionPool/basic/MyAbs.h \
    core/functionPool/basic/MyFmod.h \
    core/functionPool/basic/MyFmax.h \
    core/functionPool/basic/MyFmin.h \
    core/functionPool/explog/MyExp.h \
    core/functionPool/explog/MyExp2.h \
    core/functionPool/explog/MyLog.h \
    core/functionPool/explog/MyLog2.h \
    core/functionPool/explog/MyLog10.h \
    core/functionPool/powSqrt/MyPow.h \
    core/functionPool/powSqrt/MySqrt.h \
    core/functionPool/powSqrt/MyCbrt.h \
    core/functionPool/powSqrt/MyHypot.h \
    core/functionPool/trigonometric/MyCos.h \
    core/functionPool/trigonometric/MySin.h \
    core/functionPool/trigonometric/MyTan.h \
    core/functionPool/trigonometric/MyAsin.h \
    core/functionPool/trigonometric/MyAcos.h \
    core/functionPool/trigonometric/MyAtan.h \
    core/functionPool/trigonometric/MyAtan2.h \
    core/functionPool/nearest/MyCeil.h \
    core/functionPool/nearest/MyFloor.h \
    core/functionPool/nearest/MyTrunc.h \
    core/functionPool/nearest/MyRound.h

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
    core/variblePool \
    ##################################################
    core/functionPool \
    core/functionPool/basic \
    core/functionPool/explog \
    core/functionPool/powSqrt \
    core/functionPool/trigonometric \
    core/functionPool/nearest

FORMS += \
    mainwindow.ui

RC_ICONS = Code-Execute.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
