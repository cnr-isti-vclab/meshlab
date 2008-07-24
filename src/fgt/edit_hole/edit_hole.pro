include (../../shared.pri)

HEADERS       = edit_hole.h \
		    fillerDialog.h \
		    holeListModel.h \
		    fgtHole.h \
		    $$VCGDIR/wrap/gui/trackball.h
		    

SOURCES       = edit_hole.cpp \
		    fillerDialog.cpp\
		    holeListModel.cpp \
		    ../../meshlab/meshmodel.cpp \
		    $$VCGDIR/wrap/gui/trackball.cpp \
		    $$VCGDIR/wrap/gui/trackmode.cpp \
		    $$GLEWCODE

TARGET        = edit_hole

QT           += opengl
RESOURCES     = edit_hole.qrc
FORMS         = edit_hole.ui

		    		
QT           += opengl

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

# mac:CONFIG += x86 ppc

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}



