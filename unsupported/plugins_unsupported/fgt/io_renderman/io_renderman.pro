include (../../shared.pri)
HEADERS += io_renderman.h \
    		export_rib.h \
		RibFileStack.h \
		utilities_hqrender.h \
		ribProcedure.h

SOURCES += io_renderman.cpp \
		RibFileStack.cpp \
		parser_rib.cpp \
		utilities_hqrender.cpp


TARGET = io_renderman
