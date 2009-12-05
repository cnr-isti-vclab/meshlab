include (../../shared.pri)
DEFINES += dSINGLE
INCLUDEPATH += ../../meshlab
HEADERS = filter_physics.h \
    ODEFacade.h \
    PhysicsEngineFacade.h \
    MeshSubFilter.h \
    GravitySubFilter.h
SOURCES =  filter_physics.cpp \
    ODEFacade.cpp \
    GravitySubFilter.cpp \
    ../../common/filterparameter.cpp
TARGET = filter_physics
TEMPLATE = lib
QT += opengl
CONFIG += plugin \
    x86
LIBS += -L/usr/local/lib \
    -lode
