TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  +=  ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = filter_aging.h \
		edgepred.h
SOURCES       = filter_aging.cpp \
                ../../meshlab/filterparameter.cpp
TARGET        = filter_aging
DESTDIR       = ../../meshlab/plugins

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
# mac:CONFIG += x86 ppc

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}



