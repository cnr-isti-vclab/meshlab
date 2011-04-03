include (../../shared.pri)

HEADERS       += filter_unsharp.h \
    $$VCGDIR/vcg/complex/algorithms/crease_cut.h
				
SOURCES       += filter_unsharp.cpp 
		
TARGET        = filter_unsharp

