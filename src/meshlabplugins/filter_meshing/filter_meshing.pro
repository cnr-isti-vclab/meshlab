include (../../shared.pri)

HEADERS       = transformDialog.h \
                $$VCGDIR/vcg/complex/trimesh/clean.h \
                ../../meshlab/interfaces.h \
                ../../meshlab/meshmodel.h \
		meshfilter.h 

SOURCES       = transformDialog.cpp \
		meshfilter.cpp \ 
		quadric_simp.cpp \ 
		quadric_tex_simp.cpp \ 
		../../meshlab/GLLogStream.cpp\ 
		../../meshlab/filterparameter.cpp \
		../../meshlab/meshmodel.cpp \
		$$GLEWCODE
TARGET        = filter_meshing

# all dialog are stored in meshlab/ui directory
FORMS         = ui/transformDialog.ui 

QT += opengl
