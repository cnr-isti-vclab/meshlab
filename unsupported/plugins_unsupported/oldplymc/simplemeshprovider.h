#ifndef SIMPLEMESHPROVIDER_H
#define SIMPLEMESHPROVIDER_H
#include "../../common/alnParser.h"
#include <list>
#include <vector>
#include <vcg/space/box3.h>
#include <wrap/ply/plystuff.h>
#include <wrap/io_trimesh/import.h>

using namespace std;
using namespace vcg;

/**
 * Very simple class to implement a cache of loaded meshes to avoid reloading an processing of the same mesh multiple times. 
 * 
 */


template<class TriMeshType>
class MeshCache
{
  class Pair
  {
  public:
    Pair(){used=0;}
    TriMeshType *M;
    std::string Name;
    int used; // 'data' dell'ultimo accesso. si butta fuori quello lru
  };
  
  std::list<Pair> MV;
  
public:
  void clear();
  
  MeshCache() {MeshCacheSize=6;}
  ~MeshCache() {
    typename std::list<Pair>::iterator mi;
    for(mi=MV.begin();mi!=MV.end();++mi)
      delete (*mi).M;
  }
  

  /**
   * @brief Find load a mesh form the cache if it is in or from the disk otherwise
   * @param name what mesh to find
   * @param sm the pointer loaded mesh
   * @return true if the mesh was already in cache
   * 
   */  
  bool Find(const std::string &name,  TriMeshType * &sm)
  {
    typename std::list<Pair>::iterator mi;
    typename std::list<Pair>::iterator oldest; // quello che e' piu' tempo che non viene acceduto.
    int last;
    
    last = std::numeric_limits<int>::max();
    oldest = MV.begin();
    
    for(mi=MV.begin();mi!=MV.end();++mi)
    {
      if((*mi).used<last)
      {
        last=(*mi).used;
        oldest=mi;
      }
      if((*mi).Name==name) {
        sm=(*mi).M;
        (*mi).used++;
        return true;
      }
    }
    
    // we have not found the requested mesh
    // either allocate a new mesh or give back a previous mesh.
    
    if(MV.size()>MeshCacheSize)	{
      sm=(*oldest).M;
      (*oldest).used=0;
      (*oldest).Name=name;
    }	else	{
      MV.push_back(Pair());
      MV.back().Name=name;
      MV.back().M=new TriMeshType();
      sm=MV.back().M;
    }
    return false;
  }
  
  
  size_t MeshCacheSize;
  size_t size() const {return MV.size();}
};

template<class TriMeshType>
class SimpleMeshProvider
{
private:
  std::vector< std::string > meshnames;
  std::vector<vcg::Matrix44f> TrV;
  std::vector<float> WV;		        // weight tot be applied to each mesh.
  std::vector<vcg::Box3f> BBV;	    // bbox of the transformed meshes..
  vcg::Box3f fullBBox;
  MeshCache<TriMeshType> MC;
  
public:
  
  int size() {return meshnames.size();}
  
  int getCacheSize() {return MC.MeshCacheSize;}
  int setCacheSize(size_t newsize)
  {
    if(newsize == MC.MeshCacheSize)
      return MC.MeshCacheSize;
    if(newsize <= 0)
      return MC.MeshCacheSize;
    
    MC.MeshCacheSize = newsize;
    return newsize;
  }
  
  bool openALN (const char* alnName)
  {
    vector<RangeMap> rmaps;
    ALNParser::ParseALN(rmaps, alnName);
    
    for(size_t i=0; i<rmaps.size(); i++)
      AddSingleMesh(rmaps[i].filename.c_str(), rmaps[i].trasformation, rmaps[i].quality);
    
    return true;
  }
  
  bool AddSingleMesh(const char* meshName, const Matrix44f &tr= Matrix44f::Identity(), float meshWeight=1)
  {
    assert(WV.size()==meshnames.size() && TrV.size() == WV.size());
    TrV.push_back(tr);
    meshnames.push_back(meshName);
    WV.push_back(meshWeight);
    BBV.push_back(Box3f());
    return true;
  }
    
  vcg::Box3f bb(int i) {return BBV[i];}
  vcg::Box3f fullBB(){ return fullBBox;}
  vcg::Matrix44f Tr(int i) const  {return TrV[i];}
  std::string MeshName(int i) const {return meshnames[i];}
  float W(int i) const {return WV[i];}
  
  void Clear()
  {
    meshnames.clear();
    TrV.clear();
    WV.clear();
    BBV.clear();
    fullBBox.SetNull();
    MC.clear();
  }
  
  bool Find(int i, TriMeshType * &sm)
  {
    return MC.Find(meshnames[i],sm);
  }
  
  bool InitBBox()
  {
    fullBBox.SetNull();    
    for(int i=0;i<int(meshnames.size());++i)
    {
      bool ret;      
      printf("bbox scanning %4i/%i [%16s]      \r",i+1,(int)meshnames.size(), meshnames[i].c_str());
      if(tri::io::Importer<TriMeshType>::FileExtension(meshnames[i],"PLY") || tri::io::Importer<TriMeshType>::FileExtension(meshnames[i],"ply"))
      {
        ret=ply::ScanBBox(meshnames[i].c_str(),BBV[i],TrV[i],true,0);
      }
      else
      {
        printf("Trying to import a non-ply file %s\n",meshnames[i].c_str());fflush(stdout);
        TriMeshType m;
        ret = (tri::io::Importer<TriMeshType>::Open(m,meshnames[i].c_str()) == tri::io::Importer<TriMeshType>::E_NOERROR);
        tri::UpdatePosition<TriMeshType>::Matrix(m,TrV[i]);
        tri::UpdateBounding<TriMeshType>::Box(m);
        BBV[i].Import(m.bbox);
      }
      if( ! ret)
      {
        printf("\n\nwarning:\n file '%s' not found\n",meshnames[i].c_str());fflush(stdout);
        continue;
      }
      fullBBox.Add(BBV[i]);
    }
    return true;
  }
  
};

class SVertex;
class SFace;
class SUsedTypes: public vcg::UsedTypes < vcg::Use<SVertex>::AsVertexType,
    vcg::Use<SFace  >::AsFaceType >{};

class SVertex     : public Vertex< SUsedTypes, vertex::Coord3f, vertex::Normal3f,vertex::VFAdj, vertex::BitFlags, vertex::Color4b, vertex::Qualityf>{};
class SFace       : public Face< SUsedTypes, face::VertexRef, face::Normal3f,face::Qualityf, face::VFAdj, face::BitFlags> {};
class SMesh       : public vcg::tri::TriMesh< std::vector< SVertex>, std::vector< SFace > > {};

#endif // SIMPLEMESHPROVIDER_H
