EXTERNAL_BASE_PATH = ../
include($$EXTERNAL_BASE_PATH/ext_common.pri)
TEMPLATE = lib
TARGET = levmar
CONFIG += staticlib
DEPENDPATH += .
INCLUDEPATH += .
macx:QMAKE_CFLAGS += -Wno-unused-parameter -Wno-format -Wno-pointer-sign -Wno-deprecated-declarations
macx:QMAKE_CFLAGS_WARN_ON = 

# Input
HEADERS += compiler.h \
		   lm.h\
		   misc.h
SOURCES += Axb.c \
           lm.c \
           lmbc.c \
           lmblec.c \
           lmlec.c \
           misc.c\
 

