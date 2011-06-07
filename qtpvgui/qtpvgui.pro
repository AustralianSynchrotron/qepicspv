TARGET = qtpvgui
TEMPLATE = lib
INCLUDEPATH += ../qtpv
LIBS += -L../qtpv -lqtpv

HEADERS += \
    qtpvgui.h \
    qtpv_guielements.h

SOURCES += \
    qtpvgui.cpp \
    qtpv_guielements.cpp

FORMS += \
    qtpvgui.ui

headers.files = qtpvgui.h \
    qtpv_guielements.h \
    ui_qtpvgui.h
headers.path = $$[INSTALLBASE]/include
INSTALLS += headers

target.files = $$[TARGET]
target.path = $$[INSTALLBASE]/lib
INSTALLS += target
