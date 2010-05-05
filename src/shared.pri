# this is the common include for all the plugins

TEMPLATE      = lib
CONFIG       += plugin
QT += opengl
QT += xml

VCGDIR  = ../../../../vcglib
GLEWDIR = ../../external/glew-1.5.1

mac:LIBS += ../../common/libcommon.dylib

#correct qmake syntax requires CONFIG(option, list of options)

win32-msvc2005:  LIBS += ../../distrib/common.lib
win32-msvc2008:  LIBS += ../../distrib/common.lib
win32-g++:LIBS += -L../../distrib -lcommon
linux-g++:LIBS += -L../../distrib -lcommon
linux-g++-32:LIBS += -L../../distrib -lcommon
linux-g++-64:LIBS += -L../../distrib -lcommon

#CONFIG(debug, debug|release) {
#	win32-msvc2005:  LIBS += ../../common/debug/common.lib
#	win32-msvc2008:  LIBS += ../../common/debug/common.lib
#	win32-g++:  LIBS += -L../../common/debug -lcommon
#}

win32-msvc2005:DEFINES += GLEW_STATIC
win32-msvc2008:DEFINES += GLEW_STATIC 

# uncomment to try Eigen
# DEFINES += VCG_USE_EIGEN
# CONFIG += warn_off

INCLUDEPATH  *= ../.. $$VCGDIR $$GLEWDIR/include
DEPENDPATH += $$VCGDIR

# Uncomment these if you want to experiment with newer gcc compilers
# (here using the one provided with macports)
# macx-g++:QMAKE_CXX=g++-mp-4.3
# macx-g++:QMAKE_CXXFLAGS_RELEASE -= -Os
# macx-g++:QMAKE_CXXFLAGS_RELEASE += -O3

macx:QMAKE_CXX=g++-4.2
##macx:QMAKE_POST_LINK ="install_name_tool -change libcommon.1.dylib @executable_path/libcommon.1.dylib ../../meshlab/plugins/lib$${TARGET}.dylib"

# the following line is needed to avoid mismatch between
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

# the following line is to hide the hundred of warnings about the deprecated
# old printf are all around the code
win32-msvc2005:DEFINES	+= _CRT_SECURE_NO_DEPRECATE
win32-msvc2008:DEFINES	+= _CRT_SECURE_NO_DEPRECATE
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
