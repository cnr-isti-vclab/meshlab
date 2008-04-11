TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../ ../../../../sf ../../../../code/lib/glew/include
HEADERS = rgbt.h \
          widgetRgbT.h \
          topologicalOp.h \
          rgbInfo.h \
          rgbPrimitives.h \
          controlPoint.h \
          interactiveEdit.h \
          modButterfly.h \
          subdialog.h
          
SOURCES = rgbt.cpp \
          widgetRgbT.cpp \
          rgbPrimitives.cpp \
          controlPoint.cpp \
          interactiveEdit.cpp \
          modButterfly.cpp \
          subdialog.cpp
          
          
TARGET = editrgbtri
DESTDIR = ../../meshlab/plugins
DEFINES += GLEW_STATIC
DEFINES += NDEBUG
#Enable/Disable performance statistics
#DEFINES += TIMERON
# Enable/Disable face coloring
#DEFINES += RGBCOLOR

QT += opengl
QMAKE_CXXFLAGS += -O2
RESOURCES = editrgbtri.qrc
FORMS += widgetRgbT.ui subDialog.ui

CONFIG += debug_and_release
#CONFIG += debug
OBJECTS_DIR = obj
mac:CONFIG += x86 ppc

contains(TEMPLATE,lib){
  CONFIG(debug, debug|release){
    unix{
      TARGET = $$member(TARGET, 0)_debug
    }
    else{
      TARGET = $$member(TARGET, 0)d
    }
  }
}
win32{
  DEFINES += NOMINMAX
}
