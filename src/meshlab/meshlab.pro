HEADERS        = interfaces.h \
                 mainwindow.h \
                 meshmodel.h \
                 glarea.h \
                 plugindialog.h \
                 ../../../sf/wrap/gui/trackball.h\
                 ../../../sf/wrap/gui/trackmode.h\
                 ../../../sf/wrap/gl/trimesh.h
SOURCES        = main.cpp \
                 mainwindow.cpp \
                 meshmodel.cpp \
                 glarea.cpp \
                 plugindialog.cpp \
                 ../../../sf/wrap/ply/plylib.cpp\
                 ../../../sf/wrap/gui/trackball.cpp\
                 ../../../sf/wrap/gui/trackmode.cpp
RESOURCES     = meshlab.qrc
QT           += opengl

INCLUDEPATH += . ../../../sf
CONFIG += stl
win32:LIBS	+= C:\code\glew\lib\glew32.lib 
unix:LIBS	+= -lGLEW
# install
target.path = $$[QT_INSTALL_EXAMPLES]/tools/plugandpaint
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS plugandpaint.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tools/plugandpaint
INSTALLS += target sources
