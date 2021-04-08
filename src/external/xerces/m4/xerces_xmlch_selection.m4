dnl @synopsis XERCES_XMLCH_SELECTION
dnl
dnl Determines the which XMLCh type to use
dnl
dnl @category C
dnl @author Roger Leigh
dnl @version 2018-01-23
dnl @license AllPermissive
dnl
dnl $Id$

AC_DEFUN([XERCES_XMLCH_SELECTION],
    [
    xmlch_list=

    AS_IF([test $ac_cv_cxx_have_lstring],
    AC_DEFINE([XERCES_LSTRSUPPORT], 1, [Define if there is support for L"widestring"]))

    ######################################################
    # Test for availability of each XML character type on this host.
    # For each type that's available, and hasn't been disabled, add it to our list.
    # If the type has been explicitly "enable"d, then vote for it strongly,
    # in upper case.
    ######################################################

    char16_t_available=false
    AC_MSG_CHECKING([whether C++11 char16_t is available])
    AC_COMPILE_IFELSE(  [AC_LANG_PROGRAM([[]],
                                         [[const char16_t *unicode = u"Test ünícodè → ©";]])],
                        [char16_t_available=yes
                         AC_MSG_RESULT([yes])],
                        [char16_t_available=no
                         AC_MSG_RESULT([no])]
                     )
    AC_MSG_CHECKING([whether we can support char16_t XMLCh])
    list_add=
    AS_IF([test x"$char16_t_available" != x"no"], [
        AC_ARG_ENABLE([xmlch-char16_t],
            AS_HELP_STRING([--enable-xmlch-char16_t],
                [Enable char16_t XMLCh]),
            [AS_IF([test x"$enableval" = xyes],
                [list_add=CHAR16_T])],
            [list_add=char16_t])
    ])
    AS_IF([test x"$list_add" != x],
        [xmlch_list="$xmlch_list -$list_add-"; AC_MSG_RESULT(yes)],
        [AC_MSG_RESULT(no)]
    )

    # Check if the Windows API is defined as using wchar_t or
    # unsigned short; if it's wchar_t, we need to map XMLCh to be wchar_t
    # (this is safe because on Windows wchar_t is used to store UTF-16
    # codepoints, while it is not true on Unix)
    AC_MSG_CHECKING([whether the Windows SDK is available and using wchar_t as wide string])
    AC_COMPILE_IFELSE(  [AC_LANG_PROGRAM([[#include <windows.h>
                                            wchar_t file[] = L"dummy.file";]],
                                         [[DeleteFileW(file);]])],
                       [wchar_t_available=yes
                         AC_MSG_RESULT([yes])],
                        [wchar_t_available=no
                         AC_MSG_RESULT([no])]
                      )
    AC_MSG_CHECKING([whether we can support wchar_t XMLCh])
    list_add=
    AS_IF([test x"$wchar_t_available" = xyes],
        [AC_ARG_ENABLE([xmlch-wchar_t],
            AS_HELP_STRING([--enable-xmlch-wchar_t],
                [Enable wchar_t XMLCh]),
            [AS_IF([test x"$enableval" = xyes],
                [list_add=WCHAR_T])],
            [list_add=wchar_t])
        ])
    AS_IF([test x"$list_add" != x],
        [xmlch_list="$xmlch_list -$list_add-"; AC_MSG_RESULT(yes)],
        [AC_MSG_RESULT(no)]
    )

    AC_MSG_CHECKING([whether we can support uint16_t XMLCh])
    list_add=
    AC_ARG_ENABLE([xmlch-uint16_t],
        AS_HELP_STRING([--enable-xmlch-uint16_t],
            [Enable uint16_t XMLCh]),
        [AS_IF([test x"$enableval" = xyes],
            [list_add=UINT16_T])],
        [list_add=uint16_t])
    AS_IF([test x"$list_add" != x],
        [xmlch_list="$xmlch_list -$list_add-"; AC_MSG_RESULT(yes)],
        [AC_MSG_RESULT(no)]
    )

    ######################################################
    # Determine which XMLCh type to use.
    #
    # We do this in two passes. Types that have been enabled with "yes",
    # and which start out in upper case, get the top priority on the first pass.
    # On the second pass, we consider those which are simply available, but
    # which were not "disable"d (these won't even be in our list).
    ######################################################
    xmlch=
    az_lower=abcdefghijklmnopqrstuvwxyz
    az_upper=ABCDEFGHIJKLMNOPQRSTUVWXYZ
    AC_MSG_CHECKING([for which XMLCh type to use (choices:$xmlch_list)])
    for i in 1 2; do
        # Swap upper/lower case in the xmlch_list. Cannot use tr ranges
                # because of the portability issues.
                #
        xmlch_list=`echo $xmlch_list | tr "$az_lower$az_upper" "$az_upper$az_lower"`

        # Check for each xmlch, in implicit rank order
        case $xmlch_list in
        *-char16_t-*)
            xmlch=char16_t
            xmlch_type=char16_t
            break
            ;;

        *-wchar_t-*)
            xmlch=wchar_t
            xmlch_type=wchar_t
            break
            ;;

        *-uint16_t-*)
            xmlch=uint16_t
            xmlch_type=$xerces_cv_type_u16bit_int
            break
            ;;

        *)
            AS_IF([test $i -eq 2], [
                xmlch=uint16_t
                xmlch_type=$xerces_cv_type_u16bit_int
                AC_MSG_RESULT([none available; falling back to uint16_t])
                ]
            )
            ;;

        esac
    done

    if test x"$xmlch" != x; then
        AC_MSG_RESULT($xmlch)
    fi

    AM_CONDITIONAL([XERCES_USE_CHAR16], [test "x$xmlch" = "xchar16_t"])
    AC_DEFINE_UNQUOTED([XERCES_XMLCH_T], [$xmlch_type], [Define to the 16 bit type used to represent Xerces UTF-16 characters])
    ]
)
