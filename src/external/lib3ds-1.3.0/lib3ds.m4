dnl
dnl AM_PATH_LIB3DS([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl
AC_DEFUN([AM_PATH_LIB3DS],
[

AC_ARG_WITH(lib3ds-prefix,[  --with-lib3ds-prefix=PFX   Prefix where lib3ds is installed (optional)],
            lib3ds_config_prefix="$withval", lib3ds_config_prefix="")
AC_ARG_WITH(lib3ds-exec-prefix,[  --with-lib3ds-exec-prefix=PFX  Exec prefix where lib3ds is installed (optional)],
            lib3ds_config_exec_prefix="$withval", lib3ds_config_exec_prefix="")

  if test x$lib3ds_config_exec_prefix != x ; then
     lib3ds_config_args="$lib3ds_config_args --exec-prefix=$lib3ds_config_exec_prefix"
     if test x${LIB3DS_CONFIG+set} != xset ; then
        LIB3DS_CONFIG=$lib3ds_config_exec_prefix/bin/lib3ds-config
     fi
  fi
  if test x$lib3ds_config_prefix != x ; then
     lib3ds_config_args="$lib3ds_config_args --prefix=$lib3ds_config_prefix"
     if test x${LIB3DS_CONFIG+set} != xset ; then
        LIB3DS_CONFIG=$lib3ds_config_prefix/bin/lib3ds-config
     fi
  fi

  AC_PATH_PROG(LIB3DS_CONFIG, lib3ds-config, no)
  lib3ds_version_min=$1

  AC_MSG_CHECKING(for Lib3ds - version >= $lib3ds_version_min)
  no_lib3ds=""
  if test "$LIB3DS_CONFIG" = "no" ; then
    no_lib3ds=yes
  else
    LIB3DS_CFLAGS=`$LIB3DS_CONFIG --cflags`
    LIB3DS_LIBS=`$LIB3DS_CONFIG --libs`
    lib3ds_version=`$LIB3DS_CONFIG --version`

    lib3ds_major_version=`echo $lib3ds_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    lib3ds_minor_version=`echo $lib3ds_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    lib3ds_micro_version=`echo $lib3ds_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    lib3ds_major_min=`echo $lib3ds_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    lib3ds_minor_min=`echo $lib3ds_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    lib3ds_micro_min=`echo $lib3ds_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    lib3ds_version_proper=`expr \
        $lib3ds_major_version \> $lib3ds_major_min \| \
        $lib3ds_major_version \= $lib3ds_major_min \& \
        $lib3ds_minor_version \> $lib3ds_minor_min \| \
        $lib3ds_major_version \= $lib3ds_major_min \& \
        $lib3ds_minor_version \= $lib3ds_minor_min \& \
        $lib3ds_micro_version \>= $lib3ds_micro_min `

    if test "$lib3ds_version_proper" = "1" ; then
      AC_MSG_RESULT([$lib3ds_major_version.$lib3ds_minor_version.$lib3ds_micro_version])
    else
      AC_MSG_RESULT(no)
      no_lib3ds=yes
    fi
  fi

  if test "x$no_lib3ds" = x ; then
     ifelse([$2], , :, [$2])     
  else
     LIB3DS_CFLAGS=""
     LIB3DS_LIBS=""
     ifelse([$3], , :, [$3])
  fi

  AC_SUBST(LIB3DS_CFLAGS)
  AC_SUBST(LIB3DS_LIBS)
])



