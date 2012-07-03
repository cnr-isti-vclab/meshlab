include (../../shared.pri)

HEADERS       += filter_texture.h \
		pushpull.h \
		rastering.h \
		$$VCGDIR/vcg/complex/algorithms/parametrization/voronoi_atlas.h
SOURCES       += filter_texture.cpp  \
		$$VCGDIR/wrap/ply/plylib.cpp \

TARGET        = filter_texture

win32-msvc2005:DEFINES	+= _USE_MATH_DEFINES
win32-msvc2008:DEFINES	+= _USE_MATH_DEFINES
CONFIG +=  stl
