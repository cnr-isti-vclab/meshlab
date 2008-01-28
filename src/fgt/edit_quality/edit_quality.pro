TEMPLATE      = lib
CONFIG       += plugin
CONFIG += stl
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = const_types.h histogram.h qualitymapper.h qualitymapperdialog.h transferfunction.h util.h handle.h eqhandle.h tfhandle.h
SOURCES       = qualitymapper.cpp transferfunction.cpp qualitymapperdialog.cpp util.cpp handle.cpp eqhandle.cpp tfhandle.cpp \
								../../meshlab/filterparameter.cpp
TARGET        = editQuality
DESTDIR       = ../../meshlab/plugins
# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

RESOURCES = qualitymapper.qrc

FORMS		  = qualitymapperdialog.ui
CONFIG		+= debug_and_release
# mac:CONFIG += x86 ppc


contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
QT           += opengl
