include (../../shared.pri)

QT += opengl

HEADERS += decorate_raster_proj.h \
           $$VCGDIR/wrap/gui/trackball.h \
           ../filter_img_patch_param/GPU/*.h

SOURCES += decorate_raster_proj.cpp \
           $$VCGDIR/wrap/gui/trackball.cpp \
           $$VCGDIR/wrap/gui/trackmode.cpp \
          ../filter_img_patch_param/GPU/FrameBuffer.cpp \
          ../filter_img_patch_param/GPU/Shader.cpp \
          ../filter_img_patch_param/GPU/Texture.cpp

TARGET = decorate_raster_proj
