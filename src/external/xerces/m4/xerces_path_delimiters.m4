dnl @synopsis XERCES_PATH_DELIMITERS
dnl
dnl Configures the path delimiter characters.
dnl
dnl @category C
dnl @author James Berry
dnl @version 2005-06-07
dnl @license AllPermissive
dnl
dnl $Id$

AC_DEFUN([XERCES_PATH_DELIMITERS],
	[
		AC_MSG_CHECKING([for which path delimiter characters to accept])
		
		dnl We accept / in all cases.
		path_delims=/
		
		case $host_os in
		msdos* | windows* | mingw*)
			AC_DEFINE([XERCES_PATH_DELIMITER_BACKSLASH], 1, [Define to use backslash as an extra path delimiter character])
			path_delims="${path_delims}\\"
			;;
		esac

		AC_MSG_RESULT($path_delims)
	]
)


