TEMPLATE      = lib
CONFIG       += plugin
CONFIG += stl
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = qualityMapper.h
SOURCES       = qualityMapper.cpp \
								../../meshlab/filterparameter.cpp
TARGET        = qualityMapper
DESTDIR       = ../../meshlab/plugins
# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
# FORMS va aggiunto quando viene disegnata la gui (MAL)
# FORMS		  = qualityMapperDialog.ui
CONFIG		+= debug_and_release
# mac:CONFIG += x86 ppc


contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
QT           += opengl
