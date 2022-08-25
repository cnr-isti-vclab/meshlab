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

# path delimiters

set(path_delims /)

set(xerces_path_delimiter_backslash_default OFF)
if(WIN32)
  set(xerces_path_delimiter_backslash_default ON)
endif()

option(path-delimiter-backslash "Accept backslash as a path delimiter" ${xerces_path_delimiter_backslash_default})

set(XERCES_PATH_DELIMITER_BACKSLASH 0)
if(path-delimiter-backslash)
  set(path_delims "${path_delims}\\")
  set(XERCES_PATH_DELIMITER_BACKSLASH 1)
endif()
