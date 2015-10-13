######################
# common stuff for all components
######################

TEMPLATE = lib

CONFIG += static \
          warn_on \
          qt \
          thread \
          rtti

QT -= gui
QT += xml \
      network \
      xmlpatterns \
      core


CONFIG += debug_and_release
#CONFIG += build_all

INCLUDEPATH += ../.. \
               ../../../../../
#INCLUDEPATH += ../../../../../Platform/Master/Components/
#               ../../../../../Colorado/Shared/Master/Components/
DEPENDPATH += ../..
#              ../../../../../Platform/Master/Components/
#              ../../../../../Colorado/Shared/Master/Components/

QMAKE_CXXFLAGS += -Wall -std=c++0x
#linux-*:exists(/usr/bin/ccache):QMAKE_CXX=ccache $$QMAKE_CXX

CONFIG(debug, debug|release) {
    CONFIG_SUFFIX = dbg
} else {
    CONFIG_SUFFIX = rel
    DEFINES += QT_NO_DEBUG \
               QT_NO_DEBUG_OUTPUT \
                           DBT_TRACE_DISCARD \
               NDEBUG
}


OBJECTS_DIR = obj_$${CONFIG_SUFFIX}
MOC_DIR = moc_$${CONFIG_SUFFIX}
DESTDIR = lib_$${CONFIG_SUFFIX}
