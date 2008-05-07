TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include 
HEADERS       = io_x3d.h import_x3d.h export_x3d.h util_x3d.h ./vrml/Parser.h ./vrml/Scanner.h
				
SOURCES       = io_x3d.cpp ./vrml/Parser.cpp ./vrml/Scanner.cpp
QT           += opengl 
QT 			 += xml
				
TARGET        = io_x3d
DESTDIR       = ../../meshlab/plugins

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
} 