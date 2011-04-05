include (../../shared.pri)

HEADERS =	edit_slice_factory.h \
			editslice.h \
			slicedialog.h\
			svgpro.h

SOURCES       = edit_slice_factory.cpp \
				editslice.cpp \
				slicedialog.cpp\
                $$VCGDIR/wrap/gui/trackball.cpp \
                $$VCGDIR/wrap/gui/trackmode.cpp \
				svgpro.cpp
		
TARGET        = editslice

QT           += opengl
RESOURCES     = meshlab.qrc
FORMS         = slicedialog.ui svgpro.ui
