#ifndef SIMPLEMESHPROVIDER_H
#define SIMPLEMESHPROVIDER_H
#include "../../meshlab/alnParser.h"
#include <list>
#include <vector>
#include <vcg/space/box3.h>
#include <wrap/ply/plystuff.h>
#include <wrap/io_trimesh/import.h>

using namespace std;
using namespace vcg;

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

    MeshCache() {MaxSize=6;}
    ~MeshCache() {
        typename std::list<Pair>::iterator mi;
        for(mi=MV.begin();mi!=MV.end();++mi)
            delete (*mi).M;
    }
// Restituisce true se la mesh e' in cache;
// restituisce in ogni caso il puntatore dove sta (o dovrebbe stare) la mesh
// Gestione LRU

    bool Find(std::string &name,  TriMeshType * &sm)
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

    if(MV.size()>MaxSize)	{
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


    size_t MaxSize;
    size_t size() const {return MV.size();}
};

template<class TriMeshType>
        class SimpleMeshProvider
{
    std::vector< std::string > meshnames;
    std::vector<vcg::Matrix44f> TrV;
    std::vector<float> WV;		    // vettore con i pesi da applicare alla mesh.
    std::vector<vcg::Box3f> BBV;	    // vettore con i bbox trasformati delle mesh da scannare.
    vcg::Box3f fullBBox;
    MeshCache<TriMeshType> MC;

    public:

    int size() {return meshnames.size();}

    bool openALN (const char* alnName)
    {
      vector<RangeMap> rmaps;
      ALNParser::ParseALN(rmaps, alnName);

      for(int i=0; i<rmaps.size(); i++)
      {
	TrV.push_back(rmaps[i].trasformation);
	meshnames.push_back(rmaps[i].filename);
	WV.push_back(rmaps[i].quality);
	BBV.push_back(Box3f());
      }
      return true;
    }

    bool AddSingleMesh(const char* meshName)
    {
        assert(WV.size()==meshnames.size() && TrV.size() == WV.size());
        TrV.push_back(Matrix44f());
        TrV.back().SetIdentity();
        meshnames.push_back(meshName);
        WV.push_back(1);
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
            Box3d b;
            bool ret;
            Matrix44f mt;
            Matrix44f Id; Id.SetIdentity();
            mt.Import(TrV[i]);
	    printf("bbox scanning %4i/%i [%16s]      \r",i+1,(int)meshnames.size(), meshnames[i].c_str());
            if(tri::io::Importer<TriMeshType>::FileExtension(meshnames[i],"PLY"))
            {
	      if(!(TrV[i]==Id))
		  ret=ply::ScanBBox(meshnames[i].c_str(),BBV[i],mt,true,0);
	      else
		  ret=vcg::ply::ScanBBox(meshnames[i].c_str(),BBV[i]);
	      if( ! ret)
	      {
		  printf("\n\nwarning:\n file '%s' not found\n",meshnames[i].c_str());
		  continue;
	      }
	    }
	    else
            {
                TriMeshType m;
                tri::io::Importer<TriMeshType>::Open(m,meshnames[i].c_str());
                tri::UpdateBounding<TriMeshType>::Box(m);
                BBV[i].Import(m.bbox);
            }
	    fullBBox.Add(BBV[i]);
        }
        return true;
    }

};

class MyEdge;
class SFace;
class SVertex     : public VertexSimp2< SVertex,  MyEdge, SFace, vertex::Coord3f, vertex::Normal3f,vertex::VFAdj, vertex::BitFlags, vertex::Color4b, vertex::Qualityf>{};
class SFace       : public FaceSimp2< SVertex,    MyEdge, SFace, face::VertexRef, face::Normal3f,face::Qualityf, face::VFAdj, face::BitFlags> {};
class SMesh       : public vcg::tri::TriMesh< std::vector< SVertex>, std::vector< SFace > > {};

#endif // SIMPLEMESHPROVIDER_H
