include (../../shared.pri)

QT           += opengl

HEADERS       = filter_trioptimize.h \
		curvedgeflip.h \
		curvdata.h 

SOURCES       = filter_trioptimize.cpp \ 
		../../meshlab/filterparameter.cpp 

TARGET        = filter_trioptimize
