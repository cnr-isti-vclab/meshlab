include (../../sharedfilter.pri)
DEFINES += dSINGLE
HEADERS = filter_physics.h \
    PhysicsEngineFacade.h \
    ODEFacade.h
SOURCES =  filter_physics.cpp \
    ODEFacade.cpp
TARGET = filter_physics
TEMPLATE = lib
QT += opengl
CONFIG += plugin x86
LIBS += -L/usr/local/lib -lode
