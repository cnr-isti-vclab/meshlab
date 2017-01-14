EXTERNAL_BASE_PATH = ../
include($$EXTERNAL_BASE_PATH/ext_common.pri)
TEMPLATE = lib
TARGET = openctm
CONFIG += staticlib
DEPENDPATH += .
INCLUDEPATH += ./lib ./lib/liblzma

win32-msvc2005:QMAKE_CXXFLAGS += /nologo /Ox /W3 /c /DOPENCTM_STATIC /D_CRT_SECURE_NO_WARNINGS 
win32-msvc2008:QMAKE_CXXFLAGS += /nologo /Ox /W3 /c /DOPENCTM_STATIC /D_CRT_SECURE_NO_WARNINGS 
win32-msvc2010:QMAKE_CXXFLAGS += /nologo /Ox /W3 /c /DOPENCTM_STATIC /D_CRT_SECURE_NO_WARNINGS
win32-msvc2012:QMAKE_CXXFLAGS += /nologo /Ox /W3 /c /DOPENCTM_STATIC /D_CRT_SECURE_NO_WARNINGS
win32-msvc2015:QMAKE_CXXFLAGS += /nologo /Ox /W3 /c /DOPENCTM_STATIC /D_CRT_SECURE_NO_WARNINGS    
win32-msvc2015:DEFINES += OPENCTM_STATIC
win32-g++:QMAKE_CXXFLAGS += -O3 -W -Wall -c -std=c99 -pedantic
win32-g++:DEFINES += OPENCTM_STATIC
linux-g++:QMAKE_CXXFLAGS += -O3 -W -Wall -c -std=c99 -pedantic
linux-g++:DEFINES += OPENCTM_STATIC
linux-g++-32:QMAKE_CXXFLAGS += -O3 -W -Wall -c -std=c99 -pedantic
linux-g++-32:DEFINES += OPENCTM_STATIC
linux-g++-64:QMAKE_CXXFLAGS += -O3 -W -Wall -c -std=c99 -pedantic
linux-g++-64:DEFINES += OPENCTM_STATIC

# Input
SOURCES += 	./lib/openctm.c \
			./lib/stream.c \
			./lib/compressRAW.c \
			./lib/compressMG1.c \
			./lib/compressMG2.c \
			./lib/liblzma/Alloc.c \
            ./lib/liblzma/LzFind.c \
            ./lib/liblzma/LzmaDec.c \
            ./lib/liblzma/LzmaEnc.c \
            ./lib/liblzma/LzmaLib.c
