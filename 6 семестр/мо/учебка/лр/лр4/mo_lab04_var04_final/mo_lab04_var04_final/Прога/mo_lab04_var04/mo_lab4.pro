TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    csrs.cpp \
    cbss.cpp

HEADERS += \
    csrs.h \
    types.h \
    crand.h \
    cbss.h

QMAKE_LIBS += -lgomp -lpthread
QMAKE_CXXFLAGS += -fopenmp

