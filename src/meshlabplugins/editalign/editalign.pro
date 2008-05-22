include (../../shared.pri)

HEADERS       = editalign.h \
                meshtree.h \
                alignDialog.h \
		AlignPairDialog.h \
		AlignPairWidget.h \
		align/AlignPair.h \
		align/AlignGlobal.h \
                align/OccupancyGrid.h \
		../../meshlab/stdpardialog.h \
		 $$VCGDIR/wrap/gui/trackball.h\
                 $$VCGDIR/wrap/gui/trackmode.h\
                 $$VCGDIR/wrap/gl/trimesh.h

SOURCES       = editalign.cpp \
                meshtree.cpp \
                alignDialog.cpp \
		AlignPairWidget.cpp \
                align/AlignPair.cpp \
                align/AlignGlobal.cpp \
                align/OccupancyGrid.cpp \
		../../meshlab/meshmodel.cpp \
		../../meshlab/stdpardialog.cpp \
		../../meshlab/filterparameter.cpp \
		$$VCGDIR/wrap/ply/plylib.cpp\
                $$VCGDIR/wrap/gui/trackball.cpp\
                $$VCGDIR/wrap/gui/trackmode.cpp \
                $$GLEWCODE

FORMS 	  += alignDialog.ui
TARGET       = editalign
QT           += opengl
RESOURCES     = editalign.qrc

