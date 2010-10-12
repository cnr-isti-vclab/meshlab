include (../../shared.pri)
HEADERS += filter_csg.h \
    intercept.h \
    gmpfrac.h

SOURCES += filter_csg.cpp
TARGET = filter_csg
TEMPLATE = lib
QT += opengl
CONFIG += plugin

QMAKE_INCDIR += gmp-5.0.1/

#QMAKE_LIBS += -lgmpxx -lgmp
#QMAKE_LIBDIR += gmp-5.0.1/.libs/

OBJECTS += gmp-5.0.1/.libs/libgmpxx.a gmp-5.0.1/.libs/libgmp.a
