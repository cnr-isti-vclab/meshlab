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

# include checks

include(CheckIncludeFileCXX)

check_include_file_cxx(arpa/inet.h                 HAVE_ARPA_INET_H)
check_include_file_cxx(dlfcn.h                     HAVE_DLFCN_H)
check_include_file_cxx(fcntl.h                     HAVE_FCNTL_H)
check_include_file_cxx(float.h                     HAVE_FLOAT_H)
check_include_file_cxx(langinfo.h                  HAVE_LANGINFO_H)
check_include_file_cxx(limits.h                    HAVE_LIMITS_H)
check_include_file_cxx(locale.h                    HAVE_LOCALE_H)
check_include_file_cxx(memory.h                    HAVE_MEMORY_H)
check_include_file_cxx(netdb.h                     HAVE_NETDB_H)
check_include_file_cxx(netinet/in.h                HAVE_NETINET_IN_H)
check_include_file_cxx(nl_types.h                  HAVE_NL_TYPES_H)
check_include_file_cxx(stdbool.h                   HAVE_STDBOOL_H)
check_include_file_cxx(stddef.h                    HAVE_STDDEF_H)
check_include_file_cxx(stdlib.h                    HAVE_STDLIB_H)
check_include_file_cxx(string.h                    HAVE_STRING_H)
check_include_file_cxx(strings.h                   HAVE_STRINGS_H)
check_include_file_cxx(sys/param.h                 HAVE_SYS_PARAM_H)
check_include_file_cxx(sys/socket.h                HAVE_SYS_SOCKET_H)
check_include_file_cxx(sys/stat.h                  HAVE_SYS_STAT_H)
check_include_file_cxx(sys/time.h                  HAVE_SYS_TIME_H)
check_include_file_cxx(sys/timeb.h                 HAVE_SYS_TIMEB_H)
check_include_file_cxx(sys/types.h                 HAVE_SYS_TYPES_H)
check_include_file_cxx(unistd.h                    HAVE_UNISTD_H)
check_include_file_cxx(wchar.h                     HAVE_WCHAR_H)
check_include_file_cxx(wctype.h                    HAVE_WCTYPE_H)
check_include_file_cxx(endian.h                    HAVE_ENDIAN_H)
check_include_file_cxx(machine/endian.h            HAVE_MACHINE_ENDIAN_H)
check_include_file_cxx(arpa/nameser_compat.h       HAVE_ARPA_NAMESER_COMPAT_H)

set(XERCES_HAVE_SYS_TYPES_H ${HAVE_SYS_TYPES_H})
