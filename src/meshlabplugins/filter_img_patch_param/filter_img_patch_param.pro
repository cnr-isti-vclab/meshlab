include (../../shared.pri)


SOURCES = filter_img_patch_param.cpp \
          VisibleSet.cpp \
          VisibilityCheck.cpp \
          TexturePainter.cpp \
          GPU/FrameBuffer.cpp \
          GPU/Shader.cpp \
          GPU/Texture.cpp

HEADERS = filter_img_patch_param.h \
          VisibleSet.h \
          VisibilityCheck.h \
          TexturePainter.h \
          Patch.h \
          GPU/*.h

TARGET = filter_img_patch_param
