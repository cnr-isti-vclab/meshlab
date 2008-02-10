HEADERS        = ../meshlab/interfaces.h \
                 ../meshlab/meshmodel.h \
                 ../meshlab/filterscript.h \
                 ../meshlab/filterparameter.h 

SOURCES        = mainserver.cpp \
                 ../meshlab/filterscript.cpp \
                 ../meshlab/filterparameter.cpp \
                 ../meshlab/plugin_support.cpp \
                 ../../../code/lib/glew/src/glew.c

# to add windows icon 
RC_FILE = ../meshlab/meshlab.rc

QT           += xml opengl

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

DEFINES += GLEW_STATIC

INCLUDEPATH += . ../../../sf ../../../code/lib/glew/include
CONFIG += stl 
CONFIG += console

# Mac specific Config required to avoid to make application bundles
CONFIG -= app_bundle