# -------------------------------------------------
# Project created by QtCreator 2010-03-19T14:22:14
# -------------------------------------------------

QT -= gui

TARGET = qepicspv

TEMPLATE = lib

DEFINES += qepicspv_LIBRARY

SOURCES += qepicspv.cpp \
    src/CaConnection.cpp \
    src/CaObject.cpp \
    src/CaRecord.cpp \
    src/Generic.cpp \
    src/QCaAlarmInfo.cpp \
    src/QCaConnectionInfo.cpp \
    src/QCaDateTime.cpp \
    src/QCaEventFilter.cpp \
    src/QCaEventUpdate.cpp \
    src/QCaObject.cpp \
    src/QCaStateMachine.cpp \
    src/UserMessage.cpp


INCLUDEPATH += . src \
    ${EPICS_BASE}/include \
    ${EPICS_BASE}/include/os/Linux

HEADERS += qepicspv.h \
    src/CaConnection.h \
    src/CaObject.h \
    src/CaObjectPrivate.h \
    src/CaRecord.h \
    src/Generic.h \
    src/QCaAlarmInfo.h \
    src/QCaConnectionInfo.h \
    src/QCaDateTime.h \
    src/QCaEventFilter.h \
    src/QCaEventUpdate.h \
    src/QCaObject.h \
    src/QCaStateMachine.h \
    src/UserMessage.h \
    src/QCaPluginLibrary_global.h


LIBS += -L${EPICS_BASE}/lib/${EPICS_HOST_ARCH} \
    -lCom \
    -lca


instheaders.files = qepicspv.h
instheaders.path = $$[INSTALLBASE]/include
INSTALLS += instheaders

target.files = $$[TARGET]
target.path = $$[INSTALLBASE]/lib
INSTALLS += target
