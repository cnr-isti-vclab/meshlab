# CMake build for xerces-c
#
# Written by Roger Leigh <rleigh@codelibre.net>
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# compiler warnings

# These are annoyingly verbose, produce false positives or don't work
# nicely with all supported compiler versions, so are disabled unless
# explicitly enabled.
option(extra-warnings "Enable extra compiler warnings" OFF)

# This will cause the compiler to fail when an error occurs.
option(fatal-warnings "Compiler warnings are errors" OFF)

# Check if the compiler supports each of the following additional
# flags, and enable them if supported.  This greatly improves the
# quality of the build by checking for a number of common problems,
# some of which are quite serious.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
   CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(test_flags
      -Wall
      -Wcast-align
      -Wcast-qual
      -Wctor-dtor-privacy
      -Wextra
      -Wformat=2
      -Wimplicit-atomic-properties
      -Wmissing-declarations
      -Wno-long-long
      -Woverlength-strings
      -Woverloaded-virtual
      -Wredundant-decls
      -Wreorder
      -Wswitch-default
      -Wunused-variable
      -Wwrite-strings
      -Wno-variadic-macros
      -fstrict-aliasing)
  if(extra-warnings)
    list(APPEND test_flags
        -Wfloat-equal
        -Wmissing-prototypes
        -Wunreachable-code)
  endif()
  if(fatal-warnings)
    list(APPEND test_flags
         -Werror)
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(test_flags)
  if(extra-warnings)
    list(APPEND test_flags
         /W4)
  else()
    list(APPEND test_flags
         /W3)
  endif()
  if (fatal-warnings)
    list(APPEND test_flags
         /WX)
  endif()
endif()

include(CheckCXXCompilerFlag)

foreach(flag ${test_flags})
  string(REGEX REPLACE "[^A-Za-z0-9]" "_" flag_var "${flag}")
  set(test_c_flag "CXX_FLAG${flag_var}")
  CHECK_CXX_COMPILER_FLAG(${flag} "${test_c_flag}")
  if (${test_c_flag})
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
  endif (${test_c_flag})
endforeach(flag ${test_flags})
