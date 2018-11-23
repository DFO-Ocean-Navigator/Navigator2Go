#-------------------------------------------------
#
# Project created by QtCreator 2018-06-28T14:45:00
#
#-------------------------------------------------

QT       += core network testlib

QMAKE_CXX = g++-7
QMAKE_CXXFLAGS += -std=c++17 -pedantic

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tst_Navigator2Go
CONFIG   += console c++1z
CONFIG   -= app_bundle

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR = $$PWD/build/

SOURCES += \
    test_ioutils.cpp \
    test_network.cpp \
    main.cpp \
    tdatadownloaddesc.cpp \
    tpreferences.cpp \
    tservermanager.cpp \
    tnetcdfimportdesc.cpp \
    test_systemutils.cpp

HEADERS += \
    test_network.h \
    test_ioutils.h \
    helper_functions.h \
    tdatadownloaddesc.h \
    tpreferences.h \
    tservermanager.h \
    tnetcdfimportdesc.h \
    test_systemutils.h
