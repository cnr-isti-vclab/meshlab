dnl @synopsis XERCES_ICU_PREFIX
dnl
dnl Determines the prefix for icu
dnl
dnl @category C
dnl @author James Berry
dnl @version 2005-05-23
dnl @license AllPermissive
dnl
dnl $Id$

AC_DEFUN([XERCES_ICU_PREFIX],
        [
        AC_ARG_WITH([icu],
                [AS_HELP_STRING([--with-icu[[[[=DIR]]]]],[Specify location of icu])],
                [
                  if test x"$with_icu" = x"yes"; then
                    with_icu=
                  fi
                ],
                [with_icu=])

        # Determine if icu is available
        AC_CACHE_VAL([xerces_cv_icu_present],
        [
                xerces_cv_icu_present=no
                if test x"$with_icu" != x"no"; then

                  # See if we were given a prefix.
                  #
                  if test -n "$with_icu"; then
                    AC_PATH_PROG([icu_config], [icu-config],[],[$with_icu/bin])
                  else
                    AC_PATH_PROG([icu_config], [icu-config],[])
                  fi

                  # Add trailing slash to bin and sbin vars so that we
                  # can use them uniformly whether they are empty or not.
                  #
                  if test -n "$icu_config"; then
                    icu_flags=`$icu_config --cppflags-searchpath`
                    icu_libs=`$icu_config --ldflags`
                    icu_bin=`$icu_config --bindir`
                    icu_sbin=`$icu_config --sbindir`

                    if test -n "$icu_bin"; then
                      icu_bin="$icu_bin/"
                    fi

                    if test -n "$icu_sbin"; then
                      icu_sbin="$icu_sbin/"
                    fi

                  else
                    if test -n "$with_icu"; then
                      icu_flags="-I$with_icu/include"
                      icu_libs="-L$with_icu/lib -licuuc -licudata"
                      icu_bin="$with_icu/bin/"
                      icu_sbin="$with_icu/sbin/"
                    else
                      # Default compiler paths.
                      #
                      icu_flags=
                      icu_libs="-licuuc -licudata"
                      icu_bin=
                      icu_sbin=
                    fi
                  fi

                  # Check that the headers exist and can be compiled.
                  #
                  orig_cppflags=$CPPFLAGS
                  if test -n "$icu_flags"; then
                    CPPFLAGS="$icu_flags $CPPFLAGS"
                  fi
                  AC_CHECK_HEADER([unicode/ucnv.h], [xerces_cv_icu_present=yes])


                  if test x"$xerces_cv_icu_present" != x"no"; then

                    # Check that the library can be linked.
                    #
                    AC_MSG_CHECKING([for ucnv_open in -licuuc])

                    orig_libs=$LIBS
                    LIBS="$icu_libs $LIBS"

                    AC_LINK_IFELSE(
                    [AC_LANG_SOURCE([
                    #include <unicode/ucnv.h>

                    int main ()
                    {
                      ucnv_open (0, 0);
                      return 0;
                    }
                    ])],
                    [], [xerces_cv_icu_present=no])

                    LIBS=$orig_libs

                    if test x"$xerces_cv_icu_present" != x"no"; then
                      AC_MSG_RESULT([yes])
                    else
                      AC_MSG_RESULT([no])
                    fi
                  fi

                  CPPFLAGS=$orig_cppflags
                fi
        ])

        AC_CACHE_VAL([xerces_cv_icu_flags], [xerces_cv_icu_flags=$icu_flags])
        AC_CACHE_VAL([xerces_cv_icu_libs], [xerces_cv_icu_libs=$icu_libs])
        AC_CACHE_VAL([xerces_cv_icu_bin], [xerces_cv_icu_bin=$icu_bin])
        AC_CACHE_VAL([xerces_cv_icu_sbin], [xerces_cv_icu_sbin=$icu_sbin])

        AC_SUBST([ICU_PRESENT], [$xerces_cv_icu_present])
        AC_SUBST([ICU_FLAGS], [$xerces_cv_icu_flags])
        AC_SUBST([ICU_LIBS], [$xerces_cv_icu_libs])
        AC_SUBST([ICU_BIN], [$xerces_cv_icu_bin])
        AC_SUBST([ICU_SBIN], [$xerces_cv_icu_sbin])
        ]
)
