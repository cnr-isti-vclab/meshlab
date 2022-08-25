/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/. 
*/


#ifndef _PATATE_DEFINES_
#define _PATATE_DEFINES_

////////////////////////////////////////////////////////////////////////////////
// Compatibility types, macros, functions
//
#ifdef __CUDACC__
# include <cuda.h>
#endif

#ifdef __CUDA_ARCH__
  #define MULTIARCH_STD_MATH(FUNC)
#else
  #define MULTIARCH_STD_MATH(FUNC) using std::FUNC;
#endif

#ifdef __CUDACC__
# define MULTIARCH __host__ __device__
#else
# define MULTIARCH

// GCC: compile with -std=c++0x
# if defined(__GNUC__) && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || (__GNUC__ >= 5))
#   if defined(nullptr_t) || (__cplusplus > 199711L) || defined(HACK_GCC_ITS_CPP0X)
#     define __CPP0X__
#   endif
# endif 

#endif // ifdef __CUDACC__

#endif //#ifndef _PATATE_DEFINES_
