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

# namespace keyword

include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
namespace Outer {
  namespace Inner {
    int i = 0;
  }
}

int main() {
  using namespace Outer::Inner;
  return i;
}"
  HAVE_NAMESPACES)

set(XERCES_HAS_CPP_NAMESPACE ${HAVE_NAMESPACES})
