include (../../shared.pri)

HEADERS       += quadric_simp.h \ 
                 meshfilter.h 

SOURCES       += meshfilter.cpp \
                 quadric_simp.cpp 

TARGET        = filter_meshing

win32-msvc:QMAKE_CXXFLAGS = /bigobj
