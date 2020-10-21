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
#include "common.h"

#ifndef SMP
#define blas_cpu_number 1
#else

int blas_cpu_number = 1;

int blas_get_cpu_number(void){

  return blas_cpu_number;
}
#endif

#define FIXED_PAGESIZE 4096

void *sa = NULL;
void *sb = NULL;
static double static_buffer[BUFFER_SIZE/sizeof(double)];

void *blas_memory_alloc(int numproc){
  
  if (sa == NULL){
#if 1
    sa = (void *)qalloc(QFAST, BUFFER_SIZE); 
#else
    sa = (void *)malloc(BUFFER_SIZE); 
#endif
    sb = (void *)&static_buffer[0];
  }

  return sa;
}

void blas_memory_free(void *free_area){
  return;
}

