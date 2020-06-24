# - Try to find CFITSIO.
# Once executed, this module will define:
# Variables defined by this module:
#  CFITSIO_FOUND        - system has CFITSIO
#  CFITSIO_INCLUDE_DIR  - the CFITSIO include directory (cached)
#  CFITSIO_INCLUDE_DIRS - the CFITSIO include directories
#                         (identical to CFITSIO_INCLUDE_DIR)
#  CFITSIO_LIBRARY      - the CFITSIO library (cached)
#  CFITSIO_LIBRARIES    - the CFITSIO libraries
#                         (identical to CFITSIO_LIBRARY)
# 
# This module will use the following enviornmental variable
# when searching for CFITSIO:
#  CFITSIO_ROOT_DIR     - CFITSIO root directory
#

# 
#  Copyright (c) 2012 Brian Kloppenborg
# 
#  This file is part of the C++ OIFITS Library (CCOIFITS).
#  
#  CCOIFITS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License 
#  as published by the Free Software Foundation, either version 3 
#  of the License, or (at your option) any later version.
#  
#  CCOIFITS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#  
#  You should have received a copy of the GNU Lesser General Public 
#  License along with CCOIFITS.  If not, see <http://www.gnu.org/licenses/>.
# 

if(NOT CFITSIO_FOUND)

    find_path(CFITSIO_INCLUDE_DIR fitsio.h
        HINTS $ENV{CFITSIO_ROOT_DIR} 
        PATH_SUFFIXES include include/cfitsio)
    find_library(CFITSIO_LIBRARY cfitsio
        HINTS $ENV{CFITSIO_ROOT_DIR} 
        PATH_SUFFIXES lib)
  
    mark_as_advanced(CFITSIO_INCLUDE_DIR CFITSIO_LIBRARY)

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(CFITSIO DEFAULT_MSG
        CFITSIO_LIBRARY CFITSIO_INCLUDE_DIR)

    set(CFITSIO_INCLUDE_DIRS ${CFITSIO_INCLUDE_DIR})
    set(CFITSIO_LIBRARIES ${CFITSIO_LIBRARY})

endif(NOT CFITSIO_FOUND)
