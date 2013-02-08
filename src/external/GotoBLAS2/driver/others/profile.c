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

#include "common.h"
#include <signal.h>
#include <strings.h>
#define USE_FUNCTABLE
#include "../../interface/functable.h"

func_profile_t function_profile_table[MAX_PROF_TABLE];

int gotoblas_profile = 1;

static struct sigaction sa, ig;

void gotoblas_profile_quit(void) {

  int i;
  unsigned long long calls, fops, cycles, tcycles, area;

  sigaction(SIGPROF, &ig, NULL);

  calls   = 0;
  fops    = 0;
  cycles  = 0;
  tcycles = 0;
  area    = 0;

  for (i = 0; i < MAX_PROF_TABLE; i ++) {
    if (function_profile_table[i].calls) {
      calls   += function_profile_table[i].calls;
      cycles  += function_profile_table[i].cycles;
      tcycles += function_profile_table[i].tcycles;
      area    += function_profile_table[i].area;
      fops    += function_profile_table[i].fops;
    }
  }

  if (cycles > 0) {

    fprintf(stderr, "\n\t====== BLAS Profiling Result =======\n\n");
    fprintf(stderr, "  Function      No. of Calls   Time Consumption   Efficiency  Bytes/cycle\n");
    
    for (i = 0; i < MAX_PROF_TABLE; i ++) {
      if (function_profile_table[i].calls) {
#ifndef OS_WINDOWS
	fprintf(stderr, "%-12s  : %10Ld        %8.2f%%      %10.3f%%  %8.2f\n", 
#else
	fprintf(stderr, "%-12s  : %10lld        %8.2f%%      %10.3f%%  %8.2f\n", 
#endif
		func_table[i],
		function_profile_table[i].calls,
		(double)function_profile_table[i].cycles  / (double)cycles * 100.,
		(double)function_profile_table[i].fops    / (double)function_profile_table[i].tcycles * 100.,
		(double)function_profile_table[i].area    / (double)function_profile_table[i].cycles
		);
      }
    }

    fprintf(stderr, " --------------------------------------------------------------------\n");
    
#ifndef OS_WINDOWS
	fprintf(stderr, "%-12s  : %10Ld                       %10.3f%%  %8.2f\n", 
#else
	fprintf(stderr, "%-12s  : %10lld                       %10.3f%%  %8.2f\n", 
#endif
		"Total",
		 calls,
		(double)fops    / (double)tcycles * 100.,
		(double)area    / (double)cycles);
  }

  sigaction(SIGPROF, &sa, NULL);
}

void gotoblas_profile_clear(void) {

  int i;

  for (i = 0; i < MAX_PROF_TABLE; i ++) {
    function_profile_table[i].calls  = 0;
    function_profile_table[i].cycles = 0;
    function_profile_table[i].tcycles = 0;
    function_profile_table[i].area = 0;
    function_profile_table[i].fops = 0;
  }

}

void gotoblas_profile_init(void) {

  gotoblas_profile_clear();

  bzero(&sa, sizeof(struct sigaction));
  sa.sa_handler = (void *)gotoblas_profile_quit;
  sa.sa_flags  = SA_NODEFER | SA_RESETHAND;

  bzero(&ig, sizeof(struct sigaction));
  ig.sa_handler = SIG_IGN;
  ig.sa_flags |= SA_NODEFER | SA_RESETHAND;

  sigaction(SIGPROF, &sa, NULL);

}
