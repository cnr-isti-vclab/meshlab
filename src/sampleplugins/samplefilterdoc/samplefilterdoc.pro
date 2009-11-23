include (../../sharedfilter.pri)


QT           += opengl

HEADERS       += samplefilterdoc.h \
		../../common/filterparameter.h \
		../../meshlab/meshmodel.h

SOURCES       += samplefilterdoc.cpp \ 
		 \
		../../meshlab/meshmodel.cpp \
		$$GLEWCODE

TARGET        = samplefilterdoc
