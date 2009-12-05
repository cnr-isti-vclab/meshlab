include (../../shared.pri)

HEADERS       += io_x3d.h import_x3d.h\
		export_x3d.h util_x3d.h \
		./vrml/Parser.h \
		./vrml/Scanner.h
				
SOURCES       += io_x3d.cpp ./vrml/Parser.cpp ./vrml/Scanner.cpp
QT           += opengl xml
				
TARGET        = io_x3d

