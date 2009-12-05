include (../../shared.pri)

HEADERS       = edit_hole_factory.h \
		    edit_hole.h \
		    fillerDialog.h \
		    holeListModel.h \
		    fgtHole.h \
		    fgtBridge.h \
		    holeSetManager.h 

SOURCES       = edit_hole_factory.cpp \
		    edit_hole.cpp \
		    fillerDialog.cpp\
		    holeListModel.cpp

TARGET        = edit_hole

QT           += opengl
RESOURCES     = edit_hole.qrc
FORMS         = edit_hole.ui

QT           += opengl


