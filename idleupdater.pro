TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.c \
    idletime_x11.c


LIBS += -L/usr/lib
LIBS += -lXss -lX11 -lcurl

HEADERS += \
    idletime_x11.h \
    config.h
