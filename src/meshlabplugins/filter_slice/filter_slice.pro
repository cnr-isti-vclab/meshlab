include (../../shared.pri)

HEADERS       = filter_slice.h \
		svgpro.h
		
 
SOURCES       = filter_slice.cpp \
		$$GLEWCODE \
                $$VCGDIR/wrap/gui/trackball.cpp \
                $$VCGDIR/wrap/gui/trackmode.cpp \
		svgpro.cpp \
		../../meshlab/filterparameter.cpp
		

TARGET        = filter_slice

FORMS	      = svgpro.ui

QT           += opengl



