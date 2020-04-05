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
