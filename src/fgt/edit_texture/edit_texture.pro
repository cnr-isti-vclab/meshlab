include (../../shared.pri)

HEADERS       = edittexture.h \
		textureeditor.h \
		renderarea.h \
		$$VCGDIR/wrap/gui/trackball.h
SOURCES       = edittexture.cpp \
		textureeditor.cpp \
		renderarea.cpp \
		$$VCGDIR/wrap/gui/trackball.cpp \
		$$VCGDIR/wrap/gui/trackmode.cpp \
		$$GLEWCODE

TARGET        = edit_texture

QT           += opengl
RESOURCES     = meshlab.qrc
FORMS         = textureeditor.ui


