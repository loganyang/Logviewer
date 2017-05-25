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
QMAKE_CXXFLAGS += -Wall -std=c++0x
CONFIG += debug_and_release

CONFIG(debug, debug|release){
        CONFIG_SUFFIX = dbg
        LIBS += -L"C:\Qwt-6.1.2\Lib" -lqwtd
} else{
    CONFIG_SUFFIX = rel
    LIBS += -L"C:\Qwt-6.1.2\Lib" -lqwt
    DEFINES += QT_NO_DEBUG \
               QT_NO_DEBUG_OUTPUT \
                           DBT_TRACE_DISCARD \
               NDEBUG
}

DESTDIR = bin_$$CONFIG_SUFFIX
OBJECTS_DIR = obj_$${CONFIG_SUFFIX}
MOC_DIR = moc_$${CONFIG_SUFFIX}


SOURCES +=\
    ../Source/main.cpp\
    ../Source/mainwindow.cpp \
    ../Source/usertablemodel.cpp \
    ../Source/startup.cpp \
    ../Source/sensordataitf.cpp \
    ../Source/legend.cpp \
    ../Source/logparser.cpp \
    ../Source/lpkgreader.cpp \
    ../Source/dlgcurvesettings.cpp \
    ../Source/eventlogsortfilter.cpp


HEADERS  += \
    ../Include/mainwindow.h \
    ../Include/usertablemodel.h \
    ../Include/startup.h \
    ../Include/sensordataitf.h \
    ../Include/legend.h \
    ../Include/globaldefines.h \
    ../Include/logparser.h \
    ../Include/lpkgreader.h \
    ../Include/dlgcurvesettings.h \
    ../Include/eventlogsortfilter.h


FORMS    += \
    ../Forms/mainwindow.ui \
    ../Forms/lpkgreader.ui \
    ../Forms/dlgcurvesettings.ui \
    ../Forms/dlgParserResult.ui



################# list used platform libraries #################
PLATFORM_COMPONENTS_DIR = ../../../Components
PLATFORM_COMPONENTS = EncryptionDecryption

INCLUDEPATH += $$PLATFORM_COMPONENTS_DIR
################################################################
################# include platform libraries and set dependencies
for(TheComponent, PLATFORM_COMPONENTS) {
    THELIBPATH       = $$PLATFORM_COMPONENTS_DIR/$${TheComponent}/Build/lib_$$CONFIG_SUFFIX
    PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheComponent}.a
    LIBS            += $$THELIBPATH/lib$${TheComponent}.a
}

INCLUDEPATH += C:\Qwt-6.1.2\include
