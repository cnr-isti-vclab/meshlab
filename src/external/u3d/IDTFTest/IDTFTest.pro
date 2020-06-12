TEMPLATE = app
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt
TARGET = IDTFTestConverter
#DESTDIR = ../

INCLUDEPATH += \
	../src/IDTF/

SOURCES += \
	../src/IDTFTest/main.cpp

#substitute with the dir where libIDTFConverter has been compiled
LIB_IDTF_CONVERTER_BUILD_DIR=../

linux|macx {
	LIBS += -L$${LIB_IDTF_CONVERTER_BUILD_DIR} -lIDTFConverter -lIFXCoreStatic -ldl
}

win32 {
	CONFIG += console
	CONFIG -= debug_and_release
	LIBS += winmm.lib user32.lib -L$${LIB_IDTF_CONVERTER_BUILD_DIR} -lIDTFConverter -lIFXCoreStatic
}

