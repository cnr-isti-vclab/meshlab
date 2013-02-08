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

void get_architecture(void){
  printf("MIPS64");
}

void get_subarchitecture(void){
  printf("SICORTEX");
}

void get_subdirname(void){
  printf("mips64");
}

void get_cpuconfig(void){
  printf("#define SICORTEX\n");
  printf("#define L1_DATA_SIZE 32768\n");
  printf("#define L1_DATA_LINESIZE 32\n");
  printf("#define L2_SIZE 512488\n");
  printf("#define L2_LINESIZE 32\n");
  printf("#define DTB_ENTRIES 32\n");
  printf("#define DTB_SIZE 4096\n");
  printf("#define L2_ASSOCIATIVE 8\n");
}

void get_libname(void){
#ifdef __mips64
  printf("mips64\n");
#else
  printf("mips32\n");
#endif
}
