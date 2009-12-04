include (../../shared.pri)

HEADERS       += filter_texture.h \
		pushpull.h \
		rastering.h

SOURCES       += filter_texture.cpp  
		
TARGET        = filter_texture

win32-msvc2005:DEFINES	+= _USE_MATH_DEFINES
win32-msvc2008:DEFINES	+= _USE_MATH_DEFINES
