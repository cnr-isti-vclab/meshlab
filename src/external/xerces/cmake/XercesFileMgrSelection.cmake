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

# file manager

set(xerces_filemgr_default "POSIX")
if(WIN32)
  set(xerces_filemgr_default "windows")
endif()

set(file-manager "${xerces_filemgr_default}" CACHE STRING "File manager")
set(filemgr "${file-manager}")

if(NOT file-manager STREQUAL "POSIX" AND NOT file-manager STREQUAL "windows")
  message(FATAL_ERROR "Invalid file manager type \"${file-manager}\"")
endif()

set(XERCES_USE_FILEMGR_POSIX 0)
set(XERCES_USE_FILEMGR_WINDOWS 0)

if(file-manager STREQUAL "POSIX")
  set(XERCES_USE_FILEMGR_POSIX 1)
endif()
if(file-manager STREQUAL "windows")
  set(XERCES_USE_FILEMGR_WINDOWS 1)
endif()
