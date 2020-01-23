# This is the common include for all the plugins

include (general.pri)
VCGDIR = ../$$VCGDIR
# only adjust relative path if we're not using system Eigen
!CONFIG(system_eigen3):EIGENDIR = ../$$EIGENDIR

TEMPLATE = lib
CONFIG += plugin
QT += opengl
QT += xml
QT += xmlpatterns
QT += script

mac:LIBS += ../../common/libcommon.dylib
win32-msvc:LIBS += ../../distrib/common.lib -lopengl32 -lGLU32
win32-g++:LIBS += -L../../distrib -lcommon -lopengl32 -lGLU32
linux-g++:LIBS += -L../../distrib -lcommon -lGL -lGLU

win32-msvc:DEFINES += GLEW_STATIC _USE_MATH_DEFINES

INCLUDEPATH  *= ../.. $$VCGDIR $$EIGENDIR
!CONFIG(system_glew): INCLUDEPATH *=  ../$$GLEWDIR/include
DEPENDPATH += ../.. $$VCGDIR

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
