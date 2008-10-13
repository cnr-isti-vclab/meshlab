include (../../shared.pri)

HEADERS       = io_expe.h \
				../../meshlab/meshmodel.h

SOURCES       = io_expe.cpp \
				../../meshlab/meshmodel.cpp \
				$$GLEWCODE

TARGET        = io_expe

CONFIG       += opengl