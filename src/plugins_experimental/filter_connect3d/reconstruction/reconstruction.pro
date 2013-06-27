include (../global.pri)

CONFIG += precompile_header
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ..
DEPENDPATH += ..


PRECOMPILED_HEADER = precompiled.h

HEADERS += "c3dConstructBC.h"
HEADERS += "c3dConstructDG.h"
HEADERS += "c3dHoleTreatment.h"
HEADERS += "c3dInflate.h"
HEADERS += "c3dMakeConformBC.h"
HEADERS += "c3dMath.h"
HEADERS += "c3dMeshFairing.h"
HEADERS += "c3dSculpture.h"
HEADERS += "c3dSegmentBC.h"
HEADERS += "c3dShared.h"
HEADERS += "connect3D.h"
HEADERS += "c3dDataStructures.h"
HEADERS += "precompiled.h"

SOURCES += "c3dConstructBC.cpp"
SOURCES += "c3dConstructDG.cpp"
SOURCES += "c3dDataStructures.cpp"
SOURCES += "c3dHoleTreatment.cpp"
SOURCES += "c3dInflate.cpp"
SOURCES += "c3dMakeConformBC.cpp"
SOURCES += "c3dMath.cpp"
SOURCES += "c3dMeshFairing.cpp"
SOURCES += "c3dSculpture.cpp"
SOURCES += "c3dSegmentBC.cpp"
SOURCES += "c3dShared.cpp"
SOURCES += "connect3D.cpp"
SOURCES += "../ThirdParty/DisjointSets/DisjointSets.cpp"


TARGET = connect3d


win32 {
	win32:INCLUDEPATH += ../../CGAL-4.1/include
	win32:INCLUDEPATH += ../../boost_1_53_0
	win32:INCLUDEPATH += ../../CGAL-4.1/auxiliary/gmp/include
	win32:INCLUDEPATH += ../../CGAL-4.1/auxiliary/gmp/include
	win32:INCLUDEPATH += ../../CGAL-4.1/build/include
}


INCLUDEPATH += ../ThirdParty/cpp-btree-1.0.1
INCLUDEPATH += ../ThirdParty/DisjointSets
INCLUDEPATH += ../ThirdParty/Vector3D


unix {
	LIBS += -L/usr/local/lib -lCGAL_Core
	LIBS += -L/usr/local/lib -lCGAL
	LIBS += -L/usr/local/lib -lgmp
}

win32:LIBPATH += ../../boost_1_53_0/stage/lib
win32:LIBPATH += ../../CGAL-4.1/auxiliary/gmp/lib


CONFIG(debug, debug|release) {
	win32:LIBPATH += ../../CGAL-4.1/build/lib/Debug
	win32:LIBS += -LCGAL_Core-vc110-mt-gd-4.1
	win32:LIBS += -LCGAL-vc110-mt-gd-4.1
	win32:LIBS += -L../../CGAL-4.1/auxiliary/gmp/lib -llibgmp-10 
	win32:LIBS += -Llibmpfr-4
} else {
	win32:LIBPATH += ../../CGAL-4.1/build/lib/Release
	win32:LIBS += -LCGAL-vc110-mt-4.1 
	win32:LIBS += -LCGAL_Core-vc110-mt-4.1 
	win32:LIBS += -L../../CGAL-4.1/auxiliary/gmp/lib -llibgmp-10 
	win32:LIBS += -Llibmpfr-4 
}




