#ifndef STRUCT_H
#define STRUCT_H


#include <common/meshmodel.h>

#include "Patate/grenaille.h"

#include "Dynamic/dynamicSolver.h"
#include "Dynamic/needlemanWunsch.h"
#include "Dynamic/scaleEstimation.h"

//#include "localKdTree.h"
#include <vcg\space\index\kdtree\kdtree.h>

//// Candidates: the points that could be part of the matching
struct Options
{
  bool checkBorders;
  bool useDescriptive;
  float alignError;
  float useQuadriplets;
  float expectedScale;
  int seedNumber;
  float delta;
};

//// Candidates: the points that could be part of the matching
struct Cand
{
  int  ind;
  float scale;
  float confidence;
  bool operator()(Cand const &a, Cand const &b) {
        return a.confidence > b.confidence;
    }
};

struct SiftPoint
{
    int ind;
    int scaleId;
    bool operator()(const SiftPoint &a, const SiftPoint &b) {
          return a < b;
      }
    bool operator<(const SiftPoint &other) const{
        return scaleId < other.scaleId;
    }
};

struct DescrPoint
{
    int ind;
    float descrip;
    bool operator()(const DescrPoint &a, const DescrPoint &b) {
          return a < b;
      }
    bool operator<(const DescrPoint &other) const{
        return descrip < other.descrip;
    }
};



struct SeedCandidate
{
    int ind;
    float impact;
    bool operator()(const SeedCandidate &a, const SeedCandidate &b) {
          return a < b;
      }
    bool operator<(const SeedCandidate &other) const{
        return impact < other.impact;
    }
};


// This class defines the input data format
class MyPoint{
public:
    enum {Dim = 3};
    typedef float Scalar;
    typedef Eigen::Matrix<Scalar, Dim, 1> VectorType;
    typedef Eigen::Matrix<Scalar, Dim, Dim> MatrixType; // Type needed by Grenaille::AlgebraicSphere
    MULTIARCH inline MyPoint(const VectorType &pos = VectorType::Zero(),
                             const VectorType& normal = VectorType::Zero())
        : _pos(pos), _normal(normal) {}

    // \todo Is it correct ?
    MULTIARCH inline MyPoint(const CVertexO &v)
        { v.P().ToEigenVector(_pos);
          v.N().ToEigenVector(_normal); }
    MULTIARCH inline const VectorType& pos() const { return _pos; }
    MULTIARCH inline const VectorType& normal() const { return _normal; }
    MULTIARCH inline VectorType& pos() { return _pos; }
    MULTIARCH inline VectorType& normal() { return _normal; }
private:
    VectorType _pos, _normal;
};

// define types that will be used to process data
typedef MyPoint::Scalar Scalar;
typedef Grenaille::DistWeightFunc<MyPoint,
                                  Grenaille::SmoothWeightKernel<Scalar> >
        WeightFunc;
typedef Grenaille::Basket<MyPoint,
                          WeightFunc,
                          Grenaille::OrientedSphereFit,
                          Grenaille::GLSParam>
        SphereFit;

typedef Grenaille::Basket<MyPoint,
                          WeightFunc,
                          Grenaille::OrientedSphereFit,
                          Grenaille::GLSParam/*,
                          Grenaille::OrientedSphereScaleDer,
                          Grenaille::GLSDer,
                          Grenaille::GLSGeomVar*/>
        DerivableSphereFit;



//! \brief Functor used to compare two GLS Fit
template <typename _Scalar, class _Fit>
struct Dynamic_GLS_cmp{
    inline _Scalar eval(const _Fit&f1, const _Fit&f2) const
    {
        if (f1.isReady() && f2.isReady()){
            double res = 1 -tanh(4.*f1.compareTo(f2));
            if (res<0.0 || isnan(res))
                return 0.0;
            else
                return res;
        }
        return 0.0;
    }
};

typedef DynamicProg::NeedlemanWunsch<Scalar, DerivableSphereFit, Dynamic_GLS_cmp > DynamicProcedure;
typedef DynamicProg::DynamicSolver<Scalar, DynamicProcedure> DynamicSolver;



class DescriptorBase{
public:

    MeshModel* model;
    vcg::KdTree<float>* kdTree;
    std::vector<unsigned int> selection;
    float minScale;
    float maxScale;

    float multiplier;
    int nBScales;

    // Compute a vector containing all the selected vertices id.
    void inline computeSelectionList() {
        if (model){
            selection.clear();

            unsigned int i = 0;
            for(CMeshO::VertexIterator vi = model->cm.vert.begin(); vi != model->cm.vert.end(); ++vi, i++){
#ifdef DEBUG_SIMILARITY_MAP
                (*vi).SetS();
                (*vi).Q()=MAX_SCALE_MULTIPLIER*model->cm.bbox.Diag();
                selection.push_back(i);
#else
                if((*vi).IsS())
                    selection.push_back(i);
#endif
            }
        }else
            std::cerr << "Can estimate selection" << std::endl;
    }

    MULTIARCH inline DescriptorBase(MeshModel* _model, float _minScale, float _maxScale, float _multiplier, int _nBScales)
        { model=_model; minScale=_minScale; maxScale=_maxScale; multiplier=_multiplier; nBScales=_nBScales;
          computeSelectionList();
    }

private:

};


static inline
std::ostream & operator<< (std::ostream &o,
                          const MyPoint &p){
    o << "pos( " << p.pos().transpose();
    o << "), normal("   << p.normal().transpose();
    o << ")";
    return o;
}

#endif // STRUCT_H
