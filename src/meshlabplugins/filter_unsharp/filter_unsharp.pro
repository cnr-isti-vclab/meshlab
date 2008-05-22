include (../../shared.pri)

HEADERS       = filter_unsharp.h \
		$$VCGDIR/vcg/complex/trimesh/crease_cut.h
				
SOURCES       = filter_unsharp.cpp\ 
		../../meshlab/filterparameter.cpp
TARGET        = filter_unsharp

