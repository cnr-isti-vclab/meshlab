include (../../shared.pri)
DEFINES += dSINGLE
HEADERS = filter_physics.h \
    ODEFacade.h \
    PhysicsEngineFacade.h \
    MeshSubFilter.h \
    GravitySubFilter.h \
    RandomDropFilter.h
SOURCES = filter_physics.cpp \
    ODEFacade.cpp \
    GravitySubFilter.cpp \
    MeshSubFilter.cpp \
    RandomDropFilter.cpp
TARGET = filter_physics
CONFIG += x86
LIBS += -L/usr/local/lib \
    -lode
