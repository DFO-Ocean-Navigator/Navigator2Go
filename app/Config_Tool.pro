
QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++1z

TARGET = Config_Tool
TEMPLATE = app

#QMAKE_CXXFLAGS += -std=c++17
# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -Os # optimize for size

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR = $$PWD/../build

RESOURCES += \
    resources.qrc \
    qdarkstyle/style.qrc

FORMS += \
    mainwindow.ui \
    datasetview.ui \
    dialogpreferences.ui

unix:!macx: LIBS += -L$$PWD/netcdf4/lib/ -lnetcdf_c++4
unix:!macx: PRE_TARGETDEPS += $$PWD/netcdf4/lib/libnetcdf_c++4.a

DEPENDPATH += $$PWD/netcdf4/include

HEADERS += \
   $$PWD/datadownloaddesc.h \
   $$PWD/defines.h \
   $$PWD/dialogdatasetview.h \
   $$PWD/dialogpreferences.h \
   $$PWD/htmlhighlighter.h \
   $$PWD/ioutils.h \
   $$PWD/jsonio.h \
   $$PWD/mainwindow.h \
   $$PWD/network.h \
   $$PWD/preferences.h \
   $$PWD/process.h \
   $$PWD/QEasyDownloader.hpp \
   $$PWD/switchwidget.h

SOURCES += \
   $$PWD/datadownloaddesc.cpp \
   $$PWD/dialogdatasetview.cpp \
   $$PWD/dialogpreferences.cpp \
   $$PWD/htmlhighlighter.cpp \
   $$PWD/ioutils.cpp \
   $$PWD/jsonio.cpp \
   $$PWD/main.cpp \
   $$PWD/mainwindow.cpp \
   $$PWD/network.cpp \
   $$PWD/process.cpp \
   $$PWD/QEasyDownloader.cc \
   $$PWD/switchwidget.cpp

INCLUDEPATH += \
    $$PWD \
    $$PWD/netcdf4/include

DISTFILES +=



