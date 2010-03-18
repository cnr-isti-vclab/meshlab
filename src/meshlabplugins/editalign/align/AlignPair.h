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
/****************************************************************************
$Log: AlignPair.h,v $
Revision 1.2  2006/09/28 08:17:46  spinelli
*** empty log message ***

Revision 1.1  2005/09/23 14:52:24  ganovell
added to code from sand/box cignoni, added optionally definable
namespace, added optional compilation as exectuable

Revision 1.3  2005/08/26 01:34:17  cignoni
First Working Release of align stuff

Revision 1.2  2005/06/10 16:07:29  cignoni
*** empty log message ***


****************************************************************************/
#ifndef __VCG_ALIGNPAIR
#define __VCG_ALIGNPAIR

#include <vcg/math/histogram.h>
#include <vcg/math/matrix44.h>
#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/vertex/component.h>
#include <vcg/simplex/face/base.h>
#include <vcg/simplex/face/component.h>
#include <vcg/simplex/face/component_rt.h>
#include <vcg/space/index/grid_static_ptr.h>
#include<vcg/complex/trimesh/base.h>
#include<vcg/complex/trimesh/update/normal.h>
#include<vcg/complex/trimesh/update/bounding.h>
#include<vcg/complex/trimesh/update/edges.h>
//#include <wrap/io_trimesh/import_ply.h>
//#include <wrap/io_trimesh/export_ply.h>


namespace vcg
{
/*************************************************************************
                                 AlignPair
																 
Classe per gestire un allineamento tra DUE sole mesh.

**************************************************************************/

class AlignPair
{
public: 

  enum ErrorCode {SUCCESS, NO_COMMON_BBOX, TOO_FEW_POINTS, 
		              LSQ_DIVERGE, TOO_MUCH_SHARE, TOO_MUCH_SCALE, FORBIDDEN, INVALID, UNKNOWN_MODE };


/*********************** Classi Accessorie ****************************/

class A2Vertex;
class A2Edge ;
class A2Face ;

class A2UsedTypes: public vcg::UsedTypes < vcg::Use<A2Vertex>::AsVertexType,
                                          vcg::Use<A2Edge  >::AsEdgeType,
                                          vcg::Use<A2Face  >::AsFaceType >{};

class A2Vertex   : public vcg::Vertex<A2UsedTypes,vcg::vertex::Coord3d,vcg::vertex::Normal3d,vcg::vertex::BitFlags> {};
class A2Face     : public vcg::Face< A2UsedTypes,vcg::face::VertexRef, vcg::face::Normal3d, vcg::face::EdgePlane,vcg::face::Mark,vcg::face::BitFlags> {};
class A2Mesh     : public vcg::tri::TriMesh< std::vector<A2Vertex>, std::vector<A2Face> > 
{ 
public:
  bool Import(const char *filename) { Matrix44d Tr; Tr.SetIdentity(); return Import(filename,Tr);} 
	bool Import(const char *filename, Matrix44d &Tr, bool hasborderflag=false);
	bool Init(const Matrix44d &Tr, bool hasborderflag);
};

typedef A2Mesh::FaceContainer FaceContainer;	  
typedef A2Mesh::FaceType      FaceType;	  
typedef GridStaticPtr<FaceType, double > A2Grid;

class Stat
{
public:

	class IterInfo
		{
		public:
			IterInfo() {memset ( (void *) this, 0, sizeof(IterInfo)); }

			double MinDistAbs;    
			int DistanceDiscarded;    
			int AngleDiscarded;
			int BorderDiscarded;
			int SampleTested;  // quanti punti ho testato con la mindist
			int SampleUsed;    // quanti punti ho scelto per la computematrix
			double pcllo;
			double pcl50;
			double pclhi;
			double AVG;
			double RMS;
			double StdDev;
			int Time;  // quando e' finita questa iterazione
		
		};
  std::vector<IterInfo> I; 
	
	double LastPcl50() const {return I.back().pcl50;}
	int LastSampleUsed() const {return I.back().SampleUsed;}

	int MovVertNum;
	int FixVertNum;
	int FixFaceNum;
	int TotTime() { return I.back().Time-StartTime; }
	int IterTime(unsigned int i) const 
	{
		assert(i<I.size()); 
		if(i==0) return  (I[i].Time-StartTime ); 
		    else return (I[i].Time - I[i-1].Time) ; 
	}
	int StartTime;
	void clear();
	void Dump(FILE *fp);	
	void HTMLDump(FILE *fp);	
	bool Stable(int last);

};


class Param
{
public:
	enum AlignMode      {AMVeryFast, AMFast, AMNormal, AMRobust, AMVeryRobust};
	enum MatchModeEnum  {MMClassic, MMRigid, MMFast};
	enum SampleModeEnum {SMRandom, SMNormalEqualized};

	Param()
	{
			SampleNum    = 1000;	
			MaxPointNum     = 100000;	
			MinPointNum  =  30;	
			
			MaxIterNum   =   50;	
			TrgDistAbs  = 0.05f;	// se si inquadra un quadrato 10cm x 10cm significa 0.05 mm 
			
			MinDistAbs   = 10;	
      MaxAngleRad  = math::ToRad(45.0);	
			MaxShear     = 0.5;
			MaxScale     = 0.5; // significa che lo scale deve essere compreso tra 1-MaxScale e 1+MaxScale
			PassLoFilter = 0.05;
			PassHiFilter = 0.75;
			ReduceFactor = 0.90;
			MinMinDistPerc = 0.01;
			EndStepNum   = 5;
			MatchMode    = MMRigid;
			SampleMode   = SMNormalEqualized;
			UGExpansionFactor=10;
			MinFixVertNum=20000;
			MinFixVertNumPerc=.25;
	}
	//AlignPair::Param(AlignMode am);

	
	int SampleNum;			// numero di sample da prendere sulla mesh fix, utilizzando 
											// il SampleMode scelto tra cui poi sceglierne al piu' <MaxPointNum> 
											// e almeno <MinPointNum> da usare per l'allineamento.
	int MaxPointNum;		// numero di coppie di punti da usare quando si calcola la matrice 
											// di allienamento e che poi si mettono da parte per il globale; 
											// di solito non usato 
	int MinPointNum;		// minimo numero di coppie di punti ammissibile perche' sia considerato 
											// valido l'allineamento
	double MinDistAbs;	// distanza minima iniziale perche due punti vengano presi in 
											// considerazione. NON e' piu espressa in percentuale sul bbox della mesh nella ug.
											// Ad ogni passo puo essere ridotta per 
											// accellerare usando ReduceFactor
	double MaxAngleRad;	// massimo angolo in radianti tra due normali perche' i due 
											// punti vengano presi in considerazione.
		
	int MaxIterNum;			// massimo numero di iterazioni da fare in align
	double TrgDistAbs;			// distanza obiettivo entro la quale devono stare almeno la meta' 
										      // dei campioni scelti; di solito non entra in gioco perche' ha un default molto basso

	int EndStepNum; // numero di iterazioni da considerare per decidere se icp ha converso.
	
	double PassLoFilter; // Filtraggio utilizzato per decidere quali punti scegliere tra quello trovati abbastanza
	double PassHiFilter; // vicini. Espresso in percentili. Di solito si scarta il quelli sopra il 75 e quelli sotto il 5
	double ReduceFactor; // At each step we reduce the range of search of the nearest points. We keep 5 times the <ReduceFactor> percentile.
	double MinMinDistPerc;	// Ratio between initial starting distance (MinDistAbs) and what can reach by the application of the ReduceFactor. 

	int UGExpansionFactor; // Grandezza della UG per la mesh fix come rapporto del numero di facce della mesh fix 

														// Nel caso si usi qualche struttura multiresolution
	int   MinFixVertNum;				// Gli allineamenti si fanno mettendo nella ug come mesh fix una semplificata; 
	float MinFixVertNumPerc;  // si usa il max tra MinFixVertNum e OrigMeshSize*MinFixVertNumPerc
		

	double MaxShear;
	double MaxScale;
	MatchModeEnum MatchMode;
	SampleModeEnum SampleMode;
	void Dump(FILE *fp,double BoxDiag);
	
};

// Classe per memorizzare il risultato di un allineamento tra due mesh
// i punti si intendono nel sistema di riferimento iniziale delle due mesh.
//
// se le mesh hanno una trasformazione di base in ingresso, 
// questa appare solo durante la A2Mesh::Import e poi e' per sempre dimenticata.
// Questi punti sono quindi nei sistemi di riferimento costruiti durante la Import
// la matrice Tr quella che 
// 
// Tr*Pmov[i]== Pfix


class Result
{
public: 
	int MovName;
	int FixName;

	Matrix44d Tr;
  std::vector<Point3d> Pfix;		// vertici corrispondenti su fix (rossi)
	std::vector<Point3d> Nfix; 		// normali corrispondenti su fix (rossi)
	std::vector<Point3d> Pmov;		// vertici scelti su mov (verdi) prima della trasformazione in ingresso (Original Point Target)
	std::vector<Point3d> Nmov; 		// normali scelti su mov (verdi)
  Histogramf H;
	Stat as;
	Param ap;
	ErrorCode status;
	bool IsValid() {return status==SUCCESS;}
	double err;
	float area; // the overlapping area, a percentage as computed in Occupancy Grid.

	bool operator <  (const Result & rr) const {return (err< rr.err);}
	bool operator <= (const Result & rr) const {return (err<=rr.err);}
	bool operator >  (const Result & rr) const {return (err> rr.err);}
	bool operator >= (const Result & rr) const {return (err>=rr.err);}
	bool operator == (const Result & rr) const {return (err==rr.err);}
	bool operator != (const Result & rr) const {return (err!=rr.err);}

  std::pair<double,double> ComputeAvgErr() const 
  {
    double sum_before=0;
    double sum_after=0;
    for(unsigned int ii=0;ii<Pfix.size();++ii)
    {
      sum_before+=Distance(Pfix[ii],   Pmov[ii]);
      sum_after+=Distance(Pfix[ii], Tr*Pmov[ii]);
    }
    return std::make_pair(sum_before/double(Pfix.size()),sum_after/double(Pfix.size()) ) ;
  }

};

/******************* Fine Classi Accessorie ************************/

	static const char *ErrorMsg( ErrorCode code);
	void Clear(){status=SUCCESS;} 
  AlignPair() {Clear();}

	/******* Data Members *********/ 
	
  std::vector<A2Vertex> *mov;
	A2Mesh *fix;

	ErrorCode status;
	AlignPair::Param ap;
	
	/**** End Data Members *********/ 
	
template < class MESH >
void ConvertMesh(const MESH &M1, A2Mesh &M2)
{
	typename MESH::VertContainer::const_iterator vi;
	typename A2Mesh::VertexIterator v2i;

	std::vector<int> vmap(M1.vert.size(),-1);
	M2.vert.resize(M1.vn);
	M2.face.resize(M1.fn);
	for(vi=M1.vert.begin(),v2i=M2.vert.begin();vi!=M1.vert.end();++vi)
		if(!(*vi).IsD()){
			(*v2i).Flags()=(*vi).Flags();
			(*v2i).P().Import((*vi).P());
			(*v2i).N().Import((*vi).cN());
			(*v2i).N().Normalize();

			vmap[vi-M1.vert.begin()] = v2i - M2.vert.begin();
			++v2i;
			}	
	assert(v2i - M2.vert.begin()==M1.vn);
	typename MESH::FaceContainer::const_iterator fi;
	typename A2Mesh::FaceIterator f2i;

	for(fi=M1.face.begin(),f2i=M2.face.begin();fi!=M1.face.end();++fi)if(!(*fi).IsD()){
			(*f2i).Flags()=(*fi).Flags();
			(*f2i).V(0)=&M2.vert[ (vmap[(*fi).V(0)-&M1.vert[0] ]) ];
			(*f2i).V(1)=&M2.vert[ (vmap[(*fi).V(1)-&M1.vert[0] ]) ];
			(*f2i).V(2)=&M2.vert[ (vmap[(*fi).V(2)-&M1.vert[0] ]) ];
			//(*f2i).V(1)=M2.vert.begin()+ (vmap[(*fi).V(1)-M1.vert.begin()]);
			//(*f2i).V(2)=M2.vert.begin()+ (vmap[(*fi).V(2)-M1.vert.begin()]);
			assert((*f2i).V(0)-&M2.vert[0]>=0);
			assert((*f2i).V(1)-&M2.vert[0] >=0);
			assert((*f2i).V(2)-&M2.vert[0] >=0);
			tri::UpdateEdges<A2Mesh>::Set(*f2i);
			++f2i;
		}	
	M2.vn=M1.vn;
	M2.fn=M1.fn;
	M2.bbox.Import(M1.bbox);
}

template < class VERTEX >
void ConvertVertex(const std::vector<VERTEX> &vert1, std::vector<A2Vertex> &vert2, Box3d *Clip=0)
{

	vert2.clear();
	typename std::vector<VERTEX>::const_iterator vi;
	A2Vertex tv;
	tv.UberFlags()=0;
	 Box3<typename VERTEX::ScalarType> bb;
	if(Clip){
		bb.Import(*Clip);
		for(vi=vert1.begin();vi<vert1.end();++vi)
			if(!(*vi).IsD() && bb.IsIn((*vi).cP())){
				tv.P().Import((*vi).cP());
				tv.N().Import((*vi).cN());
				vert2.push_back(tv);
		}
	}
	else
		for(vi=vert1.begin();vi<vert1.end();++vi)
			if(!(*vi).IsD()){
				tv.P().Import((*vi).cP());
				tv.N().Import((*vi).cN());
				vert2.push_back(tv);
		}		
}

bool SampleMovVert(std::vector<A2Vertex> &vert, int SampleNum, AlignPair::Param::SampleModeEnum SampleMode);
bool SampleMovVertRandom(std::vector<A2Vertex> &vert, int SampleNum);
bool SampleMovVertNormalEqualized(std::vector<A2Vertex> &vert, int SampleNum);
/*
Una volta trovati <SampleNum> coppie di punti corrispondenti se ne sceglie 
al piu' <PointNum> per calcolare la trasformazione che li fa coincidere.
La scelta viene fatta in base ai due parametri PassLo e PassHi;
*/
bool ChoosePoints( 	std::vector<Point3d> &Ps,		// vertici corrispondenti su fix (rossi)
																std::vector<Point3d> &Ns, 		// normali corrispondenti su fix (rossi)
																std::vector<Point3d> &Pt,		// vertici scelti su mov (verdi) 
																std::vector<Point3d> &OPt,		// vertici scelti su mov (verdi) 
																//vector<Point3d> &Nt, 		// normali scelti su mov (verdi)
																double PassLo,
																double PassHi,
																Histogramf &H)
;


/*
  Minimo esempio di codice per l'uso della funzione di allineamento.

	AlignPair::A2Mesh Mov,Fix;										// le due mesh da allineare
	vector<AlignPair::A2Vertex> MovVert;					// i punti sulla mov da usare per l'allineamento
	Matrix44d In;	In.SetIdentity();               // la trasformazione iniziale che applicata a mov la porterebbe su fix.

	AlignPair aa;                                 // l'oggetto principale.
	AlignPair::Param ap;
  UGrid< AlignPair::A2Mesh::face_container > UG;

	Fix.LoadPly("FixMesh.ply");										// Standard ply loading
	Mov.LoadPly("MovMesh.ply");
  Fix.Init( Ident, false);											// Inizializzazione necessaria (normali per vert, 
	Mov.Init( Ident, false);                      // info per distanza punto faccia ecc)
	
 	AlignPair::InitFix(&Fix, ap, UG);             // la mesh fix viene messa nella ug.
	
	aa.ConvertVertex(Mov.vert,MovVert);           // si campiona la mesh Mov per trovare un po' di vertici.
	aa.SampleMovVert(MovVert, ap.SampleNum, ap.SampleMode);
 
	aa.mov=&MovVert;                              // si assegnano i membri principali dell'oggetto align pair
	aa.fix=&Fix;
  aa.ap = ap;
	
	aa.Align(In,UG,res);                          // si spera :)
                                                // il risultato sta nella matrice res.Tr;

	res.as.Dump(stdout); 

*/

bool Align(const Matrix44d &in, A2Grid &UG, Result &res)
{
	res.ap=ap;
		
	bool ret=Align(	UG,
									in,
									res.Tr,
									res.Pfix, res.Nfix,
									res.Pmov, res.Nmov,
									res.H, 
									res.as);

	res.err=res.as.LastPcl50();
	res.status=status;
	return ret;	
}

double Abs2Perc(double val, Box3d bb) const {return val/bb.Diag();}
double Perc2Abs(double val, Box3d bb) const {return val*bb.Diag();}

/************************************************************************************
Versione Vera della Align a basso livello.

Si assume che la mesh fix sia gia' stata messa nella ug u con le debite trasformazioni.
in 

************************************************************************************/

bool Align(
						A2Grid &u,
  		const Matrix44d &in,					// trasformazione Iniziale che porta i punti di mov su fix 
						Matrix44d &out,					// trasformazione calcolata
            std::vector<Point3d> &Pfix,		// vertici corrispondenti su src (rossi)
						std::vector<Point3d> &Nfix, 		// normali corrispondenti su src (rossi)
						std::vector<Point3d> &OPmov,		// vertici scelti su trg (verdi) prima della trasformazione in ingresso (Original Point Target)
						std::vector<Point3d> &ONmov, 		// normali scelti su trg (verdi)
						Histogramf &H,
						Stat &as);


bool InitMov(
		std::vector< Point3d > &movvert,
		std::vector< Point3d > &movnorm,
		Box3d &movbox,
		const Matrix44d &in	);				

static bool InitFix(A2Mesh *fm,
												AlignPair::Param &pp,
												A2Grid &u,
												int PreferredGridSize=0);

}; // end class

} // end namespace vcg

#endif
