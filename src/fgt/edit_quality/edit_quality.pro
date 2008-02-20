TEMPLATE      = lib
CONFIG       += plugin
CONFIG += stl
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include
HEADERS       = common/const_types.h \
		qualitymapper.h \
		qualitymapperdialog.h \
		common/transferfunction.h \
		common/util.h \
		common/meshmethods.h\
		handle.h \
		eqhandle.h \
		tfhandle.h

SOURCES       = qualitymapper.cpp\
		qualitymapperdialog.cpp\
		common/transferfunction.cpp\
		common/util.cpp \
		common/meshmethods.cpp\
		handle.cpp\
		eqhandle.cpp \
		tfhandle.cpp 

TARGET        = edit_quality
DESTDIR       = ../../meshlab/plugins

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

RESOURCES = qualitymapper.qrc

FORMS		  = qualitymapperdialog.ui
CONFIG		+= debug_and_release

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
QT           += opengl
