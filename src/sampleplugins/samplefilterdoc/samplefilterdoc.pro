include (../../shared.pri)


QT           += opengl

HEADERS       = samplefilterdoc.h \
		../../meshlab/filterparameter.h \
		../../meshlab/meshmodel.h

SOURCES       = samplefilterdoc.cpp \ 
		../../meshlab/filterparameter.cpp \
		../../meshlab/meshmodel.cpp \
		$$GLEWCODE

TARGET        = samplefilterdoc
