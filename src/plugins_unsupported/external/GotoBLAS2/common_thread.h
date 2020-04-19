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

#ifndef COMMON_THREAD
#define COMMON_THREAD

/* Basic Thread Debugging */
#undef SMP_DEBUG

/* Thread Timing Debugging */
#undef TIMING_DEBUG

/* Global Parameter */
extern int blas_cpu_number;
extern int blas_num_threads;
extern int blas_omp_linked;

#define BLAS_LEGACY	0x8000U
#define BLAS_PTHREAD	0x4000U
#define BLAS_NODE	0x2000U

#define BLAS_PREC	0x0003U
#define BLAS_SINGLE	0x0000U
#define BLAS_DOUBLE	0x0001U
#define BLAS_XDOUBLE	0x0002U
#define BLAS_REAL	0x0000U
#define BLAS_COMPLEX	0x0004U

#define BLAS_TRANSA	0x0030U	/* 2bit */
#define BLAS_TRANSA_N	0x0000U
#define BLAS_TRANSA_T	0x0010U
#define BLAS_TRANSA_R	0x0020U
#define BLAS_TRANSA_C	0x0030U
#define BLAS_TRANSA_SHIFT     4

#define BLAS_TRANSB	0x0300U	/* 2bit */
#define BLAS_TRANSB_N	0x0000U
#define BLAS_TRANSB_T	0x0100U
#define BLAS_TRANSB_R	0x0200U
#define BLAS_TRANSB_C	0x0300U
#define BLAS_TRANSB_SHIFT     8

#define BLAS_RSIDE      0x0400U
#define BLAS_RSIDE_SHIFT     10
#define BLAS_UPLO       0x0800U
#define BLAS_UPLO_SHIFT      11

#define BLAS_STATUS_NOTYET	0
#define BLAS_STATUS_QUEUED	1
#define BLAS_STATUS_RUNNING	2
#define BLAS_STATUS_FINISHED	4

typedef struct blas_queue {

  void *routine;
  BLASLONG position;
  BLASLONG assigned;

  blas_arg_t *args;
  void *range_m;
  void *range_n;
  void *sa, *sb;

  struct blas_queue *next;

#if defined( __WIN32__) || defined(__CYGWIN32__)
  CRITICAL_SECTION lock;
  HANDLE finish;
#else
  pthread_mutex_t	 lock;
  pthread_cond_t	 finished;
#endif

  int mode, status;

#ifdef CONSISTENT_FPCSR
  unsigned int sse_mode, x87_mode;
#endif

#ifdef SMP_DEBUG
  int    num;
#endif
#ifdef TIMING_DEBUG
  unsigned int clocks;
#endif
} blas_queue_t;

#ifdef SMP_SERVER

extern int blas_server_avail;

static __inline int num_cpu_avail(int level) {

  if ((blas_cpu_number == 1) 

#ifdef USE_OPENMP
      || omp_in_parallel()
#endif
      ) return 1;

  return blas_cpu_number;

}

static __inline void blas_queue_init(blas_queue_t *queue){

  queue -> sa    = NULL;
  queue -> sb    = NULL;
  queue-> next  = NULL;
}

int blas_thread_init(void);
int BLASFUNC(blas_thread_shutdown)(void);
int exec_blas(BLASLONG, blas_queue_t *);
int exec_blas_async(BLASLONG, blas_queue_t *);
int exec_blas_async_wait(BLASLONG, blas_queue_t *);

#else
int exec_blas_async(BLASLONG num_cpu, blas_param_t *param, pthread_t *);
int exec_blas_async_wait(BLASLONG num_cpu, pthread_t *blas_threads);
int exec_blas(BLASLONG num_cpu, blas_param_t *param, void *buffer);
#endif

#ifndef ASSEMBLER

int blas_level1_thread(int mode, BLASLONG m, BLASLONG n, BLASLONG k, void *alpha,
		       void *a, BLASLONG lda,
		       void *b, BLASLONG ldb, 
		       void *c, BLASLONG ldc, int (*function)(), int threads);

int gemm_thread_m (int mode, blas_arg_t *, BLASLONG *, BLASLONG *, int (*function)(), void *, void *, BLASLONG);

int gemm_thread_n (int mode, blas_arg_t *, BLASLONG *, BLASLONG *, int (*function)(), void *, void *, BLASLONG);

int gemm_thread_mn(int mode, blas_arg_t *, BLASLONG *, BLASLONG *, int (*function)(), void *, void *, BLASLONG);

int gemm_thread_variable(int mode, blas_arg_t *, BLASLONG *, BLASLONG *, int (*function)(), void *, void *, BLASLONG, BLASLONG);

int trsm_thread(int mode, BLASLONG m, BLASLONG n, 
		double alpha_r, double alpha_i,
		void *a, BLASLONG lda,
		void *c, BLASLONG ldc, int (*function)(), void *buffer);

int syrk_thread(int mode, blas_arg_t *, BLASLONG *, BLASLONG *, int (*function)(), void *, void *, BLASLONG);

int beta_thread(int mode, BLASLONG m, BLASLONG n, 
		double alpha_r, double alpha_i,
		void *c, BLASLONG ldc, int (*fuction)());

int getrf_thread(int mode, BLASLONG m, BLASLONG n, BLASLONG k,
		 void *offsetA, BLASLONG lda,
		 void *offsetB, BLASLONG jb,
		 void *ipiv, BLASLONG offset, int (*function)(), void *buffer);

#endif  /* ENDIF ASSEMBLER */

#endif
