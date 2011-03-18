include (../../shared.pri)

HEADERS       += filter_mutualinfo.h \
			alignset.h \
			levmarmethods.h \
			mutual.h \
			parameters.h \
			pointCorrispondence.h \
			pointOnLayer.h \
			shutils.h \
			solver.h 

SOURCES       += filter_mutualinfo.cpp \ 
			alignset.cpp  \
			levmarmethods.cpp  \
			mutual.cpp  \
			parameters.cpp   \
			pointCorrespondence.cpp  \
			solver.cpp
		
TARGET        = filter_mutualinfo
