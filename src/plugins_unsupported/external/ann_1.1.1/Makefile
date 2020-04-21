#-----------------------------------------------------------------------------
# Top-level Makefile for ANN.
#
# ANN: Approximate Nearest Neighbors
# Version: 1.1 05/03/05
#-----------------------------------------------------------------------------
# Copyright (c) 1997-2005 University of Maryland and Sunil Arya and
# David Mount.  All Rights Reserved.
# 
# This software and related documentation is part of the Approximate
# Nearest Neighbor Library (ANN).  This software is provided under
# the provisions of the Lesser GNU Public License (LGPL).  See the
# file ../ReadMe.txt for further information.
# 
# The University of Maryland (U.M.) and the authors make no
# representations about the suitability or fitness of this software for
# any purpose.  It is provided "as is" without express or implied
# warranty.
#-----------------------------------------------------------------------------
# Revision 0.1  09/06/97
#	alpha release
# Revision 0.2  06/26/98
#	Minor changes to fix compilation errors on SGI systems.
# Revision 1.0  04/01/05
#	Initial release (finally!)
#	Added linux-g++ target
# Revision 1.1  05/03/05
#	Added macosx-g++ target
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# default: list the options
# The following legacy targets are also available.
#	make sunos4		for Sun with SunOS 4.x
#	make sunos4-g++		for Sun with SunOS 4.x and g++
#	make alpha-g++		for DEC Alpha and g++
# The following targets are used for internal development only
#	make authors-debug	author's debugging
#	make authors-perf	author's performance evaluations
#	make distribution	author's generation of distribution file
#-----------------------------------------------------------------------------
default:
	@echo "Enter one of the following:"
	@echo "  make linux-g++            for Linux and g++"
	@echo "  make macosx-g++           for Mac OS X and g++"
	@echo "  make sunos5               for Sun with SunOS 5.x"
	@echo "  make sunos5-sl            for Sun with SunOS 5.x, make shared libs"
	@echo "  make sunos5-g++           for Sun with SunOS 5.x and g++"
	@echo "  make sunos5-g++-sl        for Sun with SunOS 5.x, g++, make shared libs"
	@echo "  make clean                remove .o files"
	@echo "  make realclean            remove .o, library and executable files"
	@echo " "
	@echo "See file Makefile for other compilation options, such as disabling"
	@echo "performance measurement code."

#-----------------------------------------------------------------------------
# main make entry point
#-----------------------------------------------------------------------------
alpha-g++ macosx-g++ linux-g++ sgi sunos4 sunos4-g++ sunos5 sunos5-g++ sunos5-g++-sl authors-debug authors-perf:
	cd src ; $(MAKE) $@
	cd test ; $(MAKE) $@
	cd sample ; $(MAKE) $@
	cd ann2fig ; $(MAKE) $@

#-----------------------------------------------------------------------------
# Remove .o files and core files
#-----------------------------------------------------------------------------
clean:
	cd src ; $(MAKE) clean
	cd test ; $(MAKE) clean
	cd sample ; $(MAKE) clean
	cd ann2fig ; $(MAKE) clean
	cd doc ; $(MAKE) clean
	cd MS_Win32; $(MAKE) clean
	cd validate ; $(MAKE) clean

#-----------------------------------------------------------------------------
# Remove everthing that can be remade
#-----------------------------------------------------------------------------
realclean: 
	-rm -f lib/*
	-rm -f bin/*
	cd src ; $(MAKE) realclean
	cd test ; $(MAKE) realclean
	cd sample ; $(MAKE) realclean
	cd ann2fig ; $(MAKE) realclean
	cd doc ; $(MAKE) realclean
	cd MS_Win32; $(MAKE) realclean
	cd validate ; $(MAKE) realclean

#-----------------------------------------------------------------------------
# Make distribution package (for use by authors only)
#-----------------------------------------------------------------------------
DISTR = ann_1.1.1

distribution: realclean 
	cd .. ; mv -f $(DISTR) $(DISTR)-old; mkdir $(DISTR)
	cp Copyright.txt ../$(DISTR)
	cp License.txt ../$(DISTR)
	cp Make-config ../$(DISTR)
	cp Makefile ../$(DISTR)
	cp ReadMe.txt ../$(DISTR)
	cp -r MS_Win32 ../$(DISTR)
#	cd ..; mv -f $(DISTR)_MS_Win32_bin $(DISTR)_MS_Win32_bin-old
	cp -r MS_Win32_bin ../$(DISTR)_MS_Win32_bin
	cp -r bin ../$(DISTR)
	cp -r include ../$(DISTR)
	cp -r lib ../$(DISTR)
	cp -r src ../$(DISTR)
	cp -r test ../$(DISTR)
	cp -r sample ../$(DISTR)
	cp -r ann2fig ../$(DISTR)
	cd ../$(DISTR); mkdir doc
	cp doc/*.pdf ../$(DISTR)/doc
#	cd .. ; tar -cfv $(DISTR).tar $(DISTR) ; gzip $(DISTR).tar
