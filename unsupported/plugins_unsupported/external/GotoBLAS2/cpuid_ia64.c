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
#include <string.h>
#include <sys/sysinfo.h>
#include "cpuid.h"

#ifdef __ECC
#include <ia64intrin.h>
#endif

static inline unsigned long cpuid(unsigned long regnum){ 
  unsigned long value;

#ifdef __ECC
  value = __getIndReg(_IA64_REG_INDR_CPUID, regnum);
#else
  asm ("mov %0=cpuid[%r1]" : "=r"(value) : "rO"(regnum));
#endif

  return value;
}

int have_cpuid(void){ return 1;}

int get_vendor(void){
  unsigned long cpuid0, cpuid1;
  char vendor[18];

  cpuid0 = cpuid(0);
  cpuid1 = cpuid(1);
  
  *(unsigned long *)(&vendor[0]) = cpuid0;
  *(unsigned long *)(&vendor[8]) = cpuid1;
  vendor[17] = (char)0;

  if (!strcmp(vendor, "GenuineIntel")) return VENDOR_INTEL;

  return VENDOR_UNKNOWN;
}

int get_cputype(int gettype){
  unsigned long cpuid3;

  cpuid3 = cpuid(3);
  
  switch (gettype) {
  case GET_ARCHREV :
    return BITMASK(cpuid3, 32, 0xff);
  case GET_FAMILY :
    return BITMASK(cpuid3, 24, 0xff);
  case GET_MODEL :
    return BITMASK(cpuid3, 16, 0xff);
  case GET_REVISION :
    return BITMASK(cpuid3,  8, 0xff);
  case GET_NUMBER :
    return BITMASK(cpuid3,  0, 0xff);
  }

  return 0;
}

char *get_cpunamechar(void){
  if (get_cputype(GET_FAMILY) == 0x07) return "ITANIUM";
  if (get_cputype(GET_FAMILY) == 0x1f) return "ITANIUM2";
  if (get_cputype(GET_FAMILY) == 0x20) return "ITANIUM2";

  return "UNKNOWN";
}

char *get_libname(void){
  if (get_cputype(GET_FAMILY) == 0x07) { printf("itanium"); return NULL;}
  if (get_cputype(GET_FAMILY) == 0x1f) { printf("itanium2"); return NULL;}
  if (get_cputype(GET_FAMILY) == 0x20) { printf("itanium2"); return NULL;}

  printf("UNKNOWN");

  return NULL;
}

void get_architecture(void){
  printf("IA64");
}

void get_subarchitecture(void){
    printf("%s", get_cpunamechar());
}

void get_subdirname(void){
    printf("ia64");
}

void get_cpuconfig(void){
  printf("#define %s\n", get_cpunamechar());
  printf("#define L1_DATA_SIZE 262144\n");
  printf("#define L1_DATA_LINESIZE 128\n");
  printf("#define L2_SIZE 1572864\n");
  printf("#define L2_LINESIZE 128\n");
  printf("#define DTB_SIZE 16384\n");
  printf("#define DTB_ENTRIES 128\n");
}

