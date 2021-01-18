include (../../shared.pri)

HEADERS       += filter_hqrender.h \
		export_rib.h \
		RibFileStack.h \
		utilities_hqrender.h \
		ribProcedure.h
			

SOURCES       += filter_hqrender.cpp \
		RibFileStack.cpp \
		parser_rib.cpp \
		utilities_hqrender.cpp

TARGET        = filter_hqrender
