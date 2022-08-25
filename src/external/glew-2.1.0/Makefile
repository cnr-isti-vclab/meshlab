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
SYSTEM ?= $(shell config/config.guess | cut -d - -f 3 | sed -e 's/[0-9\.]//g;')
SYSTEM.SUPPORTED = $(shell test -f config/Makefile.$(SYSTEM) && echo 1)

ifeq ($(SYSTEM.SUPPORTED), 1)
include config/Makefile.$(SYSTEM)
else
$(error "Platform '$(SYSTEM)' not supported")
endif

GLEW_PREFIX ?= /usr
GLEW_DEST ?= /usr
BINDIR    ?= $(GLEW_DEST)/bin
LIBDIR    ?= $(GLEW_DEST)/lib
INCDIR    ?= $(GLEW_DEST)/include/GL
PKGDIR    ?= $(GLEW_DEST)/lib/pkgconfig

ifneq ($(GLEW_NO_GLU), -DGLEW_NO_GLU)
LIBGLU = glu
endif

DIST_NAME    ?= glew-$(GLEW_VERSION)
DIST_SRC_ZIP ?= $(shell pwd)/$(DIST_NAME).zip
DIST_SRC_TGZ ?= $(shell pwd)/$(DIST_NAME).tgz
DIST_WIN32   ?= $(shell pwd)/$(DIST_NAME)-win32.zip

DIST_DIR := $(shell mktemp -d /tmp/glew.XXXXXX)/$(DIST_NAME)

# To disable stripping of linked binaries either:
#   - use STRIP= on gmake command-line
#   - edit this makefile to set STRIP to the empty string
# (Note: STRIP does not affect the strip in the install step)
#
# To disable symlinks:
#   - use LN= on gmake command-line

AR       ?= ar
ARFLAGS  ?= cr
INSTALL  ?= install
STRIP    ?= strip
RM       ?= rm -f
LN       ?= ln -sf
UNIX2DOS ?= unix2dos -q
DOS2UNIX ?= dos2unix -q

ifneq (,$(filter debug,$(MAKECMDGOALS)))
OPT = -g
else
OPT = $(POPT)
endif
INCLUDE = -Iinclude
CFLAGS = $(OPT) $(WARN) $(INCLUDE) $(CFLAGS.EXTRA)

all debug: glew.lib glew.bin

# GLEW shared and static libraries

LIB.LDFLAGS        := $(LDFLAGS.EXTRA) $(LDFLAGS.GL)
LIB.LIBS           := $(GL_LDFLAGS)

LIB.SRCS           := src/glew.c
LIB.SRCS.NAMES     := $(notdir $(LIB.SRCS))

LIB.OBJS           := $(addprefix tmp/$(SYSTEM)/default/static/,$(LIB.SRCS.NAMES))
LIB.OBJS           := $(LIB.OBJS:.c=.o)
LIB.SOBJS          := $(addprefix tmp/$(SYSTEM)/default/shared/,$(LIB.SRCS.NAMES))
LIB.SOBJS          := $(LIB.SOBJS:.c=.o)

glew.lib: glew.lib.shared glew.lib.static

glew.lib.shared: lib lib/$(LIB.SHARED) glew.pc
glew.lib.static: lib lib/$(LIB.STATIC) glew.pc

.PHONY: glew.lib glew.lib.shared glew.lib.static

lib:
	mkdir lib

lib/$(LIB.STATIC): $(LIB.OBJS)
ifneq ($(AR),)
	$(AR) $(ARFLAGS) $@ $^
else ifneq ($(LIBTOOL),)
	$(LIBTOOL) $@ $^
endif
ifneq ($(STRIP),)
	$(STRIP) -x $@
endif

lib/$(LIB.SHARED): $(LIB.SOBJS)
	$(LD) $(LDFLAGS.SO) -o $@ $^ $(LIB.LDFLAGS) $(LIB.LIBS)
ifneq ($(LN),)
	$(LN) $(LIB.SHARED) lib/$(LIB.SONAME)
	$(LN) $(LIB.SHARED) lib/$(LIB.DEVLNK)
endif
ifneq ($(STRIP),)
	$(STRIP) -x $@
endif

tmp/$(SYSTEM)/default/static/glew.o: src/glew.c include/GL/glew.h include/GL/wglew.h include/GL/glxew.h
	@mkdir -p $(dir $@)
	$(CC) -DGLEW_NO_GLU -DGLEW_STATIC $(CFLAGS) $(CFLAGS.SO) -o $@ -c $<

tmp/$(SYSTEM)/default/shared/glew.o: src/glew.c include/GL/glew.h include/GL/wglew.h include/GL/glxew.h
	@mkdir -p $(dir $@)
	$(CC) -DGLEW_NO_GLU -DGLEW_BUILD $(CFLAGS) $(CFLAGS.SO) -o $@ -c $<

# Force re-write of glew.pc, GLEW_DEST can vary

.PHONY: glew.pc

glew.pc: glew.pc.in
	sed \
		-e "s|@prefix@|$(GLEW_PREFIX)|g" \
		-e "s|@libdir@|$(LIBDIR)|g" \
		-e "s|@exec_prefix@|$(BINDIR)|g" \
		-e "s|@includedir@|$(INCDIR)|g" \
		-e "s|@version@|$(GLEW_VERSION)|g" \
		-e "s|@cflags@||g" \
		-e "s|@libname@|$(NAME)|g" \
		-e "s|@requireslib@|$(LIBGLU)|g" \
		< $< > $@

# GLEW utility programs

BIN.LIBS = -Llib $(LDFLAGS.DYNAMIC) -l$(NAME) $(LDFLAGS.EXTRA) $(LDFLAGS.GL)

GLEWINFO.BIN       := glewinfo$(BIN.SUFFIX)
GLEWINFO.BIN.SRC   := src/glewinfo.c
GLEWINFO.BIN.OBJ   := $(addprefix tmp/$(SYSTEM)/default/shared/,$(notdir $(GLEWINFO.BIN.SRC)))
GLEWINFO.BIN.OBJ   := $(GLEWINFO.BIN.OBJ:.c=.o)

VISUALINFO.BIN     := visualinfo$(BIN.SUFFIX)
VISUALINFO.BIN.SRC := src/visualinfo.c
VISUALINFO.BIN.OBJ := $(addprefix tmp/$(SYSTEM)/default/shared/,$(notdir $(VISUALINFO.BIN.SRC)))
VISUALINFO.BIN.OBJ := $(VISUALINFO.BIN.OBJ:.c=.o)

# Don't build glewinfo or visualinfo for NaCL, yet.

ifneq ($(filter nacl%,$(SYSTEM)),)
glew.bin: glew.lib bin
else
glew.bin: glew.lib bin bin/$(GLEWINFO.BIN) bin/$(VISUALINFO.BIN) 
endif

bin:
	mkdir bin

bin/$(GLEWINFO.BIN): $(GLEWINFO.BIN.OBJ) lib/$(LIB.SHARED)
	$(CC) $(CFLAGS) -o $@ $(GLEWINFO.BIN.OBJ) $(BIN.LIBS)
ifneq ($(STRIP),)
	$(STRIP) -x $@
endif

bin/$(VISUALINFO.BIN): $(VISUALINFO.BIN.OBJ) lib/$(LIB.SHARED)
	$(CC) $(CFLAGS) -o $@ $(VISUALINFO.BIN.OBJ) $(BIN.LIBS)
ifneq ($(STRIP),)
	$(STRIP) -x $@
endif

$(GLEWINFO.BIN.OBJ): $(GLEWINFO.BIN.SRC) include/GL/glew.h include/GL/wglew.h include/GL/glxew.h
	@mkdir -p $(dir $@)
	$(CC) -DGLEW_NO_GLU $(CFLAGS) $(CFLAGS.SO) -o $@ -c $<

$(VISUALINFO.BIN.OBJ): $(VISUALINFO.BIN.SRC) include/GL/glew.h include/GL/wglew.h include/GL/glxew.h
	@mkdir -p $(dir $@)
	$(CC) -DGLEW_NO_GLU $(CFLAGS) $(CFLAGS.SO) -o $@ -c $<

# Install targets

install.all: install install.bin

install:     install.include install.lib install.pkgconfig

install.lib: glew.lib
	$(INSTALL) -d -m 0755 "$(DESTDIR)$(LIBDIR)"
# runtime
ifeq ($(filter-out mingw% cygwin,$(SYSTEM)),)
	$(INSTALL) -d -m 0755 "$(DESTDIR)$(BINDIR)"
	$(INSTALL) -m 0755 lib/$(LIB.SHARED) "$(DESTDIR)$(BINDIR)/"
else
	$(INSTALL) -m 0644 lib/$(LIB.SHARED) "$(DESTDIR)$(LIBDIR)/"
endif
ifneq ($(LN),)
	$(LN) $(LIB.SHARED) "$(DESTDIR)$(LIBDIR)/$(LIB.SONAME)"
endif

# development files
ifeq ($(filter-out mingw% cygwin,$(SYSTEM)),)
	$(INSTALL) -m 0644 lib/$(LIB.DEVLNK) "$(DESTDIR)$(LIBDIR)/"
endif
ifneq ($(LN),)
	$(LN) $(LIB.SHARED) "$(DESTDIR)$(LIBDIR)/$(LIB.DEVLNK)"
endif
	$(INSTALL) -m 0644 lib/$(LIB.STATIC) "$(DESTDIR)$(LIBDIR)/"

install.bin: glew.bin
	$(INSTALL) -d -m 0755 "$(DESTDIR)$(BINDIR)"
	$(INSTALL) -s -m 0755 bin/$(GLEWINFO.BIN) bin/$(VISUALINFO.BIN) "$(DESTDIR)$(BINDIR)/"

install.include:
	$(INSTALL) -d -m 0755 "$(DESTDIR)$(INCDIR)"
	$(INSTALL) -m 0644 include/GL/wglew.h "$(DESTDIR)$(INCDIR)/"
	$(INSTALL) -m 0644 include/GL/glew.h "$(DESTDIR)$(INCDIR)/"
	$(INSTALL) -m 0644 include/GL/glxew.h "$(DESTDIR)$(INCDIR)/"

install.pkgconfig: glew.pc
	$(INSTALL) -d -m 0755 "$(DESTDIR)$(PKGDIR)"
	$(INSTALL) -d -m 0755 "$(DESTDIR)$(PKGDIR)"
	$(INSTALL) -m 0644 glew.pc "$(DESTDIR)$(PKGDIR)/"

uninstall:
	$(RM) "$(DESTDIR)$(INCDIR)/wglew.h"
	$(RM) "$(DESTDIR)$(INCDIR)/glew.h"
	$(RM) "$(DESTDIR)$(INCDIR)/glxew.h"
	$(RM) "$(DESTDIR)$(LIBDIR)/$(LIB.DEVLNK)"
ifeq ($(filter-out mingw% cygwin,$(SYSTEM)),)
	$(RM) "$(DESTDIR)$(BINDIR)/$(LIB.SHARED)"
else
	$(RM) "$(DESTDIR)$(LIBDIR)/$(LIB.SONAME)"
	$(RM) "$(DESTDIR)$(LIBDIR)/$(LIB.SHARED)"
endif
	$(RM) "$(DESTDIR)$(LIBDIR)/$(LIB.STATIC)"
	$(RM) "$(DESTDIR)$(BINDIR)/$(GLEWINFO.BIN)" "$(DESTDIR)$(BINDIR)/$(VISUALINFO.BIN)"

clean:
	$(RM) -r tmp/
	$(RM) -r lib/
	$(RM) -r bin/
	$(RM) glew.pc

distclean: clean
	find . -name \*~ | xargs $(RM)
	find . -name .\*.sw\? | xargs $(RM)

# Distributions

dist-win32:
	$(RM) -r $(DIST_DIR)
	mkdir -p $(DIST_DIR)
	cp -a include $(DIST_DIR)
	cp -a doc $(DIST_DIR)
	cp -a *.txt $(DIST_DIR)
	cp -a bin $(DIST_DIR)
	cp -a lib $(DIST_DIR)
	$(RM) -f $(DIST_DIR)/bin/*/*/*.pdb $(DIST_DIR)/bin/*/*/*.exp
	$(RM) -f $(DIST_DIR)/bin/*/*/glewinfo-*.exe $(DIST_DIR)/bin/*/*/visualinfo-*.exe 
	$(RM) -f $(DIST_DIR)/lib/*/*/*.pdb $(DIST_DIR)/lib/*/*/*.exp
	$(UNIX2DOS) $(DIST_DIR)/include/GL/*.h
	$(UNIX2DOS) $(DIST_DIR)/doc/*.txt
	$(UNIX2DOS) $(DIST_DIR)/doc/*.html
	$(UNIX2DOS) $(DIST_DIR)/*.txt
	rm -f $(DIST_WIN32)
	cd $(DIST_DIR)/.. && zip -rq9 $(DIST_WIN32) $(DIST_NAME)
	$(RM) -r $(DIST_DIR)

dist-src:
	$(RM) -r $(DIST_DIR)
	mkdir -p $(DIST_DIR)
	mkdir -p $(DIST_DIR)/bin
	mkdir -p $(DIST_DIR)/lib
	cp -a auto $(DIST_DIR)
	$(RM) -Rf $(DIST_DIR)/auto/registry
	cp -a build $(DIST_DIR)
	cp -a config $(DIST_DIR)
	cp -a src $(DIST_DIR)
	cp -a doc $(DIST_DIR)
	cp -a include $(DIST_DIR)
	cp -a *.md $(DIST_DIR)
	cp -a *.txt $(DIST_DIR)
	cp -a Makefile $(DIST_DIR)
	cp -a glew.pc.in $(DIST_DIR)
	find $(DIST_DIR) -name '*.o' | xargs $(RM) -r
	find $(DIST_DIR) -name '*~' | xargs $(RM) -r
	find $(DIST_DIR) -name CVS -o -name .cvsignore | xargs $(RM) -r
	find $(DIST_DIR) -name .svn | xargs $(RM) -r
	find $(DIST_DIR) -name "*.patch" | xargs $(RM) -r
	$(DOS2UNIX) $(DIST_DIR)/Makefile
	$(DOS2UNIX) $(DIST_DIR)/auto/Makefile
	$(DOS2UNIX) $(DIST_DIR)/config/*
	$(UNIX2DOS) $(DIST_DIR)/auto/core/*
	$(UNIX2DOS) $(DIST_DIR)/auto/extensions/*
	find $(DIST_DIR) -name '*.h' | xargs $(UNIX2DOS)
	find $(DIST_DIR) -name '*.c' | xargs $(UNIX2DOS)
	find $(DIST_DIR) -name '*.md' | xargs $(UNIX2DOS)
	find $(DIST_DIR) -name '*.txt' | xargs $(UNIX2DOS)
	find $(DIST_DIR) -name '*.html' | xargs $(UNIX2DOS)
	find $(DIST_DIR) -name '*.css' | xargs $(UNIX2DOS)
	find $(DIST_DIR) -name '*.sh' | xargs $(UNIX2DOS)
	find $(DIST_DIR) -name '*.pl' | xargs $(UNIX2DOS)
	find $(DIST_DIR) -name 'Makefile' | xargs $(UNIX2DOS)
	find $(DIST_DIR) -name '*.in' | xargs $(UNIX2DOS)
	find $(DIST_DIR) -name '*.pm' | xargs $(UNIX2DOS)
	find $(DIST_DIR) -name '*.rc' | xargs $(UNIX2DOS)
	rm -f $(DIST_SRC_ZIP)
	cd $(DIST_DIR)/.. && zip -rq9 $(DIST_SRC_ZIP) $(DIST_NAME)
	$(DOS2UNIX) $(DIST_DIR)/Makefile
	$(DOS2UNIX) $(DIST_DIR)/auto/Makefile
	$(DOS2UNIX) $(DIST_DIR)/config/*
	$(DOS2UNIX) $(DIST_DIR)/auto/core/*
	$(DOS2UNIX) $(DIST_DIR)/auto/extensions/*
	find $(DIST_DIR) -name '*.h' | xargs $(DOS2UNIX)
	find $(DIST_DIR) -name '*.c' | xargs $(DOS2UNIX)
	find $(DIST_DIR) -name '*.md' | xargs $(DOS2UNIX)
	find $(DIST_DIR) -name '*.txt' | xargs $(DOS2UNIX)
	find $(DIST_DIR) -name '*.html' | xargs $(DOS2UNIX)
	find $(DIST_DIR) -name '*.css' | xargs $(DOS2UNIX)
	find $(DIST_DIR) -name '*.sh' | xargs $(DOS2UNIX)
	find $(DIST_DIR) -name '*.pl' | xargs $(DOS2UNIX)
	find $(DIST_DIR) -name 'Makefile' | xargs $(DOS2UNIX)
	find $(DIST_DIR) -name '*.in' | xargs $(DOS2UNIX)
	find $(DIST_DIR) -name '*.pm' | xargs $(DOS2UNIX)
	find $(DIST_DIR) -name '*.rc' | xargs $(DOS2UNIX)
	rm -f $(DIST_SRC_TGZ)
	cd $(DIST_DIR)/.. && env GZIP=-9 tar czf $(DIST_SRC_TGZ) $(DIST_NAME)
	$(RM) -r $(DIST_DIR)

extensions:
	$(MAKE) -C auto

.PHONY: clean distclean tardist dist-win32 dist-src
