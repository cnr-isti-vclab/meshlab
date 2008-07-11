include (../../shared.pri)

# Lib name
TARGET        = edit_fit

HEADERS       = editfit.h\
				cylinder.h\
				box.h\
				primitive.h\
				fittoolbox.h
				 
SOURCES       = editfit.cpp\
  				fittoolbox.cpp\
  				primitive.cpp\
				$$GLEWCODE				

FORMS 	  += fittoolbox.ui

INCLUDEPATH += ../../../../code/lib/cminpack-1.0.1

win32-msvc2005:LIBS	+= ../../../../code/lib/cminpack-1.0.1/lib/cminpack.lib

QT           += opengl
