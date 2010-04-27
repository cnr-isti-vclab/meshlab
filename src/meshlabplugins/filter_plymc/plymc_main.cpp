/*
 *  plymc_main.cpp
 *  filter_plymc
 *
 *  Created by Paolo Cignoni on 10/23/09.
 *  Copyright 2009 ISTI - CNR. All rights reserved.
 *
 */

#include "plymc.h"
#include "simplemeshprovider.h"
#define _PLYMC_VER "4.0"

using namespace std;
using namespace vcg;



string MYbasename = "plymcout";
string VolumeBaseName;
string subtag=""; // la stringa da appendere al nome di file per generare i nomi di file relativi a vari sottopezzi
string alnfile;

/************ Command Line Parameters *******/

int saveMask=vcg::tri::io::Mask::IOM_ALL;
void usage()
{
  printf(
      "\nUsage:\n"
      "         plymc [options] filein.ply [filein.ply...]\n"
      "         plymc [options] filein.aln \n"
      "Options:  (no leading space before numeric values!) \n"
      " -oname  Set the base output name (default plymcout, without 'ply' extension)\n"
      " -vname  Enable the saving of the final volume with the specified filename\n"
      " -C##    Set numbers mesh that can be cached (default: 6)\n"
      " -c##    Set numbers of k cells (default: 10000)\n"
      " -V#     Set the required voxel size (override -c)\n"
      " -s...   Compute only a subvolume (specify 6 integers) \n"
      " -S...   Compute all the subvolumes of a partition (specify 3 int) \n"
      " -X...   Compute a range of the the subvolumes of a partition (specify 9 int)\n"
      " -M      Apply a 'safe' simplification step that removes only the unecessary triangles\n"
      " -w#     Set distance field Expansion factor in voxel (default 3)\n"
      " -W#     Set distance field Exp. as an absolute dist (override -w)\n"
      " -a#     Set angle threshold for distance field expansion (default 30)\n"
      " -f#     Set the fill threshold (default 12: all voxel having less \n"
      "         than 12 adjacent are not automaticall filled)\n"
      " -L#     Set Number of smoothing passes to be done after merging of all meshes\n"
      " -R#     Set Number of Refill passes to be done after merging of all meshes\n"
      " -l#     Make a single smoothing step after each expansion\n"
      " -G#     Disable Geodesic Quality\n"
      " -F#     Use per vertex quality defined in plyfile (geodesic is disabled)\n"
      " -O#     Set an Offset (<0!) threshold and build a double surface\n"
      " -q#     Set Quality threshold for smoothing. Only whose distance (in voxel)\n"
      "         is lower than the specified one are smoothed (default 3 voxel)\n"
      " -Q#     Same of above but expressed in absolute units.\n"
      " -p       use vertex splatting instead face rasterizing\n"
      " -d#     set <n> as verbose level (default 0)\n"
      " -D#     save <n> debug slices during processing\n"

      "\nNotes:\n\n"
      "The Quality threshold can be expressed in voxel unit or in absolute units.\n"
      "It represents the geodetic distance from the mesh border.\n"
      "I.e. -q3 means that all voxels that are within 3voxel from the mesh border\n"
      "are smoothed.\n\n"

      "A partition in the working volume is defined using 3 integers, that \n"
      "specify the subdivision along the three axis.\n"
      "To automatically compute ALL subvolumes of a given subdivision use '-S' \n"
      "-S 1 1 1 default no subdivision at all\n"
      "-S 2 2 2 compute all the octant of a octree-like subdivision\n"
      "-S 1 1 4 compute four Z-slices\n\n"

      "To work only on a SINGLE subvolume of the partition you have to specify \n"
      "six integers: the first three ints specify the subdivision along the\n"
      "three axis and the last three ones which subvolume you desire.\n"
      "the parameter to be used is '-s' \n"
      "-s 1 1 1 0 0 0 default no subdivision at all\n"
      "-s 1 1 3 0 0 1 make three Z-slices and take the middle one \n"
      "-s 2 2 2 1 1 1 the last octant in a octree-like subdivision\n\n"

      "To START FROM a specific subvolume of the partition you have to specify \n"
      "six integers: the first three ints specify the subdivision along the\n"
      "three axis and the last three ones which subvolume you want to start\n"
      "the process will go on using lexicographic order. Subvolumes are ordered\n"
      "by Z, then by Y, then by X\n"
      "The parameter to be used is '-K' \n"
      "-K 4 4 4 0 0 0 a partition of 64 blocks, starting from the first one\n"
      "-K 4 4 4 1 0 3 a partition of 64 blocks, starting from block 19 (index 1 0 3)\n\n"

      "To work only on a specific subvolume range of the partition you have \n"
      "to specify nine integers: the first three ints specify the subdivision\n"
      "along the three axis and, the next three which is the starting subvolume\n"
      "and the last three which is the last subvolume to be computed.\n"
      "the process will compute all blocks with x,y,z index included in the interval\n"
      "specified: Xstart<=X<=Xend Ystart<=Y<=Yend Zstart<=Z<=Zend\n"
      "-X 3 3 3 0 0 0 2 2 2 three subdivision on each axis, all subvolumes\n"
      "-X 2 2 2 1 0 0 1 1 1 three subdivision on each axis, only the 'right' part\n\n"
      );
  exit(-1);
}



int main(int argc, char *argv[])
{

  Histogram<float> h;
  tri::PlyMC<SMesh,SimpleMeshProvider<SMesh> > pmc;
  tri::PlyMC<SMesh,SimpleMeshProvider<SMesh> >::Parameter &p = pmc.p;


  // This line is required to be sure that the decimal separatore is ALWAYS the . and not the ,
  // see the comment at the beginning of the file
#ifdef WIN32
  setlocale(LC_ALL, "En_US");
#else
  std::setlocale(LC_ALL, "En_US");
#endif

  printf(   "\n                  PlyMC "_PLYMC_VER" ("__DATE__")\n"
	    "   Copyright 2002-2008 Visual Computing Group I.S.T.I. C.N.R.\n"
	    "             Paolo Cignoni (p.cignoni@isti.cnr.it)\n\n");
  //// Parameters
  int i=1;
  if(argc<2) usage();
  while(argv[i][0]=='-')
  {
    switch(argv[i][1])
    {
	case 'o' :  p.basename=argv[i]+2;printf("Setting Basename  to %s\n",MYbasename.c_str());break;
	case 'C' :  pmc.MP.setCacheSize(atoi(argv[i]+2));printf("Setting MaxSize of MeshCache to %i\n",atoi(argv[i]+2)); break;
	case 'c' :  p.NCell   =atoi(argv[i]+2);printf("Setting NCell  to %i\n",p.NCell); break;
	case 'v' :  p.SaveVolumeFlag=true; VolumeBaseName=argv[i]+2; printf("Saving Volume enabled: volume Basename  to %s\n",VolumeBaseName.c_str());break;
	case 'V' :  p.VoxSize =atof(argv[i]+2);printf("Setting VoxSize  to %f; overridden NCell\n",p.VoxSize);p.NCell=0;break;
	case 'w' :  p.WideNum =atoi(argv[i]+2);printf("Setting WideNum  to %i\n",p.WideNum);break;
	case 'W' :  p.WideSize=atof(argv[i]+2);printf("Setting WideSize to %f;overridden WideNum\n",p.WideSize);break;
	case 'L' :  p.SmoothNum =atoi(argv[i]+2);printf("Setting Laplacian SmoothNum to %i\n",p.SmoothNum);break;
	case 'R' :  p.RefillNum =atoi(argv[i]+2);printf("Setting Refilling Num to %i\n",p.RefillNum);break;
	case 'q' :  p.QualitySmoothVox=atof(argv[i]+2);printf("Setting QualitySmoothThr  to %f; \n",p.QualitySmoothVox);break;
    case 'Q' :  p.QualitySmoothAbs=atof(argv[i]+2);printf("Setting QualitySmoothAbsolute to %f; it will override the default %f voxel value\n",p.QualitySmoothAbs,p.QualitySmoothVox);break;
	case 'l' :  p.IntraSmoothFlag=true; printf("Setting Laplacian Smooth after expansion \n");break;
	case 'G' :  p.GeodesicQualityFlag=false; printf("Disabling Geodesic Quality\n");break;
	case 'F' :  p.PLYFileQualityFlag=true; p.GeodesicQualityFlag=false; printf("Enabling PlyFile (and disabling Geodesic) Quality\n");break;
	case 'f' :  p.FillThr=atoi(argv[i]+2);printf("Setting Fill Threshold to %i\n",p.FillThr);break;
	case 'a' :  p.ExpAngleDeg=atoi(argv[i]+2);printf("Setting Expanding Angle Threshold to %f Degree\n",p.ExpAngleDeg);break;
	case 'O' :  p.OffsetThr=atof(argv[i]+2);printf("Setting Offset Threshold to %f \n",p.OffsetThr);p.OffsetFlag=true;break;
	case 's' :
	  p.IDiv[0]=atoi(argv[++i]); p.IDiv[1]=atoi(argv[++i]); p.IDiv[2]=atoi(argv[++i]);
	  p.IPosS[0]=atoi(argv[++i]);p.IPosS[1]=atoi(argv[++i]);p.IPosS[2]=atoi(argv[++i]);
	  p.IPosE[0]=p.IPosS[0]; p.IPosE[1]=p.IPosS[1]; p.IPosE[2]=p.IPosS[2];
	  if((p.IPosS[0]>=p.IDiv[0]) || (p.IPosS[1]>=p.IDiv[1]) || (p.IPosS[2]>=p.IDiv[2]))
	  {
	    printf("the subvolume you have requested is invalid (out of bounds)");
	    exit(-1);
	  }
	  printf("Computing ONLY subvolume [%i,%i,%i] on a %ix%ix%i\n",p.IPosS[0],p.IPosS[1],p.IPosS[2],p.IDiv[0],p.IDiv[1],p.IDiv[2]);
	  break;
	case 'S' :
	  p.IDiv[0]=atoi(argv[++i]);p.IDiv[1]=atoi(argv[++i]);p.IDiv[2]=atoi(argv[++i]);
	  p.IPosS=Point3i(0,0,0);
	  p.IPosE[0]=p.IDiv[0]-1; p.IPosE[1]=p.IDiv[1]-1; p.IPosE[2]=p.IDiv[2]-1;
	  printf("Autocomputing ALL subvolumes on a %ix%ix%i\n",p.IDiv[0],p.IDiv[1],p.IDiv[2]);
	  break;
	case 'K' :
	  p.IDiv[0]=atoi(argv[++i]); p.IDiv[1]=atoi(argv[++i]);p.IDiv[2]=atoi(argv[++i]);
	  p.IPosB[0]=atoi(argv[++i]);p.IPosB[1]=atoi(argv[++i]);p.IPosB[2]=atoi(argv[++i]);
	  p.IPosS=Point3i(0,0,0);
	  p.IPosE[0]=p.IDiv[0]-1; p.IPosE[1]=p.IDiv[1]-1; p.IPosE[2]=p.IDiv[2]-1;
	  if((p.IPosB[0]>=p.IDiv[0]) || (p.IPosB[1]>=p.IDiv[1]) || (p.IPosB[2]>=p.IDiv[2]))
	  {
	    printf("the start subvolume you have requested is invalid (out of bounds)");
	    exit(-1);
	  }
	  printf("Autocomputing ALL subvolumes FROM [%i,%i,%i] on a %ix%ix%i\n",p.IPosB[0],p.IPosB[1],p.IPosB[2],p.IDiv[0],p.IDiv[1],p.IDiv[2]);
	  break;
	case 'X' :
	  p.IDiv[0]=atoi(argv[++i]); p.IDiv[1]=atoi(argv[++i]);p.IDiv[2]=atoi(argv[++i]);
	  p.IPosS[0]=atoi(argv[++i]);p.IPosS[1]=atoi(argv[++i]);p.IPosS[2]=atoi(argv[++i]);
	  p.IPosE[0]=atoi(argv[++i]);p.IPosE[1]=atoi(argv[++i]);p.IPosE[2]=atoi(argv[++i]);
	  // test if the interval is ok
	  int Istart,Iend;
	  Istart = p.IPosS[2] + (p.IPosS[1]*p.IDiv[2]) + (p.IPosS[0]*p.IDiv[2]*p.IDiv[1]);
	  Iend   = p.IPosE[2] + (p.IPosE[1]*p.IDiv[2]) + (p.IPosE[0]*p.IDiv[2]*p.IDiv[1]);
	  if((Iend-Istart)<=0)
	  {
	    printf("the range you have requested is invalid (reversed or empty)");
	    exit(-1);
	  }
	  if((p.IPosS[0]>=p.IDiv[0]) || (p.IPosS[1]>=p.IDiv[1]) || (p.IPosS[2]>=p.IDiv[2]) ||
	     (p.IPosE[0]>=p.IDiv[0]) || (p.IPosE[1]>=p.IDiv[1]) || (p.IPosE[2]>=p.IDiv[2]))
	  {
	    printf("the subvolume you have requested is invalid (out of bounds)");
	    exit(-1);
	  }
	  printf("Autocomputing subvolumes FROM [%i,%i,%i] TO [%i,%i,%i] on a %ix%ix%i\n",p.IPosS[0],p.IPosS[1],p.IPosS[2],p.IPosE[0],p.IPosE[1],p.IPosE[2],p.IDiv[0],p.IDiv[1],p.IDiv[2]);
	  break;
	  //        case 'B' :	p.SafeBorder =atoi(argv[i]+2);printf("Setting SafeBorder among blocks to %i*%i (default 1)\n",p.SafeBorder,Volume<Voxelf>::BLOCKSIDE());break;
	case 'p' :	p.VertSplatFlag =true; printf("Enabling VertexSplatting instead of face rasterization\n");break;
	case 'd' : p.VerboseLevel=atoi(argv[i]+2);printf("Enabling VerboseLevel= %i )\n",p.VerboseLevel);break;
  case 'D' : p.VerboseLevel=1; p.SliceNum=atoi(argv[i]+2);printf("Enabling Debug Volume saving of %i slices (VerboseLevel=1)\n",p.SliceNum);break;
	case 'M' :	p.SimplificationFlag =true; printf("Enabling PostReconstruction simplification\n"); break;
		default : {printf("Error unable to parse option '%s'\n",argv[i]); exit(0);}
    }
    ++i;
  }



  Matrix44f Identity;
  Identity.SetIdentity();
  string alnfile;
  while(i<argc)
  {
    if(strcmp(strrchr(argv[i],'.'),".aln")==0)
      pmc.MP.openALN(argv[i]);
    else
      pmc.MP.AddSingleMesh(argv[i]);
    ++i;
  }

  if(pmc.MP.size()==0) usage();
  printf("End Parsing\n\n");
  int t1=clock();
  pmc.Process();
  int t2=clock();


  return 0;
}
