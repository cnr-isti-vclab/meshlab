include (../../shared.pri)

HEADERS       = filter_slice.h \
                filter_slice_functors.h \
                kdtree.h \ 
     ../../meshlab/meshmodel.h
		
 
SOURCES       = filter_slice.cpp \
                $$GLEWCODE \
                #	svgpro.cpp \
                ../../meshlab/filterparameter.cpp  \
                ../../meshlab/meshmodel.cpp

TARGET        = filter_slice

#FORMS	      = svgpro.ui

QT           += opengl



