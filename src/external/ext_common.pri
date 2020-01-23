linux:DESTDIR      = $$MESHLAB_DISTRIB_DIRECTORY/lib/linux-g++
macx:DESTDIR       = $$MESHLAB_DISTRIB_DIRECTORY/lib/macx64
win32-g++:DESTDIR  = $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-gcc
win32-msvc:DESTDIR = $$MESHLAB_DISTRIB_DIRECTORY/lib/win32-msvc

unix:CONFIG(release, debug|release) {
	DEFINES *= NDEBUG
}

#not showing warnings from external libraries
QMAKE_CXXFLAGS+=-w
QMAKE_CFLAGS+=-w
