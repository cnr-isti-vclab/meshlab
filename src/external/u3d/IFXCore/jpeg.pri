JPEG_SOURCE_DIR=../src/RTL/Dependencies/jpeg

#if(MSVC)
#add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
#endif(MSVC)

#if check_include_file(stddef.h)
# define JPEG_WITH_STDDEF

INCLUDEPATH += \
	$${JPEG_SOURCE_DIR}

HEADERS += \
	$${JPEG_SOURCE_DIR}/jerror.h \
	$${JPEG_SOURCE_DIR}/jmorecfg.h \
	$${JPEG_SOURCE_DIR}/jpeglib.h \
	$${JPEG_SOURCE_DIR}/jconfig.h \
	$${JPEG_SOURCE_DIR}/cderror.h \
	$${JPEG_SOURCE_DIR}/jdct.h \
	$${JPEG_SOURCE_DIR}/jinclude.h \
	$${JPEG_SOURCE_DIR}/jmemsys.h \
	$${JPEG_SOURCE_DIR}/jpegint.h \
	$${JPEG_SOURCE_DIR}/jversion.h \
	$${JPEG_SOURCE_DIR}/transupp.h

#additional source
# JPEG_systemdependent_SRCS
# $${JPEG_SOURCE_DIR}/jmemnobs.c

macx|win32 {
    SOURCES += \
	    $${JPEG_SOURCE_DIR}/jmemnobs.c
}

SOURCES += \
	$${JPEG_SOURCE_DIR}/jaricom.c \
	$${JPEG_SOURCE_DIR}/jcapimin.c \
	$${JPEG_SOURCE_DIR}/jcapistd.c \
	$${JPEG_SOURCE_DIR}/jcarith.c \
	$${JPEG_SOURCE_DIR}/jccoefct.c \
	$${JPEG_SOURCE_DIR}/jccolor.c \
	$${JPEG_SOURCE_DIR}/jcdctmgr.c \
	$${JPEG_SOURCE_DIR}/jchuff.c \
	$${JPEG_SOURCE_DIR}/jcinit.c \
	$${JPEG_SOURCE_DIR}/jcmainct.c \
	$${JPEG_SOURCE_DIR}/jcmarker.c \
	$${JPEG_SOURCE_DIR}/jcmaster.c \
	$${JPEG_SOURCE_DIR}/jcomapi.c \
	$${JPEG_SOURCE_DIR}/jcparam.c \
	$${JPEG_SOURCE_DIR}/jcprepct.c \
	$${JPEG_SOURCE_DIR}/jcsample.c \
	$${JPEG_SOURCE_DIR}/jctrans.c \
	$${JPEG_SOURCE_DIR}/jdapimin.c \
	$${JPEG_SOURCE_DIR}/jdapistd.c \
	$${JPEG_SOURCE_DIR}/jdarith.c \
	$${JPEG_SOURCE_DIR}/jdatadst.c \
	$${JPEG_SOURCE_DIR}/jdatasrc.c \
	$${JPEG_SOURCE_DIR}/jdcoefct.c \
	$${JPEG_SOURCE_DIR}/jdcolor.c \
	$${JPEG_SOURCE_DIR}/jddctmgr.c \
	$${JPEG_SOURCE_DIR}/jdhuff.c \
	$${JPEG_SOURCE_DIR}/jdinput.c \
	$${JPEG_SOURCE_DIR}/jdmainct.c \
	$${JPEG_SOURCE_DIR}/jdmarker.c \
	$${JPEG_SOURCE_DIR}/jdmaster.c \
	$${JPEG_SOURCE_DIR}/jdmerge.c \
	$${JPEG_SOURCE_DIR}/jdpostct.c \
	$${JPEG_SOURCE_DIR}/jdsample.c \
	$${JPEG_SOURCE_DIR}/jdtrans.c \
	$${JPEG_SOURCE_DIR}/jerror.c \
	$${JPEG_SOURCE_DIR}/jfdctflt.c \
	$${JPEG_SOURCE_DIR}/jfdctfst.c \
	$${JPEG_SOURCE_DIR}/jfdctint.c \
	$${JPEG_SOURCE_DIR}/jidctflt.c \
	$${JPEG_SOURCE_DIR}/jidctfst.c \
	$${JPEG_SOURCE_DIR}/jidctint.c \
	$${JPEG_SOURCE_DIR}/jquant1.c \
	$${JPEG_SOURCE_DIR}/jquant2.c \
	$${JPEG_SOURCE_DIR}/jutils.c \
	$${JPEG_SOURCE_DIR}/jmemmgr.c
