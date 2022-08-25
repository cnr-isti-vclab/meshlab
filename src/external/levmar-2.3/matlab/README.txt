This directory contains a matlab MEX interface to levmar. This interface
has been tested with Matlab v. 6.5 R13 under linux and v. 7.4 R2007 under Windows.

FILES
The following files are included:
levmar.c: C MEX-file for levmar
Makefile: UNIX makefile for compiling levmar.c using mex
Makefile.w32: Windows makefile for compiling levmar.c using mex
levmar.m: Documentation for the MEX interface
lmdemo.m: Demonstration of using the MEX interface; run as matlab < lmdemo.m

*.m: Matlab functions implementing various objective functions and their Jacobians.
     For instance, meyer.m implements the objective function for Meyer's (reformulated)
     problem and jacmeyer.m implements its Jacobian.



COMPILING
Use the provided Makefile or Makefile.w32, depending on your platform.
Alternatively, levmar.c can be compiled from matlab's prompt with a
command like

mex -DHAVE_LAPACK -I.. -O -L<levmar library dir> -L<blas/lapack libraries dir> levmar.c -llevmar -lclapack -lblas -lf2c
          
Make sure that you substitute the angle brackets with the correct paths to
the levmar and the blas/lapack directories. Also, on certain systems,
-lf2c should be changed to -llibF77 -llibI77
If your mex compiler has not been configured, the following command should be run first:

mex -setup 


TESTING
After compiling, execute lmdemo.m with matlab < lmdemo.m 
