include (../../shared.pri)

HEADERS       = edit_texture_factory.h \ 
		edittexture.h \
		textureeditor.h \
		renderarea.h \
		$$VCGDIR/wrap/gui/trackball.h
		
SOURCES       = edit_texture_factory.cpp \
		edittexture.cpp \
		textureeditor.cpp \
		renderarea.cpp \
		$$VCGDIR/wrap/gui/trackball.cpp \
		$$VCGDIR/wrap/gui/trackmode.cpp 

TARGET        = edit_texture

QT           += opengl
RESOURCES     = meshlab.qrc
FORMS         = textureeditor.ui


