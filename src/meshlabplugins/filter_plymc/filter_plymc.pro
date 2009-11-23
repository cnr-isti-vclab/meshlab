include (../../sharedfilter.pri)

HEADERS       = filter_plymc.h \ 
                ../../meshlab/interfaces.h \
                ../../meshlab/meshmodel.h \
volume.h\
voxel.h

SOURCES       = filter_plymc.cpp \
../../../../vcglib/wrap/ply/plylib.cpp \
        ../../meshlab/meshmodel.cpp \
        $$GLEWCODE

TARGET        = filter_plymc
QT += opengl
