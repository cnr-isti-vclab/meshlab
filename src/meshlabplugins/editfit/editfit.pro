include (../../shared.pri)

# Lib name
TARGET        = edit_fit

HEADERS       = editfit.h\
				cylinder.h\
				primitive.h
				 
SOURCES       = editfit.cpp\
		$$GLEWCODE				

QT           += opengl
