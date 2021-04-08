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

# function checks

include(CheckFunctionExists)

check_function_exists(getcwd HAVE_GETCWD)
check_function_exists(pathconf HAVE_PATHCONF)
check_function_exists(realpath HAVE_REALPATH)
check_function_exists(getaddrinfo HAVE_GETADDRINFO)
check_function_exists(gethostbyaddr HAVE_GETHOSTBYADDR)
check_function_exists(gethostbyname HAVE_GETHOSTBYNAME)
check_function_exists(socket HAVE_SOCKET)
check_function_exists(clock_gettime HAVE_CLOCK_GETTIME)
check_function_exists(ftime HAVE_FTIME)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(gmtime_r HAVE_GMTIME_R)
check_function_exists(memmove HAVE_MEMMOVE)
check_function_exists(memset HAVE_MEMSET)
check_function_exists(nl_langinfo HAVE_NL_LANGINFO)
check_function_exists(setlocale HAVE_SETLOCALE)
check_function_exists(localeconv HAVE_LOCALECONV)
check_function_exists(snprintf HAVE_SNPRINTF)
check_function_exists(strcasecmp HAVE_STRCASECMP)
check_function_exists(strncasecmp HAVE_STRNCASECMP)
check_function_exists(stricmp HAVE_STRICMP)
check_function_exists(strnicmp HAVE_STRNICMP)
check_function_exists(strchr HAVE_STRCHR)
check_function_exists(strdup HAVE_STRDUP)
check_function_exists(strrchr HAVE_STRRCHR)
check_function_exists(strstr HAVE_STRSTR)
check_function_exists(strtol HAVE_STRTOL)
check_function_exists(strtoul HAVE_STRTOUL)
check_function_exists(timegm HAVE_TIMEGM)
check_function_exists(towupper HAVE_TOWUPPER)
check_function_exists(towlower HAVE_TOWLOWER)
check_function_exists(mblen HAVE_MBLEN)
check_function_exists(wcsupr HAVE_WCSUPR)
check_function_exists(wcslwr HAVE_WCSLWR)
check_function_exists(wcsnicmp HAVE_WCSNICMP)
check_function_exists(wcsicmp HAVE_WCSICMP)
