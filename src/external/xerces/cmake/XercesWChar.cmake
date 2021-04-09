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

# Wide character functions

include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
#include <wchar.h>
int main(void) {
  mbstate_t st;
  mbrlen( \"t\", 5, &st );
  return 0;
}"
  HAVE_MBRLEN)

check_cxx_source_compiles("
#include <wchar.h>
int main(void) {
  mbstate_t st;
  char buffer[2];
  const wchar_t* src=0;
  wcsrtombs(buffer, &src, 2, &st);
  return 0;
}"
  HAVE_WCSRTOMBS)

check_cxx_source_compiles("
#include <wchar.h>
int main(void) {
  mbstate_t st;
  wchar_t buffer[2];
  const char* src=0;
  mbsrtowcs(buffer, &src, 2, &st);
  return 0;
}"
  HAVE_MBSRTOWCS)
