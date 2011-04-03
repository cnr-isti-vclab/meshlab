include (../../shared.pri)

HEADERS       += $$VCGDIR/vcg/complex/algorithms/clean.h\
                 meshselect.h
SOURCES       += meshselect.cpp

TARGET        = filter_select

RESOURCES     = meshlab.qrc
