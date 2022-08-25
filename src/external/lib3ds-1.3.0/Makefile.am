 #
 # The 3D Studio File Format Library
 # Copyright (C) 1996-2007 by Jan Eric Kyprianidis <www.kyprianidis.com>
 # All rights reserved.
 #
 # This program is  free  software;  you can redistribute it and/or modify it
 # under the terms of the  GNU Lesser General Public License  as published by 
 # the  Free Software Foundation;  either version 2.1 of the License,  or (at 
 # your option) any later version.
 #
 # This  program  is  distributed in  the  hope that it will  be useful,  but
 # WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 # or  FITNESS FOR A  PARTICULAR PURPOSE.  See the  GNU Lesser General Public  
 # License for more details.
 #
 # You should  have received  a copy of the GNU Lesser General Public License
 # along with  this program;  if not, write to the  Free Software Foundation,
 # Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 #
 # $Id: Makefile.am,v 1.27 2007/06/20 09:05:29 jeh Exp $
 #


VERSION = @LIB3DS_VERSION@

SUBDIRS = lib3ds tools 

bin_SCRIPTS = lib3ds-config

AUTOMAKE_OPTIONS = 1.4

man_MANS = \
  lib3ds-config.1

EXTRA_DIST = \
  lib3ds.m4 \
  lib3ds-config.1 \
  AUTHORS \
  COPYING \
  ChangeLog \
  INSTALL \
  README \
  TODO \
  examples/3dsplay.c \
  msvc8/lib3ds.sln \
  msvc8/lib3ds.vcproj \
  msvc8/lib3ds.rc \
  msvc8/3dsdump.vcproj \
  msvc8/3dsplay.vcproj

m4datadir = $(datadir)/aclocal
m4data_DATA = lib3ds.m4

.PHONY: release snapshot

release:
	rm -rf .deps */.deps
	$(MAKE) distcheck

snapshot:
	$(MAKE) dist-zip distdir=$(PACKAGE)-`date +"%Y%m%d"`


DIST_NAME = lib3ds-$(VERSION)
DIST_DIR = /tmp/$(DIST_NAME)

RPM_SOURCE_DIR = /tmp/rpmsource-$(DIST_NAME)
RPM_BUILD_DIR = /tmp/rpmbuild-$(DIST_NAME)
RPM_DEST_DIR = .


rpm: $(DIST_NAME).tar.gz
	rm -rf $(RPM_SOURCE_DIR)
	rm -rf $(RPM_BUILD_DIR)
	mkdir -p $(RPM_SOURCE_DIR)
	mkdir -p $(RPM_BUILD_DIR)
	cp $(DIST_NAME).tar.gz $(RPM_SOURCE_DIR)

	rpmbuild -bb lib3ds.spec \
	  --define "_sourcedir ${RPM_SOURCE_DIR}" \
	  --define "_builddir ${RPM_BUILD_DIR}" \
	  --define "_rpmdir ${RPM_SOURCE_DIR}"

	mv ${RPM_SOURCE_DIR}/i386/lib3ds-*.rpm ${RPM_DEST_DIR}
	rm -rf ${RPM_SOURCE_DIR} ${RPM_BUILD_DIR}



