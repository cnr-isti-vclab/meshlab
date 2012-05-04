include (../../shared.pri)

QT += opengl

HEADERS += decorate_raster_proj.h \
           $$VCGDIR/wrap/gui/trackball.h \
           GPU/*.h

SOURCES += decorate_raster_proj.cpp \
           $$VCGDIR/wrap/gui/trackball.cpp \
           $$VCGDIR/wrap/gui/trackmode.cpp \
          GPU/FrameBuffer.cpp \
          GPU/Shader.cpp \
          GPU/Texture.cpp

TARGET = decorate_raster_proj
