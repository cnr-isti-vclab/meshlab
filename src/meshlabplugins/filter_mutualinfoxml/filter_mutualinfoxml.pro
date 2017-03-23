include (../../shared.pri)

HEADERS       += filter_mutualinfo.h \
			alignset.h \
			levmarmethods.h \
			mutual.h \
			parameters.h \
			pointCorrespondence.h \
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

macx:QMAKE_POST_LINK = "cp "$$_PRO_FILE_PWD_/$$TARGET".xml ../../distrib/plugins/"$$TARGET".xml"
linux:QMAKE_POST_LINK = "cp "$$_PRO_FILE_PWD_/$$TARGET".xml ../../distrib/plugins/"$$TARGET".xml; cd ../../distrib/plugins/ ; ln -s "$$TARGET".xml lib"$$TARGET".xml"
