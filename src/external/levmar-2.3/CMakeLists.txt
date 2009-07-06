# levmar CMake file; see http://www.cmake.org and 
#                        http://www.insightsoftwareconsortium.org/wiki/index.php/CMake_Tutorial

PROJECT(LEVMAR)
#CMAKE_MINIMUM_REQUIRED(VERSION 1.4)

# compiler flags
ADD_DEFINITIONS(-DLINSOLVERS_RETAIN_MEMORY) # do not free memory between linear solvers calls
#REMOVE_DEFINITIONS(-DLINSOLVERS_RETAIN_MEMORY)

# f2c is sometimes equivalent to libF77 & libI77; in that case, set HAVE_F2C to 0
SET(HAVE_F2C 1 CACHE BOOL "Do we have f2c or F77/I77?" )

# the directory where the lapack/blas/f2c libraries reside
SET(LAPACKBLAS_DIR /usr/lib CACHE PATH "Path to lapack/blas libraries")

# actual names for the lapack/blas/f2c libraries
SET(LAPACK_LIB lapack CACHE STRING "The name of the lapack library")
SET(BLAS_LIB blas CACHE STRING "The name of the blas library")
IF(HAVE_F2C)
  SET(F2C_LIB f2c CACHE STRING "The name of the f2c library")
ELSE(HAVE_F2C)
  SET(F77_LIB libF77 CACHE STRING "The name of the F77 library")
  SET(I77_LIB libI77 CACHE STRING "The name of the I77 library")
ENDIF(HAVE_F2C)

########################## NO CHANGES BEYOND THIS POINT ##########################

#INCLUDE_DIRECTORIES(/usr/include)
LINK_DIRECTORIES(${LAPACKBLAS_DIR})

# levmar library source files
ADD_LIBRARY(levmar STATIC
  lm.c Axb.c misc.c lmlec.c lmbc.c lmblec.c
  lm.h misc.h compiler.h
)

# demo program
ADD_EXECUTABLE(lmdemo lmdemo.c lm.h)
# libraries the demo depends on
IF(HAVE_F2C)
  TARGET_LINK_LIBRARIES(lmdemo levmar ${LAPACK_LIB} ${BLAS_LIB} ${F2C_LIB})
ELSE(HAVE_F2C)
  TARGET_LINK_LIBRARIES(lmdemo levmar ${LAPACK_LIB} ${BLAS_LIB} ${F77_LIB} ${I77_LIB})
ENDIF(HAVE_F2C)

# make sure that the library is built before the demo
ADD_DEPENDENCIES(lmdemo levmar)

#SUBDIRS(matlab)

#ADD_TEST(levmar_tst lmdemo)
