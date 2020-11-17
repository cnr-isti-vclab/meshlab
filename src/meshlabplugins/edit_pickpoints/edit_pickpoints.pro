include (../../shared.pri)

QT += network

HEADERS += \
	edit_pickpoints_factory.h \
	editpickpoints.h \
	pickpointsDialog.h \
	pickedPoints.h \
	pickPointsTemplate.h \
	../../meshlab/rich_parameter_gui/richparameterwidgets.h \
	../../meshlab/rich_parameter_gui/richparameterlistframe.h \
	../../meshlab/rich_parameter_gui/richparameterlistdialog.h

SOURCES += \
	edit_pickpoints_factory.cpp \
	editpickpoints.cpp \
	pickpointsDialog.cpp \
	pickedPoints.cpp \
	pickPointsTemplate.cpp \
	../../meshlab/rich_parameter_gui/richparameterwidgets.cpp \
	../../meshlab/rich_parameter_gui/richparameterlistframe.cpp \
	../../meshlab/rich_parameter_gui/richparameterlistdialog.cpp

RESOURCES += \
	editpickpoints.qrc

FORMS += \
	pickpointsDialog.ui

TARGET = edit_pickpoints
