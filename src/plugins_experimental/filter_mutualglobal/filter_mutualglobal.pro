include (../../shared.pri)

HEADERS       += filter_mutualglobal.h \
			alignset.h \
			levmarmethods.h \
			mutual.h \
			parameters.h \
			pointCorrespondence.h \
			pointOnLayer.h \
			shutils.h \
			solver.h 

SOURCES       += filter_mutualglobal.cpp \ 
			alignset.cpp  \
			levmarmethods.cpp  \
			mutual.cpp  \
			parameters.cpp   \
			pointCorrespondence.cpp  \
			solver.cpp
		
TARGET        = filter_mutualglobal

