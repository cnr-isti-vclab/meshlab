include (../../shared.pri)

HEADERS       = filter_slice.h

		
 
SOURCES       = filter_slice.cpp \
		$$GLEWCODE \
#	svgpro.cpp \
		../../meshlab/filterparameter.cpp
		

TARGET        = filter_slice

#FORMS	      = svgpro.ui

QT           += opengl



