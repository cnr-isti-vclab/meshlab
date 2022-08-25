dnl @synopsis XERCES_MUTEXMGR_SELECTION
dnl
dnl Determines the which XMLMutexMgr to use
dnl
dnl @category C
dnl @author James Berry
dnl @version 2005-05-25
dnl @license AllPermissive
dnl
dnl $Id$

AC_DEFUN([XERCES_MUTEXMGR_SELECTION],
	[

	######################################################
	# Test for availability of each mutex manager on this host.
        # For each mutex manager that's available, and hasn't been
	# disabled, add it to our list.  If the mutex manager has been
	# explicitly "enable"d, then vote for it strongly, in upper
	# case.
	######################################################

	ml_list=

        AC_REQUIRE([XERCES_NO_THREADS])
	AC_REQUIRE([ACX_PTHREAD])
	AC_REQUIRE([AC_CXX_HAVE_STD_THREAD])

	# Use Standard C++11 threads if available
	list_add=
	AC_MSG_CHECKING([whether we can support the Standard C++11 threads-based MutexMgr])
        AS_IF([test x$xerces_cv_no_threads = xno && test x$ac_cv_cxx_have_std_thread = xyes], [
		AC_ARG_ENABLE([mutexmgr-standard],
			AS_HELP_STRING([--enable-mutexmgr-standard],
				[Enable Standard C++11 threads mutex manager support]),
			[AS_IF([test x"$enableval" = xyes],
				[list_add=STANDARD])],
			[list_add=standard])
	])
	AS_IF([test x"$list_add" != x],
		[mm_list="$mm_list -$list_add-"; AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)]
	)

        # Check for OS-specific mutex managers
	case $host_os in
	windows* | mingw*)
		# Native Windows threads
		list_add=
		AC_MSG_CHECKING([whether we can support the Windows threads-based MutexMgr])
		AS_IF([test x$xerces_cv_no_threads = xno], [
			AC_ARG_ENABLE([mutexmgr-windows],
				AS_HELP_STRING([--enable-mutexmgr-windows],
					[Enable Windows mutex manager support]),
				[AS_IF([test x"$enableval" = xyes],
					[list_add=WINDOWS])],
				[list_add=windows])
		])
		AS_IF([test x"$list_add" != x],
			[mm_list="$mm_list -$list_add-"; AC_MSG_RESULT(yes)],
			[AC_MSG_RESULT(no)]
		)
		break
		;;
	*)
		# Native POSIX threads
		list_add=
		AC_MSG_CHECKING([whether we can support the POSIX threads (pthreads)-based MutexMgr])
		AS_IF([test x$xerces_cv_no_threads = xno], [
			AC_ARG_ENABLE([mutexmgr-posix],
				AS_HELP_STRING([--enable-mutexmgr-posix],
					[Enable POSIX mutex manager support]),
				[AS_IF([test x"$enableval" = xyes],
					[list_add=POSIX])],
				[list_add=posix])
		])
		AS_IF([test x"$list_add" != x],
			[mm_list="$mm_list -$list_add-"; AC_MSG_RESULT(yes)],
			[AC_MSG_RESULT(no)]
		)
		break
		;;
	esac

	# If no threads is specified, use the NoThread Mutex Mgr
	list_add=
	AC_MSG_CHECKING([whether we can support the NoThreads MutexMgr])
	AC_ARG_ENABLE([mutexmgr-nothreads],
		AS_HELP_STRING([--enable-mutexmgr-nothreads],
			[Enable NoThreads mutex manager support (no threading)]),
		[AS_IF([test x"$enableval" = xyes],
			[list_add=NOTHREADS])],
		[list_add=nothreads])
	AS_IF([test x"$list_add" != x],
		[mm_list="$mm_list -$list_add-"; AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)]
	)


	######################################################
	# Determine which mutex manager to use.
	#
	# We do this in two passes. Mutex managers that have been
	# enabled with "yes", and which start out in upper case, get
	# the top priority on the first pass.  On the second pass, we
	# consider those which are simply available, but which were
	# not "disable"d (these won't even be in our list).
	######################################################
	mutexmgr=
	az_lower=abcdefghijklmnopqrstuvwxyz
	az_upper=ABCDEFGHIJKLMNOPQRSTUVWXYZ
	AC_MSG_CHECKING([for which mutex manager to use (choices:$mm_list)])
	for i in 1 2; do
		# Swap upper/lower case in the mm_list. Cannot use tr ranges
                # because of the portability issues.
                #
		mm_list=`echo $mm_list | tr "$az_lower$az_upper" "$az_upper$az_lower"`

		# Check for each mutexmgr, in implicit rank order
		case $mm_list in
		*-standard-*)
			mutexmgr=standard;
			AC_DEFINE([XERCES_USE_MUTEXMGR_STD], 1, [Define to use the C++ standard mutex mgr (requires C++11)])

			case $host_os in
			mingw*)
				CXXFLAGS="${CXXFLAGS} -mthreads"
				;;
			esac
				if test "$acx_pthread_ok" = yes; then
				LIBS="${LIBS} ${PTHREAD_LIBS}"
				CXXFLAGS="${CXXFLAGS} ${PTHREAD_CFLAGS}"
			fi
			break
			;;

		*-posix-*)
			mutexmgr=POSIX;
			AC_DEFINE([XERCES_USE_MUTEXMGR_POSIX], 1, [Define to use the POSIX mutex mgr])

			# Set additional flags for link and compile
			LIBS="${LIBS} ${PTHREAD_LIBS}"
			CXXFLAGS="${CXXFLAGS} ${PTHREAD_CFLAGS}"
			break
			;;

		*-windows-*)
			mutexmgr=Windows;
			AC_DEFINE([XERCES_USE_MUTEXMGR_WINDOWS], 1, [Define to use the Windows mutex mgr])

			case $host_os in
			mingw*)
				CXXFLAGS="${CXXFLAGS} -mthreads"
				;;
			esac
			break
			;;

		*-nothreads-*)
			mutexmgr=NoThreads
			AC_DEFINE([XERCES_USE_MUTEXMGR_NOTHREAD], 1, [Define to use the NoThread mutex mgr])
			break
			;;

		*)
			AS_IF([test $i -eq 2], [
				AC_MSG_RESULT([Xerces cannot function without mutex support. You may want to --disable-threads.])
				]
			)
			;;

		esac
	done

	if test x"$mutexmgr" != x; then
		AC_MSG_RESULT($mutexmgr)
	fi

	# Define the auto-make conditionals which determine what actually gets compiled
	# Note that these macros can't be executed conditionally, which is why they're here, not above.
	AM_CONDITIONAL([XERCES_USE_MUTEXMGR_NOTHREAD],	[test x"$mutexmgr" = xNoThreads])
	AM_CONDITIONAL([XERCES_USE_MUTEXMGR_STD],	[test x"$mutexmgr" = xstandard])
	AM_CONDITIONAL([XERCES_USE_MUTEXMGR_POSIX],	[test x"$mutexmgr" = xPOSIX])
	AM_CONDITIONAL([XERCES_USE_MUTEXMGR_WINDOWS],	[test x"$mutexmgr" = xWindows])

	]
)


