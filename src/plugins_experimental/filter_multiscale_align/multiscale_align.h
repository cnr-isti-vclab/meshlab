#ifndef MULTISCALE_ALIGN_H
#define MULTISCALE_ALIGN_H
#include <common/meshmodel.h>

#include <eigenlib/Eigen/Core>
#include "Patate/grenaille.h"

//#include "localKdTree.h"
#include "generic_align.h"
#include "struct.h"

#include <omp.h>

#define PI 3.14159265

//////// Number of samples for the first poisson, it defines the size of the models which will be analysed
#define SUBSAMPLES 20000
////// size of the first, second and third points list used for matching
#define FIRST_POINTS 10
#define OTHER_POINTS 5
//#define THIRD_POINTS 5

#define MAX_SCALE_MULTIPLIER 1

//////  the alignment treshold (10 ok for mm, not so good for meters... it could be calculated based on the size of the model)
//#define ALIGNMENT_THRESHOLD 4.0

////// This permits to execute only the first search for a matching, for debug reasons
//#define ONLY_THE_FIRST_SEARCH

////// This permits to define the first point in the toalign model by hand
//#define DEBUG_SINGLE_POINT

////// This permits to compute the multiscale distance between a random seed and all the other points in the reference mesh
//#define DEBUG_SIMILARITY_MAP

#define MULTIPLIER 1.2

//#define CHECKBORDERS true

//#define SAVE_CVS

using namespace vcg;


namespace MultiscaleAlignNamespace{
    template <typename> class KdTree;
}

class MultiscaleAlign: public GenericAlign
{
private:

public:
    MultiscaleAlign();



    /**************************************************************************/
    /*   Input Parameters                                                     */
    /**************************************************************************/


    /**************************************************************************/
    /*   Processing                                                           */
    /**************************************************************************/

public:
	void checkBorders(DescriptorBase model, float radius);
    float process_scale(MeshModel* _reference, float radiusRef, float refCoarse, MeshModel* _toAlign, float radiusToAlign, float toAlignCoarse, float multiplier, unsigned int nbScales, Options opt);
	float recalcConfidence(DescriptorBase _reference, DescriptorBase _toAlign, float scale, unsigned int nbScales, int referenceID, int toAlignID);

private:
    std::vector < Cand > searchMatches(DescriptorBase toAlign, DescriptorBase reference, int ind, std::vector<float>Scales, bool computeAlignDescr = true);
    bool searchTranf(const DescriptorBase& toAlign,
                     const DescriptorBase& reference,
                     const std::vector< int >& seeds,
                     const std::vector< std::vector < Cand > >& seedList,
                     const std::vector<float>& Scales,
                     Options opt);
    std::vector<Cand> getCandidates(const DescriptorBase &toAlign,
                                    const DescriptorBase &reference,
                                    int ind,
                                    const std::vector<std::pair<int, int> > &corrs,
                                    float scaleFact,
                                    const std::vector<float> &Scales);
    /*void extractSIFTPoints(const DescriptorBase& mesh,
                           std::vector<SiftPoint> &list);*/
    void extractDescriptivePoints(const DescriptorBase& mesh,
                                  std::vector<DescrPoint> &list);

    template <class Fit>
    void computeSimilarityMap(const DescriptorBase& reference, const DescriptorBase& toAlign, std::vector<Fit>* toAlignDescr, float maxscaleToAl);

    
    /**************************************************************************/
    /*   Results                                                              */
    /**************************************************************************/


};

#endif // MULTISCALE_ALIGN_H
