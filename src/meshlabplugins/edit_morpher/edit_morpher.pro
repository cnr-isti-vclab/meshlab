include (../../shared.pri)

HEADERS       = morpher.h \
				morpherDialog.h \
				../../meshlab/meshmodel.h

SOURCES       = morpher.cpp \
				morpherDialog.cpp

FORMS        += morpherDialog.ui

TARGET        = edit_morpher

QT           += opengl

RESOURCES    += morpher.qrc
