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

# Install docs.  Install from source tree if pre-generated, then the
# build tree if generated during the build.
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html")
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/html"
  DESTINATION "${CMAKE_INSTALL_DOCDIR}"
  COMPONENT "development")
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html"
  DESTINATION "${CMAKE_INSTALL_DOCDIR}"
    COMPONENT "development")

# Add createdocs target (if Java is available, plus the needed jars)
find_package(Java COMPONENTS Runtime)
if(Java_FOUND
    AND EXISTS "${PROJECT_SOURCE_DIR}/tools/jars/stylebook-1.0-b2.jar"
    AND EXISTS "${PROJECT_SOURCE_DIR}/tools/jars/xalan.jar"
    AND EXISTS "${PROJECT_SOURCE_DIR}/tools/jars/xerces.jar")
  if(UNIX)
    set(cpsep ":")
  else()
    set(cpsep ";")
  endif()
  set(classpath "${PROJECT_SOURCE_DIR}/tools/jars/stylebook-1.0-b2.jar${cpsep}${PROJECT_SOURCE_DIR}/tools/jars/xalan.jar${cpsep}${PROJECT_SOURCE_DIR}/tools/jars/xerces.jar")
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc/style/dtd}")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/style/dtd/entities.ent.in
                 ${CMAKE_CURRENT_BINARY_DIR}/style/dtd/entities.ent @ONLY)
  add_custom_target(createdocs
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
      ${CMAKE_CURRENT_SOURCE_DIR}/style
      ${CMAKE_CURRENT_BINARY_DIR}/style
    COMMAND "${Java_JAVA_EXECUTABLE}" -Djava.awt.headless=true
    -classpath "${classpath}"
    org.apache.stylebook.StyleBook
    "targetDirectory=${CMAKE_CURRENT_BINARY_DIR}/html"
    ${CMAKE_CURRENT_SOURCE_DIR}/xerces-c_book.xml
    ${CMAKE_CURRENT_BINARY_DIR}/style
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
  set_target_properties(createdocs PROPERTIES FOLDER "Documentation")
endif()

# Add createapidocs target (if doxygen is available)
find_program(DOXYGEN_EXECUTABLE doxygen)
if(DOXYGEN_EXECUTABLE)
  set(VERSION "${PROJECT_VERSION}")
  set(abs_top_srcdir "${PROJECT_SOURCE_DIR}")
  set(abs_top_builddir "${PROJECT_BINARY_DIR}")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
                 ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(createapidocs
    COMMAND "${DOXYGEN_EXECUTABLE}" Doxyfile
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
  set_target_properties(createapidocs PROPERTIES FOLDER "Documentation")
endif()

# Add docs to IDE
file(GLOB doc_xml
  "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
file(GLOB doc_style
  "${CMAKE_CURRENT_SOURCE_DIR}/style/dtd/*.ent"
  "${CMAKE_CURRENT_SOURCE_DIR}/style/graphics/*.gif"
  "${CMAKE_CURRENT_SOURCE_DIR}/style/*.xml"
  "${CMAKE_CURRENT_SOURCE_DIR}/style/resources/*.gif"
  "${CMAKE_CURRENT_SOURCE_DIR}/style/stylesheets/*.xsl")

add_custom_target(doc-xml SOURCES ${doc_xml})
set_target_properties(doc-xml PROPERTIES FOLDER "Documentation")
add_custom_target(doc-style SOURCES ${doc_style})
set_target_properties(doc-style PROPERTIES FOLDER "Documentation")

# Source file grouping (for IDE project layout)
set(source_files ${doc_style})
list(SORT source_files)
unset(group_dir)
unset(group_files)
foreach(file IN LISTS source_files)
  get_filename_component(dir "${file}" PATH)
  file(RELATIVE_PATH dir "${CMAKE_CURRENT_SOURCE_DIR}" "${dir}")
  if(group_dir AND NOT dir STREQUAL group_dir)
    if(group_files)
      source_group("${group_dir}" FILES ${group_files})
    endif()
    unset(group_files)
  endif()
  list(APPEND group_files "${file}")
  set(group_dir "${dir}")
endforeach()
if(group_files)
  source_group("${group_dir}" FILES ${group_files})
endif()
unset(group_dir)
unset(group_files)
