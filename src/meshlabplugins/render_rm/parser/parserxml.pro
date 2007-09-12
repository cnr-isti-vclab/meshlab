TEMPLATE = app
CONFIG += QT
QT += xml

win32 { 
	CONFIG += console
}

SOURCES +=	main.cpp \
			RmXmlParser.cpp \
			RmPass.cpp \
			UniformVar.cpp



HEADERS +=  RmXmlParser.h \
			RmEffect.h \
			RmPass.h \
			UniformVar.h \
			GlState.h
