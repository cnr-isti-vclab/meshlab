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

using namespace vcg;
using namespace std;

//class for a multiscale feature based on mean curvature: curvature is computed at differents levels of smoothness
template<class MESH_TYPE, int dim>
class SmoothCurvatureFeature
{
    public:        

    class Parameters
    {
        public:

        enum CurvatureType {GAUSSIAN, MEAN, ABSOLUTE};

        typedef pair<CurvatureType,int> ScaleDescriptionType;

        vector<ScaleDescriptionType>* featureDesc;

        Parameters(){
            featureDesc = new vector<ScaleDescriptionType>();

        }

        ~Parameters(){
            delete featureDesc;
        }

        bool add(CurvatureType cType, int smoothStep){
            if(smoothStep<0 | featureDesc->size()>=dim) return false;
            featureDesc->push_back(make_pair<CurvatureType,int>(cType,smoothStep));
            return true;
        }
    };

    typedef MESH_TYPE MeshType;
    typedef SmoothCurvatureFeature<MeshType,dim> FeatureType;
    typedef typename FeatureType::Parameters ParamType;
    typedef typename MeshType::ScalarType ScalarType;
    typedef typename MeshType::VertexType VertexType;
    typedef typename MeshType::VertexIterator VertexIterator;
    typedef typename MeshType::template PerVertexAttributeHandle<FeatureType*> PVAttributeHandle;
    typedef typename vector<FeatureType*>::iterator VecFeatureIterator;

    Point3<ScalarType> pos;
    Point3<ScalarType> normal;
    float description[dim];           

    SmoothCurvatureFeature(VertexType& v);
    static char* getName();
    static int getRequirements();
    static float getNullValue();
    static bool isNullValue(float);
    static int getFeatureDimension();
    static void SetupParameters( ParamType& param );
    static bool ComputeFeature( MeshType&, ParamType& param, CallBackPos *cb=NULL);
    static bool Subset(int, MeshType&, vector<FeatureType*>&, int, CallBackPos *cb=NULL);
    static bool CheckPersistency(FeatureType* f);

    private:
    static MESH_TYPE* CreateSamplingMesh();
    static int SetupSamplingStructures(MeshType& m, typename MeshType::template PerVertexAttributeHandle<FeatureType*>& fh, MeshType* samplingMesh, vector<FeatureType*>* vecFeatures);
    static pair<float,float> FindMinMax(vector<FeatureType*>& vec, int scale);
    static void SortByBinCount(vector<FeatureType*>& vecFeatures, float min, float max, int histSize, vector<FeatureType*>& sorted);
    static int SpatialSelection(vector<FeatureType*>& container, vector<FeatureType*>& vec, int k, float radius);
    static void PreCleaning(MeshType& m);
};

template<class MESH_TYPE, int dim>
inline SmoothCurvatureFeature<MESH_TYPE,dim>::SmoothCurvatureFeature(VertexType& v)
{
    pos = v.P();
    normal = v.N();
    normal.Normalize();
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
template<class MESH_TYPE, int dim> bool SmoothCurvatureFeature<MESH_TYPE,dim>::CheckPersistency(FeatureType* f)
{
    for(int i = 0; i<FeatureType::getFeatureDimension(); i++)
    {
        if( FeatureType::isNullValue(f->description[i]) ) return false;
    }
    return true;
}

//parameters must be ordered according to smooth iterations
template<class MESH_TYPE, int dim> void SmoothCurvatureFeature<MESH_TYPE,dim>::SetupParameters(ParamType& param)
{
    //param.add(Parameters::GAUSSIAN, 5);
    param.add(Parameters::MEAN, 6);
    //param.add(Parameters::GAUSSIAN, 10);
    param.add(Parameters::MEAN, 12);
    //param.add(Parameters::GAUSSIAN, 15);
    param.add(Parameters::MEAN, 18);
    assert(param.featureDesc->size()==getFeatureDimension());
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

template<class MESH_TYPE, int dim> bool SmoothCurvatureFeature<MESH_TYPE,dim>::ComputeFeature(MeshType &m, ParamType& param, CallBackPos *cb)
{
    //variables needed for progress bar callback
    float progBar = 0.0f;
    float offset = 100.0f/((FeatureType::getFeatureDimension()+2)*m.VertexNumber());
    if(cb) cb(0,"Computing features...");

    //allocates a custom per vertex attribute in which we can store pointers to features in the heap.
    PVAttributeHandle fh = FeatureAlignment<MeshType, FeatureType>::GetFeatureAttribute(m, true);
    if(!tri::Allocator<MeshType>::IsValidHandle(m,fh)) return false;

    //for each vertex delete the related attribute
    for(VertexIterator vi = m.vert.begin(); vi!=m.vert.end(); ++vi){
        if(fh[vi] != NULL){ delete fh[vi]; fh[vi] = NULL; }
    }

    //clear the mesh to avoid wrong values during curvature computations
    PreCleaning(m);

    //copy vertex coords of the mesh before smoothing
    vector<Point3<ScalarType> > oldVertCoords(m.VertexNumber());
    for(unsigned int i = 0; i<m.vert.size(); ++i){
        oldVertCoords[i] = m.vert[i].P();

        if(cb){ progBar+=offset; cb((int)progBar,"Computing features..."); }
    }

    assert(int(oldVertCoords.size())==m.VertexNumber());

    //for all vertexes create feature object in the heap and assign it to the vertex attribute; in the meantime sets trivial values
    for(VertexIterator vi = m.vert.begin(); vi!=m.vert.end(); ++vi) fh[vi] = new FeatureType(*vi);

    //loop trough scale levels
    int smooth_step = 0, smooth_accum = 0; float hmin = 0.4f, hmax = 0.9f;
    for (unsigned int i = 0; i<FeatureType::getFeatureDimension(); i++, smooth_accum+=smooth_step)
    {
        smooth_step = (*param.featureDesc)[i].second - smooth_accum;

        tri::Smooth<MeshType>::VertexCoordLaplacian(m, smooth_step);//smooth mesh
        tri::UpdateCurvature<MeshType>::MeanAndGaussian(m);  //compute curvature
        //copy curvature values in the quality attributes; then build the histogram and take lower and upper bounds.
        switch((*param.featureDesc)[i].first){
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
        float vmin = hist.Percentile(hmin); float vmax = hist.Percentile(hmax);

        //for each vertex, creates a feature, assign it to the attribute, and set its common values. If curvature is beetween bounds
        //and vertex is not a boundary, curvature is stored in the feature, otherwise the feature is set to an empty value.
        for(VertexIterator vi = m.vert.begin(); vi!=m.vert.end(); ++vi)
        {
            if( (!(*vi).IsB()) & ((*vi).Q()<=vmin) | ((*vi).Q()>=vmax) & (!FeatureType::isNullValue((*vi).Q())) )
                fh[vi]->description[i] = (*vi).Q();
            else
                fh[vi]->description[i] = FeatureType::getNullValue();

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
int SmoothCurvatureFeature<MESH_TYPE,dim>::SetupSamplingStructures(MeshType& m, typename MeshType::template PerVertexAttributeHandle<FeatureType*>& fh, MeshType* samplingMesh, vector<FeatureType*>* vecFeatures)
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
            if(vecFeatures) vecFeatures->push_back(fh[vi]);
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
