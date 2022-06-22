/* config.h.cmake.in.  Not generated, but originated from autoheader.  */
/* This file must be kept up-to-date with needed substitutions from config.h.in. */

/* Define to 1 if you have the <cstdint> header file. */
#define HAVE_CSTDINT 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

#if defined(__cplusplus) && defined(HAVE_CSTDINT)
#include <cstdint>
#elif HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#endif

/* Define to specify no threading is used */
/* #undef APP_NO_THREADS */

/* Define to 1 if you have the <arpa/inet.h> header file. */
/* #undef HAVE_ARPA_INET_H */

/* Define to 1 if you have the <arpa/nameser_compat.h> header file. */
/* #undef HAVE_ARPA_NAMESER_COMPAT_H */

/* define if bool is a built-in type */
#define HAVE_BOOL 1

/* Define to 1 if you have the `catclose' function. */
/* #undef HAVE_CATCLOSE */

/* Define to 1 if you have the `catgets' function. */
/* #undef HAVE_CATGETS */

/* Define to 1 if you have the `catopen' function. */
/* #undef HAVE_CATOPEN */

/* Define to 1 if you have the `clock_gettime' function. */
/* #undef HAVE_CLOCK_GETTIME */

/* Define to 1 if you have the <CoreServices/CoreServices.h> header file. */
/* #undef HAVE_CORESERVICES_CORESERVICES_H */

/* Define to 1 if you have cpuid.h */
/* #undef HAVE_CPUID_H */

/* Define to 1 if you have the <ctype.h> header file. */
#define HAVE_CTYPE_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */

/* Define to 1 if you have the <endian.h> header file. */
/* #undef HAVE_ENDIAN_H */

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <float.h> header file. */
#define HAVE_FLOAT_H 1

/* Define to 1 if you have the `ftime' function. */
#define HAVE_FTIME 1

/* Define to 1 if you have the `getaddrinfo' function. */
/* #undef HAVE_GETADDRINFO */

/* Define to 1 if you have the `getcwd' function. */
#define HAVE_GETCWD 1

/* Define to 1 if you have the `gethostbyaddr' function. */
/* #undef HAVE_GETHOSTBYADDR */

/* Define to 1 if you have the `gethostbyname' function. */
/* #undef HAVE_GETHOSTBYNAME */

/* Define to 1 if you have the `gettimeofday' function. */
/* #undef HAVE_GETTIMEOFDAY */

/* Define to 1 if you have the `gmtime_r' function. */
/* #undef HAVE_GMTIME_R */

/* Define to 1 if you have the `iconv' function. */
/* #undef HAVE_ICONV */

/* Define to 1 if you have the `iconv_close' function. */
/* #undef HAVE_ICONV_CLOSE */

/* Define to 1 if you have the <iconv.h> header file. */
/* #undef HAVE_ICONV_H */

/* Define to 1 if you have the `iconv_open' function. */
/* #undef HAVE_ICONV_OPEN */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <langinfo.h> header file. */
/* #undef HAVE_LANGINFO_H */

/* Define to 1 if you have the `nsl' library (-lnsl). */
/* #undef HAVE_LIBNSL */

/* Define to 1 if you have the `socket' library (-lsocket). */
/* #undef HAVE_LIBSOCKET */

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the `localeconv' function. */
#define HAVE_LOCALECONV 1

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* define if the compiler implements L"widestring" */
#define HAVE_LSTRING 1

/* Define to 1 if you have the <machine/endian.h> header file. */
/* #undef HAVE_MACHINE_ENDIAN_H */

/* Define to 1 if you have the `mblen' function. */
#define HAVE_MBLEN 1

/* Define to 1 if you have the `mbrlen' function. */
#define HAVE_MBRLEN 1

/* Define to 1 if you have the `mbsrtowcs' function. */
#define HAVE_MBSRTOWCS 1

/* Define to 1 if you have the `mbstowcs' function. */
#define HAVE_MBSTOWCS 1

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* define if the compiler implements namespaces */
#define HAVE_NAMESPACES 1

/* Define to 1 if you have the <netdb.h> header file. */
/* #undef HAVE_NETDB_H */

/* Define to 1 if you have the <netinet/in.h> header file. */
/* #undef HAVE_NETINET_IN_H */

/* Define to 1 if you have the `nl_langinfo' function. */
/* #undef HAVE_NL_LANGINFO */

/* Define to 1 if you have the <nl_types.h> header file. */
/* #undef HAVE_NL_TYPES_H */

/* Define to 1 if you have the `pathconf' function. */
/* #undef HAVE_PATHCONF */

/* Define to 1 if you have the PATH_MAX macro. */
/* #undef HAVE_PATH_MAX */

/* Define if you have POSIX threads libraries and header files. */
/* #undef HAVE_PTHREAD */

/* Define to 1 if you have the `realpath' function. */
/* #undef HAVE_REALPATH */

/* Define to 1 if you have the `setlocale' function. */
#define HAVE_SETLOCALE 1

/* Define to 1 if you have the `snprintf' function. */
/* #undef HAVE_SNPRINTF */

/* Define to 1 if you have the `socket' function. */
/* #undef HAVE_SOCKET */

/* Define to 1 if stdbool.h conforms to C99. */
#define HAVE_STDBOOL_H 1

/* Define to 1 if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* define if the compiler supports ISO C++ standard library */
#define HAVE_STD_LIBS 1

/* define if the compiler supports the std namespace */
#define HAVE_STD_NAMESPACE 1

/* Define to 1 if you have the `strcasecmp' function. */
/* #undef HAVE_STRCASECMP */

/* Define to 1 if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the `stricmp' function. */
#define HAVE_STRICMP 1

/* Define to 1 if you have the <strings.h> header file. */
/* #undef HAVE_STRINGS_H */

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strncasecmp' function. */
/* #undef HAVE_STRNCASECMP */

/* Define to 1 if you have the `strnicmp' function. */
#define HAVE_STRNICMP 1

/* Define to 1 if you have the `strrchr' function. */
#define HAVE_STRRCHR 1

/* Define to 1 if you have the `strstr' function. */
#define HAVE_STRSTR 1

/* Define to 1 if you have the `strtol' function. */
#define HAVE_STRTOL 1

/* Define to 1 if you have the `strtoul' function. */
#define HAVE_STRTOUL 1

/* Define to 1 if you have the <sys/param.h> header file. */
/* #undef HAVE_SYS_PARAM_H */

/* Define to 1 if you have the <sys/socket.h> header file. */
/* #undef HAVE_SYS_SOCKET_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/timeb.h> header file. */
#define HAVE_SYS_TIMEB_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
/* #undef HAVE_SYS_TIME_H */

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the `timegm' function. */
/* #undef HAVE_TIMEGM */

/* Define to 1 if you have the `towlower' function. */
#define HAVE_TOWLOWER 1

/* Define to 1 if you have the `towupper' function. */
#define HAVE_TOWUPPER 1

/* Define to 1 if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */

/* Define to 1 if you have the <wchar.h> header file. */
#define HAVE_WCHAR_H 1

/* Define to 1 if you have the `wcsicmp' function. */
#define HAVE_WCSICMP 1

/* Define to 1 if you have the `wcslwr' function. */
#define HAVE_WCSLWR 1

/* Define to 1 if you have the `wcsnicmp' function. */
#define HAVE_WCSNICMP 1

/* Define to 1 if you have the `wcsrtombs' function. */
#define HAVE_WCSRTOMBS 1

/* Define to 1 if you have the `wcstombs' function. */
#define HAVE_WCSTOMBS 1

/* Define to 1 if you have the `wcsupr' function. */
#define HAVE_WCSUPR 1

/* Define to 1 if you have the <wctype.h> header file. */
#define HAVE_WCTYPE_H 1

/* Define to 1 if you have the <winsock2.h> header file. */
#define HAVE_WINSOCK2_H 1

/* Define to 1 if you have to use const char* with iconv, to 0 if you must use
   char*. */
#define ICONV_USES_CONST_POINTER 0

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of package */
#define PACKAGE "xerces-c"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "c-dev@xerces.apache.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "xerces-c"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "xerces-c 3.2.3"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "xerces-c"

/* Define to the home page for this package. */
#define PACKAGE_URL "https://xerces.apache.org/xerces-c/"

/* Define to the version of this package. */
#define PACKAGE_VERSION "3.2.3"

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 4

/* The size of `long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG 8

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* The size of `wchar_t', as computed by sizeof. */
#define SIZEOF_WCHAR_T 2

/* The size of `__int64', as computed by sizeof. */
#define SIZEOF___INT64 8

/* Version number of package */
#define VERSION "3.2.3"

/* Define if DLL symbols should be exported */
#define XERCES_DLL_EXPORT 1

/* Define if namespaces is supported by the compiler */
#define XERCES_HAS_CPP_NAMESPACE 1

/* Define to have SSE2 instruction support detected at runtime using __cpuid
   */
#define XERCES_HAVE_CPUID_INTRINSIC 1

/* Define to 1 if you have emmintrin.h */
#define XERCES_HAVE_EMMINTRIN_H 1

/* Define to have SSE2 instruction support detected at runtime using
   __get_cpuid */
/* #undef XERCES_HAVE_GETCPUID */

/* Define to 1 if you have intrin.h */
#define XERCES_HAVE_INTRIN_H 1

/* Define to 1 if we have inttypes.h */
#define XERCES_HAVE_INTTYPES_H 1

/* Define to have SSE2 instruction used at runtime */
#define XERCES_HAVE_SSE2_INTRINSIC 1

/* Define to 1 if we have sys/types.h */
#define XERCES_HAVE_SYS_TYPES_H 1

/* Define to have Xerces_autoconf_config.hpp include wchar.h */
/* #undef XERCES_INCLUDE_WCHAR_H */

/* Define if there is support for L"widestring" */
#define XERCES_LSTRSUPPORT 1

/* Define if the isstream library can be included as <iostream> */
#define XERCES_NEW_IOSTREAMS 1

/* Define to have XMemory.hpp avoid declaring a matching operator delete for
   the placement operator new */
/* #undef XERCES_NO_MATCHING_DELETE_OPERATOR */

/* Define if there is no native bool support in this environment */
/* #undef XERCES_NO_NATIVE_BOOL */

/* Define to use backslash as an extra path delimiter character */
#define XERCES_PATH_DELIMITER_BACKSLASH 1

/* Define as the platform's export attribute */
#define XERCES_PLATFORM_EXPORT __declspec(dllexport)

/* Define as the platform's import attribute */
#define XERCES_PLATFORM_IMPORT __declspec(dllimport)

/* An appropriate signed 16 bit integer type */
#define XERCES_S16BIT_INT int16_t

/* An appropriate signed 32 bit integer type */
#define XERCES_S32BIT_INT int32_t

/* An appropriate signed 64 bit integer type */
#define XERCES_S64BIT_INT int64_t

/* Define as the appropriate SIZE_MAX macro */
#define XERCES_SIZE_MAX SIZE_MAX

/* Define as the appropriate size_t type */
#define XERCES_SIZE_T size_t

/* Define as the appropriate SSIZE_MAX macro */
#define XERCES_SSIZE_MAX LONG_MAX

/* Define as the appropriate ssize_t type */
#define XERCES_SSIZE_T long

/* Define if building a static library */
/* #undef XERCES_STATIC_LIBRARY */

/* Define if the std namespace is supported */
#define XERCES_STD_NAMESPACE 1

/* An appropriate unsigned 16 bit integer type */
#define XERCES_U16BIT_INT uint16_t

/* An appropriate unsigned 32 bit integer type */
#define XERCES_U32BIT_INT uint32_t

/* An appropriate unsigned 64 bit integer type */
#define XERCES_U64BIT_INT uint64_t

/* Define to use the POSIX file mgr */
/* #undef XERCES_USE_FILEMGR_POSIX */

/* Define to use the Windows file mgr */
#define XERCES_USE_FILEMGR_WINDOWS 1

/* Define to use the iconv-based MsgLoader */
/* #undef XERCES_USE_MSGLOADER_ICONV */

/* Define to use the ICU-based MsgLoader */
/* #undef XERCES_USE_MSGLOADER_ICU */

/* Define to use the InMemory MsgLoader */
#define XERCES_USE_MSGLOADER_INMEMORY 1

/* Define to use the NoThread mutex mgr */
/* #undef XERCES_USE_MUTEXMGR_NOTHREAD */

/* Define to use the C++11 standard mutex mgr */
#define XERCES_USE_MUTEXMGR_STD 1

/* Define to use the POSIX mutex mgr */
/* #undef XERCES_USE_MUTEXMGR_POSIX */

/* Define to use the Windows mutex mgr */
/* #undef XERCES_USE_MUTEXMGR_WINDOWS */

/* Define to use the Mac OS X CFURL NetAccessor */
/* #undef XERCES_USE_NETACCESSOR_CFURL */

/* Define to use the CURL NetAccessor */
/* #undef XERCES_USE_NETACCESSOR_CURL */

/* Define to use the Sockets-based NetAccessor */
/* #undef XERCES_USE_NETACCESSOR_SOCKET */

/* Define to use the WinSock NetAccessor */
#define XERCES_USE_NETACCESSOR_WINSOCK 1

/* Define to use the GNU iconv transcoder */
/* #undef XERCES_USE_TRANSCODER_GNUICONV */

/* Define to use the iconv transcoder */
/* #undef XERCES_USE_TRANSCODER_ICONV */

/* Define to use the ICU-based transcoder */
/* #undef XERCES_USE_TRANSCODER_ICU */

/* Define to use the Mac OS UnicodeConverter-based transcoder */
/* #undef XERCES_USE_TRANSCODER_MACOSUNICODECONVERTER */

/* Define to use the Windows transcoder */
#define XERCES_USE_TRANSCODER_WINDOWS 1

/* Define to the 16 bit type used to represent Xerces UTF-16 characters */
#define XERCES_XMLCH_T char16_t

/* Define to empty if `const' does not conform to ANSI C. */
#if !1
#define const
#endif

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
#if !1
#if 0
#define inline inline
#else
#define inline
#endif
#endif
#endif

/* Define to empty if the keyword `volatile' does not work. Warning: valid
code using `volatile' can become incorrect without. Disable with care. */
#if !1
#define volatile
#endif
