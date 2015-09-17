#-------------------------------------------------
#
# Project created by QtCreator 2015-07-24T10:23:26
#
#-------------------------------------------------
CONFIG += thread

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LogViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        usertablemodel.cpp \
    startup.cpp \
    sensordataitf.cpp \
    legend.cpp \
    logparser.cpp \
    lpkgreader.cpp


HEADERS  += mainwindow.h \
    usertablemodel.h \
    startup.h \
    sensordataitf.h \
    legend.h \
    globaldefines.h \
    logparser.h \
    lpkgreader.h

FORMS    += mainwindow.ui \
    lpkgreader.ui

INCLUDEPATH += C:\Qwt-6.1.2\include
LIBS += -L"C:\Qwt-6.1.2\Lib" -lqwt
