TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include ../../../../code/lib/glut
HEADERS       = meshdecorate.h
SOURCES       = meshdecorate.cpp
TARGET        = meshdecorate
DESTDIR       = ../../meshlab/plugins
# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
# win32:LIBS	+= ../../../../code/lib/glut/lib/glut32.lib


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
