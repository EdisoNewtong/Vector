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
    luaSrc/lapi.c \
    luaSrc/lauxlib.c \
    luaSrc/lbaselib.c \
    luaSrc/lbitlib.c \
    luaSrc/lcode.c \
    luaSrc/lcorolib.c \
    luaSrc/lctype.c \
    luaSrc/ldblib.c \
    luaSrc/ldebug.c \
    luaSrc/ldo.c \
    luaSrc/ldump.c \
    luaSrc/lfunc.c \
    luaSrc/lgc.c \
    luaSrc/linit.c \
    luaSrc/liolib.c \
    luaSrc/llex.c \
    luaSrc/lmathlib.c \
    luaSrc/lmem.c \
    luaSrc/loadlib.c \
    luaSrc/lobject.c \
    luaSrc/lopcodes.c \
    luaSrc/loslib.c \
    luaSrc/lparser.c \
    luaSrc/lstate.c \
    luaSrc/lstring.c \
    luaSrc/lstrlib.c \
    luaSrc/ltable.c \
    luaSrc/ltablib.c \
    luaSrc/ltm.c \
#    luaSrc/lua.c \
#    luaSrc/luac.c \
    luaSrc/lundump.c \
    luaSrc/lutf8lib.c \
    luaSrc/lvm.c \
    luaSrc/lzio.c

HEADERS += \
    mainwindow.h \
    luaHeader/lapi.h \
    luaHeader/lauxlib.h \
    luaHeader/lcode.h \
    luaHeader/lctype.h \
    luaHeader/ldebug.h \
    luaHeader/ldo.h \
    luaHeader/lfunc.h \
    luaHeader/lgc.h \
    luaHeader/llex.h \
    luaHeader/llimits.h \
    luaHeader/lmem.h \
    luaHeader/lobject.h \
    luaHeader/lopcodes.h \
    luaHeader/lparser.h \
    luaHeader/lprefix.h \
    luaHeader/lstate.h \
    luaHeader/lstring.h \
    luaHeader/ltable.h \
    luaHeader/ltm.h \
    luaHeader/lua.h \
    luaHeader/luaconf.h \
    luaHeader/lualib.h \
    luaHeader/lundump.h \
    luaHeader/lvm.h \
    luaHeader/lzio.h \
    luaHeader/lua.hpp

FORMS += \
    mainwindow.ui

# DEFINES += LUA_USE_WINDOWS

INCLUDEPATH += luaHeader

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
