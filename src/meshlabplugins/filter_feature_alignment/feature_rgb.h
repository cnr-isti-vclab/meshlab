#include <stdio.h>
#include <vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/clean.h>

using namespace std;
using namespace vcg;

//A class that implements a feature based on RGB color
template<class MESH_TYPE, int dim>
class FeatureRGB
{
    public:    

    class Parameters
    {
        public:

        enum RGBAType {RED, GREEN, BLUE, ALPHA};

        vector<RGBAType> featureDesc;

        bool add(RGBAType cType){
            if(featureDesc.size()>=dim) return false;
            featureDesc.push_back(cType);
            return true;
        }
    };

    typedef MESH_TYPE MeshType;    
    typedef FeatureRGB<MeshType,dim> FeatureType;
    typedef typename FeatureType::Parameters ParamType;
    typedef typename MeshType::ScalarType ScalarType;
    typedef typename MeshType::VertexType VertexType;
    typedef typename MeshType::VertexIterator VertexIterator;
    typedef typename MeshType::template PerVertexAttributeHandle<FeatureType> PVAttributeHandle;

    Point3<ScalarType> pos;
    Point3<ScalarType> normal;
    float description[dim];

    FeatureRGB();
    FeatureRGB(VertexType& v);
    static char* getName();
    static int getRequirements();
    static float getNullValue();
    static bool isNullValue(float);
    static int getFeatureDimension();
    static bool CheckPersistency(FeatureType f);
    static void SetupParameters(ParamType& param);
    static bool HasBeenComputed(MeshType& m);
    static bool ComputeFeature( MeshType&, ParamType& param, CallBackPos *cb=NULL);
    static bool Subset(int, MeshType&, vector<FeatureType*>&, int, CallBackPos *cb=NULL);

    private:
    static MESH_TYPE* CreateSamplingMesh();
    static int SetupSamplingStructures(MeshType& m, typename MeshType::template PerVertexAttributeHandle<FeatureType>& fh, MeshType* samplingMesh, vector<FeatureType*>* vecFeatures);
};

template<class MESH_TYPE, int dim> inline FeatureRGB<MESH_TYPE,dim>::FeatureRGB(){}

template<class MESH_TYPE, int dim>
inline FeatureRGB<MESH_TYPE,dim>::FeatureRGB(VertexType& v):pos(v.P()),normal(v.N())
{
    normal.Normalize();
    for(int i=0; i<dim; i++) FeatureRGB::getNullValue();
}

template<class MESH_TYPE, int dim> inline char* FeatureRGB<MESH_TYPE,dim>::getName()
{
    return "FeatureRGB";
}

template<class MESH_TYPE, int dim> inline int FeatureRGB<MESH_TYPE,dim>::getRequirements()
{
    return (MeshModel::MM_VERTCOLOR);
}

template<class MESH_TYPE, int dim> inline bool FeatureRGB<MESH_TYPE,dim>::isNullValue(float val)
{
    return (val==FeatureType::getNullValue());
}

template<class MESH_TYPE, int dim> inline float FeatureRGB<MESH_TYPE,dim>::getNullValue()
{
    return -1.0f;
}

template<class MESH_TYPE, int dim> inline int FeatureRGB<MESH_TYPE,dim>::getFeatureDimension()
{
    return dim;
}

//check persistence beetween scales: return true if description is valid for all scales, false otherwise
template<class MESH_TYPE, int dim> bool FeatureRGB<MESH_TYPE,dim>::CheckPersistency(FeatureType f)
{
    for(int i = 0; i<FeatureType::getFeatureDimension(); i++)
    {
        if( FeatureType::isNullValue(f.description[i]) ) return false;
    }
    return true;
}

template<class MESH_TYPE, int dim> void FeatureRGB<MESH_TYPE,dim>::SetupParameters(ParamType& param)
{
    param.add(FeatureType::Parameters::RED);
    param.add(FeatureType::Parameters::GREEN);
    param.add(FeatureType::Parameters::BLUE);
    assert(param.featureDesc.size()==getFeatureDimension());
}

template<class MESH_TYPE, int dim> bool FeatureRGB<MESH_TYPE,dim>::HasBeenComputed(MeshType &m)
{
    //checks if the attribute exist
    return tri::HasPerVertexAttribute(m,std::string(FeatureType::getName()));
}

template<class MESH_TYPE, int dim> bool FeatureRGB<MESH_TYPE,dim>::ComputeFeature(MeshType &m, ParamType& param, CallBackPos *cb)
{
    //clear the mesh to avoid wrong values during computations
    //tri::Clean<MeshType>::RemoveUnreferencedVertex(m);

    //variables needed for progress bar callback
    float progBar = 0.0f;
    float offset = 100.0f/m.VertexNumber();
    if(cb) cb(0,"Computing features...");

    PVAttributeHandle fh = FeatureAlignment<MeshType, FeatureType>::GetFeatureAttribute(m, true);
    if(!tri::Allocator<MeshType>::IsValidHandle(m,fh)) return false;

    //for each vertex, creates a feature and set its values.
    VertexIterator vi;
    for(vi = m.vert.begin(); vi!=m.vert.end(); ++vi)
    {
        fh[vi] = FeatureType(*vi);

        //copy vertex color into feature values
        for(unsigned int i=0; i<FeatureType::getFeatureDimension(); i++)
            fh[vi].description[i] = float((*vi).C()[param.featureDesc[i]]);

        //advance progress bar
        progBar+=offset;
        if(cb) cb(int(progBar),"Computing features...");
    }
    return true;
}

//create a mesh and add a per vertex attribute to hold feature values
template<class MESH_TYPE, int dim>
MESH_TYPE* FeatureRGB<MESH_TYPE,dim>::CreateSamplingMesh()
{
    MeshType* m = new MeshType();
    PVAttributeHandle fh = FeatureAlignment<MeshType, FeatureType>::GetFeatureAttribute(*m, true);
    if(!tri::Allocator<MeshType>::IsValidHandle(*m,fh)){
        if(m) delete m;
        return NULL;
    }
    return m;
}

template<class MESH_TYPE, int dim>
int FeatureRGB<MESH_TYPE,dim>::SetupSamplingStructures(MeshType& m, typename MeshType::template PerVertexAttributeHandle<FeatureType>& fh, MeshType* samplingMesh, vector<FeatureType*>* vecFeatures)
{
    int countFeatures = 0;
    PVAttributeHandle pmfh;
    if(samplingMesh){
        pmfh = FeatureAlignment<MeshType, FeatureType>::GetFeatureAttribute(*samplingMesh);
        if(!tri::Allocator<MeshType>::IsValidHandle(*samplingMesh,pmfh)) return 0;
    }

    //fill the vector with all persistent features.
    for(VertexIterator vi=m.vert.begin(); vi!=m.vert.end(); ++vi){
        //check persistence beetween scales: if feature is persistent, add a pointer in vecFeatures
        if( FeatureType::CheckPersistency(fh[vi]) ){
            countFeatures++;  //increment counter of valid features
            if(vecFeatures) vecFeatures->push_back(&(fh[vi]));
            if(samplingMesh){
                tri::Allocator<MeshType>::AddVertices(*samplingMesh, 1);
                samplingMesh->vert.back().ImportLocal(*vi);
                pmfh[samplingMesh->vert.back()] = fh[vi];
            }
        }
    }

    return countFeatures;
}

template<class MESH_TYPE, int dim>
bool FeatureRGB<MESH_TYPE,dim>::Subset(int k, MeshType &m, vector<FeatureType*> &vecSubset, int sampType, CallBackPos *cb)
{
    //variables needed for progress bar callback
    float progBar = 0.0f;
    float offset = 100.0f/(m.VertexNumber() + k);

    //if attribute doesn't exist, return; else we can get a handle to the attribute
    PVAttributeHandle fh = FeatureAlignment<MeshType, FeatureType>::GetFeatureAttribute(m);
    if(!tri::Allocator<MeshType>::IsValidHandle(m,fh)) return false;

    //create a vector to hold valid features that later will be sampled
    vector<FeatureType*>* vecFeatures = NULL;
    MeshType* poissonMesh = NULL;
    if(sampType==0) vecFeatures = new vector<FeatureType*>();
    else poissonMesh = CreateSamplingMesh();

    //fill the vector with all persistent features.
    int countFeatures = SetupSamplingStructures(m, fh, poissonMesh, vecFeatures);
    if(countFeatures<k) k = countFeatures;  //we can't extract more of what we got!

    //perform different kinds of sampling
    FeatureType** sampler = NULL;
    switch(sampType){
        case 0:{ //uniform sampling: uses vecFeatures
            sampler = FeatureAlignment<MeshType, FeatureType>::FeatureUniform(*vecFeatures, &k);
            break;
        }
        case 1:{ //poisson disk sampling: uses poissonMesh
            sampler = FeatureAlignment<MeshType, FeatureType>::FeaturePoisson(*poissonMesh, &k);
            break;
        }
        default: assert(0);
    }

    //store features into the returned vector
    for(int i=0; i<k; ++i){
        vecSubset.push_back(sampler[i]);
        if(cb){ progBar+=offset; cb(int(progBar),"Extracting features..."); }
    }

    if(vecFeatures) delete vecFeatures;   //clear useless data
    if(poissonMesh) delete poissonMesh;
    if(sampler) delete[] sampler;

    return true;
}
