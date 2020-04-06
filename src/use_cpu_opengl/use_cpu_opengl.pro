# This project is supposed to be compiled only with windows.
# It creates a binary file inside the MESHLAB_DISTRIB folder that is called
# UseCPUOpenGL. This binary file is automatically run by meshlab
# only if glewInit() fails (therefore, drivers do not support newer OpenGL
# versions). When UseCPUOpenGL is run, a file called "opengl32.dll" will be
# placed in the meshlab folder, overriding the system one and making meshlab
# able to start using a CPU implementation of OpenGL.

win32 {

TEMPLATE = app
TARGET = UseCPUOpenGL
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= debug_and_release

QT += core widgets

DESTDIR = $$MESHLAB_DISTRIB_DIRECTORY
CONFIG += embed_manifest_exe
QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:level=\'requireAdministrator\'

SOURCES += \
    main.cpp

RESOURCES += \
    res.qrc

}
