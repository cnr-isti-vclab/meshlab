include (../../shared.pri)

HEADERS += \
    edit_pickpoints_factory.h \
    editpickpoints.h \
    pickpointsDialog.h \
    pickedPoints.h \
    pickPointsTemplate.h \
	../../meshlab/stdparframe/stdparframe.h

SOURCES += \
    edit_pickpoints_factory.cpp \
    editpickpoints.cpp \
    pickpointsDialog.cpp \
    pickedPoints.cpp \
    pickPointsTemplate.cpp \
	../../meshlab/stdparframe/stdparframe.cpp

RESOURCES += \
    editpickpoints.qrc

FORMS += \
    pickpointsDialog.ui

TARGET = edit_pickpoints

