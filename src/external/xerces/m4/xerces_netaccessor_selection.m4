dnl @synopsis XERCES_NETACCESSOR_SELECTION
dnl
dnl Determines the which netaccessor to use
dnl
dnl @category C
dnl @author James Berry
dnl @version 2005-05-23
dnl @license AllPermissive
dnl
dnl $Id$

AC_DEFUN([XERCES_NETACCESSOR_SELECTION],
	[

	network=yes
	AC_ARG_ENABLE(
	  [network],
	  AS_HELP_STRING([--disable-network], [Disable network support (enabled by default)]),
	  [AS_IF([test x"$enableval" = xno], [network=no])])

	# If network is disabled we don't need to check any of the
	# netaccessors.
	#
	na_list=

	if test x"$network" = xyes; then

	# Checks for libraries.
	AC_CHECK_LIB([socket], [socket])
	AC_CHECK_LIB([nsl], [gethostbyname])

	######################################################
	# Test for availability of each netaccessor on this host.
	# For each netaccessor that's available, and hasn't been disabled, add it to our list.
	# If the netaccessor has been explicitly "enable"d, then vote for it strongly,
	# in upper case.
	######################################################

	AC_REQUIRE([XERCES_CURL_PREFIX])
	AC_MSG_CHECKING([whether we can support the libcurl-based NetAccessor])
	list_add=
	AS_IF([test x"$xerces_cv_curl_present" != x"no"], [
		AC_ARG_ENABLE([netaccessor-curl],
			AS_HELP_STRING([--enable-netaccessor-curl],
				[Enable libcurl-based NetAccessor support]),
			[AS_IF([test x"$enableval" = xyes],
				[list_add=CURL])],
			[list_add=curl])
	])
	AS_IF([test x"$list_add" != x],
		[na_list="$na_list -$list_add-"; AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)]
	)

	AC_MSG_CHECKING([whether we can support the sockets-based NetAccessor])
	list_add=
	AS_IF([test x"$ac_cv_header_sys_socket_h" = xyes],
		[AC_ARG_ENABLE([netaccessor-socket],
			AS_HELP_STRING([--enable-netaccessor-socket],
				[Enable sockets-based NetAccessor support]),
			[AS_IF([test x"$enableval" = xyes],
				[list_add=SOCKET])],
			[list_add=socket])
		])
	AS_IF([test x"$list_add" != x],
		[na_list="$na_list -$list_add-"; AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)]
	)

	# Check for OS-specific netaccessors
	case $host_os in
	darwin*)
		list_add=
		AC_MSG_CHECKING([whether we can support the CFURL NetAccessor (Mac OS X)])
		AS_IF([test x"$ac_cv_header_CoreServices_CoreServices_h" = xyes], [
			AC_ARG_ENABLE([netaccessor-cfurl],
				AS_HELP_STRING([--enable-netaccessor-cfurl],
					[Enable cfurl-based NetAccessor support]),
				[AS_IF([test x"$enableval" = xyes],
					[list_add=CFURL])],
				[list_add=cfurl])
			]
		)
		AS_IF([test x"$list_add" != x],
			[na_list="$na_list -$list_add-"; AC_MSG_RESULT(yes)],
			[AC_MSG_RESULT(no)]
		)
		;;
	windows* | mingw*)
		list_add=
		no_winsock=false
		AC_CHECK_HEADERS([winsock2.h], [], [no_winsock=true])
		AC_MSG_CHECKING([whether we can support the WinSock NetAccessor (Windows)])
		AS_IF([! $no_winsock], [
			AC_ARG_ENABLE([netaccessor-winsock],
				AS_HELP_STRING([--enable-netaccessor-winsock],
					[Enable winsock-based NetAccessor support]),
				[AS_IF([test x"$enableval" = xyes],
					[list_add=WINSOCK])],
				[list_add=winsock])
		])
		AS_IF([test x"$list_add" != x],
			[na_list="$na_list -$list_add-"; AC_MSG_RESULT(yes)],
			[AC_MSG_RESULT(no)]
		)
		;;
	cygwin*)
		# Only add it to the list if the user explicitly asked
                # for it.
		#
		list_add=
		no_winsock=false
		AC_CHECK_HEADERS([winsock2.h], [], [no_winsock=true])
		AC_MSG_CHECKING([whether to use the WinSock NetAccessor (Windows)])
		AS_IF([! $no_winsock], [
			AC_ARG_ENABLE([netaccessor-winsock],
				AS_HELP_STRING([--enable-netaccessor-winsock],
					[Enable winsock-based NetAccessor support]),
				[AS_IF([test x"$enableval" = xyes],
					[list_add=WINSOCK])])
		])
		AS_IF([test x"$list_add" != x],
			[na_list="$na_list -$list_add-"; AC_MSG_RESULT(yes)],
			[AC_MSG_RESULT(no)]
		)
		;;
	esac


	######################################################
	# Determine which netaccessor to use.
	#
	# We do this in two passes. Accessors that have been enabled with "yes",
	# and which start out in upper case, get the top priority on the first pass.
	# On the second pass, we consider those which are simply available, but
	# which were not "disable"d (these won't even be in our list).
	######################################################
	netaccessor=
	az_lower=abcdefghijklmnopqrstuvwxyz
	az_upper=ABCDEFGHIJKLMNOPQRSTUVWXYZ
	AC_MSG_CHECKING([for which NetAccessor to use (choices:$na_list)])
	for i in 1 2; do
		# Swap upper/lower case in the na_list. Cannot use tr ranges
                # because of the portability issues.
                #
		na_list=`echo $na_list | tr "$az_lower$az_upper" "$az_upper$az_lower"`

		# Check for each netaccessor, in implicit rank order
		case $na_list in
		*-curl-*)
			netaccessor=curl
			AC_DEFINE([XERCES_USE_NETACCESSOR_CURL], 1, [Define to use the CURL NetAccessor])
			LIBS="${LIBS} ${xerces_cv_curl_libs}"
			break
			;;

		*-winsock-*)
			netaccessor=winsock
			AC_DEFINE([XERCES_USE_NETACCESSOR_WINSOCK], 1, [Define to use the WinSock NetAccessor])
			break
			;;

		*-socket-*)
			netaccessor=socket
			AC_DEFINE([XERCES_USE_NETACCESSOR_SOCKET], 1, [Define to use the Sockets-based NetAccessor])
			break
			;;

		*-cfurl-*)
			netaccessor=cfurl
			AC_DEFINE([XERCES_USE_NETACCESSOR_CFURL], 1, [Define to use the Mac OS X CFURL NetAccessor])
			XERCES_LINK_DARWIN_FRAMEWORK([CoreServices])
			break
			;;
		*)
			AS_IF([test $i -eq 2], [
				AC_MSG_RESULT([none available; there will be no network access!!!])
				]
			)
			;;

		esac
	done

	if test x"$netaccessor" != x; then
		AC_MSG_RESULT($netaccessor)
	fi

	else # network
		netaccessor=disabled # for the report
	fi

	# Define the auto-make conditionals which determine what actually gets compiled
	# Note that these macros can't be executed conditionally, which is why they're here, not above.
	AM_CONDITIONAL([XERCES_USE_NETACCESSOR_CURL],		[test x"$netaccessor" = xcurl])
	AM_CONDITIONAL([XERCES_USE_NETACCESSOR_CFURL],		[test x"$netaccessor" = xcfurl])
	AM_CONDITIONAL([XERCES_USE_NETACCESSOR_WINSOCK],	[test x"$netaccessor" = xwinsock])
	AM_CONDITIONAL([XERCES_USE_NETACCESSOR_SOCKET],		[test x"$netaccessor" = xsocket])

	]
)
