include (../../shared.pri)

QT           += opengl

HEADERS       = filter_trioptimize.h curvedgeflip.h curvdata.h \
		laplacianadjust.h

SOURCES       = filter_trioptimize.cpp \ 
		../../meshlab/filterparameter.cpp \
		$$GLEWCODE

TARGET        = filter_trioptimize
