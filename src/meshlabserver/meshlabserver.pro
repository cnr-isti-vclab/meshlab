GLEWDIR = ../external/glew-1.5.1
GLEWCODE = $$GLEWDIR/src/glew.c

HEADERS        = ../meshlab/interfaces.h \
                 ../meshlab/meshmodel.h \
                 ../common/filterscript.h \
                 ../common/filterparameter.h

SOURCES        = mainserver.cpp \
                 ../meshlab/meshmodel.cpp \
                 ../common/filterscript.cpp \
                 ../common/filterparameter.cpp \
                 ../meshlab/plugin_support.cpp \
                 $$GLEWCODE

# to add windows icon 
RC_FILE = ../meshlab/meshlab.rc

QT           += xml opengl

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

DEFINES += GLEW_STATIC

INCLUDEPATH += . .. ../../../vcglib $$GLEWDIR/include
CONFIG += stl 
CONFIG += console

# Mac specific Config required to avoid to make application bundles
CONFIG -= app_bundle
