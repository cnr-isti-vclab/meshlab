include (../../shared.pri)

HEADERS       += ../../meshlabplugins/edit_align/align/AlignPair.h \
    ../../meshlabplugins/edit_align/align/OccupancyGrid.h \
		../../common/filterparameter.h \
		filter_autoalign.h

SOURCES       += filter_autoalign.cpp \ 
                ../../meshlabplugins/edit_align/align/AlignPair.cpp \
                ../../meshlabplugins/edit_align/align/Guess.cpp \
        ../../meshlabplugins/edit_align/point_matching_scale.cpp \
		../../../../vcglib/wrap/ply/plylib.cpp \
		 
INCLUDEPATH += ../../external
		 
		 
TARGET        = filter_autoalign
