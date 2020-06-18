ZLIB_SOURCE_DIR=../src/RTL/Dependencies/zlib

#if not check_include_file(unistd.h)
# define ZLIB_WITH_Z_PREFIX


INCLUDEPATH += \
	$${ZLIB_SOURCE_DIR}

HEADERS += \
	$${ZLIB_SOURCE_DIR}/zconf.h \
	$${ZLIB_SOURCE_DIR}/zlib.h \
	$${ZLIB_SOURCE_DIR}/crc32.h \
	$${ZLIB_SOURCE_DIR}/deflate.h \
	$${ZLIB_SOURCE_DIR}/gzguts.h \
	$${ZLIB_SOURCE_DIR}/inffast.h \
	$${ZLIB_SOURCE_DIR}/inffixed.h \
	$${ZLIB_SOURCE_DIR}/inflate.h \
	$${ZLIB_SOURCE_DIR}/inftrees.h \
	$${ZLIB_SOURCE_DIR}/trees.h \
	$${ZLIB_SOURCE_DIR}/zutil.h

SOURCES += \
	$${ZLIB_SOURCE_DIR}/adler32.c \
	$${ZLIB_SOURCE_DIR}/compress.c \
	$${ZLIB_SOURCE_DIR}/crc32.c \
	$${ZLIB_SOURCE_DIR}/deflate.c \
	$${ZLIB_SOURCE_DIR}/gzclose.c \
	$${ZLIB_SOURCE_DIR}/gzlib.c \
	$${ZLIB_SOURCE_DIR}/gzread.c \
	$${ZLIB_SOURCE_DIR}/gzwrite.c \
	$${ZLIB_SOURCE_DIR}/inflate.c \
	$${ZLIB_SOURCE_DIR}/infback.c \
	$${ZLIB_SOURCE_DIR}/inftrees.c \
	$${ZLIB_SOURCE_DIR}/inffast.c \
	$${ZLIB_SOURCE_DIR}/trees.c \
	$${ZLIB_SOURCE_DIR}/uncompr.c \
	$${ZLIB_SOURCE_DIR}/zutil.c

