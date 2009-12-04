include (../../shared.pri)

HEADERS       += transformDialog.h \
                $$VCGDIR/vcg/complex/trimesh/clean.h\
		quadric_simp.h \ 
		quadric_tex_simp.h \ 
		meshfilter.h 

SOURCES       += transformDialog.cpp \
		meshfilter.cpp \ 
		quadric_simp.cpp \ 
        quadric_tex_simp.cpp
TARGET        = filter_meshing

# all dialog are stored in meshlab/ui directory
FORMS         = ui/transformDialog.ui 

QT += opengl
