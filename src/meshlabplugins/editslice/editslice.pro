include (../../shared.pri)

HEADERS =	editslice.h \
			slicedialog.h\
			svgpro.h

SOURCES       = editslice.cpp \
				slicedialog.cpp\
				$$GLEWCODE \
                $$VCGDIR/wrap/gui/trackball.cpp \
                $$VCGDIR/wrap/gui/trackmode.cpp \
				svgpro.cpp
		
TARGET        = editslice

QT           += opengl
RESOURCES     = meshlab.qrc
FORMS         = slicedialog.ui svgpro.ui
