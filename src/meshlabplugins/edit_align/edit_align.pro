include (../../shared.pri)

INCLUDEPATH += \
    $$MESHLAB_EXTERNAL_DIRECTORY/newuoa/include

HEADERS += \
    edit_align_factory.h \
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
    $$VCGDIR/wrap/gui/trackball.h \
    $$VCGDIR/wrap/gui/trackmode.h \
    $$VCGDIR/wrap/gl/trimesh.h

SOURCES += \
    edit_align_factory.cpp \
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
    $$VCGDIR/wrap/gui/trackmode.cpp

FORMS += \
    alignDialog.ui

RESOURCES += \
    edit_align.qrc

TARGET = edit_align

linux:QMAKE_LFLAGS += -fopenmp -lgomp
win32:QMAKE_CXXFLAGS   += -openmp

!CONFIG(system_glew): SOURCES += $$GLEWCODE
CONFIG(system_glew) {
    linux: LIBS += -lGLEW
}

