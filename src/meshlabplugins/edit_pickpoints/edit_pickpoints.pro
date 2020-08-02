include (../../shared.pri)

HEADERS += \
	edit_pickpoints_factory.h \
	editpickpoints.h \
	pickpointsDialog.h \
	pickedPoints.h \
	pickPointsTemplate.h \
	../../meshlab/meshlab_settings/richparameterlistframe.h \
	../../meshlab/meshlab_settings/richparameterlistdialog.h

SOURCES += \
	edit_pickpoints_factory.cpp \
	editpickpoints.cpp \
	pickpointsDialog.cpp \
	pickedPoints.cpp \
	pickPointsTemplate.cpp \
	../../meshlab/meshlab_settings/richparameterlistframe.cpp \
	../../meshlab/meshlab_settings/richparameterlistdialog.cpp

RESOURCES += \
	editpickpoints.qrc

FORMS += \
	pickpointsDialog.ui

TARGET = edit_pickpoints

