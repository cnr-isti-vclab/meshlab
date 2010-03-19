/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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

#include "AlignPair.h"
#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export_ply.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/edges.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/closest.h>

#include <vcg/math/random_generator.h>
#include <vcg/math/gen_normal.h>
#include <vcg/math/point_matching.h>
using namespace vcg;
using namespace std;

bool AlignPair::A2Mesh::Import(const char *filename, Matrix44d &Tr, bool hasborderflag)
{
	int err = tri::io::Importer<A2Mesh>::Open(*this,filename);
  if(err) {
      printf("Error in reading %s: '%s'\n",filename,tri::io::Importer<A2Mesh>::ErrorMsg(err));
      exit(-1);
    }
  printf("read mesh `%s'\n", filename);		  
return Init(Tr,hasborderflag);
}

bool AlignPair::A2Mesh::Init(const Matrix44d &Tr,bool hasborderflag)
{
	Matrix44d Idn; Idn.SetIdentity();
	//int t1=clock();
  tri::Clean<A2Mesh>::RemoveUnreferencedVertex(*this);
  //int t2=clock();
	if(Tr!=Idn) tri::UpdatePosition<A2Mesh>::Matrix(*this,Tr); 
	//int t3=clock();  
  tri::UpdateNormals<A2Mesh>::PerVertexNormalizedPerFaceNormalized(*this); 
	//int t4=clock();
	if(!hasborderflag) 		
    tri::UpdateFlags<A2Mesh>::FaceBorderFromNone(*this);
    // ComputeBorderFlag();
	//int t5=clock();
		tri::UpdateBounding<A2Mesh>::Box(*this);
  //int t6=clock();
		tri::UpdateEdges<A2Mesh>::Set(*this);
	//int t7=clock();

/*	printf("LoadPly                       %6i\n",t1-t0);
	printf("RemoveUnrefVert               %6i\n",t2-t1);
	printf("Apply                         %6i\n",t3-t2);
	printf("ComputeNormalizedVertexNormal %6i\n",t4-t3);
	printf("ComputeBorderFlag             %6i\n",t5-t4);
	printf("ComputeBBox                   %6i\n",t6-t5);
	printf("ComputeE                      %6i\n",t7-t6);
*/
	return true;
}


void AlignPair::Stat::clear()
{
	I.clear();	
	StartTime=0;			
	MovVertNum=0;
	FixVertNum=0;
	FixFaceNum=0;	
}

// Restituisce true se nelle ultime <lastiter> iterazioni non e' diminuito 
// l'errore
bool AlignPair::Stat::Stable(int lastiter)
{
	if(I.empty()) return false;
	 int parag = I.size()-lastiter;

  if(parag<0) parag=0; 
	if( I.back().pcl50 < I[parag].pcl50 ) return false; // se siamo diminuiti non e' stabile
	
	return true;

}


void AlignPair::Stat::Dump(FILE *fp)
{
	if(I.size()==0) { 
		fprintf(fp,"Empty AlignPair::Stat\n");
		return;
	}
	fprintf(fp,"Final Err %8.5f In %i iterations Total Time %ims\n",LastPcl50(),(int)I.size(),TotTime());
  fprintf(fp,"Mindist  Lo     Med   Hi    Avg  RMS   StdDev   Time Tested Used  Dist Bord Angl \n");
	for(unsigned int qi=0;qi<I.size();++qi)
			fprintf(fp,"%5.2f (%5.2f:%6.3f:%5.2f)(%5.3f:%5.2f:%6.3f)%4ims %5i %5i %4i+%4i+%4i\n",
				I[qi].MinDistAbs,
				I[qi].pcllo, I[qi].pcl50, I[qi].pclhi,
				I[qi].AVG, I[qi].RMS, I[qi].StdDev ,
				IterTime(qi),
			  I[qi].SampleTested,I[qi].SampleUsed,I[qi].DistanceDiscarded,I[qi].BorderDiscarded,I[qi].AngleDiscarded);
}

// Scrive una tabella con tutti i valori
void AlignPair::Stat::HTMLDump(FILE *fp)
{
	fprintf(fp,"Final Err %8.5f In %i iterations Total Time %ims\n",LastPcl50(),(int)I.size(),TotTime());
	fprintf(fp,"<table border>\n");
  fprintf(fp,"<tr> <th>Mindist</th><th>  Lo </th><th>    Med </th><th>  Hi </th><th>   Avg  </th><th> RMS </th><th>  StdDev  </th><th> Time </th><th> Tested </th><th> Used </th><th> Dist </th><th> Bord </th><th> Angl \n");
	for(unsigned int qi=0;qi<I.size();++qi)
			fprintf(fp,"<tr> <td> %8.5f </td><td align=\"right\"> %8.5f </td><td align=\"right\"> %9.6f </td><td align=\"right\"> %8.5f </td><td align=\"right\"> %5.3f </td><td align=\"right\"> %8.5f </td><td align=\"right\"> %9.6f </td><td align=\"right\"> %4ims </td><td align=\"right\"> %5i </td><td align=\"right\"> %5i </td><td align=\"right\"> %4i </td><td align=\"right\"> %4i </td><td align=\"right\">%4i </td><td align=\"right\"></tr>\n",
					I[qi].MinDistAbs,
				I[qi].pcllo, I[qi].pcl50, I[qi].pclhi,
				I[qi].AVG, I[qi].RMS, I[qi].StdDev ,
				IterTime(qi),
			  I[qi].SampleTested,I[qi].SampleUsed,I[qi].DistanceDiscarded,I[qi].BorderDiscarded,I[qi].AngleDiscarded);
	fprintf(fp,"</table>\n");
}



/*

Chiamata dopo ogni iterazione dell ICP per scartare le coppie di punti
secondo i due valori di filtraggio in percentile PassHi e PassLo
E' l'unica funzione ad usare PointNum e MinPointNum;

*/
bool AlignPair::ChoosePoints( 	vector<Point3d> &Ps,		// vertici corrispondenti su src (rossi)
																vector<Point3d> &Ns, 		// normali corrispondenti su src (rossi)
																vector<Point3d> &Pt,		// vertici scelti su trg (verdi) 
																vector<Point3d> &OPt,		// vertici scelti su trg (verdi) 
																double PassLo,
																double PassHi,
																Histogramf &H)
{
	const int N = ap.MaxPointNum;
  
	double newmind = H.Percentile(PassLo);
	double newmaxd = H.Percentile(PassHi);
	//printf("%5.1f of the pairs has a distance less than %g and greater than %g (0..%g) avg %g\n",	Perc*100,newmind,newmaxd,H.maxv,H.Percentile(.5));
	int sz = Ps.size();
	int fnd=0;
	int lastgood=sz-1;
  math::SubtractiveRingRNG myrnd;
	while(fnd<N && fnd<lastgood)
	{
		int index = fnd+myrnd.generate(lastgood-fnd);
		double dd=Distance(Ps[index],Pt[index]);
		if(dd<newmaxd && dd>=newmind)
		{
			swap(Ps[index],Ps[fnd]);
			swap(Ns[index],Ns[fnd]);
			swap(Pt[index],Pt[fnd]);
			swap(OPt[index],OPt[fnd]);
			++fnd;
		}
		else
		{
			swap(Ps[index],Ps[lastgood]);
			swap(Ns[index],Ns[lastgood]);
			swap(Pt[index],Pt[lastgood]);
			swap(OPt[index],OPt[lastgood]);
			lastgood--;
		}
	}
	Ps.resize(fnd);
	Ns.resize(fnd);
	Pt.resize(fnd);
	OPt.resize(fnd);
	printf("Scelte %i coppie tra le %i iniziali, scartate quelle con dist > %f\n",fnd,sz,newmaxd);
	
	if( (int)Ps.size()<ap.MinPointNum )		{
		printf("Troppi pochi punti!\n");
		Ps.clear();
		Ns.clear();
		Pt.clear();
		OPt.clear();
		return false;
	}
	return true;
}

/*
Funzione chiamata dalla 
Riempie i vettori <MovVert> e <MovNorm> con i coordinate e normali presi dal vettore di vertici mov 
della mesh da muovere trasformata secondo la matrice <In>
Calcola anche il nuovo bounding box di tali vertici trasformati.
*/
bool AlignPair::InitMov(
		vector< Point3d > &MovVert,
		vector< Point3d > &MovNorm,
		Box3d &trgbox,
		const Matrix44d &in	)				// trasformazione Iniziale (che porta i punti di trg su src)
{
	Point3d pp,nn;

	MovVert.clear();
	MovNorm.clear();
	trgbox.SetNull();

	A2Mesh::VertexIterator vi;
	for(vi=mov->begin(); vi!=mov->end(); vi++) {
		pp=in*(*vi).P();
		nn=in*Point3d((*vi).P()+(*vi).N())-pp;
		nn.Normalize();
		MovVert.push_back(pp);
		MovNorm.push_back(nn);
		trgbox.Add(pp);
	}
	return true;
}


bool AlignPair::InitFix(AlignPair::A2Mesh *fm,
												AlignPair::Param &pp,
												A2Grid &u,
												int PreferredGridSize)
{
  tri::InitFaceIMark(*fm);

	Box3d bb2=fm->bbox;
//	double MinDist= fm->bbox.Diag()*pp.MinDistPerc;
	double MinDist= pp.MinDistAbs*1.1;
	//gonfio della distanza utente il BBox della seconda mesh
	bb2.Offset(Point3d(MinDist,MinDist,MinDist));
	
	u.SetBBox(bb2);

	//Inserisco la src nella griglia
	if(PreferredGridSize==0) PreferredGridSize=fm->face.size()*pp.UGExpansionFactor;
	u.Set(fm->face.begin(), fm->face.end());//, PreferredGridSize);
	printf("UG %i %i %i\n",u.siz[0],u.siz[1],u.siz[2]);
	return true;
}
/*
Allinea due mesh;
Assume che:
la uniform grid sia gia' inizializzata con la mesh fix


*/

bool AlignPair::Align(
						A2Grid &u,
  		const	Matrix44d &in,					// trasformazione Iniziale (che porta i punti di mov su fix)
						Matrix44d &out,					// trasformazione calcolata
						vector<Point3d> &Pfix,		// vertici corrispondenti su src (rossi)
						vector<Point3d> &Nfix, 		// normali corrispondenti su src (rossi)
						vector<Point3d> &OPmov,		// vertici scelti su trg (verdi) prima della trasformazione in ingresso (Original Point Target)
						vector<Point3d> &ONmov, 		// normali scelti su trg (verdi)
						Histogramf &H,
						AlignPair::Stat &as)
{	
	vector<char> good;    // vettore per marcare i movvert che sicuramente non si devono usare 
	                      // ogni volta che un vertice si trova a distanza oltre max dist viene incrementato il suo contatore;
												// i movvert che sono stati scartati piu' di MaxCntDist volte non si guardano piu';
	const int MaxCntDist=3;
	vector< Point3d > movvert;
	vector< Point3d > movnorm;
	vector<Point3d> Pmov; // vertici scelti dopo la trasf iniziale
	status=SUCCESS;
	int tt0=clock();  

	out=in;
	
	int i;

	double CosAngleThr=cos(ap.MaxAngleRad);
//	double StartMinDist=fix->bbox.Diag()*ap.MinDistPerc;;
	double StartMinDist=ap.MinDistAbs;
	//double TrgDistAbs=fix->bbox.Diag()*ap.TrgDistPerc;;
	int tt1=clock();  
	int ttsearch=0;
	int ttleast=0;
	int nc=0;
	as.clear();
	as.StartTime=clock();
			
	good.resize(mov->size(),0);
	/*############inizio ciclo################*/
	do 
	{	
		Stat::IterInfo ii;
		Box3d movbox;
		InitMov(movvert,movnorm,movbox,out);

		H.SetRange(0,StartMinDist,512,2.5);
		Pfix.clear();
		Nfix.clear();
		Pmov.clear();
		OPmov.clear();
		ONmov.clear();
		int tts0=clock();
		ii.MinDistAbs=StartMinDist;
    int LocSampleNum=min(ap.SampleNum,int(movvert.size()));
		for(i=0;i<LocSampleNum;++i)
				if( good[i] < MaxCntDist ) 			
					if(! u.bbox.IsIn(movvert[i]) ) 
						good[i]=MaxCntDist+1;
					else{
						A2Mesh::FacePointer f=0;
						double error=StartMinDist;
						Point3d n(0,0,0),q,ip;
            double maxd= std::numeric_limits<double>::max();
           // f=vcg::tri::GetClosestFace<vcg::AlignPair::A2Mesh, vcg::AlignPair::A2Grid >(*fix, u, movvert[i], maxd, error, q);
					  f=vcg::tri::GetClosestFace<vcg::AlignPair::A2Mesh, vcg::AlignPair::A2Grid >(*fix, u, movvert[i], maxd, error, q,n,ip);
            //tri::Closest(*fix, movvert[i], u, error, n, q, f, ip);
						  //MinDistPoint(*fix, movvert[i], u, error, n, q ,f, ip);  
						ii.SampleTested++;
						if(error>=StartMinDist) {
							ii.DistanceDiscarded++;
							++good[i];
						}
						else
							if(movnorm[i].dot(f->N()) < CosAngleThr)
								ii.AngleDiscarded++;
							else	if(	(ip[0]==0 && f->IsB(1)) || 
												(ip[1]==0 && f->IsB(2)) || 
												(ip[2]==0 && f->IsB(0))   )
											ii.BorderDiscarded++;
										else
											{
												Pmov.push_back(movvert[i]);
												OPmov.push_back((*mov)[i].P());
												ONmov.push_back((*mov)[i].N()); 
												Nfix.push_back( n );
												Pfix.push_back( q );
												H.Add(float(error));
												ii.SampleUsed++;
											}
			} // End of point pair search

    int tts1=clock();
		//printf("Found %d pairs\n",(int)Pfix.size());
		//if(!ChoosePoints(Pfix,Nfix,Pmov,OPmov,ap.PassLoFilter,ap.PassHiFilter,H))		
    if(int(Pfix.size())<ap.MinPointNum)
		{
			status = TOO_FEW_POINTS;
			ii.Time=clock();
			as.I.push_back(ii);
			return false;
		}
		bool res;
    Matrix44d newout;
		switch(ap.MatchMode) {
			case AlignPair::Param::MMClassic : res=PointMatching<double>::ComputeMatchMatrix(newout,Pfix,Nfix,OPmov);   break;
      case AlignPair::Param::MMRigid   : res=PointMatching<double>::ComputeRigidMatchMatrix(newout,Pfix,OPmov);   break;
  //    case AlignPair::Param::MMFast    : res=PointMatching::ComputeMatchMatrix2(out,Pfix,Nfix,OPmov);  break;
			default :
						status = UNKNOWN_MODE;
						ii.Time=clock();
						as.I.push_back(ii);
						return false;
		}
		
		if(!res)		{ 
			status = LSQ_DIVERGE;
			ii.Time=clock();
			as.I.push_back(ii);
			return false;
		}
			
    double sum_before=0;
    double sum_after=0;
    for(unsigned int iii=0;iii<Pfix.size();++iii)
    {
      sum_before+=Distance(Pfix[iii], out*OPmov[iii]);
      sum_after+=Distance(Pfix[iii], newout*OPmov[iii]);
    }
    //printf("Distance %f -> %f\n",sum_before/double(Pfix.size()),sum_after/double(Pfix.size()) ) ;
   
		// le passate successive utilizzano quindi come trasformazione iniziale questa appena trovata.
		// Nei prossimi cicli si parte da questa matrice come iniziale.
	  out=newout;

		//printf("Found Transf\n");
		//for(int kk=0; kk<4; ++kk) {
		//	for(int jj=0; jj<4; ++jj) 	printf("%8.5lf ", out[kk][jj]); 
		//	printf("\n");
		//}
	  
		assert(Pfix.size()==Pmov.size());
		int tts2=clock();
		ttsearch+=tts1-tts0;	
		ttleast +=tts2-tts1;
		ii.pcllo=H.Percentile(ap.PassLoFilter);
		ii.pcl50=H.Percentile(.5);
		ii.pclhi=H.Percentile(ap.PassHiFilter);
		ii.AVG=H.Avg();
		ii.RMS=H.RMS();
		ii.StdDev=H.StandardDeviation();
		ii.Time=clock();
		as.I.push_back(ii);
		nc++;
		// The distance of the next points to be considered is lowered according to the <ReduceFactor> parameter. 
		// We use 5 times the <ReduceFactor> percentile of the found points. 
		if(ap.ReduceFactor<1) StartMinDist=max(ap.MinDistAbs*ap.MinMinDistPerc, min(StartMinDist,5.0*H.Percentile(ap.ReduceFactor)));
	} 
	while ( 
		nc<=ap.MaxIterNum && 
		H.Percentile(.5) > ap.TrgDistAbs && 
		(nc<ap.EndStepNum+1 || ! as.Stable(ap.EndStepNum) ) 
		); 
 /*############ fine ciclo################*/
	int tt2=clock();  
	Matrix44d ResCopy=out;
	Point3d scv,shv,rtv,trv;
	Decompose(ResCopy,scv,shv,rtv,trv);
  if(math::Abs(1-scv[0])>ap.MaxScale || math::Abs(1-scv[1])>ap.MaxScale || math::Abs(1-scv[2])>ap.MaxScale ) {
			status = TOO_MUCH_SCALE;
			return false;
		}
	if(shv[0]>ap.MaxShear || shv[1]>ap.MaxShear || shv[2]>ap.MaxShear ) {
			status = TOO_MUCH_SHARE;
			return false;
		}
  printf("Init %i\nLoop %i\n\nSearch %i\n least sqrt %i\n",tt1-tt0,tt2-tt1,ttsearch,ttleast);

	return true;
}




const char *AlignPair::ErrorMsg( ErrorCode code)
	{
  switch(code){
		  case SUCCESS:         return "Success         ";
			case NO_COMMON_BBOX : return "No Common BBox  "; 
			case TOO_FEW_POINTS : return "Too few points  "; 
			case LSQ_DIVERGE		: return "LSQ not converge"; 
			case TOO_MUCH_SHARE : return "Too much shear  "; 
			case TOO_MUCH_SCALE : return "Too much scale  "; 
			case UNKNOWN_MODE   : return "Unknown mode    "; 
			default :  assert(0); return "Catastrophic Error";
	}
	return 0;
}
/*

Questa parte era relativa all'allineatore automatico.
Da controllare se ancora funge.


// Calcola la migliore traslazione possibile,
// cioe' quella dove i punti in movvert cascano per la maggior parte 
// in celle della ug occupate dalla fixmesh
// Restituisce il numero di celle di sovrapposizione massimo trovato

int AlignPair::SearchTranslate(A2Grid &u,
								 		const Matrix44d &BaseRot,
										const int range, 
										const int step, 
										Point3d &BestTransV,  // miglior vettore di spostamento
										bool Verbose)
{
	const int wide1=(range*2+1);
	const int wide2=wide1*wide1;
  vector< Point3d > movvert;
	vector< Point3d > movnorm;
	Box3d movbox;
										
	int t0=clock();
  InitMov(movvert,movnorm,movbox,BaseRot);
	Point3i ip;
	int i,ii,jj,kk;
	vector<int> test((range*2+1)*(range*2+1)*(range*2+1),0);
  //int bx,by,bz,ex,ey,ez; 

	Point3i b,e;
	int testposii,testposjj;
	for(i=0;i<movvert.size();++i)
		{
			if(u.bbox.IsIn(movvert[i])){
				u.PToIP(movvert[i],ip);
        b=ip+Point3i(-range,-range,-range);
				e=ip+Point3i( range, range, range);
				while(b[0]<0)         b[0]+=step;
				while(e[0]>=u.siz[0]) e[0]-=step;
				while(b[1]<0)         b[1]+=step;
				while(e[1]>=u.siz[1]) e[1]-=step;
				while(b[2]<0)         b[2]+=step;
				while(e[2]>=u.siz[2]) e[2]-=step;
				
				for(ii=b[0];ii<=e[0];ii+=step) 
				{
					testposii=(ii-ip[0]+range)*wide2;
					for(jj=b[1];jj<=e[1];jj+=step) 
					{
						testposjj=testposii+(jj-ip[1]+range)*wide1-ip[2]+range;
						for(kk=b[2];kk<=e[2];kk+=step) 
						{
							UGrid< A2Mesh::face_container >::UG ** g = u.Grid(ii,jj,kk);
							//if((*(g+1))- (*g) >0) 
							if((*(g+1))!=(*g) ) 
										++test[testposjj+kk];
							assert(ii >=0 && ii < u.siz[0]);
							assert(jj >=0 && jj < u.siz[1]);
							assert(kk >=0 && kk < u.siz[2]);
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
			{const int pos =(range+ii)*wide2+(range+jj)*wide1+range+kk;
				if(test[pos]>maxfnd)
				{
					BestI=Point3i(ii,jj,kk);
					BestTransV=Point3d(ii*u.voxel[0], jj*u.voxel[1], kk*u.voxel[2]);
					maxfnd=test[pos];
				}
				//printf("Found %i su %i in %i\n",test[testcnt],movvert.size(),t1-t0);
			}
	
	int t1=clock();
	if(Verbose) printf("BestTransl (%4i in %3ims) is %8.4f %8.4f %8.4f (%3i %3i %3i)\n",maxfnd,t1-t0,BestTransV[0],BestTransV[1],BestTransV[2],BestI[0],BestI[1],BestI[2]);
	return maxfnd;
}
#if 0
int AlignPair::SearchTranslate(UGrid< A2Mesh::face_container > &u,
								 		vector< Point3d > &movvert,
										vector< Point3d > &movnorm,
										Box3d &movbox,
										const Matrix44d &BaseRot,
										const int range, 
										const int step, 
										Point3d &BestTransV,  // miglior vettore di spostamento
										bool Verbose)
{
	const int wide1=(range*2+1);
	const int wide2=wide1*wide1;
  
	int t0=clock();
  InitMov(movvert,movnorm,movbox,BaseRot);
	Point3i ip;
	int i,ii,jj,kk;
	vector<int> test((range*2+1)*(range*2+1)*(range*2+1),0);
  	for(i=0;i<movvert.size();++i)
		{
			if(u.bbox.IsIn(movvert[i])){
				u.PToIP(movvert[i],ip);
				for(ii=-range;ii<=range;ii+=step) if(ip[0]+ii>=0 && ip[0]+ii<u.siz[0])
					for(jj=-range;jj<=range;jj+=step) if(ip[1]+jj>=0 && ip[1]+jj<u.siz[1])
						for(kk=-range;kk<=range;kk+=step) if(ip[2]+kk>=0 && ip[2]+kk<u.siz[2])
						{
							UGrid< A2Mesh::face_container >::UG ** g = u.Grid(ip[0]+ii,ip[1]+jj,ip[2]+kk);
							if((*(g+1))- (*g) >0) 
							++test[(range+ii)*wide2+(range+jj)*wide1+range+kk];
						}
			}
		}
	int maxfnd=0;
	Point3i BestI;
	for(ii=-range;ii<=range;ii+=step) 
		for(jj=-range;jj<=range;jj+=step) 
			for(kk=-range;kk<=range;kk+=step)
			{const int pos =(range+ii)*wide2+(range+jj)*wide1+range+kk;
			assert(test2[pos]==test[pos]);
				if(test[pos]>maxfnd)
				{
					BestI=Point3i(ii,jj,kk);
					BestTransV=Point3d(ii*u.voxel[0], jj*u.voxel[1], kk*u.voxel[2]);
					maxfnd=test[pos];
				}
				//printf("Found %i su %i in %i\n",test[testcnt],movvert.size(),t1-t0);
			}
	
	int t1=clock();
	if(Verbose) printf("BestTransl (%4i in %3ims) is %8.4f %8.4f %8.4f (%3i %3i %3i)\n",maxfnd,t1-t0,BestTransV[0],BestTransV[1],BestTransV[2],BestI[0],BestI[1],BestI[2]);
	return maxfnd;
}
*/


/**********************************************************/
// Funzioni per la scelta dei vertici sulla mesh da muovere

bool AlignPair::SampleMovVert(vector<A2Vertex> &vert, int SampleNum, AlignPair::Param::SampleModeEnum SampleMode)
{
	switch(SampleMode)
	{
		case AlignPair::Param::SMRandom :					 return SampleMovVertRandom(vert,SampleNum);
		case AlignPair::Param::SMNormalEqualized : return SampleMovVertNormalEqualized(vert,SampleNum);
		default: assert(0);
	}
	return false;
}


// Function to retrieve a static random number generator object.
static math::SubtractiveRingRNG &LocRnd(){
    static math::SubtractiveRingRNG myrnd(time(NULL));
    return myrnd;
}
// Gets a random number in the interval [0..n].
static int LocRnd(int n){
    return LocRnd().generate(n);
}
// Scelta a caso semplice
bool AlignPair::SampleMovVertRandom(vector<A2Vertex> &vert, int SampleNum)
{
  if(int(vert.size())<=SampleNum) return true;
	int i;
	for(i=0;i<SampleNum;++i)
	{
		int pos=LocRnd(vert.size());
    assert(pos>=0 && pos < int(vert.size()));
		swap(vert[i],vert[pos]);
	}
	vert.resize(SampleNum);
	return true;
}

/*
Scelta a caso in maniera tale che la distribuzione delle normali dei 
punti scelti sia il piu' possibile uniforme. In questo modo anche piccole 
parti inclinate vengono sicuramente campionate 

Si precalcola un piccolo (42) insieme di normali e si fa bucketing di 
tutti i vertici della mesh su di esse.
Poi si scelgono <SampleNum> punti scegliendo ogni volta prima un bucket 
e poi un punto all'interno del bucket

*/

bool AlignPair::SampleMovVertNormalEqualized(vector<A2Vertex> &vert, int SampleNum)
{
//  assert(0);

//	int t0=clock();
	static vector<Point3d> NV;
	if(NV.size()==0)
		{
			GenNormal<double>::Uniform(30,NV);
      printf("Generated %i normals\n",int(NV.size()));
		}
	// Bucket vector dove, per ogni normale metto gli indici 
	// dei vertici ad essa corrispondenti
	vector<vector <int> > BKT(NV.size());
  for(size_t i=0;i<vert.size();++i)
	{
    int ind=GenNormal<double>::BestMatchingNormal(vert[i].N(),NV);
		BKT[ind].push_back(i);
	}
  //int t1=clock();
	
	// vettore di contatori per sapere quanti punti ho gia' preso per ogni bucket
	vector <int> BKTpos(BKT.size(),0);
	
  if(SampleNum >= int(vert.size())) SampleNum= vert.size()-1;

  for(int i=0;i<SampleNum;)
		{
      int ind=LocRnd(BKT.size()); // Scelgo un Bucket
 			int &CURpos = BKTpos[ind];
			vector<int> &CUR = BKT[ind];

      if(CURpos<int(CUR.size()))
			{
				swap(CUR[CURpos], CUR[ CURpos + LocRnd(BKT[ind].size()-CURpos)]);
				swap(vert[i],vert[CUR[CURpos]]);
				++BKTpos[ind];
		    ++i;
			}
		}
	vert.resize(SampleNum);
//	int t2=clock();
//	printf("Matching   %6i\n",t1-t0);
//	printf("Collecting %6i\n",t2-t1);
//  printf("Total      %6i\n",t2-t0);

	return true;
}


