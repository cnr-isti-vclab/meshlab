include (../../shared.pri)
DEFINES += dSINGLE
HEADERS = filter_physics.h \
    ODEFacade.h \
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
    DynamicMeshSubFilter.cpp \
    MeshSubFilter.cpp
TARGET = filter_physics
CONFIG += x86
mac:LIBS += -L../../external/lib/macx -lode

INCLUDEPATH  *= ../../external/ode-0.11.1/include
