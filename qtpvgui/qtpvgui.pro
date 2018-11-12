TARGET = qtpvgui
TEMPLATE = lib

QT += gui core widgets

FORMS += qtpvgui.ui

HEADERS += qtpvgui.h

SOURCES += qtpvgui.cpp

INCLUDEPATH += ../qtpv ../qtpvwidgets
LIBS += -L../qtpv -lqtpv \
    -L../qtpvwidgets -lqtpvwidgets

headers.files = $$HEADERS
headers.path = $$[INSTALLBASE]/include
INSTALLS += headers

target.files = $$[TARGET]
target.path = $$[INSTALLBASE]/lib
INSTALLS += target
