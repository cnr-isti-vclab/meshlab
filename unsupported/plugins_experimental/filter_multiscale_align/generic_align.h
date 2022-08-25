#ifndef GENERIC_ALIGN_H
#define GENERIC_ALIGN_H


////// Maximum number of points used to compute the GLS descriptor
#define GLS_SUBSAMPLE
#define GLS_MAX_NO_POINTS 20000


#include <common/meshmodel.h>

#include "struct.h"

class GenericAlign
{
public:
    GenericAlign();

    int _seedNumber;

protected:
    //  Given a couple of points, it checks if they are compatible with a
    // list of previous couples, both for distance and difference in normals
    inline bool isScaleOk(float scale1, float scale2, float multiplier)
    {
        if(scale1>=scale2) return (scale1/scale2<=multiplier);
        else               return (scale2/scale1<=multiplier);
    }

    inline float getScaleFromTransf(const vcg::Matrix44f& res)
    {
        vcg::Point3f diag(res[0][0], res[1][1], res[2][2]);
        return sqrt(diag.X()*diag.X()+diag.Y()*diag.Y()+diag.Z()*diag.Z());

    }

    // Given a couple of points, it checks if they are compatible with a list of
    // previous couples, both for distance and difference in normals
    bool isCoupleOk(const CVertexO& firstToAl,
                    const CVertexO& firstRef,
                    const CVertexO& secondToAl,
                    const CVertexO& secondRef,
                    float scale);

    // Check if two points on the toAlign model are far enough to help in
    // the registration
    bool isToAlignOk(int ind,
                     MeshModel* toAlign,
                     const std::vector <std::pair<int, int> >& corrs);


    bool isReferenceOk(int ref,
                       MeshModel* reference,
                       int al,
                       MeshModel* toAlign,
                       const std::vector <std::pair<int, int> > &corrs,
                       float scale);

    std::vector< int >
    selectSeeds(MeshModel* toAlign,
                MeshModel* /*reference*/ ,
                int start=0);

    std::vector< int >
    selectSeedsDescr(DescriptorBase toAlign,
                     const std::vector<DescrPoint> &descrList,
                     bool shuffle);

    bool checkTriplets(const DescriptorBase& toAlign,
                       const DescriptorBase& reference,
                       std::vector<std::pair<int, int> > corrs,
                       const std::vector<Cand>& thirdPoints,
                       float scale,
                       float error);
    bool checkQuadriplets(const DescriptorBase& toAlign,
                          const DescriptorBase& reference,
                          std::vector<std::pair<int, int> > corrs,
                          const std::vector<Cand>& thirdPoints,
                          float scale,
                          float error);

    // Create space to store GLS descriptors and computes it
    inline
    void preComputeDescriptor(DescriptorBase& mesh,
                              const std::vector<float> &scales,
                              bool ompParallel)
    { _preAllocateDescriptor<true>(mesh, scales, ompParallel); }

    // Create space to store GLS descriptors without computing it
    inline
    void preAllocateDescriptor(DescriptorBase& mesh,
                               const std::vector<float> &scales,
                               bool ompParallel)
    { _preAllocateDescriptor<false>(mesh, scales, ompParallel); }


    template <class Fit>
    inline void computeDescriptor( const MyPoint &query,
                                   const std::vector<float>& scales,
                                   const DescriptorBase& base,
                                   std::vector<Fit>* result, bool overwrite = true);

    float ComputeLCP( const vcg::Matrix44f& transformation,
                      float epsilon,
                      const DescriptorBase& P,
                      const DescriptorBase& Q,
                      float prevLCP);

private:
    template <bool compute>
    inline void
    _preAllocateDescriptor(DescriptorBase& mesh,
                           const std::vector<float>& scales,
                           bool ompParallel);
};



/// Compute the GLS descriptor for at the position query and for a given set of scales
template <class Fit>
void
GenericAlign::computeDescriptor( const MyPoint &query,
                                 const std::vector<float>& scales,
                                 const DescriptorBase& base,
                                 std::vector<Fit>* result, bool overwrite)
{

    unsigned int nbScale = scales.size();
    float scaleMax   = base.maxScale;

    // compile time check
    if (! overwrite) {
        if (result->size() == nbScale)
            return;
    }

    if (result->size() != nbScale)
        result->resize(nbScale);

    //QTime time;

    /// 3.1 Collect neighborhood at larger scale
    std::vector<unsigned int> n;
	std::vector<Scalar> squaredDist;

    //time.start();
#pragma omp critical
	vcg::Point3f seed(query.pos().x(), query.pos().y(), query.pos().z());
    base.kdTree->doQueryDist(seed, scaleMax, n, squaredDist);
    std::random_shuffle ( n.begin(), n.end() );
    //cout << "Nei. collection done in " << time.elapsed() << " msec" << endl;

    /// 3.2 Compute and store the fit at multiple scales
    int nId;

    //time.start();
    /// last id of the partially sorted array
    int lastnId = n.size()-1;
    for (int t = scales.size()-1; t >= 0; t--){
        Fit mfit;
        const float& scaleSize = scales[t];
        const float  sqScale   = scaleSize*scaleSize;

        if (scaleSize>base.minScale && scaleSize<=base.maxScale)
        {
#ifdef GLS_SUBSAMPLE
            // initialize descriptor
            mfit.setWeightFunc(WeightFunc(scaleSize));
            mfit.init(query.pos());

            /// last id we take into account wrt to GLS_MAX_NO_POINTS
            int lastReducedId = lastnId < GLS_MAX_NO_POINTS ? lastnId : GLS_MAX_NO_POINTS;
            for (nId = 0; nId <= lastReducedId; ){
                const unsigned int& id  = n[nId]; // it is the original id, as requested when doing the kdist query

                MyPoint p (base.model->cm.vert[id]);
                p.normal().normalize();

                if ((query.pos() - p.pos()).squaredNorm() <= sqScale){
                    mfit.addNeighbor(p);
                    nId++; //must don't go to next point when need to swap
                }else {
                    if (lastnId == 0)
                        break;

                    unsigned int tmp = n[nId];
                    n[nId]     = n[lastnId];
                    n[lastnId] = tmp;

                    lastnId--;

                    lastReducedId = lastnId < GLS_MAX_NO_POINTS ? lastnId : GLS_MAX_NO_POINTS;
                }
            }
            mfit.finalize();
           /* std::cout << scaleSize
                 << " " << mfit.tau()
                 << " " << mfit.kappa()
                 << " " << mfit.tau_normalized()
                 << " " << mfit.kappa_normalized()
                 << endl;*/

#else //GLS_SUBSAMPLE is not defined: Use all points
            // initialize descriptor
            mfit.setWeightFunc(WeightFunc(scaleSize));
            mfit.init(query.pos());

            // compute descriptor
            // we use here a partial sorting of the neighborhood:
            //  - we start by the coarser scale
            //  - each time we get point too far to be considered
            //  - we put it at the end of the neighborhood array
            //
            // Each time a neighbor is swapped, we record it and use the
            // number of swap as right-end array limit.
            for (nId = 0; nId <= lastnId; ){
                const unsigned int& id  = n[nId]; // it is the original id, as requested when doing the kdist query

                MyPoint p (base.model->cm.vert[id]);
                p.normal().normalize();

                if ((query.pos() - p.pos()).squaredNorm() <= sqScale){
                    mfit.addNeighbor(p);
                    nId++; //must don't go to next point when need to swap
                }else {
                    if (lastnId == 0)
                        break;

                    unsigned int tmp = n[nId];
                    n[nId]     = n[lastnId];
                    n[lastnId] = tmp;

                    lastnId--;
                }
            }
            mfit.finalize();
#endif //#ifdef GLS_SUBSAMPLE
        }

        (*result)[t] = mfit;
    }
}


// Precompute the GLS descriptor, only for the selected points
template <bool compute>
void
GenericAlign::_preAllocateDescriptor(DescriptorBase& mesh,
                                   const std::vector<float>& scales,
                                   bool ompParallel){
    typedef typename std::vector<DerivableSphereFit> Container;
    using vcg::tri::Allocator;

    CMeshO::PerVertexAttributeHandle<Container* > descriptorsHandler;
    descriptorsHandler =
            Allocator<CMeshO>::GetPerVertexAttribute<Container* >
            (mesh.model->cm, std::string("GLSDescr"));

#pragma omp parallel for if (ompParallel)
    for(int i = 0; i < mesh.selection.size(); i++){

        unsigned int ind = mesh.selection[i];

        descriptorsHandler[ind] = new std::vector<DerivableSphereFit>();

        if(compute)
            computeDescriptor(mesh.model->cm.vert[ind],
                              scales,
                              mesh,
                              descriptorsHandler[ind]);
    }
}

#endif // GENERIC_ALIGN_H
