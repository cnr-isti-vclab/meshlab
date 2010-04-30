include (../../shared.pri)

HEADERS       += filter_plymc.h \
                 volume.h\
                 voxel.h \
                 plymc.h \
                 simplemeshprovider.h \
tri_edge_collapse_mc.h \
 $$VCGDIR/vcg/complex/local_optimization/tri_edge_collapse.h


SOURCES       += filter_plymc.cpp \
../../../../vcglib/wrap/ply/plylib.cpp 

TARGET        = filter_plymc
QT += opengl
