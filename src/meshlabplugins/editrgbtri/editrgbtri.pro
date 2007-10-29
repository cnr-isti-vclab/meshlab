
TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../ ../../../../sf ../../../../code/lib/glew/include
HEADERS = rgbt.h \
          widgetRgbT.h \
          utilities.h \
          topologicalOp.h \
          rgbInfo.h \
          rgbPrimitives.h \
          selectiveRefinement.h \
          controlPoint.h \
          interactiveEdit.h
          
SOURCES = rgbt.cpp \
          widgetRgbT.cpp \
          utilities.cpp \
          rgbPrimitives.cpp \
          selectiveRefinement.cpp \
          controlPoint.cpp \
          interactiveEdit.cpp
          
          
TARGET = editrgbtri
DESTDIR = ../../meshlab/plugins
#DEFINES += GLEW_STATIC
DEFINES += NDEBUG
QT += opengl
#QMAKE_CXXFLAGS += -O2
RESOURCES = editrgbtri.qrc
FORMS += widgetRgbT.ui

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
