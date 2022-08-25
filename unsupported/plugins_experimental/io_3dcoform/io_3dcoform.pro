include (../../shared.pri)

HEADERS       += io_3dcoform.h \
				metadatafile_generator.h \
				$$VCGDIR/wrap/ply/plylib.h 

SOURCES       += io_3dcoform.cpp \
				metadatafile_generator.cpp \
				$$VCGDIR/wrap/ply/plylib.cpp

TARGET        = io_3dcoform

QT           += xml 