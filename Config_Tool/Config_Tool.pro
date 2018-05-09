#-------------------------------------------------
#
# Project created by QtCreator 2018-05-02T11:50:48
#
#-------------------------------------------------

QT       += core gui

CONFIG += c++1z

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Config_Tool
TEMPLATE = app

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
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RESOURCES += \
    resources.qrc \
    qdarkstyle/style.qrc

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dialogdatasetview.cpp \
    htmlhighlighter.cpp

HEADERS += \
        mainwindow.h \
    dialogdatasetview.h \
    htmlhighlighter.h

FORMS += \
        mainwindow.ui \
    datasetview.ui
