include (../../shared.pri)

# Lib name
TARGET        = edit_fit

HEADERS       = edit_fit_factory.h \
				editfit.h\
				cylinder.h\
				box.h\
				primitive.h\
				fittoolbox.h
				 
SOURCES       = edit_fit_factory.cpp \
				editfit.cpp\
  				fittoolbox.cpp\
  				primitive.cpp\
				$$GLEWCODE				

FORMS 	  += fittoolbox.ui

INCLUDEPATH += ../../../../code/lib/cminpack-1.0.1

win32-msvc2005:LIBS	+= ../../../../code/lib/cminpack-1.0.1/lib/cminpack.lib
win32-msvc2008:LIBS	+= ../../../../code/lib/cminpack-1.0.1/lib/cminpack.lib

QT           += opengl
