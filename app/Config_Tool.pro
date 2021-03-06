
QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(../VERSION.txt)

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += GIT_CURRENT_SHA1="\\\"$(shell git -C \""$$_PRO_FILE_PWD_"\" rev-parse HEAD)\\\""
DEFINES += GIT_CURRENT_BRANCH="\\\"$(shell git -C \""$$_PRO_FILE_PWD_"\" branch | sed -n 's/^[*][ ]//p')\\\""

QMAKE_CXX = g++-7

CONFIG += c++1z
QMAKE_CXXFLAGS += -std=c++17 -pedantic -Wold-style-cast -Wall

TARGET = Navigator2Go
TEMPLATE = app

QMAKE_CXXFLAGS_DEBUG += -Og # The OG optimized debug ;)

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 -g -DNDEBUG -Woverloaded-virtual -Wredundant-decls -Winit-self -Wextra -pedantic-errors

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

FORMS += $$PWD/forms/*.ui

unix:!macx: LIBS += -lnetcdf_c++4 -lbfd -ldl

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
    QEasyDownloader.hpp \
    switchwidget.h \
    widgetconfigeditor.h \
    widgetdataorder.h \
    widgetthreddsconfig.h \
    nodiscard.h \
    constants.h \
    xmlio.h \
    dialogselectvars.h \
    dialogimportnc.h \
    netcdfimportdesc.h \
    dialogthreddslogs.h \
    widgetmonthpicker.h \
    updaterunnable.h \
    systemutils.h \
    backward/backward.hpp \
    servermanager.h \
    widgetlocaldata.h

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
    QEasyDownloader.cc \
    switchwidget.cpp \
    widgetconfigeditor.cpp \
    widgetdataorder.cpp \
    widgetthreddsconfig.cpp \
    $$PWD/pugixml/pugixml.cpp \
    xmlio.cpp \
    dialogselectvars.cpp \
    dialogimportnc.cpp \
    dialogthreddslogs.cpp \
    widgetmonthpicker.cpp \
    updaterunnable.cpp \
    systemutils.cpp \
    backward/backward.cpp \
    preferences.cpp \
    servermanager.cpp \
    widgetlocaldata.cpp

INCLUDEPATH += \
    $$PWD/netcdf4/include \
    $$PWD/pugixml/

DISTFILES +=
