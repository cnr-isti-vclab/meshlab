include (../../shared.pri)

HEADERS       = splat_pyramid.h point_based_renderer.h object.h pyramid_point_renderer_base.h dialog.h \
                pyramid_point_renderer.h pyramid_point_renderer_color.h pyramid_point_renderer_er.h 

SOURCES       = splat_pyramid.cc point_based_renderer.cc object.cc pyramid_point_renderer_base.cc dialog.cpp \
                pyramid_point_renderer.cc pyramid_point_renderer_color.cc \
                $$GLEWCODE \

TARGET        = render_splatpyramid

QT            += opengl
LIBS 		  += -lglut

FORMS		  = dialog.ui

# CONFIG += debug
