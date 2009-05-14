include (../../shared.pri)
				
HEADERS       = io_u3d.h \
				$$VCGDIR/wrap/io_trimesh/export_u3d.h
SOURCES       = io_u3d.cpp \
				../../meshlab/filterparameter.cpp
				
TARGET        = io_u3d

CONFIG += STL