# this is the common include for all the plugins

CONFIG *= debug_and_release
TEMPLATE      = lib
CONFIG       += plugin

VCGDIR  = ../../../../vcglib
GLEWDIR = ../../../../code/lib/glew
GLEWCODE = $$GLEWDIR/src/glew.c
DEFINES *= GLEW_STATIC

# uncomment to try Eigen
DEFINES += VCG_USE_EIGEN
#CONFIG += warn_off

INCLUDEPATH  *= ../.. $$VCGDIR $$GLEWDIR/include


# the following line is needed to avoid mismatch between
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

# the following line is to hide the hundred of warnings about the deprecated
# old printf are all around the code
win32-msvc2005:DEFINES	+= _CRT_SECURE_NO_DEPRECATE


DESTDIR       = ../../meshlab/plugins

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
