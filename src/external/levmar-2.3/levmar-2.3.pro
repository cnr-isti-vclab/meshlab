EXTERNAL_BASE_PATH = ../
include($$EXTERNAL_BASE_PATH/ext_common.pri)
TEMPLATE = lib
TARGET = levmar
CONFIG += staticlib
DEPENDPATH += .
INCLUDEPATH += .

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
 

