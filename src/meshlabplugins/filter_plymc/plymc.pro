TARGET = plymc
LIBPATH += 
DEPENDPATH += .
INCLUDEPATH += ../../../../vcglib \
    ../../code/lib/glew/include
CONFIG += console \
    stl \
    debug_and_release

# Mac specific Config required to avoid to make application bundles
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += ../../../../vcglib/wrap/ply/plylib.cpp \
    plymc_main.cpp
HEADERS = plymc.h \
    svoxel.h \
    voxel.h \
    volume.h \
    trivial_walker.h \
    tri_edge_collapse_mc.h \
    simplemeshprovider.h
