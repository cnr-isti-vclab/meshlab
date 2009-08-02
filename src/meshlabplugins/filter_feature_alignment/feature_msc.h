#include <stdio.h>
#include <vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/curvature.h>
#include <vcg/complex/trimesh/update/quality.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/math/histogram.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/complex/trimesh/clustering.h>

#include <meshlabplugins/filter_mls/apss.h>
#include <meshlabplugins/filter_mls/implicits.h>

using namespace vcg;
using namespace std;
using namespace GaelMls;

//class for a multiscale feature based on mean curvature: curvature is computed at differents levels of smoothness
template<class MESH_TYPE, int dim>
class SmoothCurvatureFeature
{
    public:        

    class Parameters
    {
        public:       

        enum CurvatureType {GAUSSIAN, MEAN, ABSOLUTE};

        struct Item
        {
            CurvatureType type;
            int scale;
            float lower_bound, upper_bound;

            Item(CurvatureType _type, int _scale, float _lower_bound = 0.0f, float _upper_bound = 1.0f):
                 type(_type),scale(_scale),lower_bound(_lower_bound),upper_bound(_upper_bound){}
        };

        vector<Item> featureDesc;

        bool add(CurvatureType cType, int smoothStep, float lower_bound = 0.0f, float upper_bound = 1.0f){

            assert(smoothStep>=0 & featureDesc.size()<dim & lower_bound>=0.0f & upper_bound<=1.0f & lower_bound<upper_bound);
            featureDesc.push_back(Item(cType, smoothStep, lower_bound, upper_bound));
            return true;
        }
    };

    typedef MESH_TYPE MeshType;
    typedef SmoothCurvatureFeature<MeshType,dim> FeatureType;
    typedef typename FeatureType::Parameters ParamType;
    typedef typename MeshType::ScalarType ScalarType;
    typedef typename MeshType::VertexType VertexType;
    typedef typename MeshType::VertexIterator VertexIterator;
    typedef typename MeshType::template PerVertexAttributeHandle<FeatureType> PVAttributeHandle;

    Point3<ScalarType> pos;
    Point3<ScalarType> normal;
    float description[dim];           

    SmoothCurvatureFeature();
    SmoothCurvatureFeature(VertexType& v);
    static char* getName();
    static int getRequirements();
    static float getNullValue();
    static bool isNullValue(float);
    static int getFeatureDimension();
    static void SetupParameters(ParamType& param );
    static bool HasBeenComputed(MeshType& m);
    static bool ComputeFeature(MeshType&, ParamType& param, CallBackPos *cb=NULL);
    static bool Subset(int, MeshType&, vector<FeatureType*>&, int, CallBackPos *cb=NULL);
    static bool CheckPersistency(FeatureType f);

    private:
    static MESH_TYPE* CreateSamplingMesh();
    static int SetupSamplingStructures(MeshType& m, typename MeshType::template PerVertexAttributeHandle<FeatureType>& fh, MeshType* samplingMesh, vector<FeatureType*>* vecFeatures);
    static pair<float,float> FindMinMax(vector<FeatureType*>& vec, int scale);
    static void SortByBinCount(vector<FeatureType*>& vecFeatures, float min, float max, int histSize, vector<FeatureType*>& sorted);
    static int SpatialSelection(vector<FeatureType*>& container, vector<FeatureType*>& vec, int k, float radius);
    static void PreCleaning(MeshType& m);
};

template<class MESH_TYPE, int dim>
inline SmoothCurvatureFeature<MESH_TYPE,dim>::SmoothCurvatureFeature(){}

template<class MESH_TYPE, int dim>
inline SmoothCurvatureFeature<MESH_TYPE,dim>::SmoothCurvatureFeature(VertexType& v):pos(v.P()),normal(v.N())
{
    normal.Normalize();
    for(int i=0; i<dim; i++) SmoothCurvatureFeature::getNullValue();
}

template<class MESH_TYPE, int dim> inline char* SmoothCurvatureFeature<MESH_TYPE,dim>::getName()
{
    return "SmoothCurvatureFeature";
}

/* Provides needed attribute to compute feature. A detailed list follows:
    MM_FACEFACETOPO required by RemoveNonManifoldVertex and RemoveNonManifoldFace
    MM_FACEFLAGBORDER required by RemoveNonManifoldFace
    MM_VERTCURV required by curvature computation
    MM_VERTQUALITY required by curvature and histogram computation
*/
template<class MESH_TYPE, int dim> inline int SmoothCurvatureFeature<MESH_TYPE,dim>::getRequirements()
{       
    return (MeshModel::MM_VERTCURV |
            MeshModel::MM_VERTQUALITY |
            MeshModel::MM_FACEFACETOPO |
            MeshModel::MM_FACEFLAGBORDER);
}

template<class MESH_TYPE, int dim> inline bool SmoothCurvatureFeature<MESH_TYPE,dim>::isNullValue(float val)
{
    return ( ((val==FeatureType::getNullValue()) | (math::IsNAN(val))) );
}

template<class MESH_TYPE, int dim> inline float SmoothCurvatureFeature<MESH_TYPE,dim>::getNullValue()
{
    return -std::numeric_limits<float>::max();
}

template<class MESH_TYPE, int dim> inline int SmoothCurvatureFeature<MESH_TYPE,dim>::getFeatureDimension()
{
    return dim;
}

//check persistence beetween scales: return true if description is valid for all scales, false otherwise
template<class MESH_TYPE, int dim> bool SmoothCurvatureFeature<MESH_TYPE,dim>::CheckPersistency(FeatureType f)
{
    for(int i = 0; i<FeatureType::getFeatureDimension(); i++)
    {
        if( FeatureType::isNullValue(f.description[i]) ) return false;
    }
    return true;
}

//parameters must be ordered according to smooth iterations
template<class MESH_TYPE, int dim> void SmoothCurvatureFeature<MESH_TYPE,dim>::SetupParameters(ParamType& param)
{
    param.add(Parameters::GAUSSIAN, 5, 0.4f, 0.9f);
    param.add(Parameters::MEAN, 5, 0.4f, 0.9f);
    param.add(Parameters::GAUSSIAN, 10, 0.4f, 0.9f);
    param.add(Parameters::MEAN, 10, 0.4f, 0.9f);
    param.add(Parameters::GAUSSIAN, 15, 0.4f, 0.9f);
    param.add(Parameters::MEAN, 15, 0.4f, 0.9f);
    assert(int(param.featureDesc.size())==getFeatureDimension());
}

template<class MESH_TYPE, int dim> void SmoothCurvatureFeature<MESH_TYPE,dim>::PreCleaning(MeshType& m)
{
    tri::Clean<MeshType>::RemoveZeroAreaFace(m);
    tri::Clean<MeshType>::RemoveDuplicateFace(m);
    tri::Clean<MeshType>::RemoveDuplicateVertex(m);
    tri::Clean<MeshType>::RemoveNonManifoldFace(m);
    //tri::Clean<MeshType>::RemoveNonManifoldVertex(m);
    tri::Clean<MeshType>::RemoveUnreferencedVertex(m);
    tri::Allocator<MeshType>::CompactVertexVector(m);
}

template<class MESH_TYPE, int dim> bool SmoothCurvatureFeature<MESH_TYPE,dim>::HasBeenComputed(MeshType &m)
{
    //checks if the attribute exist
    return tri::HasPerVertexAttribute(m,std::string(FeatureType::getName()));
}

template<class MESH_TYPE, int dim> bool SmoothCurvatureFeature<MESH_TYPE,dim>::ComputeFeature(MeshType &m, ParamType& param, CallBackPos *cb)
{
    //variables needed for progress bar callback
    float progBar = 0.0f;
    float offset = 100.0f/((FeatureType::getFeatureDimension()+2)*m.VertexNumber());
    if(cb) cb(0,"Computing features...");

    //allocates a custom per vertex attribute in which we can store pointers to features in the heap.
    PVAttributeHandle fh = FeatureAlignment<MeshType, FeatureType>::GetFeatureAttribute(m, true);
    if(!tri::Allocator<MeshType>::IsValidHandle(m,fh)) return false;

    //clear the mesh to avoid wrong values during curvature computations
    PreCleaning(m);

    //copy vertex coords of the mesh before smoothing
    vector<Point3<ScalarType> > oldVertCoords(m.VertexNumber());
    for(unsigned int i = 0; i<m.vert.size(); ++i){
        oldVertCoords[i] = m.vert[i].P();

        if(cb){ progBar+=offset; cb((int)progBar,"Computing features..."); }
    }

    assert(int(oldVertCoords.size())==m.VertexNumber());

    //creates a feature for each vertex
    for(VertexIterator vi = m.vert.begin(); vi!=m.vert.end(); ++vi) fh[vi] = FeatureType(*vi);

    //loop trough scale levels
    int smooth_step = 0, smooth_accum = 0;
    for (unsigned int i = 0; i<FeatureType::getFeatureDimension(); i++, smooth_accum+=smooth_step)
    {
        smooth_step = param.featureDesc[i].scale - smooth_accum;

        tri::Smooth<MeshType>::VertexCoordLaplacian(m, smooth_step);//smooth mesh
        tri::UpdateCurvature<MeshType>::MeanAndGaussian(m);  //compute curvature
        //copy curvature values in the quality attributes; then build the histogram and take lower and upper bounds.
        switch(param.featureDesc[i].type){
            case Parameters::GAUSSIAN:{
                tri::UpdateQuality<MeshType>::VertexFromGaussianCurvature(m);
                break;
            }
            case Parameters::MEAN:{
                tri::UpdateQuality<MeshType>::VertexFromMeanCurvature(m);
                break;
            }
            case Parameters::ABSOLUTE:{
                tri::UpdateQuality<MeshType>::VertexFromAbsoluteCurvature(m);
                break;
            }
            default: assert(0);
        }
        Histogram<ScalarType> hist = Histogram<ScalarType>();
        tri::Stat<MeshType>::ComputePerVertexQualityHistogram(m, hist);
        float vmin = hist.Percentile(param.featureDesc[i].lower_bound); float vmax = hist.Percentile(param.featureDesc[i].upper_bound);

        //If curvature is beetween bounds and vertex is not a boundary, curvature is stored in the feature, otherwise the feature is set to an empty value.
        for(VertexIterator vi = m.vert.begin(); vi!=m.vert.end(); ++vi)
        {
            if( (!(*vi).IsB()) & ((*vi).Q()<=vmin) | ((*vi).Q()>=vmax) & (!FeatureType::isNullValue((*vi).Q())) )
                fh[vi].description[i] = (*vi).Q();
            else
                fh[vi].description[i] = FeatureType::getNullValue();

            if(cb){ progBar+=offset; cb((int)progBar,"Computing features..."); }
        }
    }
    //restore old coords
    for(unsigned int i = 0; i<m.vert.size(); ++i){
        m.vert[i].P() = oldVertCoords[i];

        if(cb){ progBar+=offset; cb((int)progBar,"Computing features..."); }
    }

    return true;
}

template<class MESH_TYPE, int dim>
MESH_TYPE* SmoothCurvatureFeature<MESH_TYPE,dim>::CreateSamplingMesh()
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
int SmoothCurvatureFeature<MESH_TYPE,dim>::SetupSamplingStructures(MeshType& m, typename MeshType::template PerVertexAttributeHandle<FeatureType>& fh, MeshType* samplingMesh, vector<FeatureType*>* vecFeatures)
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
        if( FeatureType::CheckPersistency(fh[vi])){
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
bool SmoothCurvatureFeature<MESH_TYPE,dim>::Subset(int k, MeshType &m, vector<FeatureType*> &vecSubset, int sampType, CallBackPos *cb)
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

    ////UNCOMMENT FOLLOW TO GET A RARE FEATURE SELECTION////
/*
    int histSize = 10000;  //number of bins of the histogram
    vector<FeatureType*>* sorted = new vector<FeatureType*>();  //vector that holds features sorted by bin cont

    //compute min val e max val between all features; min and max are needed to bound the histogram
    pair<float,float> minmax = FindMinMax(*vecFeatures, 0);

    //fill multimap with features sorted by bin count
    SortByBinCount(*vecFeatures, minmax.first, minmax.second, histSize, *sorted);

    //select the first k entries from mulptimap, and put them into vecSubset
    typename vector<FeatureType*>::iterator it = sorted->begin();
    ///UNCOMMENT THIS LOOP FOR A SORTED SELECTION
    for(int i=0; i<k & it!=sorted->end(); i++, it++)
    {
        (*it)->selected = true;        //mark features as selected
        vecSubset.push_back(*it);
    }
    ///UNCOMMENT THIS LOOP FOR A EQUALIZED SELECTION
    int off = int(sorted->size()/k);
    for(it = sorted->begin(); it<sorted->end(); it=it+off)
    {
        (*it)->selected = true;        //mark features as selected
        vecSubset.push_back(*it);
    }

    //delete vector and set pointer to NULL for safety
    if(vecFeatures) delete vecFeatures;   //clear useless data
    sorted->clear();
    delete sorted;
    sorted = NULL;
    return true;
*/
}

//scan the vector of features and return a pair containig the min and max description values
template<class MESH_TYPE, int dim> pair<float,float> SmoothCurvatureFeature<MESH_TYPE,dim>::FindMinMax(vector<FeatureType*>& vec, int scale)
{
    assert(scale>=0 && scale<FeatureType::GetFeatureDimension());

    typename vector<FeatureType*>::iterator vi;
    pair<float,float> minmax = make_pair(numeric_limits<float>::max(),-numeric_limits<float>::max());
    for(vi = vec.begin(); vi!=vec.end(); ++vi)
    {
        if( !FeatureType::isNullValue((*vi)->description[scale]))  //invalid values are discarded
        {
            if( (*vi)->description[scale] < minmax.first) minmax.first=(*vi)->description[scale];
            if( (*vi)->description[scale] > minmax.second ) minmax.second=(*vi)->description[scale];
        }
    }
    return minmax;
}

//fill a vector that holds features pointers sorted by bin count; i.e first all the very infrequent features and last all the very frequent ones.
template<class MESH_TYPE, int dim> void SmoothCurvatureFeature<MESH_TYPE,dim>::SortByBinCount(vector<FeatureType*>& vecFeatures, float min, float max, int histSize, vector<FeatureType*>& sorted)
{
    //vector to hold bins ranges
    vector<float>* bins = new vector<float>(histSize, 0);
    //vector to count content of each bin
    vector<int> *hToC = new vector<int>(histSize, 0);
    //multimap to keep track of features for each bin
    multimap<int, FeatureType* >* hToF = new multimap<int, FeatureType* >();
    //multimap to store pairs of (count, FeatureType*). multimap is naturally sorted by count.
    multimap<int, FeatureType* >* cToF = new multimap<int, FeatureType* >();

    //offset beetween min and max is divided into histSize uniform bins
    for(unsigned int i = 0; i<bins->size(); i++) bins->at(i) = min + i*(max-min)/float(histSize);

    //build histogram, hToF and hToC; all with just one features scan
    typename multimap<int, FeatureType* >::iterator hfIt = hToF->begin();
    typename vector<FeatureType*>::iterator vi;
    for(vi = vecFeatures.begin(); vi!=vecFeatures.end(); ++vi)
    {
        float val = (*vi)->description[0];
        // lower_bound returns an iterator pointing to the first element "not less than" val, or end() if every element is less than val.
        typename vector<float>::iterator it = lower_bound(bins->begin(),bins->end(),val);
        //if val is out of range, skip iteration and take in account next val
        if(it==bins->begin() || it==bins->end()) continue;
        //determine in which bin got to insert val
        int binId = (it - bins->begin())-1;
        assert(binId>=0);
        assert (bins->at(binId) < val);
        assert (val <= bins->at(binId+1));

        //increment bin count
        hToC->at(binId)++;
        //remember in which bin has been inserted this feature
        hfIt = hToF->insert(hfIt,make_pair(binId, (*vi)));
    }

    //delete bins and set pointer to NULL for safety
    bins->clear();
    delete bins;
    bins = NULL;

    //for all bin index insert in the multimap an entry with key bin count and value a feature. Entries are naturally
    //sorted by key in the multimap
    typename multimap<int, FeatureType* >::iterator cfIt = cToF->begin();
    pair< typename multimap<int, FeatureType* >::iterator, typename multimap<int, FeatureType* >::iterator> range;
    for(unsigned int i=0; i<hToC->size(); i++)
    {
        //if bin count is zero, then skip; nothing to put in the multimap
        if(hToC->at(i)!=0)
        {
            range = hToF->equal_range(i);
            assert(range.first!=range.second);
            for (; range.first!=range.second; ++range.first)
            {
                cfIt = cToF->insert( cfIt, make_pair(hToC->at(i), (*range.first).second) );
            }
        }
    }

    typename multimap<int, FeatureType* >::iterator it;
    for(it = cToF->begin(); it != cToF->end(); it++)
        sorted.push_back((*it).second);

    assert(sorted.size()==cToF->size());

    //delete all ausiliary structures and set pointers to NULL for safety
    hToF->clear();
    delete hToF;
    hToF = NULL;
    hToC->clear();
    delete hToC;
    hToC = NULL;
    cToF->clear();
    delete cToF;
    cToF = NULL;
}

template<class MESH_TYPE, int dim> int SmoothCurvatureFeature<MESH_TYPE,dim>::SpatialSelection(vector<FeatureType*>& container, vector<FeatureType*>& vec, int k, float radius)
{
    //variables to manage the kDTree which works on features position
    ANNpointArray   dataPts = NULL;	// data points
    ANNpoint        queryPnt = NULL;	// query points
    ANNkd_tree*     kdTree = NULL;	// search structure

    queryPnt = annAllocPt(3);

    typename vector<FeatureType*>::iterator ci = container.begin();
    while(ci != container.end() && vec.size()<k)
    {
        if(vec.size()==0)
        {
            vec.push_back(*ci);
            (*ci)->selected = true;
            ci++;
            continue;
        }

        if(dataPts){ annDeallocPts(dataPts); dataPts = NULL; }
        if(kdTree){ delete kdTree; kdTree = NULL; }
        dataPts = annAllocPts(vec.size(), 3);

        for(int i = 0; i < vec.size(); i++)
        {
            for (int j = 0; j < 3; j++)
            {
                (dataPts[i])[j] = (ANNcoord)(vec.at(i)->pos[j]);
            }
        }
        //build search structure
        kdTree = new ANNkd_tree(dataPts,vec.size(),3);

        for (int j = 0; j < 3; j++)
        {
            queryPnt[j] = (ANNcoord)((*ci)->pos[j]);
        }

        //if there aren't features yet selected in the range distance
        if(!kdTree->annkFRSearch(queryPnt, math::Sqr(radius), 0, NULL, NULL, 0.0))
        {
             vec.push_back(*ci);
             (*ci)->selected = true;
        }
        ci++;
    }

    if(dataPts){ annDeallocPts(dataPts); dataPts = NULL; }
    if(queryPnt){ annDeallocPt(queryPnt); queryPnt = NULL; }
    if(kdTree){ delete kdTree; kdTree = NULL; }

    return vec.size();
}

//class for a multiscale feature based on APSS curvature. Works with point clouds.
template<class MESH_TYPE, int dim>
class APSSCurvatureFeature
{
    public:

    class Parameters
    {
        public:

        enum CurvatureType { MEAN, GAUSSIAN, K1, K2, APSS};

        struct Item
        {
            public:
            CurvatureType type;
            float lower_bound, upper_bound, scale;  //indicates how much sub sample the mesh. 1 = whole mesh, 0.5 = half mesh, etc

            Item(CurvatureType _type, float _scale, float _lower_bound = 0.0f, float _upper_bound = 1.0f):
                 type(_type),scale(_scale),lower_bound(_lower_bound),upper_bound(_upper_bound){}
        };

        vector<Item> featureDesc;

        bool add(CurvatureType cType, float subSampAmount, float lower_bound = 0.0f, float upper_bound = 1.0f)
        {
            assert(subSampAmount>=0 & subSampAmount<=1 & featureDesc.size()<dim & lower_bound>=0.0f & upper_bound<=1.0f & lower_bound<upper_bound);
            featureDesc.push_back(Item(cType, subSampAmount, lower_bound, upper_bound));
            return true;
        }
    };

    typedef MESH_TYPE MeshType;
    typedef APSSCurvatureFeature<MeshType,dim> FeatureType;
    typedef typename FeatureType::Parameters ParamType;
    typedef typename MeshType::ScalarType ScalarType;
    typedef typename MeshType::VertexType VertexType;
    typedef typename MeshType::VertexIterator VertexIterator;
    typedef typename MeshType::template PerVertexAttributeHandle<FeatureType> PVAttributeHandle;

    Point3<ScalarType> pos;
    Point3<ScalarType> normal;
    float description[dim];

    APSSCurvatureFeature();
    APSSCurvatureFeature(VertexType& v);
    static char* getName();
    static int getRequirements();
    static float getNullValue();
    static bool isNullValue(float);
    static int getFeatureDimension();
    static void SetupParameters(ParamType& param );
    static bool HasBeenComputed(MeshType& m);
    static bool ComputeFeature(MeshType&, ParamType& param, CallBackPos *cb=NULL);
    static bool Subset(int, MeshType&, vector<FeatureType*>&, int, CallBackPos *cb=NULL);
    static bool CheckPersistency(FeatureType f);

    private:
    static bool ComputeAPSS(MeshType& m, int type, float filterScale, int maxProjIter, float projAcc, float sphPar, bool accNorm, bool selectionOnly);
    static MESH_TYPE* CreateSamplingMesh();
    static int SetupSamplingStructures(MeshType& m, typename MeshType::template PerVertexAttributeHandle<FeatureType>& fh, MeshType* samplingMesh, vector<FeatureType*>* vecFeatures);
    static pair<float,float> FindMinMax(vector<FeatureType*>& vec, int scale);
    static void SortByBinCount(vector<FeatureType*>& vecFeatures, float min, float max, int histSize, vector<FeatureType*>& sorted);
    static int SpatialSelection(vector<FeatureType*>& container, vector<FeatureType*>& vec, int k, float radius);
    static void PreCleaning(MeshType& m);
};

template<class MESH_TYPE, int dim>
inline APSSCurvatureFeature<MESH_TYPE,dim>::APSSCurvatureFeature(){}

template<class MESH_TYPE, int dim>
inline APSSCurvatureFeature<MESH_TYPE,dim>::APSSCurvatureFeature(VertexType& v):pos(v.P()),normal(v.N())
{
    normal.Normalize();
    for(int i=0; i<dim; i++) APSSCurvatureFeature::getNullValue();
}

template<class MESH_TYPE, int dim> inline char* APSSCurvatureFeature<MESH_TYPE,dim>::getName()
{
    return "APSSCurvatureFeature";
}

/* Provides needed attribute to compute feature. A detailed list follows:
    MM_VERTCURV required by curvature computation
    MM_VERTQUALITY required by curvature and histogram computation
    MM_VERTRADIUS required by APSS curvature computation
    MM_VERTCURVDIR required by APSS curvature computation
*/
template<class MESH_TYPE, int dim> inline int APSSCurvatureFeature<MESH_TYPE,dim>::getRequirements()
{
    return (MeshModel::MM_VERTCURVDIR | MeshModel::MM_VERTQUALITY | MeshModel::MM_VERTRADIUS);
}

template<class MESH_TYPE, int dim> inline bool APSSCurvatureFeature<MESH_TYPE,dim>::isNullValue(float val)
{
    return ( ((val==FeatureType::getNullValue()) | (math::IsNAN(val))) );
}

template<class MESH_TYPE, int dim> inline float APSSCurvatureFeature<MESH_TYPE,dim>::getNullValue()
{
    return -std::numeric_limits<float>::max();
}

template<class MESH_TYPE, int dim> inline int APSSCurvatureFeature<MESH_TYPE,dim>::getFeatureDimension()
{
    return dim;
}

//check persistence beetween scales: return true if description is valid for all scales, false otherwise
template<class MESH_TYPE, int dim> bool APSSCurvatureFeature<MESH_TYPE,dim>::CheckPersistency(FeatureType f)
{
    for(int i = 0; i<FeatureType::getFeatureDimension(); i++)
    {
        if( FeatureType::isNullValue(f.description[i]) ) return false;
    }
    return true;
}

//parameters must be ordered according to smooth iterations
template<class MESH_TYPE, int dim> void APSSCurvatureFeature<MESH_TYPE,dim>::SetupParameters(ParamType& param)
{
    param.add(Parameters::MEAN, 1.0f, 0.3f, 0.9f);
    param.add(Parameters::MEAN, 0.75f, 0.3f, 0.9f);
    param.add(Parameters::MEAN, 0.5f, 0.3f, 0.9f);
    assert(int(param.featureDesc.size())==getFeatureDimension());
}

template<class MESH_TYPE, int dim> void APSSCurvatureFeature<MESH_TYPE,dim>::PreCleaning(MeshType& m)
{
    //if we are not working on point clouds, clean up faces
    if(m.fn>0)
    {
        tri::Clean<MeshType>::RemoveZeroAreaFace(m);
        tri::Clean<MeshType>::RemoveDuplicateFace(m);
        tri::Clean<MeshType>::RemoveDuplicateVertex(m);
        tri::Clean<MeshType>::RemoveUnreferencedVertex(m);
    }
    tri::Allocator<MeshType>::CompactVertexVector(m);
}

template<class MESH_TYPE, int dim> bool APSSCurvatureFeature<MESH_TYPE,dim>::ComputeAPSS(MeshType& m, int type = 0, float filterScale = 2.0f, int maxProjIter = 15, float projAcc = 1e-4f, float sphPar = 1.0f, bool accNorm = true, bool selectionOnly=true)
{
    // create the MLS surface
    APSS<MeshType>* mls = new APSS<MeshType>(m);

    // We require a per vertex radius so as a first thing compute it
    mls->computeVertexRaddi();

    mls->setFilterScale(filterScale);
    mls->setMaxProjectionIters(maxProjIter);
    mls->setProjectionAccuracy(projAcc);
    mls->setSphericalParameter(sphPar);
    mls->setGradientHint(accNorm ? GaelMls::MLS_DERIVATIVE_ACCURATE : GaelMls::MLS_DERIVATIVE_APPROX);

    uint size = m.vert.size();
    vcg::Point3f grad;
    vcg::Matrix33f hess;

    //computes curvatures and statistics
    for (unsigned int i = 0; i< size; i++)
    {
        if ( (!selectionOnly) || (m.vert[i].IsS()) )
        {
            Point3f p = mls->project(m.vert[i].P());
            float c = 0;

            if (type==Parameters::APSS) c = mls->approxMeanCurvature(p);
            else
            {
                int errorMask;
                grad = mls->gradient(p, &errorMask);
                if (errorMask == MLS_OK && grad.Norm() > 1e-8)
                {
                    hess = mls->hessian(p, &errorMask);
                    implicits::WeingartenMap<float> W(grad,hess);

                    m.vert[i].PD1() = W.K1Dir();
                    m.vert[i].PD2() = W.K2Dir();
                    m.vert[i].K1() =  W.K1();
                    m.vert[i].K2() =  W.K2();

                    switch(type){
                        case Parameters::MEAN: c = W.MeanCurvature(); break;
                        case Parameters::GAUSSIAN: c = W.GaussCurvature(); break;
                        case Parameters::K1: c = W.K1(); break;
                        case Parameters::K2: c = W.K2(); break;
                        default: assert(0 && "invalid curvature type");
                    }
                }
                assert(!math::IsNAN(c) && "You should never try to compute Histogram with Invalid Floating points numbers (NaN)");
            }
            m.vert[i].Q() = c;
        }
    }

    delete mls;

    return true;
}

template<class MESH_TYPE, int dim> bool APSSCurvatureFeature<MESH_TYPE,dim>::HasBeenComputed(MeshType &m)
{
    //checks if the attribute exist
    return tri::HasPerVertexAttribute(m,std::string(FeatureType::getName()));
}

template<class MESH_TYPE, int dim> bool APSSCurvatureFeature<MESH_TYPE,dim>::ComputeFeature(MeshType &m, ParamType& param, CallBackPos *cb)
{
    //variables needed for progress bar callback
    float progBar = 0.0f;
    float offset = 100.0f/((FeatureType::getFeatureDimension())*m.VertexNumber());
    if(cb) cb(0,"Computing features...");

    //allocates a custom per vertex attribute in which we can store pointers to features in the heap.
    PVAttributeHandle fh = FeatureAlignment<MeshType, FeatureType>::GetFeatureAttribute(m, true);
    if(!tri::Allocator<MeshType>::IsValidHandle(m,fh)) return false;

    //clear the mesh to avoid wrong values during curvature computations
    PreCleaning(m);

    //creates a feature for each vertex
    for(VertexIterator vi = m.vert.begin(); vi!=m.vert.end(); ++vi) fh[vi] = FeatureType(*vi);

    //loop trough scale levels
    for (unsigned int i = 0; i<FeatureType::getFeatureDimension(); i++)
    {
        //compute the amount of verteces desidered for this scale
        int targetSize = int(m.VertexNumber()*param.featureDesc[i].scale);
        //set up the clustering structure and perform a logaritmic search in order to get a number
        //of clustered vertex very close to the target number
        tri::Clustering<MeshType, tri::NearestToCenter<MeshType> > ClusteringGrid;
        bool done = false;
        bool onlySelected = (i==0) ? false : true; //we subsample the whole mesh just the first time, then we subsample from the previous scale!
        int size = 10*targetSize, errSize = int(0.02f*targetSize), inf = 0, sup = 0;
        do{
            ClusteringGrid.Init(m.bbox,size);
            ClusteringGrid.AddPointSet(m,onlySelected);
            int sel = ClusteringGrid.CountPointSet();
            if(sel<targetSize-errSize){
                inf = size;
                if(sup) size+=(sup-inf)/2;
                else size*=2;
            }
            else if(sel>targetSize+errSize){
                sup = size;
                if(inf) size-=(sup-inf)/2;
                else size/=2;
            }
            else done=true;
        }while(!done);

        //perform clustering. this set some vertesec of m as selected
        ClusteringGrid.SelectPointSet(m);

        ComputeAPSS(m);  //compute curvature

        //compute curvature histogram just on selected verteces
        Histogram<ScalarType> hist = Histogram<ScalarType>();
        tri::Stat<MeshType>::ComputePerVertexQualityHistogram(m, hist, true);
        float vmin = hist.Percentile(param.featureDesc[i].lower_bound); float vmax = hist.Percentile(param.featureDesc[i].upper_bound);

        //If curvature is beetween bounds and vertex is selected and it is not a boundary, curvature is stored in the feature, otherwise the feature is set to an empty value.
        for(VertexIterator vi = m.vert.begin(); vi!=m.vert.end(); ++vi)
        {
            if( ((*vi).IsS()) & ((!(*vi).IsB()) & ((*vi).Q()<=vmin) | ((*vi).Q()>=vmax) & (!FeatureType::isNullValue((*vi).Q()))) )
                fh[vi].description[i] = (*vi).Q();
            else
                fh[vi].description[i] = FeatureType::getNullValue();

            if(cb){ progBar+=offset; cb((int)progBar,"Computing features..."); }
        }
    }
    return true;
}

template<class MESH_TYPE, int dim>
MESH_TYPE* APSSCurvatureFeature<MESH_TYPE,dim>::CreateSamplingMesh()
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
int APSSCurvatureFeature<MESH_TYPE,dim>::SetupSamplingStructures(MeshType& m, typename MeshType::template PerVertexAttributeHandle<FeatureType>& fh, MeshType* samplingMesh, vector<FeatureType*>* vecFeatures)
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
        if( FeatureType::CheckPersistency(fh[vi])){
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
bool APSSCurvatureFeature<MESH_TYPE,dim>::Subset(int k, MeshType &m, vector<FeatureType*> &vecSubset, int sampType, CallBackPos *cb)
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

    ////UNCOMMENT FOLLOW TO GET A RARE FEATURE SELECTION////
/*
    int histSize = 10000;  //number of bins of the histogram
    vector<FeatureType*>* sorted = new vector<FeatureType*>();  //vector that holds features sorted by bin cont

    //compute min val e max val between all features; min and max are needed to bound the histogram
    pair<float,float> minmax = FindMinMax(*vecFeatures, 0);

    //fill multimap with features sorted by bin count
    SortByBinCount(*vecFeatures, minmax.first, minmax.second, histSize, *sorted);

    //select the first k entries from mulptimap, and put them into vecSubset
    typename vector<FeatureType*>::iterator it = sorted->begin();
    ///UNCOMMENT THIS LOOP FOR A SORTED SELECTION
    for(int i=0; i<k & it!=sorted->end(); i++, it++)
    {
        (*it)->selected = true;        //mark features as selected
        vecSubset.push_back(*it);
    }
    ///UNCOMMENT THIS LOOP FOR A EQUALIZED SELECTION
    int off = int(sorted->size()/k);
    for(it = sorted->begin(); it<sorted->end(); it=it+off)
    {
        (*it)->selected = true;        //mark features as selected
        vecSubset.push_back(*it);
    }

    //delete vector and set pointer to NULL for safety
    if(vecFeatures) delete vecFeatures;   //clear useless data
    sorted->clear();
    delete sorted;
    sorted = NULL;
    return true;
*/
}

//scan the vector of features and return a pair containig the min and max description values
template<class MESH_TYPE, int dim> pair<float,float> APSSCurvatureFeature<MESH_TYPE,dim>::FindMinMax(vector<FeatureType*>& vec, int scale)
{
    assert(scale>=0 && scale<FeatureType::GetFeatureDimension());

    typename vector<FeatureType*>::iterator vi;
    pair<float,float> minmax = make_pair(numeric_limits<float>::max(),-numeric_limits<float>::max());
    for(vi = vec.begin(); vi!=vec.end(); ++vi)
    {
        if( !FeatureType::isNullValue((*vi)->description[scale]))  //invalid values are discarded
        {
            if( (*vi)->description[scale] < minmax.first) minmax.first=(*vi)->description[scale];
            if( (*vi)->description[scale] > minmax.second ) minmax.second=(*vi)->description[scale];
        }
    }
    return minmax;
}

//fill a vector that holds features pointers sorted by bin count; i.e first all the very infrequent features and last all the very frequent ones.
template<class MESH_TYPE, int dim> void APSSCurvatureFeature<MESH_TYPE,dim>::SortByBinCount(vector<FeatureType*>& vecFeatures, float min, float max, int histSize, vector<FeatureType*>& sorted)
{
    //vector to hold bins ranges
    vector<float>* bins = new vector<float>(histSize, 0);
    //vector to count content of each bin
    vector<int> *hToC = new vector<int>(histSize, 0);
    //multimap to keep track of features for each bin
    multimap<int, FeatureType* >* hToF = new multimap<int, FeatureType* >();
    //multimap to store pairs of (count, FeatureType*). multimap is naturally sorted by count.
    multimap<int, FeatureType* >* cToF = new multimap<int, FeatureType* >();

    //offset beetween min and max is divided into histSize uniform bins
    for(unsigned int i = 0; i<bins->size(); i++) bins->at(i) = min + i*(max-min)/float(histSize);

    //build histogram, hToF and hToC; all with just one features scan
    typename multimap<int, FeatureType* >::iterator hfIt = hToF->begin();
    typename vector<FeatureType*>::iterator vi;
    for(vi = vecFeatures.begin(); vi!=vecFeatures.end(); ++vi)
    {
        float val = (*vi)->description[0];
        // lower_bound returns an iterator pointing to the first element "not less than" val, or end() if every element is less than val.
        typename vector<float>::iterator it = lower_bound(bins->begin(),bins->end(),val);
        //if val is out of range, skip iteration and take in account next val
        if(it==bins->begin() || it==bins->end()) continue;
        //determine in which bin got to insert val
        int binId = (it - bins->begin())-1;
        assert(binId>=0);
        assert (bins->at(binId) < val);
        assert (val <= bins->at(binId+1));

        //increment bin count
        hToC->at(binId)++;
        //remember in which bin has been inserted this feature
        hfIt = hToF->insert(hfIt,make_pair(binId, (*vi)));
    }

    //delete bins and set pointer to NULL for safety
    bins->clear();
    delete bins;
    bins = NULL;

    //for all bin index insert in the multimap an entry with key bin count and value a feature. Entries are naturally
    //sorted by key in the multimap
    typename multimap<int, FeatureType* >::iterator cfIt = cToF->begin();
    pair< typename multimap<int, FeatureType* >::iterator, typename multimap<int, FeatureType* >::iterator> range;
    for(unsigned int i=0; i<hToC->size(); i++)
    {
        //if bin count is zero, then skip; nothing to put in the multimap
        if(hToC->at(i)!=0)
        {
            range = hToF->equal_range(i);
            assert(range.first!=range.second);
            for (; range.first!=range.second; ++range.first)
            {
                cfIt = cToF->insert( cfIt, make_pair(hToC->at(i), (*range.first).second) );
            }
        }
    }

    typename multimap<int, FeatureType* >::iterator it;
    for(it = cToF->begin(); it != cToF->end(); it++)
        sorted.push_back((*it).second);

    assert(sorted.size()==cToF->size());

    //delete all ausiliary structures and set pointers to NULL for safety
    hToF->clear();
    delete hToF;
    hToF = NULL;
    hToC->clear();
    delete hToC;
    hToC = NULL;
    cToF->clear();
    delete cToF;
    cToF = NULL;
}

template<class MESH_TYPE, int dim> int APSSCurvatureFeature<MESH_TYPE,dim>::SpatialSelection(vector<FeatureType*>& container, vector<FeatureType*>& vec, int k, float radius)
{
    //variables to manage the kDTree which works on features position
    ANNpointArray   dataPts = NULL;	// data points
    ANNpoint        queryPnt = NULL;	// query points
    ANNkd_tree*     kdTree = NULL;	// search structure

    queryPnt = annAllocPt(3);

    typename vector<FeatureType*>::iterator ci = container.begin();
    while(ci != container.end() && vec.size()<k)
    {
        if(vec.size()==0)
        {
            vec.push_back(*ci);
            (*ci)->selected = true;
            ci++;
            continue;
        }

        if(dataPts){ annDeallocPts(dataPts); dataPts = NULL; }
        if(kdTree){ delete kdTree; kdTree = NULL; }
        dataPts = annAllocPts(vec.size(), 3);

        for(int i = 0; i < vec.size(); i++)
        {
            for (int j = 0; j < 3; j++)
            {
                (dataPts[i])[j] = (ANNcoord)(vec.at(i)->pos[j]);
            }
        }
        //build search structure
        kdTree = new ANNkd_tree(dataPts,vec.size(),3);

        for (int j = 0; j < 3; j++)
        {
            queryPnt[j] = (ANNcoord)((*ci)->pos[j]);
        }

        //if there aren't features yet selected in the range distance
        if(!kdTree->annkFRSearch(queryPnt, math::Sqr(radius), 0, NULL, NULL, 0.0))
        {
             vec.push_back(*ci);
             (*ci)->selected = true;
        }
        ci++;
    }

    if(dataPts){ annDeallocPts(dataPts); dataPts = NULL; }
    if(queryPnt){ annDeallocPt(queryPnt); queryPnt = NULL; }
    if(kdTree){ delete kdTree; kdTree = NULL; }

    return vec.size();
}
