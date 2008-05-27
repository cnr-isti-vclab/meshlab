include (../../shared.pri)

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

#Enable/Disable performance statistics
#DEFINES += TIMERON
# Enable/Disable face coloring
#DEFINES += RGBCOLOR

QT += opengl
RESOURCES = editrgbtri.qrc
FORMS += widgetRgbT.ui subDialog.ui

