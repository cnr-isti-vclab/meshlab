TEMPLATE = app
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
	main.cpp

#substitute with the dir where libIDTFConverter has been compiled
LIB_IDTF_CONVERTER_BUILD_DIR=../build-u3d-Desktop_Qt_5_14_2_GCC_64bit-Release

LIBS += -L$${LIB_IDTF_CONVERTER_BUILD_DIR} -lIDTFConverter


