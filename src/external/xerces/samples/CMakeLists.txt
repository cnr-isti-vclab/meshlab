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
  data/personal.dtd
  data/personal.xml
  data/personal.xsd
  data/personal-schema.xml
  data/redirect.dtd
  data/long.xml
  )

macro(add_sample_executable name)
  add_executable(${name} ${ARGN})
  target_link_libraries(${name} xerces-c)
  set_target_properties(${name} PROPERTIES FOLDER "Samples")
  list(APPEND sample_programs ${name})
endmacro()

add_sample_executable(CreateDOMDocument
  src/CreateDOMDocument/CreateDOMDocument.cpp
  )

add_sample_executable(DOMCount
  src/DOMCount/DOMCount.cpp
  src/DOMCount/DOMCount.hpp
  )

add_sample_executable(DOMPrint
  src/DOMPrint/DOMPrint.cpp
  src/DOMPrint/DOMPrintErrorHandler.cpp
  src/DOMPrint/DOMPrintErrorHandler.hpp
  src/DOMPrint/DOMPrintFilter.cpp
  src/DOMPrint/DOMPrintFilter.hpp
  src/DOMPrint/DOMTreeErrorReporter.cpp
  src/DOMPrint/DOMTreeErrorReporter.hpp
  )

add_sample_executable(EnumVal
  src/EnumVal/EnumVal.cpp
  )

add_sample_executable(MemParse
  src/MemParse/MemParse.cpp
  src/MemParse/MemParse.hpp
  src/MemParse/MemParseHandlers.cpp
  src/MemParse/MemParseHandlers.hpp
  )

add_sample_executable(PParse
  src/PParse/PParse.cpp
  src/PParse/PParse.hpp
  src/PParse/PParseHandlers.cpp
  src/PParse/PParseHandlers.hpp
  )

add_sample_executable(PSVIWriter
  src/PSVIWriter/PSVIWriter.cpp
  src/PSVIWriter/PSVIWriter.hpp
  src/PSVIWriter/PSVIWriterHandlers.cpp
  src/PSVIWriter/PSVIWriterHandlers.hpp
  )

add_sample_executable(Redirect
  src/Redirect/Redirect.cpp
  src/Redirect/Redirect.hpp
  src/Redirect/RedirectHandlers.cpp
  src/Redirect/RedirectHandlers.hpp
  )

add_sample_executable(SAX2Count
  src/SAX2Count/SAX2Count.cpp
  src/SAX2Count/SAX2Count.hpp
  src/SAX2Count/SAX2CountHandlers.cpp
  src/SAX2Count/SAX2CountHandlers.hpp
  )

add_sample_executable(SAX2Print
  src/SAX2Print/SAX2FilterHandlers.cpp
  src/SAX2Print/SAX2FilterHandlers.hpp
  src/SAX2Print/SAX2Print.cpp
  src/SAX2Print/SAX2Print.hpp
  src/SAX2Print/SAX2PrintHandlers.cpp
  src/SAX2Print/SAX2PrintHandlers.hpp
  )

add_sample_executable(SAXCount
  src/SAXCount/SAXCount.cpp
  src/SAXCount/SAXCount.hpp
  src/SAXCount/SAXCountHandlers.cpp
  src/SAXCount/SAXCountHandlers.hpp
  )

add_sample_executable(SAXPrint
  src/SAXPrint/SAXPrint.cpp
  src/SAXPrint/SAXPrint.hpp
  src/SAXPrint/SAXPrintHandlers.cpp
  src/SAXPrint/SAXPrintHandlers.hpp
  )

add_sample_executable(SCMPrint
  src/SCMPrint/SCMPrint.cpp
  )

add_sample_executable(SEnumVal
  src/SEnumVal/SEnumVal.cpp
  )

add_sample_executable(StdInParse
  src/StdInParse/StdInParse.cpp
  src/StdInParse/StdInParse.hpp
  src/StdInParse/StdInParseHandlers.cpp
  src/StdInParse/StdInParseHandlers.hpp
  )

add_sample_executable(XInclude
  src/XInclude/XInclude.cpp
  src/XInclude/XInclude.hpp
  )

install(
  TARGETS ${sample_programs}
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  COMPONENT "runtime")

# Run tests
include(XercesTest)

add_xerces_sample_test(SAXCount          COMMAND SAXCount EXPECT_FAIL)
add_xerces_sample_test(SAXCount1         COMMAND SAXCount -v=never personal.xml)
add_xerces_sample_test(SAXCount2         COMMAND SAXCount          personal.xml)
add_xerces_sample_test(SAXCount3         COMMAND SAXCount -n -s    personal-schema.xml)
add_xerces_sample_test(SAXPrint          COMMAND SAXPrint EXPECT_FAIL)
add_xerces_sample_test(SAXPrint1         COMMAND SAXPrint -v=never personal.xml)
add_xerces_sample_test(SAXPrint2         COMMAND SAXPrint          personal.xml)
add_xerces_sample_test(SAXPrint3         COMMAND SAXPrint -n -s    personal-schema.xml)
add_xerces_sample_test(SAX2Count         COMMAND SAX2Count EXPECT_FAIL)
add_xerces_sample_test(SAX2Count1        COMMAND SAX2Count -v=never personal.xml)
add_xerces_sample_test(SAX2Count2        COMMAND SAX2Count          personal.xml)
add_xerces_sample_test(SAX2Count3        COMMAND SAX2Count -p       personal-schema.xml)
add_xerces_sample_test(SAX2Print         COMMAND SAX2Print EXPECT_FAIL)
add_xerces_sample_test(SAX2Print1        COMMAND SAX2Print -v=never personal.xml)
add_xerces_sample_test(SAX2Print2        COMMAND SAX2Print          personal.xml)
add_xerces_sample_test(SAX2Print3        COMMAND SAX2Print -p       personal-schema.xml)
add_xerces_sample_test(SAX2Print4        COMMAND SAX2Print          personal.xsd)
add_xerces_sample_test(SAX2Print5        COMMAND SAX2Print -sa      personal.xsd)
add_xerces_sample_test(MemParse          COMMAND MemParse)
add_xerces_sample_test(MemParse1         COMMAND MemParse -v=never)
add_xerces_sample_test(Redirect          COMMAND Redirect EXPECT_FAIL)
add_xerces_sample_test(Redirect1         COMMAND Redirect personal.xml)
add_xerces_sample_test(DOMCount          COMMAND DOMCount EXPECT_FAIL)
add_xerces_sample_test(DOMCount1         COMMAND DOMCount -v=never personal.xml)
add_xerces_sample_test(DOMCount2         COMMAND DOMCount          personal.xml)
add_xerces_sample_test(DOMCount3         COMMAND DOMCount -n -s    personal-schema.xml)
add_xerces_sample_test(DOMPrint          COMMAND DOMPrint EXPECT_FAIL)
add_xerces_sample_test(DOMPrint1         COMMAND DOMPrint -wfpp=on -wddc=off -v=never personal.xml)
add_xerces_sample_test(DOMPrint2         COMMAND DOMPrint -wfpp=on -wddc=off          personal.xml)
add_xerces_sample_test(DOMPrint3         COMMAND DOMPrint -wfpp=on -wddc=on           personal.xml)
add_xerces_sample_test(DOMPrint4         COMMAND DOMPrint -wfpp=on -wddc=off -n -s    personal-schema.xml)
add_xerces_sample_test(DOMPrint5         COMMAND DOMPrint -v=never -xpath=//name      personal.xml)
add_xerces_sample_test(StdInParse1       COMMAND StdInParse STDIN personal.xml)
add_xerces_sample_test(StdInParse2       COMMAND StdInParse -v=never STDIN personal.xml)
add_xerces_sample_test(StdInParse3       COMMAND StdInParse STDIN personal-schema.xml)
add_xerces_sample_test(PParse            COMMAND PParse EXPECT_FAIL)
add_xerces_sample_test(PParse1           COMMAND PParse       personal.xml)
add_xerces_sample_test(PParse2           COMMAND PParse -n -s personal-schema.xml)
add_xerces_sample_test(EnumVal           COMMAND EnumVal EXPECT_FAIL)
add_xerces_sample_test(EnumVal1          COMMAND EnumVal personal.xml)
add_xerces_sample_test(SEnumVal          COMMAND SEnumVal EXPECT_FAIL)
add_xerces_sample_test(SEnumVal1         COMMAND SEnumVal personal-schema.xml)
add_xerces_sample_test(CreateDOMDocument COMMAND CreateDOMDocument)
add_xerces_sample_test(SAXPrintLong      COMMAND SAXPrint -v=never long.xml)
