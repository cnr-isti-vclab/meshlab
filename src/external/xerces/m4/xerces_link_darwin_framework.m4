dnl @synopsis XERCES_LINK_DARWIN_FRAMEWORK
dnl
dnl Adds the specified framework to LIBS if it's not already there.
dnl
dnl @category C
dnl @author James Berry
dnl @version 2005-02-20
dnl @license AllPermissive
dnl
dnl $Id$

AC_DEFUN([XERCES_LINK_DARWIN_FRAMEWORK], [
	case $host_os in
	darwin*)
		test -z "${xerces_darwin_frameworks}" && xerces_darwin_frameworks="-"
		case ${xerces_darwin_frameworks} in
		*-$1-*)
			;;
		*)
			xerces_darwin_frameworks="-$1${xerces_darwin_frameworks}"
			LIBS="-Wl,-framework -Wl,$1 $LIBS"
			;;
		esac
		;;
	esac
])

