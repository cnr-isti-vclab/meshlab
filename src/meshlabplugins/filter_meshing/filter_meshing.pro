include (../../shared.pri)

HEADERS       += quadric_simp.h \ 
                 meshfilter.h 

SOURCES       += meshfilter.cpp \
                 quadric_simp.cpp 

TARGET        = filter_meshing

win32-msvc2010:QMAKE_CXXFLAGS = /bigobj
win32-msvc2005:QMAKE_CXXFLAGS = /bigobj
win32-msvc2008:QMAKE_CXXFLAGS = /bigobj
win32-msvc2012:QMAKE_CXXFLAGS = /bigobj
win32-msvc2015:QMAKE_CXXFLAGS = /bigobj
