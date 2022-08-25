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

# test macros

find_program(DIFF_EXECUTABLE diff)

set(xerces_test_dir "${CMAKE_CURRENT_LIST_DIR}")

macro(add_generic_xerces_test name)
  set(options EXPECT_FAIL)
  set(oneValueArgs WORKING_DIRECTORY OBSERVED_DIR EXPECTED_DIR STDIN)
  set(multiValueArgs COMMAND)
  cmake_parse_arguments(AXT "${options}" "${oneValueArgs}"
    "${multiValueArgs}" ${ARGN})

  list(GET AXT_COMMAND 0 command)
  list(REMOVE_AT AXT_COMMAND 0)

  if(NOT AXT_WORKING_DIRECTORY)
    set(AXT_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/samples/data")
  endif()

  unset(xerces_test_extra_args)
  if(WIN32)
    list(APPEND xerces_test_extra_args "-DWIN32=${WIN32}")
  endif()
  if(CYGWIN)
    list(APPEND xerces_test_extra_args "-DCYGWIN=${CYGWIN}")
  endif()

  add_test(NAME "${name}"
    COMMAND "${CMAKE_COMMAND}"
    "-DNAME=${name}"
    "-DPROGRAM=$<TARGET_FILE:${command}>"
    "-DARGS=${AXT_COMMAND}"
    "-DLIBXERCES_C=$<TARGET_FILE:xerces-c>"
    "-DWORKDIR=${AXT_WORKING_DIRECTORY}"
    "-DSTDIN=${AXT_STDIN}"
    "-DEXPECT_FAIL=${AXT_EXPECT_FAIL}"
    "-DOBSERVED_DIR=${CMAKE_CURRENT_BINARY_DIR}/observed"
    "-DEXPECTED_DIR=${CMAKE_CURRENT_SOURCE_DIR}/expected"
    "-DDIFF=${DIFF_EXECUTABLE}"
    "-DNLS_HOME=${PROJECT_BINARY_DIR}/src"
    ${xerces_test_extra_args}
    -P "${xerces_test_dir}/RunTest.cmake"
    )

  if(AXT_STDIN)
    set_property(TEST "${name}" PROPERTY RUN_SERIAL 1)
  endif()
endmacro()

macro(add_xerces_test name)
  add_generic_xerces_test(
    ${name}
    OBSERVED_DIR "${PROJECT_BINARY_DIR}/tests/observed"
    EXPECTED_DIR "${PROJECT_SOURCE_DIR}/tests/expected"
    ${ARGN})
endmacro()

macro(add_xerces_sample_test name)
  add_generic_xerces_test(
    ${name}
    OBSERVED_DIR "${PROJECT_BINARY_DIR}/samples/observed"
    EXPECTED_DIR "${PROJECT_SOURCE_DIR}/samples/expected"
    ${ARGN})
endmacro()
