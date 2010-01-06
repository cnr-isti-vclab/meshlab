include (../../shared.pri)
DEFINES += dSINGLE
HEADERS = filter_physics.h \
    ODEFacade.h \
    PhysicsEngineFacade.h \
    MeshSubFilter.h \
    GravitySubFilter.h \
    RandomDropFilter.h \
    RandomFillFilter.h \
    DynamicMeshSubFilter.h
SOURCES = filter_physics.cpp \
    ODEFacade.cpp \
    GravitySubFilter.cpp \
    RandomDropFilter.cpp \
    RandomFillFilter.cpp \
    DynamicMeshSubFilter.cpp
TARGET = filter_physics
CONFIG += x86
LIBS += -L/usr/local/lib \
    -lode
