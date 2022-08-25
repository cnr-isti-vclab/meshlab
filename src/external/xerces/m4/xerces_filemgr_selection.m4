dnl @synopsis XERCES_FILEMGR_SELECTION
dnl
dnl Determines the which filemgr to use
dnl
dnl @category C
dnl @author James Berry
dnl @version 2005-05-25
dnl @license AllPermissive
dnl
dnl $Id$

AC_DEFUN([XERCES_FILEMGR_SELECTION],
	[
	
	AC_MSG_CHECKING([for which File Manager to use])
	filemgr=
	
	# Platform specific checks
	case $host_os in
	windows* | mingw*)
		filemgr=Windows;
		AC_DEFINE([XERCES_USE_FILEMGR_WINDOWS], 1, [Define to use the Windows file mgr])
		;;
	esac
	
	# Fall back to using posix files
	AS_IF([test -z "$filemgr"],
		[filemgr=POSIX;
			AC_DEFINE([XERCES_USE_FILEMGR_POSIX], 1, [Define to use the POSIX file mgr])
		])

	AC_MSG_RESULT($filemgr)
	
	# Define the auto-make conditionals which determine what actually gets compiled
	# Note that these macros can't be executed conditionally, which is why they're here, not above.
	AM_CONDITIONAL([XERCES_USE_FILEMGR_POSIX],	[test x"$filemgr" = xPOSIX])
	AM_CONDITIONAL([XERCES_USE_FILEMGR_WINDOWS],[test x"$filemgr" = xWindows])
	
	]
)

