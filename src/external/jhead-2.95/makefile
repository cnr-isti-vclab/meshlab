#--------------------------------
# jhead makefile for Unix
#--------------------------------
OBJ=.
SRC=.
CFLAGS= -O3 -Wall

all: jhead

objs = $(OBJ)/jhead.o $(OBJ)/jpgfile.o $(OBJ)/paths.o \
	$(OBJ)/exif.o $(OBJ)/iptc.o $(OBJ)/gpsinfo.o $(OBJ)/makernote.o 

$(OBJ)/%.o:$(SRC)/%.c
	${CC} $(CFLAGS) -c $< -o $@

jhead: $(objs) jhead.h
	${CC} -o jhead $(objs) -lm

clean:
	rm -f $(objs) jhead

install:
	cp jhead ${DESTDIR}/usr/local/bin/
