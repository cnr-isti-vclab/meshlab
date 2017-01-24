include (../../shared.pri)
OPENCVDIR = ../../../../opencv/build/include

HEADERS       = edit_CADtexturing_factory.h \
				CADtexturingedit.h \
				CADtexturingControl.h
				 
SOURCES       = edit_CADtexturing_factory.cpp \
                                CADtexturingedit.cpp \
                                CADtexturingControl.cpp

LIBS         += -lopencv_world310 -lopencv_line_descriptor310
FORMS         = CADtexturingControl.ui

TARGET        = CADtexturingedit

RESOURCES     = CADtexturingedit.qrc
