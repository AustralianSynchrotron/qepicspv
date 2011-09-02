TARGET = qtpvgui
TEMPLATE = lib
INCLUDEPATH += ../qtpv ../qtpvwidgets
LIBS += -L../qtpv -lqtpv \
    -L../qtpvwidgets -lqtpvwidgets

HEADERS += qtpvgui.h

SOURCES += qtpvgui.cpp

FORMS += qtpvgui.ui

headers.files = $$HEADERS
headers.path = $$[INSTALLBASE]/include
INSTALLS += headers

target.files = $$[TARGET]
target.path = $$[INSTALLBASE]/lib
INSTALLS += target
