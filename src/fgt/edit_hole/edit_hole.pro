include (../../shared.pri)

HEADERS       = edit_hole \
		$$VCGDIR/wrap/gui/trackball.h
SOURCES       = edit_hole.cpp \
		$$VCGDIR/wrap/gui/trackball.cpp \
		$$VCGDIR/wrap/gui/trackmode.cpp \
		$$GLEWCODE

TARGET        = edit_hole

QT           += opengl
RESOURCES     = edit_hole.qrc
FORMS         = edit_hole.ui


