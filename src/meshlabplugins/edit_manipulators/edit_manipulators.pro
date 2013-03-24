include (../../shared.pri)

HEADERS   = edit_manipulators_factory.h \
            edit_manipulators.h

SOURCES   = edit_manipulators_factory.cpp \
            edit_manipulators.cpp \
	    ../../../../vcglib/wrap/gui/trackball.cpp \
	    ../../../../vcglib/wrap/gui/trackmode.cpp 

TARGET    = edit_manipulators

RESOURCES = edit_manipulators.qrc
