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

    int getCacheSize() {return MC.MaxSize;}
    int setCacheSize(int newsize)
    {
      if(newsize == MC.MaxSize)
	return MC.MaxSize;
      if(newsize <= 0)
	return MC.MaxSize;

      MC.MaxSize = newsize;
      return newsize;
    }

    bool openALN (const char* alnName)
    {
      vector<RangeMap> rmaps;
      ALNParser::ParseALN(rmaps, alnName);

      for(int i=0; i<rmaps.size(); i++)
          AddSingleMesh(rmaps[i].filename.c_str(), rmaps[i].trasformation, rmaps[i].quality);

      return true;
    }

    bool AddSingleMesh(const char* meshName, Matrix44f &tr, float meshWeight=1)
    {
        assert(WV.size()==meshnames.size() && TrV.size() == WV.size());
        TrV.push_back(tr);
        meshnames.push_back(meshName);
        WV.push_back(meshWeight);
        BBV.push_back(Box3f());
        return true;
    }

    bool AddSingleMesh(const char* meshName)
    {
        Matrix44f identity; identity.SetIdentity();
        return AddSingleMesh(meshName, identity);
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

            }
            else
            {   printf("Trying to import a non-ply file %s\n",meshnames[i].c_str());fflush(stdout);
                TriMeshType m;
                int retVal=tri::io::Importer<TriMeshType>::Open(m,meshnames[i].c_str());
                ret = (retVal==0);
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
        class SEdge;
        class SFace;
        class SUsedTypes: public vcg::UsedTypes < vcg::Use<SVertex>::AsVertexType,
                                                  vcg::Use<SEdge   >::AsEdgeType,
                                                  vcg::Use<SFace  >::AsFaceType >{};

class SVertex     : public Vertex< SUsedTypes, vertex::Coord3f, vertex::Normal3f,vertex::VFAdj, vertex::BitFlags, vertex::Color4b, vertex::Qualityf>{};
class SFace       : public Face< SUsedTypes, face::VertexRef, face::Normal3f,face::Qualityf, face::VFAdj, face::BitFlags> {};
class SMesh       : public vcg::tri::TriMesh< std::vector< SVertex>, std::vector< SFace > > {};

#endif // SIMPLEMESHPROVIDER_H
