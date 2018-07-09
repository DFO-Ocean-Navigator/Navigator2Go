
QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXX = g++-7

CONFIG += c++1z
QMAKE_CXXFLAGS += -std=c++17

TARGET = Config_Tool
TEMPLATE = app

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

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
    dialogpreferences.ui \
    widgetdashboard.ui \
    dialogdatasetview.ui \
    widgetconfigeditor.ui \
    widgetdataorder.ui \
    widgetthreddsconfig.ui

unix:!macx: LIBS += -L$$PWD/netcdf4/lib/ -lnetcdf_c++4
unix:!macx: PRE_TARGETDEPS += $$PWD/netcdf4/lib/libnetcdf_c++4.a

DEPENDPATH += \
    $$PWD/netcdf4/include \
    $$PWD/pugixml/

HEADERS += \
    datadownloaddesc.h \
    dialogdatasetview.h \
    dialogpreferences.h \
    htmlhighlighter.h \
    ioutils.h \
    jsonio.h \
    mainwindow.h \
    network.h \
    preferences.h \
    process.h \
    QEasyDownloader.hpp \
    switchwidget.h \
    widgetdashboard.h \
    widgetconfigeditor.h \
    widgetdataorder.h \
    widgetthreddsconfig.h \
    nodiscard.h \
    constants.h

SOURCES += \
    datadownloaddesc.cpp \
    dialogdatasetview.cpp \
    dialogpreferences.cpp \
    htmlhighlighter.cpp \
    ioutils.cpp \
    jsonio.cpp \
    main.cpp \
    mainwindow.cpp \
    network.cpp \
    process.cpp \
    QEasyDownloader.cc \
    switchwidget.cpp \
    widgetdashboard.cpp \
    widgetconfigeditor.cpp \
    widgetdataorder.cpp \
    widgetthreddsconfig.cpp \
    $$PWD/pugixml/pugixml.cpp

INCLUDEPATH += \
    $$PWD \
    $$PWD/netcdf4/include \
    $$PWD/pugixml/

DISTFILES +=



