dnl @synopsis XERCES_CURL_PREFIX
dnl
dnl Determines the prefix for libcurl
dnl
dnl @category C
dnl @author James Berry
dnl @version 2005-05-23
dnl @license AllPermissive
dnl
dnl $Id$

AC_DEFUN([XERCES_CURL_PREFIX],
	[
	AC_ARG_WITH([curl],
		[AS_HELP_STRING([--with-curl[[[[=DIR]]]]],[Specify location of libcurl])],
		[
			if test x"$with_curl" = x"yes"; then
				with_curl=
			fi
		],
		[with_curl=])

	# Determine if curl is available
	AC_CACHE_VAL([xerces_cv_curl_present],
	[
		xerces_cv_curl_present=no
		if test x"$with_curl" != x"no"; then

		# See if we were given a prefix.
		#
		if test -n "$with_curl"; then
			AC_PATH_PROG([curl_config], [curl-config],[],[$with_curl/bin])
		else
			AC_PATH_PROG([curl_config], [curl-config],[])
		fi

		if test -n "$curl_config"; then
			curl_flags=`$curl_config --cflags`
			curl_libs=`$curl_config --libs`
		else
			if test -n "$with_curl"; then
				curl_flags="-I$with_curl/include"
				curl_libs="-L$with_curl/lib -lcurl"
			else
				# Default compiler paths.
				#
				curl_flags=
				curl_libs=-lcurl
			fi
		fi

		# Check that the headers exist and can be compiled.
		#
		orig_cppflags=$CPPFLAGS
		if test -n "$curl_flags"; then
			CPPFLAGS="$curl_flags $CPPFLAGS"
		fi
		AC_CHECK_HEADER([curl/curl.h], [xerces_cv_curl_present=yes])
		CPPFLAGS=$orig_cppflags

		if test x"$xerces_cv_curl_present" != x"no"; then

			# Check that the library can be linked.
			#
			AC_MSG_CHECKING([for curl_multi_init in -lcurl])

			orig_libs=$LIBS
			LIBS="$curl_libs $LIBS"

			AC_LINK_IFELSE(
				[AC_LANG_SOURCE([
                    #include <curl/curl.h>
                    #include <curl/multi.h>
                    #include <curl/easy.h>

                    int main ()
                    {
                      curl_multi_init();
                      return 0;
                    }
				])],
				[], [xerces_cv_curl_present=no])

			LIBS=$orig_libs

			if test x"$xerces_cv_curl_present" != x"no"; then
				AC_MSG_RESULT([yes])
			else
				AC_MSG_RESULT([no])
			fi
		fi
	fi
	])

	AC_CACHE_VAL([xerces_cv_curl_flags], [xerces_cv_curl_flags=$curl_flags])
	AC_CACHE_VAL([xerces_cv_curl_libs], [xerces_cv_curl_libs=$curl_libs])

	AC_SUBST([CURL_PRESENT], [$xerces_cv_curl_present])
	AC_SUBST([CURL_FLAGS], [$xerces_cv_curl_flags])
	AC_SUBST([CURL_LIBS], [$xerces_cv_curl_libs])
	]
)
