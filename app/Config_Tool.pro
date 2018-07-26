
QT += core gui network charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += GIT_CURRENT_SHA1="\\\"$(shell git -C \""$$_PRO_FILE_PWD_"\" rev-parse HEAD)\\\""
DEFINES += GIT_CURRENT_BRANCH="\\\"$(shell git -C \""$$_PRO_FILE_PWD_"\" branch | sed -n 's/^[*][ ]//p')\\\""

QMAKE_CXX = g++-7

CONFIG += c++1z
QMAKE_CXXFLAGS += -std=c++17

TARGET = Config_Tool
TEMPLATE = app

QMAKE_CXXFLAGS_DEBUG += -Og # The OG optimized debug ;)

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 -DNDEBUG -msse2

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
    widgetthreddsconfig.ui \
    dialogselectvars.ui \
    dialogimportnc.ui \
    dialogthreddslogs.ui \
    widgetmonthpicker.ui

unix:!macx: LIBS += -lnetcdf_c++4

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
    constants.h \
    xmlio.h \
    dialogselectvars.h \
    dialogimportnc.h \
    netcdfimportdesc.h \
    dialogthreddslogs.h \
    widgetmonthpicker.h

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
    $$PWD/pugixml/pugixml.cpp \
    xmlio.cpp \
    dialogselectvars.cpp \
    dialogimportnc.cpp \
    dialogthreddslogs.cpp \
    widgetmonthpicker.cpp

INCLUDEPATH += \
    $$PWD/netcdf4/include \
    $$PWD/pugixml/

DISTFILES +=
