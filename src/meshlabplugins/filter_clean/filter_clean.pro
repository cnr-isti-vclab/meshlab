include (../../shared.pri)

HEADERS       += cleanfilter.h \
		../../common/filterparameter.h

SOURCES       += cleanfilter.cpp	

#align stuff

HEADERS      += align_tools.h \
				../../meshlabplugins/edit_pickpoints/pickedPoints.h \
				../../meshlabplugins/editalign/align/align_parameter.h \
				../../meshlabplugins/editalign/meshtree.h \
                ../../meshlabplugins/editalign/align/AlignPair.h \
				../../meshlabplugins/editalign/align/AlignGlobal.h \
                ../../meshlabplugins/editalign/align/OccupancyGrid.h \
                $$VCGDIR/wrap/gl/trimesh.h

SOURCES      += align_tools.cpp \
				../../meshlabplugins/edit_pickpoints/pickedPoints.cpp \
				../../meshlabplugins/editalign/align/align_parameter.cpp \
				../../meshlabplugins/editalign/meshtree.cpp \
                ../../meshlabplugins/editalign/align/AlignPair.cpp \
                ../../meshlabplugins/editalign/align/AlignGlobal.cpp \
                ../../meshlabplugins/editalign/align/OccupancyGrid.cpp \
				$$VCGDIR/wrap/ply/plylib.cpp


TARGET        = filter_clean

QT           += opengl
QT           += xml