#-------------------------------------------------
#
# Project created by QtCreator 2011-06-06T14:39:46
#
#-------------------------------------------------

QT       += core gui

TARGET = ptpv
TEMPLATE = app

LIBS += -L../qtpv -lqtpv \
        -L../qtpvgui -lqtpvgui \
	-L../qtpvwidgets -lqtpvwidgets

INCLUDEPATH += ../qtpv ../qtpvwidgets ../qtpvgui

SOURCES +=\
        qpvcontrol.cpp \
    qpvcontrol_main.cpp

HEADERS  += qpvcontrol.h

FORMS    += qpvcontrol.ui

target.files = $$[TARGET]
target.path = $$[INSTALLBASE]/bin
INSTALLS += target

RESOURCES += \
    qtpvcontrol.qrc
