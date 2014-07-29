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
#ifndef ALIGN_OCCUPANCY_GRID_H
#define ALIGN_OCCUPANCY_GRID_H

#include "AlignPair.h"
#include <bitset>
#include <vcg/space/index/grid_static_obj.h>

namespace vcg
{
/* Occupancy Grid
 * Used to find the mesh pairs (arcs) to be used for ICP
 * It build a grid and for each cell count how many meshes passes through that cell
 * It compute the overlaps and returns a set of arcs with overlap greater than a given threshold.
 */
class OccupancyGrid{
public:
  typedef AlignPair::A2Mesh A2Mesh;

  // Class to keep for each voxel the id of the mesh passing throuhg it.
  // based on sorted vectors
  class MeshCounterV
  {
#define _MAX_MCV_SIZE 63 // Max number of meshes passing through a single cell.
	short last;
	short cnt[_MAX_MCV_SIZE];
  public:
    MeshCounterV(){last=0;cnt[last]=-1;}
    inline bool Empty() const {return last==0;}
    inline void Clear() {last=0;}
    inline bool IsSet(short const i) const
    {
      if(last==0) return false;
      const short *pt=std::lower_bound(cnt,cnt+last,i);
      return pt != (cnt+last);
      //	  return *pt==i;
    }

    int Count() const { return last; }

    void Set(int i) {
      assert(last>=0);
      if(last==0) {
        cnt[0]=i;
        ++last;
        cnt[last]=-1;
        return;
      }
      short *pt=std::lower_bound(cnt,cnt+last,i);
      if(*pt==i) return;
      if(pt-cnt<last)
        memmove(pt+1,pt,(pt-cnt)*sizeof(short));
      *pt=i;
      ++last;
      cnt[last]=-1;
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

  // Class to keep for each voxel the id of the mesh passing throuhg it.
  // based on bitset
  class MeshCounterB
  {
  private:
#define _MAX_MCB_SIZE 1024
    std::bitset<_MAX_MCB_SIZE> cnt;
  public:
    static int MaxVal() {return _MAX_MCB_SIZE;}
    bool Empty() const {return cnt.none();}
    void Clear() {cnt.reset();}
    bool IsSet(int i) const {return cnt.test(i);}
    void Set(int i) {cnt.set(i);}
    void UnSet(int i) {cnt.reset(i);}
    size_t Count() const { return cnt.count();}

    // Return a vector with all the id of the meshes
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
  };

  /***********************************************************/
  // Uncomment one of the following to switch between the two possible implementations of MeshCounter
  //typedef  MeshCounterV MeshCounter;
  typedef  MeshCounterB MeshCounter;
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
    }

    std::vector<int> unicityDistribution; // Distribution of the occupancy ratios:
                                          // unicityDistribution[i] says how many voxel (among the ones coverd by this mesh) are covered by <i> othermeshes.
                                          // if unicityDistribution[1] > 0 means that this is the unique to cover some portion of the space.
    int coverage;  // quanto e' ricoperta da altre mesh eccetto se stessa (eg: se ho due mesh di 1000 con overlap al 30% la covrg e' 300)
    int area;      // numero di voxel toccati da questa mesh
    bool operator < (OGMeshInfo &o) const { return area<o.area;}
    static int MaxStat() { return 64;}
    bool used;
  };
  /* Classe con informazioni su un arco plausibile
*/
  class OGArcInfo {
  public:

    enum sort {AREA,NORM_AREA,DEGREE};
    int s,t; // source and target (come indici nel gruppo corrente)
    //ArcPt a;
    int area;  //
    float norm_area;
    OGArcInfo(int _s,int _t,int _area,float _n){s=_s;t=_t;area=_area;norm_area=_n;}
    OGArcInfo(int _s,int _t,int _a){s=_s;t=_t;area=_a;}

    bool operator <  (const OGArcInfo &p) const {return norm_area <  p.norm_area;}
  };



  void Clear();
  bool Init(int _mn, Box3d bb, int size);

  void Add(const char *MeshName, Matrix44d &Tr, int id);
  void AddMeshes(std::vector<std::string> &names, std::vector<Matrix44d> &trv,int size);
  template <class MESH>
  void AddMesh(MESH &M, const Matrix44d &Tr, int ind);

  void RemoveMesh(int id);



  void ChooseArcs(std::vector<std::pair<int,int> > &AV, std::vector<int> &BNV,std::vector<int> &adjcnt, float normarea= 0.3);
  void Compute();
  void ComputeUsefulMesh(FILE *elfp=0);
  void Dump(FILE *fp);
  void ComputeTotalArea();
  GridStaticObj<MeshCounter, float> G;
  std::vector<int> VA; // virtual arcs
  int mn;
  int TotalArea;
  int MaxCount;   // massimo numero di mesh che passano per una cella;
  std::vector<OGArcInfo>  SVA; // SortedVirtual Arcs;
  std::vector<OGMeshInfo> VM;  // vettore delle informazioni raccolte sulle mesh.
  std::vector<OGUseInfo>  Use; // vettore con gli indici delle piu'utili mesh
};

// Implementation of the templated AddMesh
template <class MESH>
void OccupancyGrid::AddMesh(MESH &M, const Matrix44d &Tr, int ind)
{
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


}

#endif
