    **************************************************************
                                LEVMAR
                              version 2.3
                          By Manolis Lourakis

                     Institute of Computer Science
            Foundation for Research and Technology - Hellas
                       Heraklion, Crete, Greece
    **************************************************************


GENERAL
This is levmar, a copylefted C/C++ implementation of the Levenberg-Marquardt non-linear
least squares algorithm. levmar includes double and single precision LM versions, both
with analytic and finite difference approximated jacobians. levmar also has some support
for constrained non-linear least squares, allowing linear equation and box constraints.
You have the following options regarding the solution of the underlying augmented normal
equations:

1) Assuming that you have LAPACK (or an equivalent vendor library such as ESSL, MKL,
   NAG, ...) installed, you can use the included LAPACK-based solvers (default).

2) If you don't have LAPACK or decide not to use it, undefine HAVE_LAPACK in lm.h
   and a LAPACK-free, LU-based linear systems solver will by used. Also, the line
   setting the variable LAPACKLIBS in the Makefile should be commented out.

It is strongly recommended that you *do* employ LAPACK; if you don't have it already,
I suggest getting clapack from http://www.netlib.org/clapack. However, LAPACK's
use is not mandatory and the 2nd option makes levmar totally self-contained.
See lmdemo.c for examples of use and http://www.ics.forth.gr/~lourakis/levmar
for general comments.

The mathematical theory behind levmar is described in the lecture notes entitled
"Methods for Non-Linear Least Squares Problems", by K. Madsen, H.B. Nielsen and O. Tingleff,
Technical University of Denmark (http://www.imm.dtu.dk/courses/02611/nllsq.pdf). 

LICENSE
levmar is released under the GNU Public License (GPL), which can be found in the included
LICENSE file. Note that under the terms of GPL, commercial use is allowed only if a software
employing levmar is also published in source under the GPL. However, if you are interested
in using levmar in a proprietary commercial apprlication, a commercial license for levmar
can be obtained by contacting the author using the email address at the end of this file.

COMPILATION
 - You might first consider setting a few configuration options at the top of
   lm.h. See the accompanying comments for more details.

 - On a Linux/Unix system, typing "make" will build both levmar and the demo
   program using gcc. Alternatively, if Intel's C++ compiler is installed, it
   can be used by typing "make -f Makefile.icc".

 - Under Windows and if Visual C is installed & configured for command line
   use, type "nmake /f Makefile.vc" in a cmd window to build levmar and the
   demo program. In case of trouble, read the comments on top of Makefile.vc

 - levmar can also be built under various platforms using the CMake cross-platform
   build system. The included CMakeLists.txt file can be used to generate makefiles
   for Unix systems or project files for Windows systems. See http://www.cmake.org
   for details.

MATLAB INTERFACE
Since version 2.2, the levmar distrubution includes a matlab interface.
See the 'matlab' subdirectory for more information and examples of use.

Notice that *_core.c files are not to be compiled directly; For example,
Axb_core.c is included by Axb.c, to provide single and double precision
routine versions.


Send your comments/bug reports to lourakis at ics forth gr
