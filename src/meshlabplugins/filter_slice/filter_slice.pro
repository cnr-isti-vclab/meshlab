include (../../sharedfilter.pri)

HEADERS       += filter_slice.h \
                filter_slice_functors.h \
                kdtree.h \ 
     ../../meshlab/meshmodel.h
		
 
SOURCES       += filter_slice.cpp \
                $$GLEWCODE \
                ../../meshlab/meshmodel.cpp

TARGET        = filter_slice

#FORMS	      = svgpro.ui

QT           += opengl



