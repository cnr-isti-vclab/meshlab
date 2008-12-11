include (../../shared.pri)

HEADERS       = meshcolorize.h curvature.h equalizerDialog.h smoothcolor.h
SOURCES       = meshcolorize.cpp \
		equalizerDialog.cpp\ 
		../../meshlab/filterparameter.cpp

TARGET        = filter_colorize

FORMS		= equalizerDialog.ui

QT           += opengl