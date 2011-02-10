include (../../shared.pri)

HEADERS       += ../../meshlabplugins/editalign/align/AlignPair.h \
		../../meshlabplugins/editalign/align/OccupancyGrid.h \
		../../common/filterparameter.h \
		filter_autoalign.h

SOURCES       += filter_autoalign.cpp \ 
                ../../meshlabplugins/editalign/align/AlignPair.cpp \
                ../../meshlabplugins/editalign/align/Guess.cpp \
				../../meshlabplugins/editalign/point_matching_scale.cpp \
		../../../../vcglib/wrap/ply/plylib.cpp \
		 
INCLUDEPATH += ../../external
		 
		 
TARGET        = filter_autoalign
