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

#include  <sys/utsname.h>
#ifdef _AIX
#include <sys/vminfo.h>
#endif
#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <mach/host_info.h>
#include <mach/machine.h>
#endif

#define CPUTYPE_UNKNOWN    0
#define CPUTYPE_POWER3     1
#define CPUTYPE_POWER4     2
#define CPUTYPE_PPC970     3
#define CPUTYPE_POWER5     4
#define CPUTYPE_POWER6     5
#define CPUTYPE_CELL       6
#define CPUTYPE_PPCG4	   7

char *cpuname[] = {
  "UNKNOWN",
  "POWER3",
  "POWER4",
  "PPC970",
  "POWER5",
  "POWER6",
  "CELL",
  "PPCG4",
};

char *lowercpuname[] = {
  "unknown",
  "power3",
  "power4",
  "ppc970",
  "power5",
  "power6",
  "cell",
  "ppcg4",
};

char *corename[] = {
  "UNKNOWN",
  "POWER3",
  "POWER4",
  "POWER4",
  "POWER4",
  "POWER6",
  "CELL",
  "PPCG4",
};

int detect(void){

#ifdef linux
  FILE *infile;
  char buffer[512], *p;

  p = (char *)NULL;
  infile = fopen("/proc/cpuinfo", "r");
  while (fgets(buffer, sizeof(buffer), infile)){
    if (!strncmp("cpu", buffer, 3)){
	p = strchr(buffer, ':') + 2;
#if 0
	fprintf(stderr, "%s\n", p);
#endif
	break;
      }
  }

  fclose(infile);

  if (!strncasecmp(p, "POWER3", 6)) return CPUTYPE_POWER3;
  if (!strncasecmp(p, "POWER4", 6)) return CPUTYPE_POWER4;
  if (!strncasecmp(p, "PPC970", 6)) return CPUTYPE_PPC970;
  if (!strncasecmp(p, "POWER5", 6)) return CPUTYPE_POWER5;
  if (!strncasecmp(p, "POWER6", 6)) return CPUTYPE_POWER6;
  if (!strncasecmp(p, "Cell",   4)) return CPUTYPE_CELL;
  if (!strncasecmp(p, "7447",   4)) return CPUTYPE_PPCG4;

  return CPUTYPE_UNKNOWN;
#endif

#ifdef _AIX
  return CPUTYPE_POWER5;
#endif

#ifdef __APPLE__
  host_basic_info_data_t   hostInfo;
  mach_msg_type_number_t  infoCount;

  infoCount = HOST_BASIC_INFO_COUNT;
  host_info(mach_host_self(), HOST_BASIC_INFO, (host_info_t)&hostInfo, &infoCount);

  if (hostInfo.cpu_subtype == CPU_SUBTYPE_POWERPC_7450) return CPUTYPE_PPCG4;
  if (hostInfo.cpu_subtype == CPU_SUBTYPE_POWERPC_970)  return CPUTYPE_PPC970;
 
  return  CPUTYPE_PPC970;
#endif
}

void get_architecture(void){
  printf("POWER");
}

void get_subdirname(void){
    printf("power");
}


void get_subarchitecture(void){
  printf("%s", cpuname[detect()]);
}

void get_cpuconfig(void){
#if 0
#ifdef _AIX
  struct vminfo info;
#endif
#endif

  printf("#define %s\n", cpuname[detect()]);
  printf("#define CORE_%s\n", corename[detect()]);

  printf("#define L1_DATA_SIZE 32768\n");
  printf("#define L1_DATA_LINESIZE 128\n");
  printf("#define L2_SIZE 524288\n");
  printf("#define L2_LINESIZE 128 \n");
  printf("#define DTB_ENTRIES 128\n");
  printf("#define DTB_SIZE 4096\n");
  printf("#define L2_ASSOCIATIVE 8\n");

#if 0
#ifdef _AIX
  if (vmgetinfo(&info, VMINFO, 0) == 0) {
    if ((info.lgpg_size >> 20) >= 1024) {
      printf("#define ALLOC_HUGETLB\n");
    }
  }
#endif
#endif

}

void get_libname(void){
  printf("%s", lowercpuname[detect()]);
}

char *get_corename(void){
  return cpuname[detect()];
}
