include (../../shared.pri)


HEADERS       = edit_sample_factory.h \
				sampleedit.h
				 
SOURCES       = edit_sample_factory.cpp \
				sampleedit.cpp 

TARGET        = sampleedit

QT           += opengl
RESOURCES     = sampleedit.qrc
