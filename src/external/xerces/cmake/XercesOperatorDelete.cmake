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

# Check whether the compiler chokes on a placement operator delete

include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
#include <stdlib.h>

class XMemory {
public:
  void* operator new(size_t s) { return 0; }
  void* operator new(size_t s, void* ptr) { return 0; }
  void operator delete(void* p) {}
  void operator delete(void* p, void* ptr) {}
};

int main() {
  return 0;
}"
  CXX_matching-delete-operator)

set(XERCES_NO_MATCHING_DELETE_OPERATOR 0)
if(NOT CXX_matching-delete-operator)
  set(XERCES_NO_MATCHING_DELETE_OPERATOR 1)
endif()
