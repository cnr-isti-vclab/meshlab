# this is the common include for all the plugins
include (../../shared.pri)

INCLUDEPATH +=  ../../external/openkinect/include \
                /usr/include/libusb-1.0/

LIBS += /media/ACER_/devel/meshlab/src/external/lib/linux/libfreenect.a \
        /usr/lib/libusb-1.0.a

HEADERS = 	edit_kinect_factory.h \
                ui_kinect.h \
                edit_kinect.h \
    calibration_data.h

SOURCES =   edit_kinect_factory.cpp \
            edit_kinect.cpp \ # ../../../../meshlab/src/meshlab/glarea.cpp \
            freenect.cpp \
            ../../../../vcglib/wrap/gui/trackmode.cpp \
            ../../../../vcglib/wrap/gui/trackball.cpp \ # ../../../../vcglib/wrap/gui/rubberband.cpp \
    calibration_load.cpp

TARGET = edit_kinect
QT += opengl
RESOURCES = edit_kinect.qrc
FORMS += kinect.ui

# OCME src/ocme FILES
# Lib headers
