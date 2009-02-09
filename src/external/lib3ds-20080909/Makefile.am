VERSION = @LIB3DS_VERSION@

SUBDIRS = src examples

bin_SCRIPTS = lib3ds-config

EXTRA_DIST = \
  lib3ds.m4 \
  lib3ds-config.in \
  lib3ds.sln

m4datadir = $(datadir)/aclocal
m4data_DATA = lib3ds.m4

.PHONY: release snapshot

release:
	rm -rf .deps */.deps
	$(MAKE) distcheck

snapshot:
	$(MAKE) dist-zip distdir=$(PACKAGE)-`date +"%Y%m%d"`

