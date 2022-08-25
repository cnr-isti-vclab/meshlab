dnl @synopsis XERCES_MSGLOADER_SELECTION
dnl
dnl Determine which msgloader to use.
dnl
dnl @category C
dnl @author James Berry
dnl @version 2005-05-23
dnl @license AllPermissive
dnl
dnl $Id$

AC_DEFUN([XERCES_MSGLOADER_SELECTION],
	[

	######################################################
	# Test for availability of each msgloader on this host.
	# For each msgloader that's available, and hasn't been disabled, add it to our list.
	# If the msgloader has been explicitly "enable"d, then vote for it strongly,
	# in upper case.
	######################################################

	ml_list=

	# Check for inmemory msgloader
	AC_MSG_CHECKING([whether we support the InMemory MsgLoader])
	list_add=
	AS_IF([true], [
		AC_ARG_ENABLE([msgloader-inmemory],
			AS_HELP_STRING([--enable-msgloader-inmemory],
				[Enable InMemory MsgLoader support]),
			[AS_IF([test x"$enableval" = xyes],
				[list_add=INMEMORY])],
			[list_add=inmemory])
	])
	AS_IF([test x"$list_add" != x],
		[ml_list="$ml_list -$list_add-"; AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)]
	)

	# Check for ICU
	AC_REQUIRE([XERCES_ICU_PREFIX])
	AC_MSG_CHECKING([whether we support the ICU MsgLoader])
	list_add=
	AS_IF([test x"$xerces_cv_icu_present" != x"no"], [
		AC_ARG_ENABLE([msgloader-icu],
			AS_HELP_STRING([--enable-msgloader-icu],
				[Enable ICU-based MsgLoader support]),
			[AS_IF([test x"$enableval" = xyes],
				[list_add=ICU])],
			[list_add=icu])
	])
	AS_IF([test x"$list_add" != x],
		[ml_list="$ml_list -$list_add-"; AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)]
	)

	# Check for iconv support
	no_iconv=false
	AC_CHECK_HEADERS([nl_types.h], [], [no_iconv=true])
	AC_CHECK_FUNCS([catopen catclose catgets], [], [no_iconv=true])
	AC_MSG_CHECKING([whether we can support the iconv MsgLoader])
	list_add=
	AS_IF([! $no_iconv], [
		AC_ARG_ENABLE([msgloader-iconv],
			AS_HELP_STRING([--enable-msgloader-iconv],
				[Enable Iconv-based MsgLoader support]),
			[AS_IF([test x"$enableval" = xyes],
				[list_add=ICONV])],
			[list_add=iconv])
	])
	AS_IF([test x"$list_add" != x],
		[ml_list="$ml_list -$list_add-"; AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)]
	)

	# TODO: Add test for additional msgloaders


	######################################################
	# Determine which msgloader to use.
	#
	# We do this in two passes. MsgLoaders that have been enabled with "yes",
	# and which start out in upper case, get the top priority on the first pass.
	# On the second pass, we consider those which are simply available, but
	# which were not "disable"d (these won't even be in our list).
	######################################################
	msgloader=
	az_lower=abcdefghijklmnopqrstuvwxyz
	az_upper=ABCDEFGHIJKLMNOPQRSTUVWXYZ
	AC_MSG_CHECKING([for which MsgLoader to use (choices:$ml_list)])
	for i in 1 2; do
		# Swap upper/lower case in the ml_list. Cannot use tr ranges
                # because of the portability issues.
                #
		ml_list=`echo $ml_list | tr "$az_lower$az_upper" "$az_upper$az_lower"`

		# Check for each msgloader, in implicit rank order
		case $ml_list in
		*-inmemory-*)
			AC_DEFINE([XERCES_USE_MSGLOADER_INMEMORY], 1, [Define to use the InMemory MsgLoader])
			msgloader=inmemory
			break
			;;

		*-icu-*)
			AC_DEFINE([XERCES_USE_MSGLOADER_ICU], 1, [Define to use the ICU-based MsgLoader])
			msgloader=icu
			LIBS="${LIBS} ${xerces_cv_icu_libs}"
			break
			;;

		*-iconv-*)
			AC_DEFINE([XERCES_USE_MSGLOADER_ICONV], 1, [Define to use the iconv-based MsgLoader])
			msgloader=iconv
			break
			;;

		*)
			AS_IF([test $i -eq 2], [
				AC_MSG_RESULT([none])
				AC_MSG_ERROR([Xerces cannot function without a MsgLoader])
				]
			)
			;;
		esac
	done
	if test x"$msgloader" != x; then
		AC_MSG_RESULT($msgloader)
	fi

	# Define the auto-make conditionals which determine what actually gets compiled
	# Note that these macros can't be executed conditionally, which is why they're here, not above.
	AM_CONDITIONAL([XERCES_USE_MSGLOADER_ICU], 	[test x"$msgloader" = xicu])
	AM_CONDITIONAL([XERCES_USE_MSGLOADER_ICONV], 	[test x"$msgloader" = xiconv])
	AM_CONDITIONAL([XERCES_USE_MSGLOADER_INMEMORY],	[test x"$msgloader" = xinmemory])

	]
)
