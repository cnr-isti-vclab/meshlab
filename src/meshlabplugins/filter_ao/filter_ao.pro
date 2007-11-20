
TEMPLATE = lib
CONFIG += plugin
TARGET = filter_ao
DESTDIR = ../../meshlab/plugins
QT += opengl
CONFIG += debug_and_release


INCLUDEPATH += ./../.. \
    ./../../../../sf \
    ./../../../../code/lib/glew/include \
    ./debug \
    .
LIBS += -lopengl32 \
    -lglu32 \
    -l../../../../code/lib/glew/lib/glew32 \
    -l../../../../code/lib/glut/lib/glut32


win32{
  DEFINES += NOMINMAX
}

DEPENDPATH += .

OBJECTS_DIR += debug

HEADERS += ./filter_ao.h
SOURCES += ./filter_ao.cpp \
    ../../meshlab/filterparameter.cpp

