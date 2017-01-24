include (../../shared.pri)

macx:QMAKE_CXX = clang++-mp-3.9
macx:QMAKE_CXXFLAGS+=-fopenmp
macx:QMAKE_LFLAGS += -L/opt/local/lib/libomp -lomp
linux:QMAKE_LFLAGS += -fopenmp -lgomp
macx:QMAKE_CXXFLAGS_RELEASE+= -O3 -DRELEASE -funroll-loops -ffast-math  -Wno-sign-compare -Wno-unused-parameter


win32:QMAKE_CXXFLAGS   += -openmp


HEADERS       = edit_align_factory.h \
                edit_align.h \
                meshtree.h \
                alignDialog.h \
                AlignPairDialog.h \
                AlignPairWidget.h \
                align/AlignPair.h \
                align/AlignGlobal.h \
                align/OccupancyGrid.h \
                align/align_parameter.h \
                ../../meshlab/stdpardialog.h \
                $$VCGDIR/wrap/gui/trackball.h\
                $$VCGDIR/wrap/gui/trackmode.h\
                $$VCGDIR/wrap/gl/trimesh.h

SOURCES       = edit_align_factory.cpp \
                point_matching_scale.cpp \
                edit_align.cpp \
                meshtree.cpp \
                alignDialog.cpp \
                AlignPairWidget.cpp \
                AlignPairDialog.cpp \
                align/AlignPair.cpp \
                align/AlignGlobal.cpp \
                align/OccupancyGrid.cpp \
                align/align_parameter.cpp \
                ../../meshlab/stdpardialog.cpp \
                $$VCGDIR/wrap/ply/plylib.cpp\
                $$VCGDIR/wrap/gui/trackball.cpp\
                $$VCGDIR/wrap/gui/trackmode.cpp \
                $$GLEWCODE 
    
                
INCLUDEPATH += ../../external

FORMS 	  += alignDialog.ui
TARGET       = edit_align
RESOURCES     = edit_align.qrc

