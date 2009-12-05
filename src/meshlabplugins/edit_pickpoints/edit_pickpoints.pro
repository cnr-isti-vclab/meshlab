include (../../shared.pri)

HEADERS      += edit_pickpoints_factory.h \
				editpickpoints.h \
				pickpointsDialog.h \
				pickedPoints.h \
				pickPointsTemplate.h \
				../../meshlab/stdpardialog.h

SOURCES      += edit_pickpoints_factory.cpp \
				editpickpoints.cpp \
				pickpointsDialog.cpp \
				pickedPoints.cpp \
				pickPointsTemplate.cpp \
                ../../meshlab/stdpardialog.cpp

RESOURCES    += editpickpoints.qrc

FORMS        += pickpointsDialog.ui

TARGET        = edit_pickpoints

QT           += opengl 
QT           += xml
