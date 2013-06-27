include (../../shared.pri)

#CONFIG += precompile_header
#PRECOMPILED_HEADER = reconstruction/precompiled.h


win32 {
	DEFINES += WINVER=0x501
	DEFINES += _WIN32_WINNT=0x501
	DEFINES += __WIN32__
	DEFINES += __x86__
	DEFINES += _CRT_SECURE_NO_WARNINGS 
	DEFINES += _CRT_NONSTDC_NO_WARNINGS 
	DEFINES += _SCL_SECURE_NO_WARNINGS
	DEFINES += WIN32_LEAN_AND_MEAN
	DEFINES += NOMINMAX 
}



# Always create debugging symbols
unix:QMAKE_CXXFLAGS += -g
unix:QMAKE_CXXFLAGS += -std=c++0x
unix:QMAKE_CXXFLAGS += -frounding-math

win32:QMAKE_CXXFLAGS += /MP

win32:QMAKE_CXXFLAGS += /Fd$(TargetDir)\\$(TargetName).pdb

Debug|Win32 {
	win32:QMAKE_CXXFLAGS += /MDd /RTC1 /GS
}

Release|Win32 {
	win32:QMAKE_CXXFLAGS += /MD /Zi /Ot /GS
}

win32:QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:LIBCMTD 
win32:QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:LIBCMT /DEBUG /OPT:REF /OPT:ICF


INCLUDEPATH += ..
DEPENDPATH += ..


HEADERS += "reconstruction/c3dConstructBC.h"
HEADERS += "reconstruction/c3dConstructDG.h"
HEADERS += "reconstruction/c3dHoleTreatment.h"
HEADERS += "reconstruction/c3dInflate.h"
HEADERS += "reconstruction/c3dMakeConformBC.h"
HEADERS += "reconstruction/c3dMath.h"
HEADERS += "reconstruction/c3dMeshFairing.h"
HEADERS += "reconstruction/c3dSculpture.h"
HEADERS += "reconstruction/c3dSegmentBC.h"
HEADERS += "reconstruction/c3dShared.h"
HEADERS += "reconstruction/connect3D.h"
HEADERS += "reconstruction/c3dDataStructures.h"
HEADERS += "reconstruction/precompiled.h"
HEADERS += "filter_connect3d.h"

SOURCES += "reconstruction/c3dConstructBC.cpp"
SOURCES += "reconstruction/c3dConstructDG.cpp"
SOURCES += "reconstruction/c3dDataStructures.cpp"
SOURCES += "reconstruction/c3dHoleTreatment.cpp"
SOURCES += "reconstruction/c3dInflate.cpp"
SOURCES += "reconstruction/c3dMakeConformBC.cpp"
SOURCES += "reconstruction/c3dMath.cpp"
SOURCES += "reconstruction/c3dMeshFairing.cpp"
SOURCES += "reconstruction/c3dSculpture.cpp"
SOURCES += "reconstruction/c3dSegmentBC.cpp"
SOURCES += "reconstruction/c3dShared.cpp"
SOURCES += "reconstruction/connect3D.cpp"
SOURCES += "ThirdParty/DisjointSets/DisjointSets.cpp"
SOURCES += "filter_connect3d.cpp"

TARGET = filter_connect3d


INCLUDEPATH += ThirdParty/cpp-btree-1.0.1
INCLUDEPATH += ThirdParty/Vector3D
INCLUDEPATH += ThirdParty/DisjointSets
INCLUDEPATH += ../../external/CGAL-4.1/include
INCLUDEPATH += ../../external/glew-1.7.0/include
INCLUDEPATH += ../../external/boost_1_53_0


win32 {
	win32:INCLUDEPATH += ../../external/CGAL-4.1/auxiliary/gmp/include
	win32:INCLUDEPATH += ../../external/CGAL-4.1/auxiliary/gmp/include
	win32:INCLUDEPATH += ../../external/CGAL-4.1/build/include
}



unix {
	LIBS += -L/usr/local/lib -lCGAL_Core
	LIBS += -L/usr/local/lib -lCGAL
	LIBS += -L/usr/local/lib -lgmp
}


win32:LIBPATH += ../../external/CGAL-4.1/auxiliary/gmp/lib
win32:LIBPATH += ../../external/boost_1_53_0/stage/lib

CONFIG(debug, debug|release) {
	win32:LIBPATH += ../../external/CGAL-4.1/build/lib/Debug
	win32:LIBS += -LCGAL_Core-vc110-mt-gd-4.1
	win32:LIBS += -LCGAL-vc110-mt-gd-4.1
	win32:LIBS += -L../../external/CGAL-4.1/auxiliary/gmp/lib -llibgmp-10 
	win32:LIBS += -Llibmpfr-4
} else {
	win32:LIBPATH += ../../external/CGAL-4.1/build/lib/Release
	win32:LIBS += -LCGAL-vc110-mt-4.1 
	win32:LIBS += -LCGAL_Core-vc110-mt-4.1 
	win32:LIBS += -L../../external/CGAL-4.1/auxiliary/gmp/lib -llibgmp-10 
	win32:LIBS += -Llibmpfr-4  
}




