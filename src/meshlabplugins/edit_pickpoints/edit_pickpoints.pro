include (../../shared.pri)

HEADERS      += editpickpoints.h \
				pickpointsDialog.h \
				pickedPoints.h \
				pickPointsTemplate.h \
				../../meshlab/stdpardialog.h

SOURCES      += editpickpoints.cpp \
				pickpointsDialog.cpp \
				pickedPoints.cpp \
				pickPointsTemplate.cpp \
				../../meshlab/stdpardialog.cpp \
				../../meshlab/filterparameter.cpp \
				$$GLEWCODE

RESOURCES    += editpickpoints.qrc

FORMS        += pickpointsDialog.ui

TARGET        = edit_pickpoints

QT           += opengl 
QT           += xml
