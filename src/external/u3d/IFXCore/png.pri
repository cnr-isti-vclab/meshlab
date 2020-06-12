PNG_SOURCE_DIR=../src/RTL/Dependencies/png

#if(MSVC)
#add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
#endif(MSVC)


INCLUDEPATH += \
	$${PNG_SOURCE_DIR}

HEADERS += \
	$${PNG_SOURCE_DIR}/png.h \
	$${PNG_SOURCE_DIR}/pngconf.h \
	$${PNG_SOURCE_DIR}/pnglibconf.h

SOURCES += \
	$${PNG_SOURCE_DIR}/pngdebug.h \
	$${PNG_SOURCE_DIR}/pnginfo.h \
	$${PNG_SOURCE_DIR}/pngpriv.h \
	$${PNG_SOURCE_DIR}/pngstruct.h \
	$${PNG_SOURCE_DIR}/png.c \
	$${PNG_SOURCE_DIR}/pngerror.c \
	$${PNG_SOURCE_DIR}/pngget.c \
	$${PNG_SOURCE_DIR}/pngmem.c \
	$${PNG_SOURCE_DIR}/pngpread.c \
	$${PNG_SOURCE_DIR}/pngread.c \
	$${PNG_SOURCE_DIR}/pngrio.c \
	$${PNG_SOURCE_DIR}/pngrtran.c \
	$${PNG_SOURCE_DIR}/pngrutil.c \
	$${PNG_SOURCE_DIR}/pngset.c \
	$${PNG_SOURCE_DIR}/pngtrans.c \
	$${PNG_SOURCE_DIR}/pngwio.c \
	$${PNG_SOURCE_DIR}/pngwrite.c \
	$${PNG_SOURCE_DIR}/pngwtran.c \
	$${PNG_SOURCE_DIR}/pngwutil.c

