# This is the common include for all the plugins

include (general.pri)
VCGDIR = ../$$VCGDIR

TEMPLATE      = lib
CONFIG       += plugin
QT += opengl
QT += xml
QT += xmlpatterns
QT += script

mac:LIBS += ../../common/libcommon.dylib

#correct qmake syntax requires CONFIG(option, list of options)

win32-msvc2012:  LIBS += ../../distrib/common.lib -lopengl32 -lGLU32
win32-msvc2013:  LIBS += ../../distrib/common.lib -lopengl32 -lGLU32
win32-msvc2015:  LIBS += ../../distrib/common.lib -lopengl32 -lGLU32
win32-g++:LIBS += -L../../distrib -lcommon -lopengl32 -lGLU32
linux-g++:LIBS += -L../../distrib -lcommon -lGL -lGLU
linux-g++-32:LIBS += -L../../distrib -lcommon -lGL -lGLU
linux-g++-64:LIBS += -L../../distrib -lcommon -lGL -lGLU

win32-msvc2013:DEFINES += GLEW_STATIC _USE_MATH_DEFINES
win32-msvc2015:DEFINES += GLEW_STATIC _USE_MATH_DEFINES

# uncomment to try Eigen
# DEFINES += VCG_USE_EIGEN
# CONFIG += warn_off

INCLUDEPATH  *= ../.. $$VCGDIR ../$$GLEWDIR/include
DEPENDPATH += ../.. $$VCGDIR

# Uncomment these if you want to experiment with newer gcc compilers
# (here using the one provided with macports)
# macx-g++:QMAKE_CXX=g++-mp-4.3
# macx-g++:QMAKE_CXXFLAGS_RELEASE -= -Os
# macx-g++:QMAKE_CXXFLAGS_RELEASE += -O3

# the following line is to hide the hundred of warnings about the deprecated
# old printf are all around the code
win32-msvc2013:DEFINES	+= _CRT_SECURE_NO_DEPRECATE
win32-msvc2015:DEFINES	+= _CRT_SECURE_NO_DEPRECATE
CONFIG(release,debug | release){
# Uncomment the following line to disable assert in mingw
#DEFINES += NDEBUG
 }

DESTDIR       = ../../distrib/plugins
# uncomment in you local copy only in emergency cases.
# We should never be too permissive
# win32-g++:QMAKE_CXXFLAGS += -fpermissive

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}

