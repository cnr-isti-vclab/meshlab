include (../../shared.pri)

FORMS         = ui/u3d_gui.ui 				
HEADERS       = io_u3d.h \
				u3d_gui.h \
				$$VCGDIR/wrap/io_trimesh/export_u3d.h
SOURCES       = io_u3d.cpp \
				u3d_gui.cpp
TARGET        = io_u3d

CONFIG += STL