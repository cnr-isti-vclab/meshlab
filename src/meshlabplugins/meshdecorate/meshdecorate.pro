TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include ../../../../code/lib/glut
HEADERS       = meshdecorate.h
SOURCES       = meshdecorate.cpp ../../../../sf/wrap/gui/coordinateframe.cpp
TARGET        = meshdecorate
DESTDIR       = ../../meshlab/plugins
# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
CONFIG		+= debug_and_release

# mac:CONFIG += x86 ppc


contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
QT           += opengl

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tools/plugandpaint/plugins
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS extrafilters.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tools/plugandpaintplugins/extrafilters
INSTALLS += target sources
