
QT += widgets gui

TARGET = qtpvwidgets
TEMPLATE = lib
HEADERS += qtpvwidgets.h
SOURCES += qtpvwidgets.cpp

headers.files = $$HEADERS
headers.path = $$[INSTALLBASE]/include
INSTALLS += headers

target.files = $$[TARGET]
target.path = $$[INSTALLBASE]/lib
INSTALLS += target
