include (../../shared.pri)


HEADERS       = sampleedit.h 
SOURCES       = sampleedit.cpp $$GLEWCODE
TARGET        = sampleedit

QT           += opengl
RESOURCES     = sampleedit.qrc
