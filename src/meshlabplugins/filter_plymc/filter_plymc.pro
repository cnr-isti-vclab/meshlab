include (../../shared.pri)

HEADERS       += filter_plymc.h \
                 volume.h\
                 voxel.h \
                 plymc.h \
                 simplemeshprovider.h

SOURCES       += filter_plymc.cpp \
../../../../vcglib/wrap/ply/plylib.cpp 

TARGET        = filter_plymc
QT += opengl
