include (../../shared.pri)

HEADERS       = edit_texture_factory.h \ 
		edittexture.h \
		textureeditor.h \
		renderarea.h \
		../../meshlab/GLLogStream.h \
		$$VCGDIR/wrap/gui/trackball.h
		
SOURCES       = edit_texture_factory.cpp \
		edittexture.cpp \
		textureeditor.cpp \
		renderarea.cpp \
		../../meshlab/GLLogStream.cpp \
		$$VCGDIR/wrap/gui/trackball.cpp \
		$$VCGDIR/wrap/gui/trackmode.cpp \
		$$GLEWCODE

TARGET        = edit_texture

QT           += opengl
RESOURCES     = meshlab.qrc
FORMS         = textureeditor.ui


