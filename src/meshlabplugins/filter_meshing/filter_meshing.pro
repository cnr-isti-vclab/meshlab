include (../../shared.pri)

HEADERS       += $$VCGDIR/vcg/complex/trimesh/clean.h\
		quadric_simp.h \ 
		quadric_tex_simp.h \ 
		meshfilter.h 

SOURCES       += meshfilter.cpp \
		quadric_simp.cpp \ 
        quadric_tex_simp.cpp

TARGET        = filter_meshing

QT += opengl
