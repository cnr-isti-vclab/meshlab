dnl @synopsis XERCES_NO_THREADS
dnl
dnl Determines the whether we've been configured for no threads,
dnl or whether threads are not used for some other reason.
dnl
dnl @category C
dnl @author James Berry
dnl @version 2005-05-23
dnl @license AllPermissive
dnl
dnl $Id$

AC_DEFUN([XERCES_NO_THREADS],
	[
	AC_ARG_ENABLE([threads],
		AS_HELP_STRING([--disable-threads],
			[Disable threads (enabled by default)]),
		[AS_IF([test x"$enableval" = xno],
			[xerces_cv_no_threads=yes])],
		[xerces_cv_no_threads=no])
	AS_IF([test x$xerces_cv_no_threads = xyes],
		[AC_DEFINE([APP_NO_THREADS], 1, [Define to specify no threading is used])])
	]
)
