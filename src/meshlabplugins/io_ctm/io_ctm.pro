include (../../shared.pri)
HEADERS += io_ctm.h \
    $$VCGDIR/wrap/io_trimesh/import_ctm.h \
    $$VCGDIR/wrap/io_trimesh/export_ctm.h
SOURCES += io_ctm.cpp
TARGET = io_ctm

!CONFIG(system_openctm): INCLUDEPATH *= ../../external/OpenCTM-1.0.3/lib

win32-msvc:QMAKE_CXXFLAGS += /DOPENCTM_STATIC
win32-g++:DEFINES += OPENCTM_STATIC

win32-msvc:LIBS	+= ../../external/lib/win32-msvc/openctm.lib
win32-g++:LIBS		+= ../../external/lib/win32-gcc/libopenctm.a
linux-g++:LIBS		+= -lopenctm
#macx:LIBS += ../../external/lib/macx/libopenctm.a
macx:LIBS += $$MACLIBDIR/libopenctm.a
