include (../../shared.pri)

HEADERS       += filter_3dcoform.h \
				 metadatafile_generator.h \
				 coform_interface.h \
		$$VCGDIR/wrap/ply/plylib.h 

SOURCES += filter_3dcoform.cpp \
			metadatafile_generator.cpp \
		   coform_interface.cpp \
		   $$VCGDIR//wrap/ply/plylib.cpp

FORMS        += ui/coform_interface.ui 

QT           += xml

TARGET        = filter_3dcoform



