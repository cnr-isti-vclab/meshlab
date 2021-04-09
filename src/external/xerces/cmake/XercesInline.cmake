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

# inline keyword

include(CheckCXXSourceCompiles)

unset(inline_keyword)
set(NEED_INLINE "0")

# Inspired from /usr/share/autoconf/autoconf/c.m4
foreach(test_keyword "inline" "__inline__" "__inline")
  set(CMAKE_REQUIRED_DEFINITIONS_SAVE ${CMAKE_REQUIRED_DEFINITIONS})
  set(CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS}
    "-Dinline=${test_keyword}")
  check_cxx_source_compiles("
        typedef int foo_t;
        static inline foo_t static_foo() {return 0;}
        foo_t foo(){return 0;}
        int main(int argc, char *argv[]) {return 0;}"
    CXX_HAS_${test_keyword})
  set(CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS_SAVE})
  if(CXX_HAS_${test_keyword})
    if(NOT test_keyword STREQUAL "inline")
      set(NEED_INLINE 1)
    else()
      set(HAVE_INLINE 1)
    endif()
    set(inline_keyword "${test_keyword}")
    break()
  endif()
endforeach()
