#-------------------------------------------------
#
# Project created by QtCreator 2011-06-06T14:39:46
#
#-------------------------------------------------

QT       += core gui

TARGET = qepicspvapp
TEMPLATE = app

LIBS += -L../qepicspv -lqepicspv \
        -L../qepicspvgui -lqepicspvgui

INCLUDEPATH += ../qepicspv ../qepicspvgui

SOURCES +=\
        qepicspvapp.cpp \
    qepicspvapp_main.cpp

HEADERS  += qepicspvapp.h

FORMS    += qepicspvapp.ui
