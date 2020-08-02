include (../../shared.pri)

INCLUDEPATH += \
	$$VCGDIR/wrap/newuoa/include

HEADERS += \
	edit_align_factory.h \
	edit_align.h \
	meshtree.h \
	alignDialog.h \
	AlignPairDialog.h \
	AlignPairWidget.h \
	align/AlignGlobal.h \
	align/OccupancyGrid.h \
	align/align_parameter.h \
	../../meshlab/meshlab_settings/richparameterwidgets.h \
	../../meshlab/meshlab_settings/richparameterlistframe.h \
	../../meshlab/meshlab_settings/richparameterlistdialog.h \
	$$VCGDIR/wrap/gui/trackball.h \
	$$VCGDIR/wrap/gui/trackmode.h \
	$$VCGDIR/wrap/gl/trimesh.h

SOURCES += \
	edit_align_factory.cpp \
	edit_align.cpp \
	meshtree.cpp \
	alignDialog.cpp \
	AlignPairWidget.cpp \
	AlignPairDialog.cpp \
	align/AlignGlobal.cpp \
	align/OccupancyGrid.cpp \
	align/align_parameter.cpp \
	../../meshlab/meshlab_settings/richparameterwidgets.cpp \
	../../meshlab/meshlab_settings/richparameterlistframe.cpp \
	../../meshlab/meshlab_settings/richparameterlistdialog.cpp \
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

