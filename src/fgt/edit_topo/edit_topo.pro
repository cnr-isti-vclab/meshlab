include (../../shared.pri)

HEADERS       = edit_topo.h \
		    $$VCGDIR/wrap/gui/trackball.h
		    

SOURCES       = edit_topo.cpp \
		    ../../meshlab/meshmodel.cpp \
		    $$VCGDIR/wrap/gui/trackball.cpp \
		    $$VCGDIR/wrap/gui/trackmode.cpp \
		    $$GLEWCODE

TARGET        = edit_topo

QT           += opengl
RESOURCES     = edit_topo.qrc
FORMS         = edit_topo.ui
		    		
QT           += opengl



