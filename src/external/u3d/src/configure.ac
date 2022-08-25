AC_INIT()
# LT_INIT(dlopen)
AC_CONFIG_SRCDIR(RTL)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([Config])
AM_INIT_AUTOMAKE(u3d,1.0)

AC_PROG_CC
AC_PATH_XTRA
# prevent Autoconf from setting default CXXFLAGS (-g -O2)
if test ! ${CXXFLAGS+set}; then
  CXXFLAGS=""
fi

AC_PROG_CXX
AC_PROG_INSTALL
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

AC_CHECK_HEADER(png.h, [echo "Found PNG headers"], [echo "Install PNG headers" && exit])
# AC_CHECK_LIB(png, png_get_io_ptr, [], [echo "Install png library" && exit])
AC_CHECK_HEADER(jpeglib.h, [echo "Found JPEG headers"], [echo "Install JPEG headers" && exit])
LIBS_MYBAK=${LIBS}
AC_SEARCH_LIBS(dlopen, dl)
DL_LIBS=${LIBS} AC_SUBST(DL_LIBS)
LIBS=${LIBS_MYBAK}

AC_ARG_ENABLE(debug, [  --enable-debug enable debug output and produce debug information],
[case "${enableval}" in
 yes)  AM_CXXFLAGS="-Dgcc -Wall -DU3D_NO_ASM -DDEBUG -D_DEBUG -O0 -g3" ;;
 no)   AM_CXXFLAGS="-Dgcc -Wall -DU3D_NO_ASM -DNDEBUG -O2" ;;
 *)    AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
esac],
[      AM_CXXFLAGS="-Dgcc -Wall -DU3D_NO_ASM -DNDEBUG -O2"])

case "${host}" in
  *darwin*)
	AM_CXXFLAGS="${AM_CXXFLAGS} -DMAC32 -fno-strict-aliasing" AC_SUBST(AM_CXXFLAGS)
	U3D_PLATFORM="Mac32" AC_SUBST(U3D_PLATFORM)
	darwin=true
       ;;

  *)
	AM_CXXFLAGS="${AM_CXXFLAGS} -DLINUX -fno-strict-aliasing" AC_SUBST(AM_CXXFLAGS)
	U3D_PLATFORM="Lin32" AC_SUBST(U3D_PLATFORM)
	linux=true
       ;;

esac
AM_CONDITIONAL(Mac,(test x$darwin = xtrue))
AM_CONDITIONAL(Linux,(test x$linux = xtrue))

AC_ARG_ENABLE(stdiohack, [  --enable-stdiohack enable stdin input of IDTF and stdout output of (slightly broken) U3D files],
[case "${enableval}" in
 yes)  AM_CXXFLAGS="${AM_CXXFLAGS} -DSTDIO_HACK" AC_SUBST(AM_CXXFLAGS) ;;
 no)   AM_CXXFLAGS="${AM_CXXFLAGS} -USTDIO_HACK" AC_SUBST(AM_CXXFLAGS) ;;
 *)    AC_MSG_ERROR(bad value ${enableval} for --enable-stdiohack) ;;
esac] )

AC_CONFIG_FILES([
Makefile
RTL/Makefile
RTL/Component/Include/Makefile
RTL/Kernel/Include/Makefile
RTL/Platform/Include/Makefile
RTL/Platform/Mac32/Makefile
RTL/Platform/Mac32/IFXCore/Makefile
RTL/Platform/Mac32/IFXCoreStatic/Makefile
RTL/Platform/Mac32/IFXExporting/Makefile
RTL/Platform/Mac32/IFXImporting/Makefile
RTL/Platform/Mac32/IFXScheduling/Makefile
RTL/Platform/Lin32/Makefile
RTL/Platform/Lin32/IFXCore/Makefile
RTL/Platform/Lin32/IFXCoreStatic/Makefile
RTL/Platform/Lin32/IFXExporting/Makefile
RTL/Platform/Lin32/IFXImporting/Makefile
RTL/Platform/Lin32/IFXScheduling/Makefile
IDTF/Makefile
libIDTF/Makefile
Samples/Makefile
Samples/SampleCode/Makefile
Samples/TestScenes/Makefile
Docs/Makefile
])
AC_OUTPUT
