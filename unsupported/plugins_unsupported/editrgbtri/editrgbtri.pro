include (../../shared.pri)

HEADERS = edit_rgbtri_factory.h \
          rgbt.h \
          widgetRgbT.h \
          topologicalOp.h \
          rgbInfo.h \
          rgbPrimitives.h \
          controlPoint.h \
          interactiveEdit.h \
          modButterfly.h \
          subdialog.h
          
SOURCES = edit_rgbtri_factory.cpp \
          rgbt.cpp \
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

DEFINES += NDEBUG

QT += opengl
CONFIG += STL

RESOURCES = editrgbtri.qrc
FORMS += widgetRgbT.ui subDialog.ui

