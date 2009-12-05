include (../../shared.pri)
				
HEADERS       += io_u3d.h \
				$$VCGDIR/wrap/io_trimesh/export_u3d.h
SOURCES       += io_u3d.cpp
SOURCES       += ../../common/pluginmanager.cpp

TARGET        = io_u3d

CONFIG += STL
