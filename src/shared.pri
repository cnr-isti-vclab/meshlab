# This is the common include for all the plugins

include (general.pri)

TEMPLATE = lib
CONFIG += plugin
QT += opengl
QT += xml
QT += xmlpatterns

win32-msvc:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/lib/meshlab-common.lib -lopengl32 -lGLU32
win32-g++:LIBS += -lmeshlab-common -lopengl32 -lGLU32
linux:LIBS += -fopenmp -lmeshlab-common -lGL -lGLU

macx:LIBS += $$MESHLAB_DISTRIB_DIRECTORY/lib/libmeshlab-common.dylib
macx:QMAKE_POST_LINK = " \ #every plugin needs to point to meshlab-common placed in the appbundle
    install_name_tool -change libmeshlab-common.1.dylib @rpath/libmeshlab-common.1.dylib $$MESHLAB_DISTRIB_DIRECTORY/plugins/lib$${TARGET}.dylib; \
"

win32-msvc:DEFINES += GLEW_STATIC _USE_MATH_DEFINES

INCLUDEPATH *= ../.. $$VCGDIR $$EIGENDIR
!CONFIG(system_glew): INCLUDEPATH *=  $$GLEWDIR/include
DEPENDPATH += ../.. $$VCGDIR

CONFIG(release,debug | release){
# Uncomment the following line to disable assert in mingw
#DEFINES += NDEBUG
 }

DESTDIR = $$MESHLAB_DISTRIB_DIRECTORY/plugins
# uncomment in you local copy only in emergency cases.
# We should never be too permissive
# win32-g++:QMAKE_CXXFLAGS += -fpermissive

contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
