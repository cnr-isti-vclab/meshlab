include (../../shared.pri)


HEADERS       = edit_sample_factory.h \
				sampleedit.h
				 
SOURCES       = edit_sample_factory.cpp \
				sampleedit.cpp \
				$$GLEWCODE

TARGET        = sampleedit

QT           += opengl
RESOURCES     = sampleedit.qrc
