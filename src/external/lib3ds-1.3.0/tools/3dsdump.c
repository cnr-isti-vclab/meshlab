/*
 * The 3D Studio File Format Library
 * Copyright (C) 1996-2007 by Jan Eric Kyprianidis <www.kyprianidis.com>
 * All rights reserved.
 *
 * This program is  free  software;  you can redistribute it and/or modify it
 * under the terms of the  GNU Lesser General Public License  as published by 
 * the  Free Software Foundation;  either version 2.1 of the License,  or (at 
 * your option) any later version.
 *
 * This  program  is  distributed in  the  hope that it will  be useful,  but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or  FITNESS FOR A  PARTICULAR PURPOSE.  See the  GNU Lesser General Public  
 * License for more details.
 *
 * You should  have received  a copy of the GNU Lesser General Public License
 * along with  this program;  if not, write to the  Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: 3dsdump.c,v 1.4 2007/06/14 11:29:39 jeh Exp $
 */
#include <lib3ds/file.h>
#include <lib3ds/chunk.h>
#include <stdlib.h>
#include <string.h>


/*!
\example 3dsdump.c

Displays imformation about the content of a <i>3DS</i> file.

\code
Syntax: 3dsdump [options] filename [options]

Options:
  -h/--help           This help
  -s/--structure      Dump structure of file
  -u/--unknown        Report unknown chunks  
  -m/--materials      Dump materials
  -t/--trimeshes      Dump meshes
  -i/--instance       Dump mesh instances
  -c/--cameras        Dump cameras
  -l/--lights         Dump lights
  -n/--nodes          Dump node hierarchy
  -w/--write filename Write new 3ds file to disk
\endcode
*/


static void
help()
{
  fprintf(stderr,
"The 3D Studio File Format Library - 3dsdump\n"
"Copyright (C) 1996-2007 by Jan Eric Kyprianidis <www.kyprianidis.com>\n"
"All rights reserved.\n"
"\n"
"Syntax: 3dsdump [options] filename [options]\n"
"\n"
"Options:\n"
"  -h/--help           This help\n"
"  -s/--structure      Dump structure of file\n"
"  -u/--unknown        Report unknown chunks\n"  
"  -m/--materials      Dump materials\n"
"  -t/--trimeshes      Dump meshes\n"
"  -i/--instance       Dump mesh instances\n"
"  -c/--cameras        Dump cameras\n"
"  -l/--lights         Dump lights\n"
"  -n/--nodes          Dump node hierarchy\n"
"  -w/--write filename Write new 3ds file to disk\n"
"\n"
);
  exit(1);
}

typedef enum _Flags {
  LIB3DSDUMP_STRUCTURE  =0x0001,
  LIB3DSDUMP_UNKNOWN    =0x0002,
  LIB3DSDUMP_MATERIALS  =0x0004,
  LIB3DSDUMP_TRIMESHES  =0x0008,
  LIB3DSDUMP_INSTANCES  =0x0010,
  LIB3DSDUMP_CAMERAS    =0x0020,
  LIB3DSDUMP_LIGHTS     =0x0040,
  LIB3DSDUMP_NODES      =0x0080,
  LIB3DSDUMP_WRITE_3DS  =0x0100
} Flags;

static const char* filename=0;
static const char* output=0;
static Lib3dsDword flags=0;


static void
parse_args(int argc, char **argv)
{
  int i;
  
  for (i=1; i<argc; ++i) {
    if (argv[i][0]=='-') {
      if ((strcmp(argv[i],"-h")==0) || (strcmp(argv[i],"--help")==0)) {
        help();
      }
      else
      if ((strcmp(argv[i],"-s")==0) || (strcmp(argv[i],"--structure")==0)) {
        flags|=LIB3DSDUMP_STRUCTURE;
      }
      else
      if ((strcmp(argv[i],"-u")==0) || (strcmp(argv[i],"--unknown")==0)) {
        flags|=LIB3DSDUMP_UNKNOWN;
      }
      else
      if ((strcmp(argv[i],"-m")==0) || (strcmp(argv[i],"--material")==0)) {
        flags|=LIB3DSDUMP_MATERIALS;
      }
      else
      if ((strcmp(argv[i],"-t")==0) || (strcmp(argv[i],"--trimesh")==0)) {
        flags|=LIB3DSDUMP_TRIMESHES;
      }
      else
      if ((strcmp(argv[i],"-i")==0) || (strcmp(argv[i],"--instance")==0)) {
        flags|=LIB3DSDUMP_INSTANCES;
      }
      else
      if ((strcmp(argv[i],"-c")==0) || (strcmp(argv[i],"--camera")==0)) {
        flags|=LIB3DSDUMP_CAMERAS;
      }
      else
      if ((strcmp(argv[i],"-l")==0) || (strcmp(argv[i],"--light")==0)) {
        flags|=LIB3DSDUMP_LIGHTS;
      }
      else
      if ((strcmp(argv[i],"-n")==0) || (strcmp(argv[i],"--nodes")==0)) {
        flags|=LIB3DSDUMP_NODES;
      }
      else
      if ((strcmp(argv[i],"-w")==0) || (strcmp(argv[i],"--write")==0)) {
        flags|=LIB3DSDUMP_WRITE_3DS;
        ++i;
        if (i>=argc) {
          help();
        }
        output=argv[i];
      }
      else {
        help();
      }
    }
    else {
      if (filename) {
        help();
      }
      filename=argv[i];
    }
  }
  if (!filename) {
    help();
  }
  if (!flags) {
    flags=0xFFFFFFFF;
  }
}


int
main(int argc, char **argv)
{
  Lib3dsFile *f=0;

  parse_args(argc, argv);

  lib3ds_chunk_enable_dump(flags&LIB3DSDUMP_STRUCTURE, flags&LIB3DSDUMP_UNKNOWN);
  f=lib3ds_file_load(filename);
  if (!f) {
    fprintf(stderr, "***ERROR***\nLoading file %s failed\n", filename);
    exit(1);
  }

  if (flags&LIB3DSDUMP_MATERIALS) {
    printf("Dumping materials:\n");
    lib3ds_file_dump_materials(f);
    printf("\n");
  }
  if (flags&LIB3DSDUMP_TRIMESHES) {
    printf("Dumping meshes:\n");
    lib3ds_file_dump_meshes(f);
    printf("\n");
  }
  if (flags&LIB3DSDUMP_INSTANCES) {
    printf("Dumping instances:\n");
    lib3ds_file_dump_instances(f);
    printf("\n");
  }
  if (flags&LIB3DSDUMP_CAMERAS) {
    printf("Dumping cameras:\n");
    lib3ds_file_dump_cameras(f);
    printf("\n");
  }
  if (flags&LIB3DSDUMP_LIGHTS) {  
    printf("Dumping lights:\n");
    lib3ds_file_dump_lights(f);
    printf("\n");
  }
  if (flags&LIB3DSDUMP_NODES) {
    printf("Dumping node hierarchy:\n");
    lib3ds_file_dump_nodes(f);
    printf("\n");
  }
  if (output && (flags&LIB3DSDUMP_WRITE_3DS)) {
    if (!lib3ds_file_save(f, output)) {
      printf("***ERROR**** Writing %s\n", output);
    }
  }

  lib3ds_file_free(f);
  return(0);
}
