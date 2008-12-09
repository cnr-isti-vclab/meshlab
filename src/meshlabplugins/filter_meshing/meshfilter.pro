include (../../shared.pri)

HEADERS       = transformDialog.h \
                $$VCGDIR/vcg/complex/trimesh/clean.h \
                ../../meshlab/interfaces.h \
                meshfilter.h 

SOURCES       = transformDialog.cpp \
		meshfilter.cpp \ 
		quadricsimp.cpp \ 
		quadricsteximp.cpp \ 
		../../meshlab/GLLogStream.cpp\ 
		../../meshlab/filterparameter.cpp 

TARGET        = meshfilter

# all dialog are stored in meshlab/ui directory
FORMS         = ui/transformDialog.ui 

QT += opengl
