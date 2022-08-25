#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#

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
#

#
# $Id$
#

AC_PREREQ(2.60)
AC_INIT([xerces-c],[3.2.3])
INTERFACE_VERSION=3.2
GRAMMAR_SERIALIZATION_LEVEL=7

XERCES_VERSION_MAJOR=$(echo $PACKAGE_VERSION | cut -d. -f1)
XERCES_VERSION_MINOR=$(echo $PACKAGE_VERSION | cut -d. -f2)
XERCES_VERSION_REVISION=$(echo $PACKAGE_VERSION | cut -d. -f3)

INTERFACE_VERSION_MAJOR=$(echo $INTERFACE_VERSION | cut -d. -f1)
INTERFACE_VERSION_MINOR=$(echo $INTERFACE_VERSION | cut -d. -f2)

INTERFACE_VERSION_D="${INTERFACE_VERSION_MAJOR}.${INTERFACE_VERSION_MINOR}"
INTERFACE_VERSION_U="${INTERFACE_VERSION_MAJOR}_${INTERFACE_VERSION_MINOR}"

AC_SUBST([XERCES_VERSION_MAJOR])
AC_SUBST([XERCES_VERSION_MINOR])
AC_SUBST([XERCES_VERSION_REVISION])
AC_SUBST([INTERFACE_VERSION_MAJOR])
AC_SUBST([INTERFACE_VERSION_MINOR])
AC_SUBST([INTERFACE_VERSION_D])
AC_SUBST([INTERFACE_VERSION_U])

AC_DEFINE_UNQUOTED([XERCES_VERSION_MAJOR], $XERCES_VERSION_MAJOR, [Definition of Xerces major version])
AC_DEFINE_UNQUOTED([XERCES_VERSION_MINOR], $XERCES_VERSION_MINOR, [Definition of Xerces minor version])
AC_DEFINE_UNQUOTED([XERCES_VERSION_REVISION], $XERCES_VERSION_REVISION, [Definition of Xerces patch version])

AC_DEFINE_UNQUOTED([INTERFACE_VERSION_MAJOR], $INTERFACE_VERSION_MAJOR, [Definition of Xerces interface major version])
AC_DEFINE_UNQUOTED([INTERFACE_VERSION_MINOR], $INTERFACE_VERSION_MINOR, [Definition of Xerces interface minor version])

AC_DEFINE_UNQUOTED([XERCES_GRAMMAR_SERIALIZATION_LEVEL], $GRAMMAR_SERIALIZATION_LEVEL, [Definition of Xerces grammar serialization level])

AC_CONFIG_SRCDIR([src/Makefile.am])
AC_CONFIG_LIBOBJ_DIR([src])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h src/xercesc/util/Xerces_autoconf_config.hpp])
AC_CONFIG_HEADER([src/xercesc/util/XercesVersion.hpp])
AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 doc/Doxyfile
                 doc/style/dtd/entities.ent
                 src/Makefile
                 tests/Makefile
                 samples/Makefile
		 xerces-c.pc
                 scripts/run-test])

# Check the target system
AC_CANONICAL_HOST

# Initialize automake
AM_SILENT_RULES([yes])
AM_INIT_AUTOMAKE([foreign subdir-objects dist-bzip2 tar-ustar])
AM_MAINTAINER_MODE

# Check if rpath is disabled
AC_MSG_CHECKING(whether to use rpath)
AC_ARG_ENABLE(rpath,
	[AC_HELP_STRING([--disable-rpath],
			[Patches libtool to not use rpath in the libraries produced.])],
	[xerces_cv_rpath="$enable_rpath"], [xerces_cv_rpath="yes"])
AC_MSG_RESULT($xerces_cv_rpath)

# Checks for programs.
AC_PROG_CXX
# Use the C++ compiler for the compile tests
AC_LANG(C++)
# used by "make check"
AC_PROG_SED
#it would be nice to also check for PERL...
#AC_PROG_PERL

#skip detection of Fortran
m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])

AC_PROG_LN_S
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AM_PROG_CC_C_O

AS_IF([test x$AR = xfalse],
	AC_MSG_ERROR([Cannot find a valid 'ar' tool]))

# Patch libtool to not use rpath if requested.
#
AC_CONFIG_COMMANDS([libtool-rpath-patch],
[if test "$libtool_patch_use_rpath" = "no"; then
   echo config.status: patching libtool to not use rpath
   sed < libtool > libtool-2 's/^hardcode_libdir_flag_spec.*$'/'hardcode_libdir_flag_spec=" -D__LIBTOOL_NO_RPATH__ "/'
   mv libtool-2 libtool
   chmod 755 libtool
fi],
[libtool_patch_use_rpath=$xerces_cv_rpath])

# Export information on whether we are building static/shared libraries.
# enable_shared and enable_static are defined by AC_PROG_LIBTOOL.
#
AC_SUBST([BUILD_SHARED], [$enable_shared])
AC_SUBST([BUILD_STATIC], [$enable_static])

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS([arpa/inet.h fcntl.h float.h inttypes.h langinfo.h limits.h locale.h \
                  memory.h netdb.h netinet/in.h nl_types.h stddef.h stdint.h stdlib.h \
                  string.h strings.h \
                  sys/param.h sys/socket.h sys/time.h sys/timeb.h \
                  unistd.h wchar.h wctype.h \
                  CoreServices/CoreServices.h \
                  endian.h machine/endian.h arpa/nameser_compat.h \
                 ])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_C_VOLATILE

AC_CHECK_SIZEOF(wchar_t)
AC_CHECK_TYPE(size_t)
AC_CHECK_TYPE(ssize_t)

AC_TYPE_OFF_T
AC_TYPE_SIZE_T

XERCES_INT_TYPES

AC_CXX_HAVE_BOOL
AC_CXX_HAVE_NAMESPACES
AC_CXX_HAVE_STD_NAMESPACE
AC_CXX_HAVE_STD_LIBS
AC_CXX_HAVE_LSTRING

ACX_PTHREAD

# Checks for library functions.
#AC_FUNC_ERROR_AT_LINE
#AC_FUNC_MALLOC
#AC_FUNC_MEMCMP
#AC_FUNC_STRCOLL
#AC_FUNC_STRTOD
AC_CHECK_FUNCS([getcwd pathconf realpath \
		getaddrinfo gethostbyaddr gethostbyname socket \
		clock_gettime ftime gettimeofday timegm gmtime_r \
		memmove memset nl_langinfo setlocale localeconv \
		strcasecmp strncasecmp stricmp strnicmp strchr strdup \
		strrchr strstr strtol strtoul snprintf \
		towupper towlower mblen \
		wcsupr wcslwr wcsnicmp wcsicmp \
		])

# Some Unix systems, like Gnu Hurd, don't define PATH_MAX
AC_MSG_CHECKING([for PATH_MAX])
AC_COMPILE_IFELSE( [AC_LANG_PROGRAM([[#include <limits.h>]],
                                    [[char dummy[PATH_MAX];]])],
                   [
                      AC_MSG_RESULT([yes])
                      AC_DEFINE_UNQUOTED([HAVE_PATH_MAX], 1, [Define to 1 if you have the PATH_MAX macro.])
                    ],
                    [
                      AC_MSG_RESULT([no])
                      AC_DEFINE_UNQUOTED([HAVE_PATH_MAX], 0, [Define to 1 if you have the PATH_MAX macro.])
                    ]
                 )

# Check for functional cstdint header
AC_MSG_CHECKING([for cstdint])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <cstdint>]],
[[
  uint32_t v1 = 342;
  int64_t v2 = -23;
  return 0;
]])],
                    [
                      AC_MSG_RESULT([yes])
                      AC_DEFINE_UNQUOTED([XERCES_HAVE_CSTDINT], 1, [Define to 1 if cstdint is functional.])
                    ],
                    [
                      AC_MSG_RESULT([no])
                      AC_DEFINE_UNQUOTED([XERCES_HAVE_CSTDINT], 0, [Define to 1 if cstdint is functional.])
                    ]
                 )

# The check for mbrlen, wcsrtombs and mbsrtowcs gives a false
# positive on HP-UX, so we use a different snippet to set the
# corresponding macro
# Furthermore, OpenBSD 5.1 doesn't follow the standard that states
# that the "src" pointer must be set to NULL when a \0 has been 
# converted in the source data, a behaviour that we rely on
# So we also check for this scenario before using these functions
AC_MSG_CHECKING([for mbrlen])
AC_COMPILE_IFELSE(  [AC_LANG_PROGRAM([[#include <wchar.h>]],
                                     [[mbstate_t st; mbrlen( "t", 5, &st );]])],
                    [
                      AC_MSG_RESULT([yes])
                      AC_DEFINE_UNQUOTED([HAVE_MBRLEN], 1, [Define to 1 if you have the `mbrlen' function.])
                    ],
                    [
                      AC_MSG_RESULT([no])
                      AC_DEFINE_UNQUOTED([HAVE_MBRLEN], 0, [Define to 1 if you have the `mbrlen' function.])
                    ]
                 )
AC_MSG_CHECKING([for wcsrtombs])
AC_RUN_IFELSE(  [AC_LANG_PROGRAM([[#include <wchar.h>
#include <string.h>]],
                                 [[
mbstate_t st;
memset(&st, 0, sizeof(st));
char buffer[32];
const wchar_t* src=L"help";
wcsrtombs(buffer, &src, 32, &st);
if(src==0)
    return 0;
else
    return 1;]])],
                    [
                      AC_MSG_RESULT([yes])
                      AC_DEFINE_UNQUOTED([HAVE_WCSRTOMBS], 1, [Define to 1 if you have the `wcsrtombs' function.])
                    ],
                    [
                      AC_MSG_RESULT([no])
                      AC_DEFINE_UNQUOTED([HAVE_WCSRTOMBS], 0, [Define to 1 if you have the `wcsrtombs' function.])
                    ]
                 )
AC_MSG_CHECKING([for mbsrtowcs])
AC_RUN_IFELSE(  [AC_LANG_PROGRAM([[#include <wchar.h>
#include <string.h>]],
                                 [[
mbstate_t st;
memset(&st, 0, sizeof(st));
wchar_t buffer[32];
const char* src="help";
mbsrtowcs(buffer, &src, 32, &st);
if(src==0)
    return 0;
else
    return 1;]])],
                    [
                      AC_MSG_RESULT([yes])
                      AC_DEFINE_UNQUOTED([HAVE_MBSRTOWCS], 1, [Define to 1 if you have the `mbsrtowcs' function.])
                    ],
                    [
                      AC_MSG_RESULT([no])
                      AC_DEFINE_UNQUOTED([HAVE_MBSRTOWCS], 0, [Define to 1 if you have the `mbsrtowcs' function.])
                    ]
                 )

AC_MSG_CHECKING([if iconv uses const pointers])
AC_COMPILE_IFELSE(  [AC_LANG_PROGRAM([[#include <iconv.h>]],
                                     [[
                                       const char *fromPtr=0;
                                       size_t     fromLen=0;
                                       char       *toPtr=0;
                                       size_t     toLen=0;
                                       iconv_t    cv=0;
                                       iconv(cv, &fromPtr, &fromLen, &toPtr, &toLen);
                                      ]])],
                    [
                      AC_MSG_RESULT([yes])
                      AC_DEFINE_UNQUOTED([ICONV_USES_CONST_POINTER], 1, [Define to 1 if you have to use const char* with iconv, to 0 if you must use char*.])
                    ],
                    [
                      AC_MSG_RESULT([no])
                      AC_DEFINE_UNQUOTED([ICONV_USES_CONST_POINTER], 0, [Define to 1 if you have to use const char* with iconv, to 0 if you must use char*.])
                    ]
                 )

AC_SUBST([SHREXT], [$shrext_cmds])

#
# Orchestrate the replacement of missing functions
#
AC_REPLACE_FUNCS([stricmp strnicmp towlower towupper])

abs_top_srcdir=`cd $srcdir; pwd`
AC_SUBST(abs_top_srcdir)
abs_top_builddir=`pwd`
AC_SUBST(abs_top_builddir)

######################################################
# Look for availability of available packages
######################################################

XERCES_PATH_DELIMITERS

XERCES_MUTEXMGR_SELECTION
XERCES_NETACCESSOR_SELECTION
XERCES_TRANSCODER_SELECTION
XERCES_MSGLOADER_SELECTION
XERCES_FILEMGR_SELECTION

# Allow the user to specify the pkgconfig directory.
#
AC_ARG_WITH(pkgconfigdir,
	AC_HELP_STRING([--with-pkgconfigdir=DIR],[Specify location of pkgconfig dir (default is libdir/pkgconfig)]),
	[pkgconfigdir=${withval}],
	[pkgconfigdir='${libdir}/pkgconfig'])

AC_SUBST([pkgconfigdir])

# Allow the user to disable the SSE2 support
#
AC_ARG_ENABLE(sse2,
    AC_HELP_STRING([--disable-sse2],[disable SSE2 optimizations]),
    [have_sse2=${enableval}],
    [have_sse2=yes])

######################################################
# Define some namespace-protected macros for use in the
# publicly visible Xerces_autoconf_config.h file.
######################################################

AC_DEFINE([XERCES_AUTOCONF], 1, [Define to true if autoconf is used in this configuration])

AS_IF([test x$ac_cv_header_stdint_h = xyes],
	AC_DEFINE([XERCES_HAVE_STDINT_H], 1, [Define to 1 if we have stdint.h]))
AS_IF([test x$ac_cv_header_sys_types_h = xyes],
	AC_DEFINE([XERCES_HAVE_SYS_TYPES_H], 1, [Define to 1 if we have sys/types.h]))
AS_IF([test x$ac_cv_header_inttypes_h = xyes],
	AC_DEFINE([XERCES_HAVE_INTTYPES_H],    1, [Define to 1 if we have inttypes.h]))

case $host in
*-*-msdos* | *-*-mingw32* | *-*-cygwin* | *-*-windows* )
	platform_export="__declspec(dllexport)"
	platform_import="__declspec(dllimport)"
        template_extern=""
	;;
* )
	platform_export=""
	platform_import=""
        template_extern="extern"
	;;
esac

AC_DEFINE_UNQUOTED([XERCES_PLATFORM_EXPORT], [$platform_export], [Define as the platform's export attribute])
AC_DEFINE_UNQUOTED([XERCES_PLATFORM_IMPORT], [$platform_import], [Define as the platform's import attribute])
AC_DEFINE_UNQUOTED([XERCES_TEMPLATE_EXTERN], [$template_extern], [Define as the platform's template extern attribute])

AS_IF([test ! $ac_cv_cxx_have_bool],
	AC_DEFINE([XERCES_NO_NATIVE_BOOL], 1, [Define if there is no native bool support in this environment]))

XERCES_XMLCH_SELECTION

AC_MSG_CHECKING([whether the compiler chokes on a placement operator delete])
AC_COMPILE_IFELSE(  [AC_LANG_PROGRAM([[#include <stdlib.h>

                                        class XMemory
                                        {
                                        public :
                                            void* operator new(size_t s) { return 0; }
                                            void* operator new(size_t s, void* ptr) { return 0; }
                                            void operator delete(void* p) {}
                                            void operator delete(void* p, void* ptr) {}
                                        };]],
                                     [[ ]])],
                    [
                      AC_MSG_RESULT([no])
                    ],
                    [
                      AC_MSG_RESULT([yes])
                      AC_DEFINE_UNQUOTED([XERCES_NO_MATCHING_DELETE_OPERATOR], 1, [Define to have XMemory.hpp avoid declaring a matching operator delete for the placement operator new])
                    ]
                 )

if test "$have_sse2" = "yes"; then
    no_sse2_CXXFLAGS="$CXXFLAGS"

    save_CXXFLAGS="$CXXFLAGS"
    CXXFLAGS="$CXXFLAGS -msse2"
	AC_MSG_CHECKING([whether we need to add -msse2])
	AC_COMPILE_IFELSE(  [AC_LANG_PROGRAM([[#include <emmintrin.h>]], [[__m128i one;]])],
						[msse2_ok=yes],
						[msse2_ok=no]
					 )
    AC_MSG_RESULT($msse2_ok)
    if test x"$msse2_ok" = xno; then
        CXXFLAGS="$save_CXXFLAGS"
    fi
# Sun CC option test. Currently disabled since it doesn't work. Also note
# that Sun CC will accept -msse2 (i.e., it won't fail) but will keep issuing
# warnings. So if enabled this test needs to be moved before -msse2 and if
# it passes, then -msse2 should be skipped.
#
#    save_CXXFLAGS="$CXXFLAGS"
#    CXXFLAGS="$CXXFLAGS -xarch=sse2"
#      AC_MSG_CHECKING([whether we need to add -xarch=sse2])
#      AC_COMPILE_IFELSE(  [AC_LANG_PROGRAM([[#include <emmintrin.h>]], [[__m128i one;]])],
#						[xarchsse2_ok=yes],
#						[xarchsse2_ok=no]
#					 )
#    AC_MSG_RESULT($xarchsse2_ok)
#    if test x"$xarchsse2_ok" = xno; then
#        CXXFLAGS="$save_CXXFLAGS"
#    fi

    AC_MSG_CHECKING([for intrin.h usability])
    AC_COMPILE_IFELSE(  [AC_LANG_PROGRAM([[#include <intrin.h>]],
                                         [[]])],
                        [
                          AC_MSG_RESULT([yes])
                          AC_DEFINE_UNQUOTED([XERCES_HAVE_INTRIN_H], 1, [Define to 1 if you have intrin.h])
                        ],
                        [
                          AC_MSG_RESULT([no])
                        ]
                     )
    AC_MSG_CHECKING([for emmintrin.h usability])
    AC_COMPILE_IFELSE(  [AC_LANG_PROGRAM([[#include <emmintrin.h>]],
                                         [[]])],
                        [
                          AC_MSG_RESULT([yes])
                          AC_DEFINE_UNQUOTED([XERCES_HAVE_EMMINTRIN_H], 1, [Define to 1 if you have emmintrin.h])
                        ],
                        [
                          AC_MSG_RESULT([no])
                        ]
                     )
    AC_MSG_CHECKING([for cpuid.h usability])
    AC_COMPILE_IFELSE(  [AC_LANG_PROGRAM([[#include <cpuid.h>]],
                                         [[]])],
                        [
                          AC_MSG_RESULT([yes])
                          AC_DEFINE_UNQUOTED([HAVE_CPUID_H], 1, [Define to 1 if you have cpuid.h])
                        ],
                        [
                          AC_MSG_RESULT([no])
                        ]
                     )

	AC_MSG_CHECKING([whether the compiler has the CPUID intrinsic])
	AC_COMPILE_IFELSE(  [AC_LANG_PROGRAM([[#include <intrin.h>]],
										 [[int CPUInfo[4];
										   __cpuid(CPUInfo, 1);
										 ]])],
						[
						  AC_MSG_RESULT([yes])
						  AC_DEFINE_UNQUOTED([XERCES_HAVE_CPUID_INTRINSIC], 1, [Define to have SSE2 instruction support detected at runtime using __cpuid])
						],
						[
						  AC_MSG_RESULT([no])
						]
					 )

	AC_MSG_CHECKING([whether the compiler has the _get_cpuid intrinsic])
	AC_COMPILE_IFELSE(  [AC_LANG_PROGRAM([[#include <cpuid.h>]],
										 [[unsigned int eax, ebx, ecx, edx;
                                           __get_cpuid (1, &eax, &ebx, &ecx, &edx);
										 ]])],
						[
						  AC_MSG_RESULT([yes])
						  AC_DEFINE_UNQUOTED([XERCES_HAVE_GETCPUID], 1, [Define to have SSE2 instruction support detected at runtime using __get_cpuid])
						],
						[
						  AC_MSG_RESULT([no])
						]
					 )

	AC_MSG_CHECKING([whether the compiler has the SSE2 intrinsic])
	AC_COMPILE_IFELSE(  [AC_LANG_PROGRAM([[#include <emmintrin.h>]],
										 [[__m128i* one=(__m128i*)_mm_malloc(4, 16);
										   __m128i* two=(__m128i*)_mm_malloc(4, 16);
										   __m128i xmm1 = _mm_load_si128(one);
										   __m128i xmm2 = _mm_load_si128(two);
										   __m128i xmm3 = _mm_or_si128(xmm1, xmm2);
										   _mm_store_si128(one, xmm3);
										   _mm_free(one);
										   _mm_free(two);
										 ]])],
						[
						  AC_MSG_RESULT([yes])
						  sse2_usable=yes
						  AC_DEFINE_UNQUOTED([XERCES_HAVE_SSE2_INTRINSIC], 1, [Define to have SSE2 instruction used at runtime])
						],
						[
						  AC_MSG_RESULT([no])
                                                  sse2_usable=no
						]

					 )
  # Restore original CXXFLAGS if SSE2 is not usable.
  #
  if test "$sse2_usable" = "no"; then
    CXXFLAGS="$no_sse2_CXXFLAGS"
  fi
fi

AS_IF([test x$ac_cv_type_size_t = xyes],
	AC_DEFINE([XERCES_SIZE_T], [size_t], [Define as the appropriate size_t type])
	AC_DEFINE([XERCES_SIZE_MAX], [SIZE_MAX], [Define as the appropriate SIZE_MAX macro]),
	AC_DEFINE([XERCES_SIZE_T], [unsigned long], [Define as the appropriate size_t type])
	AC_DEFINE([XERCES_SIZE_MAX], [ULONG_MAX], [Define as the appropriate SIZE_MAX macro]))

AS_IF([test x$ac_cv_type_ssize_t = xyes],
	AC_DEFINE([XERCES_SSIZE_T], [ssize_t], [Define as the appropriate ssize_t type])
	AC_DEFINE([XERCES_SSIZE_MAX], [SSIZE_MAX], [Define as the appropriate SSIZE_MAX macro]),
	AC_DEFINE([XERCES_SSIZE_T], [long], [Define as the appropriate ssize_t type])
	AC_DEFINE([XERCES_SSIZE_MAX], [LONG_MAX], [Define as the appropriate SSIZE_MAX macro]))

AS_IF([test x$ac_cv_cxx_have_namespaces = xyes],
	AC_DEFINE([XERCES_HAS_CPP_NAMESPACE], 1, [Define if namespaces is supported by the compiler]))

AS_IF([test x$ac_cv_cxx_have_std_namespace = xyes],
	AC_DEFINE([XERCES_STD_NAMESPACE], 1, [Define if the std namespace is supported]))

AS_IF([test x$ac_cv_cxx_have_std_libs = xyes],
	AC_DEFINE([XERCES_NEW_IOSTREAMS], 1, [Define if the isstream library can be included as <iostream>]))


AC_OUTPUT

AC_MSG_NOTICE
AC_MSG_NOTICE([Report:])
AC_MSG_NOTICE([  File Manager: $filemgr])
AC_MSG_NOTICE([  Mutex Manager: $mutexmgr])
AC_MSG_NOTICE([  Transcoder: $transcoder])
AC_MSG_NOTICE([  NetAccessor: $netaccessor])
AC_MSG_NOTICE([  Message Loader: $msgloader])
AC_MSG_NOTICE([  XMLCh Type: $xmlch])
