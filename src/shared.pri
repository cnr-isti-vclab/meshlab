# this is the common include for all the plugins

TEMPLATE      = lib
CONFIG       += plugin
QT += opengl
QT += xml

VCGDIR  = ../../../../vcglib
GLEWDIR = ../../external/glew-1.5.1
mac:LIBS += ../../meshlab/meshlab.app/Contents/MacOS/libcommon.dylib
win32-msvc2005:LIBS += XXXXXXX
win32-msvc2008:LIBS += XXXXXXX

win32-g++:debug:  LIBS += -L../../common/debug -lcommon
win32-g++:release:LIBS += -L../../common/release -lcommon

# uncomment to try Eigen
# DEFINES += VCG_USE_EIGEN
# CONFIG += warn_off

INCLUDEPATH  *= ../.. $$VCGDIR $$GLEWDIR/include

# Uncomment these if you want to experiment with newer gcc compilers
# (here using the one provided with macports)
# macx-g++:QMAKE_CXX=g++-mp-4.3
# macx-g++:QMAKE_CXXFLAGS_RELEASE -= -Os
# macx-g++:QMAKE_CXXFLAGS_RELEASE += -O3

macx:QMAKE_CXX=g++-4.2

# the following line is needed to avoid mismatch between
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

# the following line is to hide the hundred of warnings about the deprecated
# old printf are all around the code
win32-msvc2005:DEFINES	+= _CRT_SECURE_NO_DEPRECATE
win32-msvc2008:DEFINES	+= _CRT_SECURE_NO_DEPRECATE
CONFIG(release){
# Uncomment the following line to disable assert in mingw
#DEFINES += NDEBUG
 }

DESTDIR       = ../../meshlab/plugins
# uncomment in you local copy only in emergency cases. 
# We should never be too permissive
# win32-g++:QMAKE_CXXFLAGS += -fpermissive

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
