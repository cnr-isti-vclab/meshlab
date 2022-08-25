/*********************************************************************/
/*                                                                   */
/*             Optimized BLAS libraries                              */
/*                     By Kazushige Goto <kgoto@tacc.utexas.edu>     */
/*                                                                   */
/* Copyright (c) The University of Texas, 2009. All rights reserved. */
/* UNIVERSITY EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES CONCERNING  */
/* THIS SOFTWARE AND DOCUMENTATION, INCLUDING ANY WARRANTIES OF      */
/* MERCHANTABILITY, FITNESS FOR ANY PARTICULAR PURPOSE,              */
/* NON-INFRINGEMENT AND WARRANTIES OF PERFORMANCE, AND ANY WARRANTY  */
/* THAT MIGHT OTHERWISE ARISE FROM COURSE OF DEALING OR USAGE OF     */
/* TRADE. NO WARRANTY IS EITHER EXPRESS OR IMPLIED WITH RESPECT TO   */
/* THE USE OF THE SOFTWARE OR DOCUMENTATION.                         */
/* Under no circumstances shall University be liable for incidental, */
/* special, indirect, direct or consequential damages or loss of     */
/* profits, interruption of business, or related expenses which may  */
/* arise from use of Software or Documentation, including but not    */
/* limited to those resulting from defects in Software and/or        */
/* Documentation, or loss or inaccuracy of data of any kind.         */
/*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "common.h"

#ifndef USE_OPENMP

#include "blas_server.c"

#else

int blas_server_avail = 0;

int blas_thread_init(void){

  blas_get_cpu_number();

  blas_server_avail = 1;

  return 0;
}

int BLASFUNC(blas_thread_shutdown)(void){

  blas_server_avail = 0;

  return 0;
}

static void legacy_exec(void *func, int mode, blas_arg_t *args, void *sb){

      if (!(mode & BLAS_COMPLEX)){
#ifdef EXPRECISION
	if (mode & BLAS_XDOUBLE){
	  /* REAL / Extended Double */
	  void (*afunc)(BLASLONG, BLASLONG, BLASLONG, xdouble, 
			xdouble *, BLASLONG, xdouble *, BLASLONG, 
			xdouble *, BLASLONG, void *) = func;

	  afunc(args -> m, args -> n, args -> k,
		((xdouble *)args -> alpha)[0],
		args -> a, args -> lda,
		args -> b, args -> ldb,
		args -> c, args -> ldc, sb);
	} else 
#endif
	  if (mode & BLAS_DOUBLE){
	    /* REAL / Double */
	    void (*afunc)(BLASLONG, BLASLONG, BLASLONG, double, 
			  double *, BLASLONG, double *, BLASLONG, 
			  double *, BLASLONG, void *) = func;
	    
	    afunc(args -> m, args -> n, args -> k,
		  ((double *)args -> alpha)[0],
		  args -> a, args -> lda,
		  args -> b, args -> ldb,
		  args -> c, args -> ldc, sb);
	  } else {
	    /* REAL / Single */
	    void (*afunc)(BLASLONG, BLASLONG, BLASLONG, float, 
			  float *, BLASLONG, float *, BLASLONG, 
			  float *, BLASLONG, void *) = func;
	    
	    afunc(args -> m, args -> n, args -> k,
		  ((float *)args -> alpha)[0],
		  args -> a, args -> lda,
		  args -> b, args -> ldb,
		  args -> c, args -> ldc, sb);
	  }
      } else {
#ifdef EXPRECISION
	if (mode & BLAS_XDOUBLE){
	  /* COMPLEX / Extended Double */
	  void (*afunc)(BLASLONG, BLASLONG, BLASLONG, xdouble, xdouble,
			xdouble *, BLASLONG, xdouble *, BLASLONG, 
			xdouble *, BLASLONG, void *) = func;

	  afunc(args -> m, args -> n, args -> k,
		((xdouble *)args -> alpha)[0],
		((xdouble *)args -> alpha)[1],
		args -> a, args -> lda,
		args -> b, args -> ldb,
		args -> c, args -> ldc, sb);
	} else
#endif
	  if (mode & BLAS_DOUBLE){
	    /* COMPLEX / Double */
	  void (*afunc)(BLASLONG, BLASLONG, BLASLONG, double, double,
			double *, BLASLONG, double *, BLASLONG, 
			double *, BLASLONG, void *) = func;

	  afunc(args -> m, args -> n, args -> k,
		((double *)args -> alpha)[0],
		((double *)args -> alpha)[1],
		args -> a, args -> lda,
		args -> b, args -> ldb,
		args -> c, args -> ldc, sb);
	  } else {
	    /* COMPLEX / Single */
	  void (*afunc)(BLASLONG, BLASLONG, BLASLONG, float, float,
			float *, BLASLONG, float *, BLASLONG, 
			float *, BLASLONG, void *) = func;

	  afunc(args -> m, args -> n, args -> k,
		((float *)args -> alpha)[0],
		((float *)args -> alpha)[1],
		args -> a, args -> lda,
		args -> b, args -> ldb,
		args -> c, args -> ldc, sb);
	  }
      }
}

static void exec_threads(blas_queue_t *queue){

  void *buffer, *sa, *sb;

  buffer = NULL;
  sa = queue -> sa;
  sb = queue -> sb;

#ifdef CONSISTENT_FPCSR
  __asm__ __volatile__ ("ldmxcsr %0" : : "m" (queue -> sse_mode));
  __asm__ __volatile__ ("fldcw %0"   : : "m" (queue -> x87_mode));
#endif

  if ((sa == NULL) && (sb == NULL) && ((queue -> mode & BLAS_PTHREAD) == 0)) {

    buffer = blas_memory_alloc(2);

    if (sa == NULL) sa = (void *)((BLASLONG)buffer + GEMM_OFFSET_A);
    
    if (sb == NULL) {
      if (!(queue -> mode & BLAS_COMPLEX)){
#ifdef EXPRECISION
	if (queue -> mode & BLAS_XDOUBLE){
	  sb = (void *)(((BLASLONG)sa + ((QGEMM_P * QGEMM_Q * sizeof(xdouble) 
					  + GEMM_ALIGN) & ~GEMM_ALIGN)) + GEMM_OFFSET_B);
	} else 
#endif
	  if (queue -> mode & BLAS_DOUBLE){
	    sb = (void *)(((BLASLONG)sa + ((DGEMM_P * DGEMM_Q * sizeof(double)
					    + GEMM_ALIGN) & ~GEMM_ALIGN)) + GEMM_OFFSET_B);
	    
	  } else {
	    sb = (void *)(((BLASLONG)sa + ((SGEMM_P * SGEMM_Q * sizeof(float)
					    + GEMM_ALIGN) & ~GEMM_ALIGN)) + GEMM_OFFSET_B);
	  }
      } else {
#ifdef EXPRECISION
	if (queue -> mode & BLAS_XDOUBLE){
	  sb = (void *)(((BLASLONG)sa + ((XGEMM_P * XGEMM_Q * 2 * sizeof(xdouble)
					  + GEMM_ALIGN) & ~GEMM_ALIGN)) + GEMM_OFFSET_B);
	} else
#endif
	  if (queue -> mode & BLAS_DOUBLE){
	    sb = (void *)(((BLASLONG)sa + ((ZGEMM_P * ZGEMM_Q * 2 * sizeof(double)
					    + GEMM_ALIGN) & ~GEMM_ALIGN)) + GEMM_OFFSET_B);
	  } else {
	    sb = (void *)(((BLASLONG)sa + ((CGEMM_P * CGEMM_Q * 2 * sizeof(float)
					    + GEMM_ALIGN) & ~GEMM_ALIGN)) + GEMM_OFFSET_B);
	  }
      }
    }
  }

  if (queue -> mode & BLAS_LEGACY) {
    legacy_exec(queue -> routine, queue -> mode, queue -> args, sb);
  } else
    if (queue -> mode & BLAS_PTHREAD) {
      void (*pthreadcompat)(void *) = queue -> routine;
      (pthreadcompat)(queue -> args);

    } else {
      int (*routine)(blas_arg_t *, void *, void *, void *, void *, BLASLONG) = queue -> routine;

      (routine)(queue -> args, queue -> range_m, queue -> range_n, sa, sb, queue -> position);

    }

  if (buffer != NULL) blas_memory_free(buffer);

}

int exec_blas(BLASLONG num, blas_queue_t *queue){

  BLASLONG i;

  if ((num <= 0) || (queue == NULL)) return 0;

#ifdef CONSISTENT_FPCSR
  for (i = 0; i < num; i ++) {
    __asm__ __volatile__ ("fnstcw %0"  : "=m" (queue[i].x87_mode));
    __asm__ __volatile__ ("stmxcsr %0" : "=m" (queue[i].sse_mode));
  }
#endif

#pragma omp parallel for schedule(static)
  for (i = 0; i < num; i ++) {

#ifndef USE_SIMPLE_THREADED_LEVEL3
    queue[i].position = i;
#endif

    exec_threads(&queue[i]);
  }

  return 0;
}

#endif
