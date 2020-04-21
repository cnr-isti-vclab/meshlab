#-----------------------------------------------------------------------------
# Makefile for ANN library
#----------------------------------------------------------------------
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
#----------------------------------------------------------------------
# History:
#	Revision 0.1  03/04/98
#		Initial release
#	Revision 1.0  04/01/05
#		Renamed files from .cc to .cpp for Microsoft Visual C++
#		Added kd_dump.cpp
#	Revision 1.1  05/03/05
#		Added kd_fix_rad_search.cpp and bd_fix_rad_search.cpp
#----------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Some basic definitions:
#		BASEDIR		where include, src, lib, ... are
#		INCLIB		include directory
#		LIBLIB		library directory
#-----------------------------------------------------------------------------
BASEDIR	= ..
INCDIR	= $(BASEDIR)/include
LIBDIR	= $(BASEDIR)/lib

SOURCES = ANN.cpp brute.cpp kd_tree.cpp kd_util.cpp kd_split.cpp \
	kd_dump.cpp kd_search.cpp kd_pr_search.cpp kd_fix_rad_search.cpp \
	bd_tree.cpp bd_search.cpp bd_pr_search.cpp bd_fix_rad_search.cpp \
	perf.cpp

HEADERS = kd_tree.h kd_split.h kd_util.h kd_search.h \
	kd_pr_search.h kd_fix_rad_search.h perf.h pr_queue.h pr_queue_k.h

OBJECTS = $(SOURCES:.cpp=.o)

#-----------------------------------------------------------------------------
# Make the library
#-----------------------------------------------------------------------------

default:
	@echo "Specify a target configuration"

targets: $(LIBDIR)/$(ANNLIB)

$(LIBDIR)/$(ANNLIB): $(OBJECTS)
	$(MAKELIB) $(ANNLIB) $(OBJECTS)
	$(RANLIB) $(ANNLIB)
	mv $(ANNLIB) $(LIBDIR)

#-----------------------------------------------------------------------------
# Make object files
#-----------------------------------------------------------------------------

ANN.o: ANN.cpp
	$(C++) -c -I$(INCDIR) $(CFLAGS) ANN.cpp

brute.o: brute.cpp
	$(C++) -c -I$(INCDIR) $(CFLAGS) brute.cpp

kd_tree.o: kd_tree.cpp
	$(C++) -c -I$(INCDIR) $(CFLAGS) kd_tree.cpp

kd_util.o: kd_util.cpp
	$(C++) -c -I$(INCDIR) $(CFLAGS) kd_util.cpp

kd_split.o: kd_split.cpp
	$(C++) -c -I$(INCDIR) $(CFLAGS) kd_split.cpp

kd_search.o: kd_search.cpp
	$(C++) -c -I$(INCDIR) $(CFLAGS) kd_search.cpp

kd_pr_search.o: kd_pr_search.cpp
	$(C++) -c -I$(INCDIR) $(CFLAGS) kd_pr_search.cpp

kd_fix_rad_search.o: kd_fix_rad_search.cpp
	$(C++) -c -I$(INCDIR) $(CFLAGS) kd_fix_rad_search.cpp

kd_dump.o: kd_dump.cpp
	$(C++) -c -I$(INCDIR) $(CFLAGS) kd_dump.cpp

bd_tree.o: bd_tree.cpp
	$(C++) -c -I$(INCDIR) $(CFLAGS) bd_tree.cpp

bd_search.o: bd_search.cpp
	$(C++) -c -I$(INCDIR) $(CFLAGS) bd_search.cpp

bd_pr_search.o: bd_pr_search.cpp
	$(C++) -c -I$(INCDIR) $(CFLAGS) bd_pr_search.cpp

bd_fix_rad_search.o: bd_fix_rad_search.cpp
	$(C++) -c -I$(INCDIR) $(CFLAGS) bd_fix_rad_search.cpp

perf.o: perf.cpp
	$(C++) -c -I$(INCDIR) $(CFLAGS) perf.cpp

#-----------------------------------------------------------------------------
# Configuration definitions
#-----------------------------------------------------------------------------

include ../Make-config

#-----------------------------------------------------------------------------
# Cleaning
#-----------------------------------------------------------------------------

clean:
	-rm -f *.o core

realclean: clean

