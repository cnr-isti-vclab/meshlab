unix:CONFIG(release, debug|release) {
	DEFINES *= NDEBUG
}

#not showing warnings from external libraries
QMAKE_CXXFLAGS+=-w
QMAKE_CFLAGS+=-w

DESTDIR = $$MESHLAB_DISTRIB_EXT_DIRECTORY

