include (../../shared.pri)


HEADERS += decorate_raster_proj.h \
           $$VCGDIR/wrap/gui/trackball.h \

SOURCES += decorate_raster_proj.cpp \
           $$VCGDIR/wrap/gui/trackball.cpp \
           $$VCGDIR/wrap/gui/trackmode.cpp \

TARGET = decorate_raster_proj
