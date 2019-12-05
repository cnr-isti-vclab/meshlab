/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <QtGlobal>
#include "Guess.h"
#include <vcg/math/gen_normal.h>

using namespace std;

namespace vcg
{
namespace tri
{




// Fill movVert with the transformed version of movVertBase;
void Guess::ApplyTransformation(const Point3f &BaseTranslation, const Matrix44f &BaseRot,
                                                    std::vector< Point3f > &movVert,
                                                    std::vector< Point3f > &movNorm,
                                                    Box3f &movBox
                                                    ) const

/*
    Very Low level function
    Given a Occupancy grid initialized with the Fix mesh.
    We search the best translation matrix T such that
    T * R * Mov has the greatest overlap with Fix
*/

int Guess::SearchBestTranslation(	GridStaticObj<bool,ScalarType> &U,
                                        const Matrix44x &BaseRot,
                                        const int range,
                                        Point3x &StartTrans,
                                        Point3x &BestTrans  // miglior vettore di spostamento
                                        )



















#if 0
/*
Funzione che allinea due mesh a caso.

Parametri fondamentali

- Spazio di ricerca della searchtranslate
Ragionevole range=8 step =2 nel Second step
e range=8,step =1 nel third step di raffinamento

- Parametri allineamento
fondamentale mettere una mindist non molto alta
e disabilitare il reducefactor

Un po' difficile capire quando l'allinamento e' venuto bene
*/
void Guess::ComputeTrans(std::vector<Result> &rv)
{
    int StartTm=clock();
    std::vector<Result> rvt;
    Point3d BestTransV;

  // Second Step: Compute for each possible Orientation the Best Translation
    Matrix44d TrBase;
  size_t i;
    for(i=0;i<MV.size();++i)
    {
        printf("Testing Direction %3i on %li    \r",i,MV.size());
        int maxfnd=SearchTranslate(2,MV[i],pp.Range,pp.Step,BestTransV,false);
        rvt.push_back(Result(MV[i],maxfnd,i,BestTransV));
    }
    printf("Completed Translation Search; Tested %li orientation in %lu ms\n",MV.size(),clock()-StartTm);
    sort(rvt.begin(),rvt.end());
    reverse(rvt.begin(),rvt.end());
    for(i=0;i<15;++i)
      printf("MV[%4i] Err %5.0f\n",rvt[i].id,rvt[i].err);
  // Third Step: For each one of the best 50 orientation:
    //   - try to improve the translation
    int ImpCnt=0;
    Matrix44d StartM;

    for(i=0;i<rvt.size();++i)
        {
            printf("Improving Result %3i on %i    \r",i,int(rvt.size()));
            GetM(rvt[i],StartM);
            int fnd=SearchTranslate(1,StartM,4,1,BestTransV,false);
            rvt[i]=Result(rvt[i].m,double(fnd),rvt[i].id, rvt[i].BestTr+BestTransV);
        }
    printf("Completed \n");
    sort(rvt.begin(),rvt.end());
    reverse(rvt.begin(),rvt.end());
    for(i=0;i<15;++i)
      printf("MV[%4i] Err %5.0f\n",rvt[i].id,rvt[i].err);

    for(i=0;i<rvt.size();++i)
        {
            printf("Improving Result %3i on %i    \r",i,int(rvt.size()));
            GetM(rvt[i],StartM);
            int fnd=SearchTranslate(0,StartM,4,1,BestTransV,false);
            rv.push_back(Result(rvt[i].m,fnd,rvt[i].id, rvt[i].BestTr+BestTransV));
        }

    printf("Completed Improving Search; improved %i orientation in %lu ms\n",ImpCnt,clock()-StartTm);
    sort(rv.begin(),rv.end());
    reverse(rv.begin(),rv.end());
    for(i=0;i<15;++i)
      printf("MV[%4i] Err %5.0f\n",rv[i].id,rv[i].err);

}
void Guess::ComputeAlign(std::vector<Result> &rv)
{
    FILE * lfp=fopen("Guesslog.txt","w");
        //   - try to align and store the best results
    Point3d BestTransV;
  int StartTm=clock();
    AlignPair::Result ar;
    vector<AlignPair::Result> Alns;
    Matrix44d BestTransM;
    int cnt=0;
//	UGrid< AlignPair::A2Mesh::face_container > U;
    AlignPair::A2Grid U;
    aa.InitFix(aa.fix,aa.ap,U);
    for(int i=0;i<20 && cnt < 4;++i)
        {
            int fnd;
            GetM(rv[i],BaseRot);
            do
            {
                fnd=SearchTranslate(0,BaseRot,8,1,BestTransV,false);
                BestTransM.SetTranslate(BestTransV);
                BaseRot=BaseRot*BestTransM;
            }
            while(BestTransV != Point3d(0,0,0));
      if(fnd>rv[i].err) printf("improved solution %i from %i to %i\n",i,int(rv[i].err),fnd);

            //AlignStat as;
            aa.ap = ap;
            aa.ap.MaxIterNum = 100;
            aa.ap.MinDistAbs    = aa.Abs2Perc(0.05f,aa.fix->bbox);
            aa.ap.SampleNum = 2000;
    //		aa.ap.TrgDist    = TrgDist;
        //	aa.ap.PointNum   = PointNum;
            //aa.ap.MaxAngle   = ToRad(MaxAngle);
            aa.ap.MatchMode=AlignPair::Param::MMRigid;
            aa.ap.EndStepNum = 8;
            aa.ap.ReduceFactor = .95;

            Matrix44d trinc=BaseRot;
            Matrix44d TrRes;

            aa.InitMov(movvert,movnorm,movbox,BaseRot);
            //aa.Align(U,  movvert, movnorm, movbox,trinc,ar);
            aa.Align(U,trinc,ar.Tr,
                                ar.Pfix, ar.Nfix,
                                ar.Pmov, ar.Nmov,
                                ar.H, ar.as);
            ar.err=ar.as.LastPcl50();
//			printf("Ornt %2i n %3i (%6.3f %6.3f %6.3f) angle %2i %s Err:%8.5f %4i  \n",
//				i, rv[i].nind,NV[rv[i].nind][0],NV[rv[i].nind][1],NV[rv[i].nind][2],rv[i].angl,
//				AlignPair::ErrorMsg(aa.status),
//				ar.err,
//				ar.as.TotTime());

                ar.as.Dump(lfp);

            if(aa.status==AlignPair::SUCCESS ){
                if(ar.err<1.0){
                    Alns.push_back(ar);
                    printf("Alignment Succeeded!:\n");
                    cnt++;
                //as.Dump(stdout);
                //printf("\nInputM\n");print(BaseRot);
                //printf("\nResult\n");print(TrRes);
                //BaseRot=TrRes;
                }
            }
        }
  sort(Alns.begin(),Alns.end());
    printf("Completed Alignment in %lu ms, found %li good alignments\n",clock()-StartTm,Alns.size());
    for(int i=0;i<Alns.size();++i)
    {
            printf("Solution %i %f\n",i,Alns[i].err);
//			print(Alns[i].Tr);
    }
    BaseRot=Alns[0].Tr;
    fclose(lfp);
}


void Guess::GetM(Result &rr, Matrix44d &Res)
{
    Matrix44d BestTranM;
    BestTranM.SetTranslate(rr.BestTr);
    Res=rr.m*BestTranM;
}


int Guess::SearchTranslate2(const int UGLev,
                                        const Matrix44d &BaseRot,
                                        const int range,
                                        const int step,
                                        Point3d &BestTransV,  // miglior vettore di spostamento
                                        bool Verbose)
{
//	const int wide1=(range*2+1);
  std::vector< Point3d > movvert;
    std::vector< Point3d > movnorm;
    Box3d movbox;
    //SGrid< bool,float> &U=u[UGLev];
    GridStaticObj< bool,float> &U=u[UGLev];
    //SGrid<int> RG;
    GridStaticObj<int> RG;
    RG.Create(Point3i((range*2+1),(range*2+1),(range*2+1)),0);

    int t0=clock();
  aa.InitMov(movvert,movnorm,movbox,BaseRot);
    Point3i ip;
    int i,ii,jj,kk;

    Point3i b,e;
    for(i=0;i<movvert.size();++i)
        {
            Point3f tp; tp.Import(movvert[i]);
            if(U.bbox.IsIn(tp)){
                U.PToIP(tp,ip);
        b=ip+Point3i(-range,-range,-range);
                e=ip+Point3i( range, range, range);
                while(b[0]<0)         b[0]+=step;
                while(e[0]>=U.siz[0]) e[0]-=step;
                while(b[1]<0)         b[1]+=step;
                while(e[1]>=U.siz[1]) e[1]-=step;
                while(b[2]<0)         b[2]+=step;
                while(e[2]>=U.siz[2]) e[2]-=step;

                for(ii=b[0];ii<=e[0];ii+=step)
                {
                    for(jj=b[1];jj<=e[1];jj+=step)
                    {
                        for(kk=b[2];kk<=e[2];kk+=step)
                        {
                            if(U.Grid(ii,jj,kk))
                                        ++RG.Grid(ii-b[0]+range,jj-b[1]+range,kk-b[2]+range);
                            assert(ii >=0 && ii < U.siz[0]);
                            assert(jj >=0 && jj < U.siz[1]);
                            assert(kk >=0 && kk < U.siz[2]);
                        }
                    }
                }
            }
        }
    int maxfnd=0;
    Point3i BestI;
    for(ii=-range;ii<=range;ii+=step)
        for(jj=-range;jj<=range;jj+=step)
            for(kk=-range;kk<=range;kk+=step)
            {

                if(RG.Grid(ii+range,jj+range,kk+range)>maxfnd)
                {
                    BestI=Point3i(ii,jj,kk);
                    BestTransV=Point3d(-ii*U.voxel[0], -jj*U.voxel[1], -kk*U.voxel[2]);
                    maxfnd=RG.Grid(ii+range,jj+range,kk+range);
                }
                //printf("Found %i su %i in %i\n",test[testcnt],movvert.size(),t1-t0);
            }

    int t1=clock();
    if(Verbose) printf("BestTransl (%4i in %3ims) is %8.4f %8.4f %8.4f (%3i %3i %3i)\n",maxfnd,t1-t0,BestTransV[0],BestTransV[1],BestTransV[2],BestI[0],BestI[1],BestI[2]);
    return maxfnd;
}


/* Check che non ci siano troppe matrici uguali
    Hist H;
    for(i=0;i<MV.size();++i)
        {
            for(j=i+1;j<MV.size();++j)
            {
                if(MV[i]==MV[j]) printf("Warning %i and %i are equal\n",i,j);
                Matrix44d diff=MV[i]-MV[j];
                double maxdiff=0;
                for(int ii=0;ii<4;++ii)
                    for(int jj=0;jj<4;++jj)
                            maxdiff+=(diff[ii][jj]*diff[ii][jj]);
                if(maxdiff<.1) printf("Warning %i and %i are similar\n",i,j);
            }
        }
*/


#endif
} // end namespace tri
} // end namespace vcg

