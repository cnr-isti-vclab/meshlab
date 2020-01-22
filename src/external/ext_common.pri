linux:DESTDIR = $$EXTERNAL_BASE_PATH/lib/linux-g++
macx:DESTDIR       = $$EXTERNAL_BASE_PATH/lib/macx64
win32-g++:DESTDIR       = $$EXTERNAL_BASE_PATH/lib/win32-gcc  
win32-msvc:DESTDIR       = $$EXTERNAL_BASE_PATH/lib/win32-msvc

unix:CONFIG(release, debug|release) {
	DEFINES *= NDEBUG
}

QMAKE_CXXFLAGS+=-w
QMAKE_CFLAGS+=-w
