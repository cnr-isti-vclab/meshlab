// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2015 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_DEPRECATED_H
#define IGL_DEPRECATED_H
// Macro for marking a function as deprecated.
// Use C++14 feature [[deprecated]] if available.
// See also https://stackoverflow.com/questions/295120/c-mark-as-deprecated/21265197#21265197

#ifdef __has_cpp_attribute
#  define IGL_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#  define IGL_HAS_CPP_ATTRIBUTE(x) 0
#endif

#ifdef _MSC_VER
#  define IGL_MSC_VER _MSC_VER
#else
#  define IGL_MSC_VER 0
#endif

#ifndef IGL_DEPRECATED
#  if (IGL_HAS_CPP_ATTRIBUTE(deprecated) && __cplusplus >= 201402L) || \
      IGL_MSC_VER >= 1900
#    define IGL_DEPRECATED [[deprecated]]
#  else
#    if defined(__GNUC__) || defined(__clang__)
#      define IGL_DEPRECATED __attribute__((deprecated))
#    elif IGL_MSC_VER
#      define IGL_DEPRECATED __declspec(deprecated)
#    else
#      pragma message("WARNING: You need to implement IGL_DEPRECATED for this compiler")
#      define IGL_DEPRECATED /* deprecated */
#    endif
#  endif
#endif

// Usage:
//
//     template <typename T>
//     IGL_INLINE void my_func(Arg1 a);
//
// becomes
//
//     template <typename T>
//     IGL_DEPRECATED IGL_INLINE void my_func(Arg1 a);
#endif
