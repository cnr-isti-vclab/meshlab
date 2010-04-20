include (../../shared.pri)

HEADERS       += $$VCGDIR/vcg/complex/trimesh/clean.h\
                 meshselect.h
SOURCES       += meshselect.cpp

TARGET        = filter_select

RESOURCES     = meshlab.qrc
