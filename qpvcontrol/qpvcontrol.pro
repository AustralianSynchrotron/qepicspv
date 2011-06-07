#-------------------------------------------------
#
# Project created by QtCreator 2011-06-06T14:39:46
#
#-------------------------------------------------

QT       += core gui

TARGET = qpvcontrol
TEMPLATE = app

LIBS += -L../qepicspv -lqepicspv \
        -L../qepicspvgui -lqepicspvgui

INCLUDEPATH += ../qepicspv ../qepicspvgui

SOURCES +=\
        qpvcontrol.cpp \
    qpvcontrol_main.cpp

HEADERS  += qpvcontrol.h

FORMS    += qpvcontrol.ui

target.files = $$[TARGET]
target.path = $$[INSTALLBASE]/bin
INSTALLS += target
