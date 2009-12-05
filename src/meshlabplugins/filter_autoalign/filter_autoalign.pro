include (../../shared.pri)

HEADERS       += ../../meshlabplugins/editalign/align/AlignPair.h \
		../../meshlabplugins/editalign/align/OccupancyGrid.h \
		../../common/filterparameter.h \
		filter_autoalign.h

SOURCES       += filter_autoalign.cpp \ 
                ../../meshlabplugins/editalign/align/AlignPair.cpp \
                ../../meshlabplugins/editalign/align/Guess.cpp \
		../../../../vcglib/wrap/ply/plylib.cpp \
		 

TARGET        = filter_autoalign
