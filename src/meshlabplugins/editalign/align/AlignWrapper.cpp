#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include<time.h>
#include <vector>
#include <bitset>
#include <vector>
#include <set>
#include <bitset>
#include <algorithm>

#include <vcg/math/point_matching.h>
#include "AlignWrapper.h"

#include<vcg/complex/trimesh/update/normal.h>
#include<vcg/complex/trimesh/update/bounding.h>
#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/export_ply.h>


using namespace vcg;
using namespace std;
using namespace vcg::tri;
using namespace vcg::face;


bool AlignWrapper::MeshHandle::ImportFromPly(const char *plyname)
{
return true;
}

bool AlignWrapper::OpenAln(const char *alnfilename)
{
  return true;
}

void AlignWrapper::FileMatrixWrite(FILE *fp, const Matrix44d &Tr)
{
		fprintf(fp,"%6.3lf %6.3lf %6.3lf %6.3lf \n",(Tr[0][0]),(Tr[0][1]),(Tr[0][2]),(Tr[0][3]));
		fprintf(fp,"%6.3lf %6.3lf %6.3lf %6.3lf \n",(Tr[1][0]),(Tr[1][1]),(Tr[1][2]),(Tr[1][3]));
		fprintf(fp,"%6.3lf %6.3lf %6.3lf %6.3lf \n",(Tr[2][0]),(Tr[2][1]),(Tr[2][2]),(Tr[2][3]));
		fprintf(fp,"%6.3lf %6.3lf %6.3lf %6.3lf \n",(Tr[3][0]),(Tr[3][1]),(Tr[3][2]),(Tr[3][3]));
}

int AlignWrapper::ParseAln(const char  *alnfile, vector<string> &names, vector<Matrix44d> &LTr, bool RigidFlag)
{
	printf("Parsing aln file %s\n",alnfile);
	FILE *fp=fopen(alnfile,"r");
	if(!fp)
	{
	 printf("unable to open file %s\n",alnfile);
	 return 0;
	}

	int mn;
	fscanf(fp,"%i\n",&mn);
	printf("Aln file contain %i meshes\n",mn);
	names.resize(mn);
	LTr.resize(mn);
  Matrix44d TrTmp;
	Matrix44d Res;
	Point3d Sc,Sh,R,T;
	Quaterniond qq; Point3d trd;
	AlignPair::A2Mesh tm;
	vector<Point3d> TrVert;
	vector<Point3d> OrVert;

	char buf[1025];
	for(int i=0;i<mn;++i)
	{
		fgets(buf,1024,fp);
		*strchr(buf,'\n')=0;
		names[i]=buf;
		printf("mesh %i : '%s'\n",i,names[i].c_str());
		fgets(buf,1024,fp);
		*strchr(buf,'\n')=0;
		if(buf[0]!='#') 
		{
			printf("error in parsing aln file: I was expecting a comment; read '%s'\n",buf);
			fclose(fp);
			return false;
		}

		fscanf(fp,"%lf %lf %lf %lf \n",&(LTr[i][0][0]),&(LTr[i][0][1]),&(LTr[i][0][2]),&(LTr[i][0][3]));
		fscanf(fp,"%lf %lf %lf %lf \n",&(LTr[i][1][0]),&(LTr[i][1][1]),&(LTr[i][1][2]),&(LTr[i][1][3]));
		fscanf(fp,"%lf %lf %lf %lf \n",&(LTr[i][2][0]),&(LTr[i][2][1]),&(LTr[i][2][2]),&(LTr[i][2][3]));
		fscanf(fp,"%lf %lf %lf %lf \n",&(LTr[i][3][0]),&(LTr[i][3][1]),&(LTr[i][3][2]),&(LTr[i][3][3]));
		//Transpose(LTr[i]);
		double DefThr = 1e-5;
		if(RigidFlag){
				TrTmp=LTr[i];
				if(! Decompose(TrTmp,Sc,Sh,R,T)) 
        {
          printf("Warning Failed to decompose matrix!\n");
          return false;
        }
				if(fabs(1.0-Sc[0])>DefThr || fabs(1.0-Sc[2])>DefThr || fabs(1.0-Sc[1])>DefThr ) 
					printf("Warning Input aln transf. for mesh %i has a scale of %6.3f %6.3f %6.3f\n",i,Sc[0],Sc[1],Sc[2]);
        if(fabs(Sh[0])>DefThr || fabs(Sh[2])>DefThr || fabs(Sh[1])>DefThr ) 
        {
					printf("Warning Input aln transf. for mesh %i has a shear of %6.3f %6.3f %6.3f\n",i,Sh[0],Sh[1],Sh[2]);
					printf("Trying to make %i rigid\n",i);
					FileMatrixWrite(stdout, LTr[i]);
					tm.Import(names[i].c_str());
					TrVert.clear();
					OrVert.clear();
					AlignPair::A2Mesh::VertexIterator vi;
					for(vi=tm.vert.begin();vi!=tm.vert.end();++vi)if(!(*vi).IsD())
					{
						OrVert.push_back((*vi).P());
						TrVert.push_back(LTr[i]*OrVert.back());
					}
					// Si calcola la matrice rigida che porta i punti originali (passati come mov)
					// su quelli trasformati come trasf non rigida (passati quindi come fix)
					PointMatching<double>::ComputeRigidMatchMatrix(Res,TrVert,OrVert,qq,trd);
          double avg=0;
					for(int jj=0;jj<OrVert.size();++jj)
						avg+=Distance(TrVert[jj], Res*OrVert[jj]);
					avg/=OrVert.size();
					printf("Recovered rigid transformation has an avg error of %f\n",avg);

					LTr[i]=Res;
					TrTmp=LTr[i];
					Decompose(TrTmp,Sc,Sh,R,T);
					if(fabs(1.0-Sc[0])>DefThr || fabs(1.0-Sc[2])>DefThr || fabs(1.0-Sc[1])>DefThr ) 
						printf("Catastrofic Error Input aln transf. for mesh %i has a scale of %6.3f %6.3f %6.3f\n",i,Sc[0],Sc[1],Sc[2]);
					if(fabs(Sh[0])>DefThr || fabs(Sh[2])>DefThr || fabs(Sh[1])>DefThr ) 
						printf("Catastrofic Error Input aln transf. for mesh %i has a shear of %6.3f %6.3f %6.3f\n",i,Sh[0],Sh[1],Sh[2]);
        }
			}
	}
	fclose(fp);
	return mn;
}