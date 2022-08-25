dnl @synopsis XERCES_TRANSCODER_SELECTION
dnl
dnl Determines the which transcoder to use
dnl
dnl @category C
dnl @author James Berry
dnl @version 2005-05-23
dnl @license AllPermissive
dnl
dnl $Id$

AC_DEFUN([XERCES_TRANSCODER_SELECTION],
	[

	######################################################
	# Test for availability of each transcoder on this host.
	# For each transcoder that's available, and hasn't been disabled, add it to our list.
	# If the transcoder has been explicitly "enable"d, then vote for it strongly,
	# in upper case.
	######################################################

	tc_list=

	# Check for GNU iconv support
	no_GNUiconv=false
	AC_CHECK_HEADERS([iconv.h wchar.h string.h stdlib.h stdio.h ctype.h locale.h errno.h], [], [no_GNUiconv=true])
        # The code in iconv needs just on of these include files
	AC_CHECK_HEADER([endian.h],
                        [],
                        [
                         AC_CHECK_HEADER([machine/endian.h],
                                         [],
                                         [
                                          AC_CHECK_HEADER([arpa/nameser_compat.h],
                                                          [],
                                                          [no_GNUiconv=true])
                                         ])
                        ])
	AC_CHECK_FUNCS([iconv_open iconv_close iconv], [], [no_GNUiconv=true])
	AC_MSG_CHECKING([whether we can support the GNU iconv Transcoder])
	list_add=
	AS_IF([! $no_GNUiconv], [
		AC_ARG_ENABLE([transcoder-gnuiconv],
			AS_HELP_STRING([--enable-transcoder-gnuiconv],
				[Enable GNU iconv-based transcoder support]),
			[AS_IF([test x"$enableval" = xyes],
				[list_add=GNUICONV])],
			[list_add=gnuiconv])
	])
	AS_IF([test x"$list_add" != x],
		[tc_list="$tc_list -$list_add-"; AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)]
	)

	# Check for iconv support
	no_iconv=false
	AC_CHECK_HEADERS([wchar.h], [], [no_iconv=true])
	AC_CHECK_FUNCS([mblen wcstombs mbstowcs], [], [no_iconv=true])
	AC_MSG_CHECKING([whether we can support the iconv Transcoder])
	list_add=
	AS_IF([! $no_iconv], [
		AC_ARG_ENABLE([transcoder-iconv],
			AS_HELP_STRING([--enable-transcoder-iconv],
				[Enable iconv-based transcoder support]),
			[AS_IF([test x"$enableval" = xyes],
				[list_add=ICONV])],
			[list_add=iconv])
	])
	AS_IF([test x"$list_add" != x],
		[tc_list="$tc_list -$list_add-"; AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)]
	)

	# Check for ICU
	AC_REQUIRE([XERCES_ICU_PREFIX])
	AC_MSG_CHECKING([whether we can support the ICU Transcoder])
	list_add=
	AS_IF([test x"$xerces_cv_icu_present" != x"no"], [
		AC_ARG_ENABLE([transcoder-icu],
			AS_HELP_STRING([--enable-transcoder-icu],
				[Enable icu-based transcoder support]),
			[AS_IF([test x"$enableval" = xyes],
				[list_add=ICU])],
			[list_add=icu])
	])
	AS_IF([test x"$list_add" != x],
		[tc_list="$tc_list -$list_add-"; AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)]
	)


	# Check for platform-specific transcoders
	list_add=
	case $host_os in
	darwin*)
		AC_MSG_CHECKING([whether we can support the MacOSUnicodeConverter Transcoder])
		AS_IF([test x"$ac_cv_header_CoreServices_CoreServices_h" = xyes], [
			AC_ARG_ENABLE([transcoder-macosunicodeconverter],
				AS_HELP_STRING([--enable-transcoder-macosunicodeconverter],
					[Enable MacOSUnicodeConverter-based transcoder support]),
				[AS_IF([test x"$enableval" = xyes],
					[list_add=MACOSUNICODECONVERTER])],
				[list_add=macosunicodeconverter])
		])
		AS_IF([test x"$list_add" != x],
			[tc_list="$tc_list -$list_add-"; AC_MSG_RESULT(yes)],
			[AC_MSG_RESULT(no)]
		)
		;;
	windows* | mingw*)
		AC_MSG_CHECKING([whether we can support the Windows Transcoder])
		AC_ARG_ENABLE([transcoder-windows],
			AS_HELP_STRING([--enable-transcoder-windows],
				[Enable Windows-based transcoder support]),
			[AS_IF([test x"$enableval" = xyes],
				[list_add=WINDOWS])],
			[list_add=windows])
		AS_IF([test x"$list_add" != x],
			[tc_list="$tc_list -$list_add-"; AC_MSG_RESULT(yes)],
			[AC_MSG_RESULT(no)]
		)
		;;
	cygwin*)
		# Only add it to the list if the user explicitly asked
                # for it.
		#
		AC_MSG_CHECKING([whether to use the Windows Transcoder])
		AC_ARG_ENABLE([transcoder-windows],
			AS_HELP_STRING([--enable-transcoder-windows],
				[Enable Windows-based transcoder support]),
			[AS_IF([test x"$enableval" = xyes],
			       [list_add=WINDOWS])])
		AS_IF([test x"$list_add" != x],
			[tc_list="$tc_list -$list_add-"; AC_MSG_RESULT(yes)],
			[AC_MSG_RESULT(no)]
		)
		;;
	esac

	# TODO: Tests for additional transcoders

	######################################################
	# Determine which transcoder to use.
	#
	# We do this in two passes. Transcoders that have been enabled with "yes",
	# and which start out in upper case, get the top priority on the first pass.
	# On the second pass, we consider those which are simply available, but
	# which were not "disable"d (these won't even be in our list).
	######################################################
	transcoder=
	az_lower=abcdefghijklmnopqrstuvwxyz
	az_upper=ABCDEFGHIJKLMNOPQRSTUVWXYZ
	AC_MSG_CHECKING([for which Transcoder to use (choices:$tc_list)])
	for i in 1 2; do
 		# Swap upper/lower case in the tc_list. Cannot use tr ranges
                # because of the portability issues.
                #
		tc_list=`echo $tc_list | tr "$az_lower$az_upper" "$az_upper$az_lower"`

		# Check for each transcoder, in implicit rank order
		case $tc_list in

		*-icu-*)
			transcoder=icu
			AC_DEFINE([XERCES_USE_TRANSCODER_ICU], 1, [Define to use the ICU-based transcoder])
			LIBS="${LIBS} ${xerces_cv_icu_libs}"
			break
			;;

		*-macosunicodeconverter-*)
			transcoder=macosunicodeconverter
			AC_DEFINE([XERCES_USE_TRANSCODER_MACOSUNICODECONVERTER], 1, [Define to use the Mac OS UnicodeConverter-based transcoder])
			XERCES_LINK_DARWIN_FRAMEWORK([CoreServices])
			break
			;;

		*-gnuiconv-*)
			transcoder=gnuiconv
			AC_DEFINE([XERCES_USE_TRANSCODER_GNUICONV], 1, [Define to use the GNU iconv transcoder])
			break
			;;

		*-windows-*)
			transcoder=windows
			AC_DEFINE([XERCES_USE_TRANSCODER_WINDOWS], 1, [Define to use the Windows transcoder])
			break
			;;

		*-iconv-*)
			transcoder=iconv
			AC_DEFINE([XERCES_USE_TRANSCODER_ICONV], 1, [Define to use the iconv transcoder])
			break
			;;

		*)
			AS_IF([test $i -eq 2], [
				AC_MSG_RESULT([none])
				AC_MSG_ERROR([Xerces cannot function without a transcoder])
				]
			)
			;;
		esac
	done
	if test x"$transcoder" != x; then
		AC_MSG_RESULT($transcoder)
	fi

	# Define the auto-make conditionals which determine what actually gets compiled
	# Note that these macros can't be executed conditionally, which is why they're here, not above.
	AM_CONDITIONAL([XERCES_USE_TRANSCODER_ICU],			[test x"$transcoder" = xicu])
	AM_CONDITIONAL([XERCES_USE_TRANSCODER_MACOSUNICODECONVERTER],	[test x"$transcoder" = xmacosunicodeconverter])
	AM_CONDITIONAL([XERCES_USE_TRANSCODER_GNUICONV],		[test x"$transcoder" = xgnuiconv])
	AM_CONDITIONAL([XERCES_USE_TRANSCODER_ICONV],			[test x"$transcoder" = xiconv])
	AM_CONDITIONAL([XERCES_USE_TRANSCODER_WINDOWS],			[test x"$transcoder" = xwindows])

	]
)
