TARGET = qepicspvgui
TEMPLATE = lib
INCLUDEPATH += ../qepicspv
LIBS += -L../qepicspv -lqepicspv

HEADERS += \
    qepicspvgui.h \
    qepicspv_guielements.h

SOURCES += \
    qepicspvgui.cpp \
    qepicspv_guielements.cpp

FORMS += \
    qepicspvgui.ui

headers.files = qepicspvgui.h \
    qepicspv_guielements.h \
    ui_qepicspvgui.h
headers.path = $$[INSTALLBASE]/include
INSTALLS += headers

target.files = $$[TARGET]
target.path = $$[INSTALLBASE]/lib
INSTALLS += target
