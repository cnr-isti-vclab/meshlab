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

#ifdef ARCH_X86
#define EXTERN extern
#else
#define EXTERN
#endif

EXTERN gotoblas_t  gotoblas_KATMAI;
EXTERN gotoblas_t  gotoblas_COPPERMINE;
EXTERN gotoblas_t  gotoblas_NORTHWOOD;
EXTERN gotoblas_t  gotoblas_BANIAS;
EXTERN gotoblas_t  gotoblas_ATHLON;

extern gotoblas_t  gotoblas_PRESCOTT;
extern gotoblas_t  gotoblas_ATOM;
extern gotoblas_t  gotoblas_NANO;
extern gotoblas_t  gotoblas_CORE2;
extern gotoblas_t  gotoblas_PENRYN;
extern gotoblas_t  gotoblas_DUNNINGTON;
extern gotoblas_t  gotoblas_NEHALEM;
extern gotoblas_t  gotoblas_OPTERON;
extern gotoblas_t  gotoblas_OPTERON_SSE3;
extern gotoblas_t  gotoblas_BARCELONA;

#define VENDOR_INTEL      1
#define VENDOR_AMD        2
#define VENDOR_CENTAUR    3
#define VENDOR_UNKNOWN   99

#define BITMASK(a, b, c) ((((a) >> (b)) & (c)))

static int get_vendor(void){
  int eax, ebx, ecx, edx;
  char vendor[13];

  cpuid(0, &eax, &ebx, &ecx, &edx);
  
  *(int *)(&vendor[0]) = ebx;
  *(int *)(&vendor[4]) = edx;
  *(int *)(&vendor[8]) = ecx;
  vendor[12] = (char)0;

  if (!strcmp(vendor, "GenuineIntel")) return VENDOR_INTEL;
  if (!strcmp(vendor, "AuthenticAMD")) return VENDOR_AMD;
  if (!strcmp(vendor, "CentaurHauls")) return VENDOR_CENTAUR;

  if ((eax == 0) || ((eax & 0x500) != 0)) return VENDOR_INTEL;

  return VENDOR_UNKNOWN;
}

static gotoblas_t *get_coretype(void){

  int eax, ebx, ecx, edx;
  int family, exfamily, model, vendor, exmodel;

  cpuid(1, &eax, &ebx, &ecx, &edx);

  family   = BITMASK(eax,  8, 0x0f);
  exfamily = BITMASK(eax, 20, 0xff);
  model    = BITMASK(eax,  4, 0x0f);
  exmodel  = BITMASK(eax, 16, 0x0f);

  vendor = get_vendor();

  if (vendor == VENDOR_INTEL){
    switch (family) {
    case 0x6:
      switch (exmodel) {
      case 0:
	if (model <= 0x7) return &gotoblas_KATMAI;
	if ((model == 0x8) || (model == 0xa) || (model == 0xb)) return &gotoblas_COPPERMINE;
	if ((model == 0x9) || (model == 0xd)) return &gotoblas_BANIAS;
	if (model == 14) return &gotoblas_BANIAS;
	if (model == 15) return &gotoblas_CORE2;
	return NULL;

      case 1:
	if (model == 6) return &gotoblas_CORE2;
	if (model == 7) return &gotoblas_PENRYN;
	if (model == 13) return &gotoblas_DUNNINGTON;
	if ((model == 10) || (model == 11) || (model == 14) || (model == 15)) return &gotoblas_NEHALEM;
	if (model == 12) return &gotoblas_ATOM;
	return NULL;
      }
      case 0xf:
      if (model <= 0x2) return &gotoblas_NORTHWOOD;
      return &gotoblas_PRESCOTT;
    }
  }

  if (vendor == VENDOR_AMD){
    if (family <= 0xe) return &gotoblas_ATHLON;
    if (family == 0xf){
      if ((exfamily == 0) || (exfamily == 2)) {
	if (ecx & (1 <<  0)) return &gotoblas_OPTERON_SSE3; 
	else return &gotoblas_OPTERON;
      }  else {
	return &gotoblas_BARCELONA;
      }
    }
  }

  if (vendor == VENDOR_CENTAUR) {
    switch (family) {
    case 0x6:
      return &gotoblas_NANO;
      break;
    }
  }
  
  return NULL;
}

static char *corename[] = {
    "Unknown",
    "Katmai",
    "Coppermine",
    "Northwood",
    "Prescott",
    "Banias",
    "Atom",
    "Core2",
    "Penryn",
    "Dunnington",
    "Nehalem",
    "Athlon",
    "Opteron",
    "Opteron(SSE3)",
    "Barcelona",
    "Nano",
};

char *gotoblas_corename(void) {

  if (gotoblas == &gotoblas_KATMAI)       return corename[ 1];
  if (gotoblas == &gotoblas_COPPERMINE)   return corename[ 2];
  if (gotoblas == &gotoblas_NORTHWOOD)    return corename[ 3];
  if (gotoblas == &gotoblas_PRESCOTT)     return corename[ 4];
  if (gotoblas == &gotoblas_BANIAS)       return corename[ 5];
  if (gotoblas == &gotoblas_ATOM)         return corename[ 6];
  if (gotoblas == &gotoblas_CORE2)        return corename[ 7];
  if (gotoblas == &gotoblas_PENRYN)       return corename[ 8];
  if (gotoblas == &gotoblas_DUNNINGTON)   return corename[ 9];
  if (gotoblas == &gotoblas_NEHALEM)      return corename[10];
  if (gotoblas == &gotoblas_ATHLON)       return corename[11];
  if (gotoblas == &gotoblas_OPTERON_SSE3) return corename[12]; 
  if (gotoblas == &gotoblas_OPTERON)      return corename[13];
  if (gotoblas == &gotoblas_BARCELONA)    return corename[14];
  if (gotoblas == &gotoblas_NANO)         return corename[15];
  
  return corename[0];
}

void gotoblas_dynamic_init(void) {
  
  if (gotoblas) return;

  gotoblas = get_coretype();
  
#ifdef ARCH_X86
  if (gotoblas == NULL) gotoblas = gotoblas_KATMAI;
#else
  if (gotoblas == NULL) gotoblas = gotoblas_PRESCOTT;
#endif
  
  if (gotoblas && gotoblas -> init) {
    gotoblas -> init();
  } else {
    fprintf(stderr, "GotoBLAS : Architecture Initialization failed. No initialization function found.\n");
    exit(1);
  }
  
}

void gotoblas_dynamic_quit(void) {
  
  gotoblas = NULL;

}
