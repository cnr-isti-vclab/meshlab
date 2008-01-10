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
$Log: OccupancyGrid.h,v $
Revision 1.2  2006/09/28 09:46:46  spinelli
*** empty log message ***

Revision 1.1  2005/09/23 14:52:24  ganovell
added to code from sand/box cignoni, added optionally definable
namespace, added optional compilation as exectuable

Revision 1.1  2005/08/26 01:34:17  cignoni
First Working Release of align stuff

Revision 1.2  2005/06/10 16:07:29  cignoni
*** empty log message ***


****************************************************************************/

/*

Griglia per valutare nuovi archi

Si costruisce una ug e si conta per ogni cella quali sono le mesh che la toccano:
per ogni mesh si valuta poi quali sono le altre mesh con cui condivide maggiormente 
lo spazio e 

*/
#ifndef ALIGN_OCCUPANCY_GRID_H
#define ALIGN_OCCUPANCY_GRID_H

// Necessari per SimpleOGVertex e SimpleOGFace
#include <bitset>
#include <algorithm>

#include <vcg/space/index/grid_static_obj.h>
#include <vcg/math/histogram.h>


namespace vcg
{

class OccupancyGrid{
public:
  typedef AlignPair::A2Mesh A2Mesh;

#define _MAX_MCV_SIZE 63 // massima numero di valori memorizzabili in un mesh counter
class MeshCounterV
{
	short last;
	short cnt[_MAX_MCV_SIZE];
public:
	MeshCounterV(){last=0;}
	bool Empty() const {return last==0;};
	void Clear() {last=0;}
	bool IsSet(short const i) const {
		if(last==0) return false;
    const short *pt=std::lower_bound(cnt,cnt+last,i);
		return *pt==i;
	}
	
	int Count() const { return last; }
	
	void Set(int i) {
		assert(last>=0);
		if(last==0) {
			cnt[0]=i;
			++last;
			return;
		}
		short *pt=std::lower_bound(cnt,cnt+last,i);
		if(*pt==i) return;
		if(pt-cnt<last) 
			memmove(pt+1,pt,(pt-cnt)*2);
		*pt=i;
		++last;
		assert(last>=0);
		
		if(last>=_MAX_MCV_SIZE) {
				
				abort();
			}
		}
	
	void UnSet(int i) {
		if(last==0)	return;
		short *pt=std::lower_bound(cnt,cnt+last,i);
		if(*pt!=i) return;
		memmove(pt,pt+1,((cnt+last)-pt)*2);
		--last;
		}
  void Pack(std::vector<int> &v) const
	{
		v.resize(last);
		for(int i=0;i<last;++i)
		{
			//assert(v[i]>=0);
			//assert(v[i]<v[i+i] || i<last+1); 
			v[i]=cnt[i];
		}
	}
	static int MaxVal() {return 32767;}
};

#define _MAX_MCB_SIZE 1024
// Tiene per ogni voxel quali mesh ci passano.
// Notare che: 
//   il massimo numero di mesh e' MAXMESH
class MeshCounterB
{
	std::bitset<_MAX_MCB_SIZE> cnt;
public:
	static int MaxVal() {return _MAX_MCB_SIZE;}
	bool Empty() const {return cnt.none();};
	void Clear() {cnt.reset();}
  bool IsSet(int i) const {return cnt[i];}
	void Set(int i) {cnt[i]=true;}
	void UnSet(int i) {cnt[i]=false;}
	int Count() const { return cnt.count();}
  void Pack(std::vector<int> &v) const
		{
			v.clear();
			for(int i=0;i<_MAX_MCB_SIZE;++i)
				if(cnt[i]==1) v.push_back(i);
		}
		void Dump() const
		{
			for(int i=0;i<64;++i) {
				if((i%32)==0) printf(" " );
				if(cnt[i]) printf("1"); else printf("0");
			}
		}

		bool operator < (const MeshCounterB &c) const {		
		int ii=0;
		if(cnt==c.cnt) return false;
		while(ii<_MAX_MCB_SIZE){
			if(cnt[ii]!=c.cnt[ii]) return cnt[ii]<c.cnt[ii];
			++ii;
			}
		return false;
		}
		/*
		bool operator < (const MeshCounter &c) const {		
			bitset<MAXMESH> ulb(0xffffffff);
			unsigned long ul =(ulb & cnt).to_ulong();
			unsigned long ula=(ulb & c.cnt).to_ulong();
			//printf("\n<(%8x-%8x)\n",ul,ula);fflush(stdout);
			if(ul!=ula) return ul<ula;
			if(cnt==c.cnt) return false;
			//printf("\nComparing: ");   Dump();
			//printf("\n           "); c.Dump();
			int ii=MAXMESH/(sizeof(unsigned long)*8)-1;
			bitset<MAXMESH> cc = cnt;
			bitset<MAXMESH> cca = c.cnt;
			while (ii>0){
				cc>>=(sizeof(unsigned long)*8);
				cca>>=(sizeof(unsigned long)*8);
				unsigned long ul=(cc&ulb).to_ulong();
				unsigned long ula=(cca&ulb).to_ulong();
				//printf("\n<(%8x-%8x)\n",ul,ula);fflush(stdout);
				if(ul!=ula) return ul<ula;
				--ii;
			}
			return false;
		}
		*/
};


/***********************************************************/
typedef  MeshCounterV MeshCounter;  // per usare i vettori compattati
//typedef  MeshCounterB MeshCounter;  // per usare i bitset
/***********************************************************/

class OGUseInfo
{
public:
	OGUseInfo() {id=-1; area=0;}
	OGUseInfo(const int _id, const int _area) :id(_id),area(_area){}
	int id;
	int area; 
	bool operator < (const OGUseInfo &o) const { return area<o.area;}	

};

class OGMeshInfo
{
public:
	enum {maxcnt =3};
	OGMeshInfo() {Init(); used=false;}
	void Init() {
		coverage=0;area=0;
		memset(unique,0,32*4);
	}
	int unique[32];    // quanti sono i voxel unici 
	int coverage;  // quanto e' ricoperta da altre mesh eccetto se stessa (eg: se ho due mesh di 1000 con overlap al 30% la covrg e' 300)
	int area;      // numero di voxel toccati da questa mesh
	bool operator < (OGMeshInfo &o) const { return area<o.area;}	
	static const int MaxStat() { return 64;}
	bool used;
};
/* Classe con informazioni su un arco plausibile
*/
class OGArcInfo {
public:
	static	bool IdLess ( OGArcInfo &p1, OGArcInfo &p2 )	
	{
		if(p1.s!=p2.s)  return p1.s<p2.s;
		else return p1.t<p2.t;
	}


	enum sort {AREA,NORM_AREA,DEGREE};
	int s,t; // source and target (come indici nel gruppo corrente)
	//ArcPt a;
	int area;  // 
	float norm_area; 
	OGArcInfo(int _s,int _t,int _area,float _n){s=_s;t=_t;area=_area;norm_area=_n;}
	OGArcInfo(int _s,int _t,int _a){s=_s;t=_t;area=_a;}

	bool operator <  (const OGArcInfo &p) const {return norm_area <  p.norm_area;}
	bool operator <= (const OGArcInfo &p) const {return norm_area <= p.norm_area;}
	bool operator >  (const OGArcInfo &p) const {return norm_area >  p.norm_area;}
	bool operator >= (const OGArcInfo &p) const {return norm_area >= p.norm_area;}
};




	void Clear();
	bool Init(int _mn, Box3d bb, int size);
	void Add(const char *MeshName, Matrix44d &Tr, int id);
  void AddVert(std::vector<Point3f> &vv, Matrix44d &Tr, int id);
	void RemoveMesh(int id);

  void AddMeshes(std::vector<std::string> &names, std::vector<Matrix44d> &trv,int size);

template <class MESH>
void AddMesh(MESH &M, const Matrix44d &Tr, int ind)
{
// Riempimento Griglia
	
	Point3f tmp;
	Point3d tmpd;
  Matrix44f Trf;
	Trf.Import(Tr);
	typename MESH::VertexIterator vi;
	for(vi=M.vert.begin();vi!=M.vert.end();++vi)
    {
      if(!(*vi).IsD())
        G.Grid( Trf * Point3f::Construct((*vi).P()) ).Set(ind);
    }
	VM[ind].Init();
	VM[ind].used=true;
}


//	void Add(A2Mesh &m, Matrix44d &Tr, int id);
void ChooseArcs(std::vector<std::pair<int,int> > &AV, std::vector<int> &BNV,std::vector<int> &adjcnt, float normarea= 0.3);
	void Compute();
	void ComputeUsefulMesh(FILE *elfp=0);
	void Dump(FILE *fp);
	void DrawBox();
	void DrawPoint();
	void ComputeColor();
	void ComputeTotalArea();
	void DrawCell(int x,int y, int z);
	GridStaticObj<MeshCounter, float> G;
//	map<AMesh *,int> M2I;
//	map<int,AMesh *> I2M;
	std::vector<int> VA; // virtual arcs
	//vector<int> MS; // Mesh surface (expressed in voxels)
	int mn;
  int TotalArea;
	int MaxCount;   // massimo numero di mesh che passano per una cella;
	Histogramf OH; // Occupancy histogram; 
 	std::vector<OGArcInfo>  SVA; // SortedVirtual Arcs;
	std::vector<OGMeshInfo> VM;  // vettore delle informazioni raccolte sulle mesh.
	std::vector<Color4b>    VC;  // vettore con i colori per ogni cella per disegnare velocemente la ug;
	std::vector<OGUseInfo>  Use; // vettore con gli indici delle piu'utili mesh
};

}

#endif
