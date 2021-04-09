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

# SSE2 support

include(CheckCXXSourceCompiles)

option(sse2 "SSE2 support" ON)
if(sse2)
  set(CMAKE_CXX_FLAGS_SAVE "${CMAKE_CXX_FLAGS}")
  if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_SAVE "${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
    check_cxx_source_compiles("
#include <emmintrin.h>

int main() {
  __m128i one;
  return 0;
}"
      CXX_NEEDS_msse2)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_SAVE}")
    if(CXX_NEEDS_msse2)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
    endif()
  endif()

    check_cxx_source_compiles("
#include <intrin.h>

int main() {
  return 0;
}"
    XERCES_HAVE_INTRIN_H)
    check_cxx_source_compiles("
#include <emmintrin.h>

int main() {
  return 0;
}"
    XERCES_HAVE_EMMINTRIN_H)
    check_cxx_source_compiles("
#include <cpuid.h>

int main() {
  return 0;
}"
    HAVE_CPUID_H)
    check_cxx_source_compiles("
#include <intrin.h>

int main() {
  int CPUInfo[4];
  __cpuid(CPUInfo, 1);
  return 0;
}"
    XERCES_HAVE_CPUID_INTRINSIC)
    check_cxx_source_compiles("
#include <cpuid.h>

int main() {
  unsigned int eax, ebx, ecx, edx;
  __get_cpuid (1, &eax, &ebx, &ecx, &edx);
  return 0;
}"
    XERCES_HAVE_GETCPUID)
    check_cxx_source_compiles("
#include <emmintrin.h>

int main() {
  __m128i* one=(__m128i*)_mm_malloc(4, 16);
  __m128i* two=(__m128i*)_mm_malloc(4, 16);
  __m128i xmm1 = _mm_load_si128(one);
  __m128i xmm2 = _mm_load_si128(two);
  __m128i xmm3 = _mm_or_si128(xmm1, xmm2);
  _mm_store_si128(one, xmm3);
  _mm_free(one);
  _mm_free(two);
  return 0;
}"
    XERCES_HAVE_SSE2_INTRINSIC)

  # SSE2 not functional; restore flags
  if(NOT XERCES_HAVE_SSE2_INTRINSIC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_SAVE}")
  endif()
endif()
