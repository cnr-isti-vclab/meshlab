include (../../shared.pri)


HEADERS       = edit_CADtexturing_factory.h \
				CADtexturingedit.h \
				CADtexturingControl.h
				 
SOURCES       = edit_CADtexturing_factory.cpp \
                                CADtexturingedit.cpp \
                                CADtexturingControl.cpp


FORMS = CADtexturingControl.ui \


TARGET        = CADtexturingedit

RESOURCES     = CADtexturingedit.qrc
