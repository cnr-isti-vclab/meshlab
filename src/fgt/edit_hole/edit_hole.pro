include (../../shared.pri)

HEADERS       = edit_hole.h \
		    fillerDialog.h \
		    holeListModel.h \
		    fgtHole.h \
		    fgtBridge.h \


SOURCES       = edit_hole.cpp \
		    fillerDialog.cpp\
		    holeListModel.cpp \
		    ../../meshlab/meshmodel.cpp \
		    $$GLEWCODE

TARGET        = edit_hole

QT           += opengl
RESOURCES     = edit_hole.qrc
FORMS         = edit_hole.ui

QT           += opengl


