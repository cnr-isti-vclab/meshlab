include (../../shared.pri)

HEADERS       = filter_sampling.h \
		$$VCGDIR/vcg/complex/trimesh/point_sampling.h
				
SOURCES       = filter_sampling.cpp\ 
		../../meshlab/filterparameter.cpp\
		$$GLEWCODE

TARGET        = filter_sampling

CONFIG       += opengl

