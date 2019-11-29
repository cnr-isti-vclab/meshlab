include (../../shared.pri)

HEADERS       = filter_rangemap.h

SOURCES       = filter_rangemap.cpp \ 
		../../meshlab/filterparameter.cpp
!CONFIG(system_glew): SOURCES += $$GLEWCODE
CONFIG(system_glew) {
    linux: LIBS += -lGLEW
}

TARGET        = filter_rangemap

CONFIG       += opengl



