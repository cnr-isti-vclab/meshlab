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
 # $Id: Makefile.am,v 1.5 2007/06/14 09:59:10 jeh Exp $
 #

INCLUDES = \
  -I$(top_srcdir) 

bin_PROGRAMS = \
  3dsdump 

LDADD = \
  $(top_builddir)/lib3ds/lib3ds.la 

MANPAGES = \
  3dsdump.1 

man_MANS = $(MANPAGES)
EXTRA_DIST = $(MANPAGES) 





