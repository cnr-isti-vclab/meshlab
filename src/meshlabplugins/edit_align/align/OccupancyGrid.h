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
 * It counts over the cell of a grid how many meshes passes through that cell.
 * It compute the relative overlaps and returns a set of arcs with overlap greater than a given threshold.
 */
class OccupancyGrid{
public:
  typedef AlignPair::A2Mesh A2Mesh;

  // Class to keep for each voxel the id of the mesh passing throuhg it.
  // based on bitset
  class MeshCounter
  {
  private:
#define OG_MAX_MCB_SIZE 2048
    std::bitset<OG_MAX_MCB_SIZE> cnt;
  public:
    static int MaxVal() {return OG_MAX_MCB_SIZE;}
    bool Empty() const {return cnt.none();}
    void Clear() {cnt.reset();}
    bool IsSet(size_t i) const {return cnt.test(i);}
    void Set(size_t i) {cnt.set(i);}
    void UnSet(size_t i) {cnt.reset(i);}
    size_t Count() const { return cnt.count();}

    // Return a vector with all the id of the meshes
    void Pack(std::vector<int> &v) const
    {
      v.clear();
      for(size_t i=0;i<OG_MAX_MCB_SIZE;++i)
        if(cnt.test(i)) v.push_back(i);
    }

    bool operator < (const MeshCounter &c) const {
      size_t ii=0;
      if(cnt==c.cnt) return false;
      while(ii<OG_MAX_MCB_SIZE){
        if(cnt[ii]!=c.cnt[ii]) return cnt[ii]<c.cnt[ii];
        ++ii;
      }
      return false;
    }
  };
  
  // Class for collecting cumulative information about each mesh in the OG.
  // This info are collected in the Compute() by scanning the OG after we filled it with all the meshes.
  class OGMeshInfo
  {
  public:
    OGMeshInfo() {Init(-1); used=false;}
    void Init(int _id) {
      coverage=0;area=0; id=_id;
    }    
    int id;        // the id of the mesh
    int area;      // number of voxels in the OG touched by this mesh
    int coverage;  // quanto e' ricoperta da altre mesh eccetto se stessa (eg: se ho due mesh di 1000 con overlap al 30% la covrg e' 300)
    
    std::vector<int> densityDistribution; // Distribution of the of the density of the voxels touched by this mesh:
                                          // densityDistribution[i] says how many voxel (among the ones coverd by this mesh) 
                                          // are covered by <i> othermeshes. Sum(densityDistribution) == area;
                                          // if densityDistribution[1] > 0 means that this mesh is the unique to cover some portion of the space.
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
  int mn;
  int TotalArea;
  int MaxCount;   // massimo numero di mesh che passano per una cella;

  std::vector<OGArcInfo>  SVA;  // SortedVirtual Arcs;
  std::map<int,OGMeshInfo> VM;  // High level information for each mesh. Mapped by mesh id
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
	VM[ind].Init(ind);
	VM[ind].used=true;
}


}

#endif
