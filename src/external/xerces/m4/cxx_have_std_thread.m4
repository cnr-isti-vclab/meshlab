dnl @synopsis AC_CXX_HAVE_STD_THREAD
dnl
dnl If the compiler supports ISO C++11 <thread> and <mutex>, define
dnl HAVE_STD_THREAD.
dnl
dnl @category Cxx
dnl @author Roger Leigh
dnl @author Roger Leigh <rleigh@codelibre.net>
dnl @version 2017-06-09
dnl @license AllPermissive

AC_DEFUN([AC_CXX_HAVE_STD_THREAD],
[AC_CACHE_CHECK(whether the compiler supports ISO C++11 <thread> and <mutex>,
ac_cv_cxx_have_std_thread,
[AC_REQUIRE([AC_CXX_HAVE_NAMESPACES])
 AC_REQUIRE([ACX_PTHREAD])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 save_CFLAGS="$CFLAGS"
 CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
 AC_TRY_COMPILE([#include <thread>
#include <mutex>
#include <iostream>

namespace
{

  std::mutex m1;
  std::recursive_mutex m2;

  void
  threadmain()
  {
    std::lock_guard<std::mutex> lock1(m1);
    std::lock_guard<std::recursive_mutex> lock2(m2);
    std::cout << "In thread" << std::endl;
  }

}
],[std::thread foo(threadmain);
  foo.join();
return 0;],
 ac_cv_cxx_have_std_thread=yes, ac_cv_cxx_have_std_thread=no)
 CFLAGS="$save_CFLAGS"
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_have_std_thread" = yes; then
  AC_DEFINE(HAVE_STD_THREAD,,[define if the compiler supports ISO C++11 <thread> and <mutex>])
fi
])
