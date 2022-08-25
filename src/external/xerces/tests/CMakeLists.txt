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

# Definitions required for building
add_definitions(
  -DHAVE_CONFIG_H=1
)

# Search the project binary dir for config.h
include_directories(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
)

set(EXTRA_DIST
  src/DOM/Normalizer/expectedOutput
  src/DOM/TypeInfo/data/combined.dtd
  src/DOM/TypeInfo/data/combined.xml
  src/DOM/TypeInfo/data/combined.xsd
  src/DOM/TypeInfo/data/SecondSchema.xsd
  src/DOM/TypeInfo/data/TypeInfo.dtd
  src/DOM/TypeInfo/data/TypeInfo.xml
  src/DOM/TypeInfo/data/TypeInfo.xsd
  src/DOM/TypeInfo/data/TypeInfoJustDTD.xml
  src/DOM/TypeInfo/data/TypeInfoNoDTD.xml
  src/DOM/TypeInfo/data/TypeInfoNoDTD.xsd
  src/XSTSHarness/regression
  src/xinclude
)

macro(add_test_executable name)
  add_executable(${name} ${ARGN})
  target_link_libraries(${name} xerces-c)
  set_target_properties(${name} PROPERTIES FOLDER "Tests")
  list(APPEND test_programs ${name})
endmacro()

add_test_executable(DOMTest
  src/DOM/DOMTest/DTest.cpp
  src/DOM/DOMTest/DTest.h
)

add_test_executable(DOMMemTest
  src/DOM/DOMMemTest/DOMMemTest.cpp
)

add_test_executable(Normalizer
  src/DOM/Normalizer/Normalizer.cpp
  src/DOM/Normalizer/Normalizer.hpp
)

add_test_executable(RangeTest
  src/DOM/RangeTest/RangeTest.cpp
  )

add_test_executable(DOMTraversalTest
  src/DOM/Traversal/Traversal.cpp
)

add_test_executable(DOMTypeInfoTest
  src/DOM/TypeInfo/TypeInfo.cpp
  src/DOM/TypeInfo/TypeInfo.hpp
)

add_test_executable(EncodingTest
  src/EncodingTest/EncodingTest.cpp
)

add_test_executable(InitTermTest
  src/InitTermTest/InitTermTest.cpp
  src/InitTermTest/InitTermTest.hpp
)

add_test_executable(MemHandlerTest
  src/MemHandlerTest/MemoryMonitor.cpp
  src/MemHandlerTest/MemoryMonitor.hpp
  src/MemHandlerTest/SimpleValueHashTableOf.hpp
)

add_test_executable(NetAccessorTest
  src/NetAccessorTest/NetAccessorTest.cpp
)

# Doesn't compile under gcc4 for some reason
# dcargill says this is obsolete and we can delete it.
#add_test_executable(ParserTest
#  src/ParserTest/ParserTest.cpp
#  src/ParserTest/ParserTest.hpp
#  src/ParserTest/ParserTest_Parser.cpp
#  src/ParserTest/ParserTest_Parser.hpp
#)

if(NOT XERCES_USE_MUTEXMGR_NOTHREAD)
  add_test_executable(ThreadTest
    src/ThreadTest/ThreadTest.cpp
  )
endif()

# Fails to compile under gcc 4 (ambiguous calls to NullPointerException)
# dcargill says this is obsolete and we can delete it.
#add_test_executable(UtilTests
#  src/UtilTests/CoreTests_BitSet.cpp
#  src/UtilTests/CoreTests_CountedPointer.cpp
#  src/UtilTests/CoreTests_RefArray.cpp
#  src/UtilTests/CoreTests_RefHashTable.cpp
#  src/UtilTests/CoreTests_RefStack.cpp
#  src/UtilTests/CoreTests_RefVector.cpp
#  src/UtilTests/CoreTests_String.cpp
#  src/UtilTests/CoreTests_Transcoders.cpp
#  src/UtilTests/CoreTests_URL.cpp
#  src/UtilTests/CoreTests_ValueArray.cpp
#  src/UtilTests/CoreTests_ValueStack.cpp
#  src/UtilTests/CoreTests_ValueVector.cpp
#  src/UtilTests/CoreTestsMain.cpp
#  src/UtilTests/CoreTests.hpp
#)

add_test_executable(XSerializerTest
  src/XSerializerTest/XSerializerHandlers.cpp
  src/XSerializerTest/XSerializerHandlers.hpp
  src/XSerializerTest/XSerializerTest.cpp
  src/XSerializerTest/XSerializerTest.hpp
)

add_test_executable(XSTSHarness
  src/XSTSHarness/XSTSHarness.cpp
  src/XSTSHarness/XSTSHarness.hpp
  src/XSTSHarness/XSTSHarnessHandlers.cpp
  src/XSTSHarness/XSTSHarnessHandlers.hpp
  src/XSTSHarness/XMLHarnessHandlers.cpp
  src/XSTSHarness/XMLHarnessHandlers.hpp
)

# Fails to compile under gcc4 (values too long for long type)
add_test_executable(XSValueTest
  src/XSValueTest/XSValueTest.cpp
  src/XSValueTest/XSValueTest.hpp
)

if(XERCES_XMLCH_T STREQUAL "char16_t")
  add_test_executable(Char16Test
    src/Char16Test/Char16Test.cpp
  )
  if(MSVC)
    set_target_properties(Char16Test PROPERTIES COMPILE_FLAGS "/utf-8")
  endif()
endif()

# Run tests
include(XercesTest)

add_xerces_test(DOMTest          COMMAND DOMTest)
add_xerces_test(DOMMemTest       COMMAND DOMMemTest)
add_xerces_test(RangeTest        COMMAND RangeTest)
add_xerces_test(DOMTraversalTest COMMAND DOMTraversalTest)
add_xerces_test(XSerializerTest  COMMAND XSerializerTest EXPECT_FAIL)
add_xerces_test(XSerializerTest1 COMMAND XSerializerTest -v=never  personal.xml)
add_xerces_test(XSerializerTest2 COMMAND XSerializerTest -v=always personal.xml)
add_xerces_test(XSerializerTest3 COMMAND XSerializerTest -v=never  personal-schema.xml)
add_xerces_test(XSerializerTest4 COMMAND XSerializerTest -v=always personal-schema.xml)
add_xerces_test(XSerializerTest5 COMMAND XSerializerTest -v=always -f personal-schema.xml)
add_xerces_test(XSValueTest      COMMAND XSValueTest)

add_xerces_test(InitTermTest     COMMAND InitTermTest EXPECT_FAIL)
add_xerces_test(InitTermTest1    COMMAND InitTermTest personal.xml)
add_xerces_test(InitTermTest2    COMMAND InitTermTest -n -s personal-schema.xml)
add_xerces_test(InitTermTest3    COMMAND InitTermTest -n -s -f personal-schema.xml)

if(NOT XERCES_USE_MUTEXMGR_NOTHREAD)
  add_xerces_test(ThreadTest       COMMAND ThreadTest EXPECT_FAIL)
  add_xerces_test(ThreadTest1      COMMAND ThreadTest -parser=sax               -v=never  -quiet -threads 10 -time 20 personal.xml)
  add_xerces_test(ThreadTest2      COMMAND ThreadTest -parser=dom               -v=never  -quiet -threads 10 -time 20 personal.xml)
  add_xerces_test(ThreadTest3      COMMAND ThreadTest -parser=sax2              -v=never  -quiet -threads 10 -time 20 personal.xml)
  add_xerces_test(ThreadTest4      COMMAND ThreadTest -parser=sax               -v=always -quiet -threads 10 -time 20 personal.xml)
  add_xerces_test(ThreadTest5      COMMAND ThreadTest -parser=dom               -v=always -quiet -threads 10 -time 20 personal.xml)
  add_xerces_test(ThreadTest6      COMMAND ThreadTest -parser=sax2              -v=always -quiet -threads 10 -time 20 personal.xml)
  add_xerces_test(ThreadTest7      COMMAND ThreadTest -parser=sax  -gc          -v=always -quiet -threads 10 -time 20 personal.xml)
  add_xerces_test(ThreadTest8      COMMAND ThreadTest -parser=dom  -gc          -v=always -quiet -threads 10 -time 20 personal.xml)
  add_xerces_test(ThreadTest9      COMMAND ThreadTest -parser=sax2 -gc          -v=always -quiet -threads 10 -time 20 personal.xml)
  add_xerces_test(ThreadTest10     COMMAND ThreadTest -parser=sax  -n -s -f     -v=always -quiet -threads 10 -time 20 personal-schema.xml)
  add_xerces_test(ThreadTest11     COMMAND ThreadTest -parser=dom  -n -s -f     -v=always -quiet -threads 10 -time 20 personal-schema.xml)
  add_xerces_test(ThreadTest12     COMMAND ThreadTest -parser=sax2 -n -s -f     -v=always -quiet -threads 10 -time 20 personal-schema.xml)
  add_xerces_test(ThreadTest13     COMMAND ThreadTest -parser=sax  -gc -n -s -f -v=always -quiet -threads 10 -time 20 personal-schema.xml)
  add_xerces_test(ThreadTest14     COMMAND ThreadTest -parser=dom  -gc -n -s -f -v=always -quiet -threads 10 -time 20 personal-schema.xml)
  add_xerces_test(ThreadTest15     COMMAND ThreadTest -parser=sax2 -gc -n -s -f -v=always -quiet -threads 10 -time 20 personal-schema.xml)
endif()

add_xerces_test(MemHandlerTest   COMMAND MemHandlerTest EXPECT_FAIL)
add_xerces_test(MemHandlerTest1  COMMAND MemHandlerTest -v=always -n -r=2 personal.xml)
add_xerces_test(MemHandlerTest2  COMMAND MemHandlerTest -v=always -n -r=2 -s -f personal-schema.xml)

add_xerces_test(DOMTypeInfoTest WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/DOM/TypeInfo" COMMAND DOMTypeInfoTest)

if(XERCES_XMLCH_T STREQUAL "char16_t")
  add_xerces_test(Char16Test       COMMAND Char16Test)
endif()
