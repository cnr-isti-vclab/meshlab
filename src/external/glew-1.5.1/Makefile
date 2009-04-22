#!gmake
## The OpenGL Extension Wrangler Library
## Copyright (C) 2002-2008, Milan Ikits <milan ikits[]ieee org>
## Copyright (C) 2002-2008, Marcelo E. Magallon <mmagallo[]debian org>
## Copyright (C) 2002, Lev Povalahev
## All rights reserved.
## 
## Redistribution and use in source and binary forms, with or without 
## modification, are permitted provided that the following conditions are met:
## 
## * Redistributions of source code must retain the above copyright notice, 
##   this list of conditions and the following disclaimer.
## * Redistributions in binary form must reproduce the above copyright notice, 
##   this list of conditions and the following disclaimer in the documentation 
##   and/or other materials provided with the distribution.
## * The name of the author may be used to endorse or promote products 
##   derived from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
## AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
## LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
## CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
## SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
## INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
## CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
## THE POSSIBILITY OF SUCH DAMAGE.

include config/version

SHELL = /bin/sh
SYSTEM = $(shell config/config.guess | cut -d - -f 3 | sed -e 's/[0-9\.]//g;')
SYSTEM.SUPPORTED = $(shell test -f config/Makefile.$(SYSTEM) && echo 1)

ifeq ($(SYSTEM.SUPPORTED), 1)
include config/Makefile.$(SYSTEM)
else
$(error "Platform '$(SYSTEM)' not supported")
endif

GLEW_DEST ?= /usr
BINDIR ?= $(GLEW_DEST)/bin
LIBDIR ?= $(GLEW_DEST)/lib
INCDIR ?= $(GLEW_DEST)/include/GL
SHARED_OBJ_EXT ?= o
TARDIR = ../glew-$(GLEW_VERSION)
TARBALL = ../glew_$(GLEW_VERSION).tar.gz

AR = ar
INSTALL = install
STRIP = strip
RM = rm -f
LN = ln -sf
ifeq ($(MAKECMDGOALS), debug)
OPT = -g
else
OPT = $(POPT)
endif
INCLUDE = -Iinclude
CFLAGS = $(OPT) $(WARN) $(INCLUDE) $(CFLAGS.EXTRA)

LIB.SRCS = src/glew.c
LIB.OBJS = $(LIB.SRCS:.c=.o)
LIB.SOBJS = $(LIB.SRCS:.c=.$(SHARED_OBJ_EXT))
LIB.LDFLAGS = $(LDFLAGS.EXTRA) $(LDFLAGS.GL)
LIB.LIBS = $(GL_LDFLAGS)

GLEWINFO.BIN = glewinfo$(BIN.SUFFIX)
GLEWINFO.BIN.SRCS = src/glewinfo.c
GLEWINFO.BIN.OBJS = $(GLEWINFO_BIN.SRCS:.c=.o)
VISUALINFO.BIN = visualinfo$(BIN.SUFFIX)
VISUALINFO.BIN.SRCS = src/visualinfo.c
VISUALINFO.BIN.OBJS = $(VISUALINFO_BIN.SRCS:.c=.o)
BIN.LIBS = -Llib $(LDFLAGS.DYNAMIC) -l$(NAME) $(LDFLAGS.EXTRA) $(LDFLAGS.GL)

all debug: lib/$(LIB.SHARED) lib/$(LIB.STATIC) bin/$(GLEWINFO.BIN) bin/$(VISUALINFO.BIN)

lib:
	mkdir lib

lib/$(LIB.STATIC): $(LIB.OBJS)
	$(AR) cr $@ $^

lib/$(LIB.SHARED): $(LIB.SOBJS)
	$(LD) $(LDFLAGS.SO) -o $@ $^ $(LIB.LDFLAGS) $(LIB.LIBS)
ifeq ($(patsubst mingw%,mingw,$(SYSTEM)), mingw)
else
	$(LN) $(LIB.SHARED) lib/$(LIB.SONAME)
	$(LN) $(LIB.SHARED) lib/$(LIB.DEVLNK)
endif

bin/$(GLEWINFO.BIN): $(GLEWINFO.BIN.SRCS) lib/$(LIB.SHARED)
	$(CC) $(CFLAGS) -o $@ $(GLEWINFO.BIN.SRCS) $(BIN.LIBS)

bin/$(VISUALINFO.BIN): $(VISUALINFO.BIN.SRCS) lib/$(LIB.SHARED)
	$(CC) $(CFLAGS) -o $@ $(VISUALINFO.BIN.SRCS) $(BIN.LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

src/glew.o: src/glew.c include/GL/glew.h include/GL/wglew.h include/GL/glxew.h
	$(CC) $(CFLAGS) $(CFLAGS.SO) -o $@ -c $<

src/glew.pic_o: src/glew.c include/GL/glew.h include/GL/wglew.h include/GL/glxew.h
	$(CC) $(CFLAGS) $(PICFLAG) $(CFLAGS.SO) -o $@ -c $<

install: all
# directories
	$(INSTALL) -d -m 0755 $(BINDIR)
	$(INSTALL) -d -m 0755 $(INCDIR)
	$(INSTALL) -d -m 0755 $(LIBDIR)
# runtime
ifeq ($(patsubst mingw%,mingw,$(SYSTEM)), mingw)
	$(STRIP) -x lib/$(LIB.SHARED)
	$(INSTALL) -m 0644 lib/$(LIB.SHARED) $(BINDIR)/
else
	$(STRIP) -x lib/$(LIB.SHARED)
	$(INSTALL) -m 0644 lib/$(LIB.SHARED) $(LIBDIR)/
	$(LN) $(LIB.SHARED) $(LIBDIR)/$(LIB.SONAME)
endif
# development files
	$(INSTALL) -m 0644 include/GL/wglew.h $(INCDIR)/
	$(INSTALL) -m 0644 include/GL/glew.h $(INCDIR)/
	$(INSTALL) -m 0644 include/GL/glxew.h $(INCDIR)/
ifeq ($(patsubst mingw%,mingw,$(SYSTEM)), mingw)
	$(INSTALL) -m 0644 lib/$(LIB.DEVLNK) $(LIBDIR)/
else
	strip -x lib/$(LIB.STATIC)
	$(INSTALL) -m 0644 lib/$(LIB.STATIC) $(LIBDIR)/
	$(LN) $(LIB.SHARED) $(LIBDIR)/$(LIB.DEVLNK)
endif
# utilities
	$(INSTALL) -s -m 0755 bin/$(GLEWINFO.BIN) bin/$(VISUALINFO.BIN) $(BINDIR)/

uninstall:
	$(RM) $(INCDIR)/wglew.h
	$(RM) $(INCDIR)/glew.h
	$(RM) $(INCDIR)/glxew.h
	$(RM) $(LIBDIR)/$(LIB.DEVLNK)
ifeq ($(patsubst mingw%,mingw,$(SYSTEM)), mingw)
	$(RM) $(BINDIR)/$(LIB.SHARED)
else
	$(RM) $(LIBDIR)/$(LIB.SONAME)
	$(RM) $(LIBDIR)/$(LIB.SHARED)
	$(RM) $(LIBDIR)/$(LIB.STATIC)
endif
	$(RM) $(BINDIR)/$(GLEWINFO.BIN) $(BINDIR)/$(VISUALINFO.BIN)

clean:
	$(RM) $(LIB.OBJS)
	$(RM) $(LIB.SOBJS)
	$(RM) lib/$(LIB.STATIC) lib/$(LIB.SHARED) lib/$(LIB.DEVLNK) lib/$(LIB.SONAME) $(LIB.STATIC)
	$(RM) $(GLEWINFO.BIN.OBJS) bin/$(GLEWINFO.BIN) $(VISUALINFO.BIN.OBJS) bin/$(VISUALINFO.BIN)
# Compiler droppings
	$(RM) so_locations

distclean: clean
	find . -name \*~ | xargs $(RM)
	find . -name .\*.sw\? | xargs $(RM)

tardist:
	$(RM) -r $(TARDIR)
	mkdir $(TARDIR)
	cp -a . $(TARDIR)
	find $(TARDIR) -name CVS -o -name .cvsignore | xargs $(RM) -r
	$(MAKE) -C $(TARDIR) distclean
	$(MAKE) -C $(TARDIR)
	$(MAKE) -C $(TARDIR) distclean
	$(RM) -r $(TARDIR)/auto/registry
	env GZIP=-9 tar -C `dirname $(TARDIR)` -cvzf $(TARBALL) `basename $(TARDIR)`

extensions:
	$(MAKE) -C auto

.PHONY: clean distclean tardist
