//
// Created by Sandra Alfaro on 24/05/18.
//

#include <vector>
#include <atomic>
#include <chrono>
#include <numeric> // std::iota

#ifdef OpenGR_USE_OPENMP
#include <omp.h>
#endif

#include "gr/algorithms/matchBase.h"
#include "gr/shared.h"
#include "gr/sampling.h"
#include "gr/accelerators/kdtree.h"
#include "gr/utils/logger.h"

#ifdef TEST_GLOBAL_TIMINGS
#   include "../utils/timer.h"
#endif


#define MATCH_BASE_TYPE MatchBase<PointType, TransformVisitor, OptExts ... >


namespace gr {

template <typename PointType, typename TransformVisitor, template < class, class > typename ... OptExts>
MATCH_BASE_TYPE::MatchBase(const typename MATCH_BASE_TYPE::OptionsType &options,
                      const Utils::Logger& logger
                       )
    : max_base_diameter_(-1)
    , P_mean_distance_(1.0)
    , randomGenerator_(options.randomSeed)
    , logger_(logger)
    , options_(options)
{}

template <typename PointType, typename TransformVisitor, template < class, class > typename ... OptExts>
MATCH_BASE_TYPE::~MatchBase(){}


template <typename PointType, typename TransformVisitor, template < class, class > typename ... OptExts>
typename MATCH_BASE_TYPE::Scalar
MATCH_BASE_TYPE::MeanDistance() const {
    const Scalar kDiameterFraction = 0.2;
    using RangeQuery = typename gr::KdTree<Scalar>::template RangeQuery<>;

    int number_of_samples = 0;
    Scalar distance = 0.0;

    for (size_t i = 0; i < sampled_P_3D_.size(); ++i) {

        RangeQuery query;
        query.sqdist = P_diameter_ * kDiameterFraction;
        query.queryPoint = sampled_P_3D_[i].pos().template cast<Scalar>();

        auto resId = kd_tree_.doQueryRestrictedClosestIndex(query , i).first;

        if (resId != gr::KdTree<Scalar>::invalidIndex()) {
            distance += (sampled_P_3D_[i].pos() - sampled_P_3D_[resId].pos()).norm();
            number_of_samples++;
        }
    }

    return distance / number_of_samples;
}

template <typename PointType, typename TransformVisitor, template < class, class > typename ... OptExts>
bool
MATCH_BASE_TYPE::SelectRandomTriangle(int &base1, int &base2, int &base3) {
    int number_of_points = sampled_P_3D_.size();
    base1 = base2 = base3 = -1;

    // Pick the first point at random.
    int first_point = randomGenerator_() % number_of_points;

    const Scalar sq_max_base_diameter_ = max_base_diameter_*max_base_diameter_;

    // Try fixed number of times retaining the best other two.
    Scalar best_wide = 0.0;
    for (int i = 0; i < kNumberOfDiameterTrials; ++i) {
        // Pick and compute
        const int second_point = randomGenerator_() % number_of_points;
        const int third_point = randomGenerator_() % number_of_points;
        const VectorType u =
                sampled_P_3D_[second_point].pos() -
                sampled_P_3D_[first_point].pos();
        const VectorType w =
                sampled_P_3D_[third_point].pos() -
                sampled_P_3D_[first_point].pos();
        // We try to have wide triangles but still not too large.
        Scalar how_wide = (u.cross(w)).norm();
        if (how_wide > best_wide &&
                u.squaredNorm() < sq_max_base_diameter_ &&
                w.squaredNorm() < sq_max_base_diameter_) {
            best_wide = how_wide;
            base1 = first_point;
            base2 = second_point;
            base3 = third_point;
        }
    }
    return base1 != -1 && base2 != -1 && base3 != -1;
}

template <typename PointType, typename TransformVisitor, template < class, class > typename ... OptExts>
void
MATCH_BASE_TYPE::initKdTree(){
    size_t number_of_points = sampled_P_3D_.size();

    // Build the kdtree.
    kd_tree_ = gr::KdTree<Scalar>(number_of_points);

    for (size_t i = 0; i < number_of_points; ++i) {
        kd_tree_.add(sampled_P_3D_[i].pos());
    }
    kd_tree_.finalize();
}


template <typename PointType, typename TransformVisitor, template < class, class > typename ... OptExts>
template <typename Coordinates>
bool
MATCH_BASE_TYPE::ComputeRigidTransformation(const Coordinates& ref,
        const Coordinates& candidate,
        const Eigen::Matrix<Scalar, 3, 1>& centroid1,
        Eigen::Matrix<Scalar, 3, 1> centroid2,
        Eigen::Ref<MatrixType> transform,
        Scalar& rms_,
        bool computeScale ) const {
    static const Scalar pi = std::acos(-1);

    rms_ = std::numeric_limits<Scalar>::max();

    Scalar kSmallNumber = 1e-6;

    // We only use the first 3 pairs. This simplifies the process considerably
    // because it is the planar case.

    const VectorType& p0 = ref[0]->pos();
    const VectorType& p1 = ref[1]->pos();
    const VectorType& p2 = ref[2]->pos();
    VectorType  q0 = candidate[0]->pos();
    VectorType  q1 = candidate[1]->pos();
    VectorType  q2 = candidate[2]->pos();

    Scalar scaleEst (1.);

    // Compute scale factor if needed
    if (computeScale){
        const VectorType& p3 = ref[3]->pos();
        const VectorType& q3 = candidate[3]->pos();

        const Scalar ratio1 = (p1 - p0).norm() / (q1 - q0).norm();
        const Scalar ratio2 = (p3 - p2).norm() / (q3 - q2).norm();

        const Scalar ratioDev  = std::abs(ratio1/ratio2 - Scalar(1.));  // deviation between the two
        const Scalar ratioMean = (ratio1+ratio2)/Scalar(2.);            // mean of the two

        if ( ratioDev > Scalar(0.1) )
            return std::numeric_limits<Scalar>::max();


        //Log<LogLevel::Verbose>( ratio1, " ", ratio2, " ", ratioDev, " ", ratioMean);
        scaleEst = ratioMean;

        // apply scale factor to q
        q0 = q0*scaleEst;
        q1 = q1*scaleEst;
        q2 = q2*scaleEst;
        centroid2 *= scaleEst;
    }

    VectorType vector_p1 = p1 - p0;
    if (vector_p1.squaredNorm() == 0) return std::numeric_limits<Scalar>::max();
    vector_p1.normalize();
    VectorType vector_p2 = (p2 - p0) - ((p2 - p0).dot(vector_p1)) * vector_p1;
    if (vector_p2.squaredNorm() == 0) return std::numeric_limits<Scalar>::max();
    vector_p2.normalize();
    VectorType vector_p3 = vector_p1.cross(vector_p2);

    VectorType vector_q1 = q1 - q0;
    if (vector_q1.squaredNorm() == 0) return std::numeric_limits<Scalar>::max();
    vector_q1.normalize();
    VectorType vector_q2 = (q2 - q0) - ((q2 - q0).dot(vector_q1)) * vector_q1;
    if (vector_q2.squaredNorm() == 0) return std::numeric_limits<Scalar>::max();
    vector_q2.normalize();
    VectorType vector_q3 = vector_q1.cross(vector_q2);

    //cv::Mat rotation = cv::Mat::eye(3, 3, CV_64F);
    Eigen::Matrix<Scalar, 3, 3> rotation = Eigen::Matrix<Scalar, 3, 3>::Identity();

    Eigen::Matrix<Scalar, 3, 3> rotate_p;
    rotate_p.row(0) = vector_p1;
    rotate_p.row(1) = vector_p2;
    rotate_p.row(2) = vector_p3;

    Eigen::Matrix<Scalar, 3, 3> rotate_q;
    rotate_q.row(0) = vector_q1;
    rotate_q.row(1) = vector_q2;
    rotate_q.row(2) = vector_q3;

    rotation = rotate_p.transpose() * rotate_q;


    // Discard singular solutions. The rotation should be orthogonal.
    if (((rotation * rotation).diagonal().array() - Scalar(1) > kSmallNumber).any())
        return false;

    //Filter transformations.
    // \fixme Need to consider max_translation_distance and max_scale too
    if (options_.max_angle >= 0) {
        Scalar mangle = options_.max_angle * pi / 180.0;
        // Discard too large solutions (todo: lazy evaluation during boolean computation
        if (! (
                    std::abs(std::atan2(rotation(2, 1), rotation(2, 2)))
                    <= mangle &&

                    std::abs(std::atan2(-rotation(2, 0),
                                        std::sqrt(std::pow(rotation(2, 1),2) +
                                                  std::pow(rotation(2, 2),2))))
                    <= mangle &&

                    std::abs(atan2(rotation(1, 0), rotation(0, 0)))
                    <= mangle
                    ))
            return false;
    }


    //FIXME
    // Compute rms and return it.
    rms_ = Scalar(0.0);
    {
        VectorType first, transformed;

        //cv::Mat first(3, 1, CV_64F), transformed;
        for (int i = 0; i < 3; ++i) {
            first = scaleEst*candidate[i]->pos() - centroid2;
            transformed = rotation * first;
            rms_ += (transformed - ref[i]->pos() + centroid1).norm();
        }
    }

    rms_ /= Scalar(ref.size());

    Eigen::Transform<Scalar, 3, Eigen::Affine> etrans (Eigen::Transform<Scalar, 3, Eigen::Affine>::Identity());
    transform = etrans
            .scale(scaleEst)
            .translate(centroid1)
            .rotate(rotation)
            .translate(-centroid2)
            .matrix();

    return true;
}

template <typename PointType, typename TransformVisitor, template < class, class > typename ... OptExts>
template <typename InputRange1, typename InputRange2, template<typename> typename Sampler>
void MATCH_BASE_TYPE::init(const InputRange1& P,
              const InputRange2& Q,
              const Sampler<PointType>& sampler) {

    centroid_P_ = VectorType::Zero();
    centroid_Q_ = VectorType::Zero();

    sampled_P_3D_.clear();
    sampled_Q_3D_.clear();

    // prepare P
    if (P.size() > options_.sample_size){
        std::vector<typename InputRange1::value_type > sampled_P_3D;

        sampler(P, options_, sampled_P_3D_);
    }
    else
    {
        Log<LogLevel::ErrorReport>( "(P) More samples requested than available: use whole cloud" );

        // copy all the points
        std::copy(P.begin(), P.end(), std::back_inserter(sampled_P_3D_));
    }

    // prepare Q
    if (Q.size() > options_.sample_size){
        std::vector<typename InputRange2::value_type> uniform_Q, sampled_Q_3D;

        sampler(Q, options_, uniform_Q);
    
        std::vector<int> indices(uniform_Q.size());
        std::iota( std::begin(indices), std::end(indices), 0 );
        std::shuffle(indices.begin(), indices.end(), randomGenerator_);
        size_t nbSamples = std::min(uniform_Q.size(), options_.sample_size);
        indices.resize(nbSamples);

        // using the indices, copy elements from uniform_Q to sampled_P_3D_
        for(int i : indices) 
            sampled_Q_3D_.emplace_back(uniform_Q[i]);
        
        uniform_Q.clear();
    }
    else
    {
        Log<LogLevel::ErrorReport>( "(Q) More samples requested than available: use whole cloud" );
        
        // copy all the points
        std::copy(Q.begin(), Q.end(), std::back_inserter(sampled_Q_3D_));
    }


    // center points around centroids
    auto centerPoints = [](std::vector<PosMutablePoint>&container,
            VectorType& centroid){
        for(auto& p : container) centroid += p.pos();
        centroid /= Scalar(container.size());
        for(auto& p : container) p.pos() -= centroid;
    };
    centerPoints(sampled_P_3D_, centroid_P_);
    centerPoints(sampled_Q_3D_, centroid_Q_);


    initKdTree();
    
    // Compute the diameter of P approximately (randomly). This is far from being
    // Guaranteed close to the diameter but gives good results for most common
    // objects if they are densely sampled.
    P_diameter_ = 0.0;
    for (int i = 0; i < kNumberOfDiameterTrials; ++i) {
        int at = randomGenerator_() % sampled_Q_3D_.size();
        int bt = randomGenerator_() % sampled_Q_3D_.size();

        Scalar l = (sampled_Q_3D_[bt].pos() - sampled_Q_3D_[at].pos()).norm();
        if (l > P_diameter_) {
            P_diameter_ = l;
        }
    }

    // Mean distance and a bit more... We increase the estimation to allow for
    // noise, wrong estimation and non-uniform sampling.
    P_mean_distance_ = MeanDistance();

    // Normalize the delta (See the paper) and the maximum base distance.
    // delta = P_mean_distance_ * delta;
    max_base_diameter_ = P_diameter_;  // * estimated_overlap_;

    transform_ = Eigen::Matrix<Scalar, 4, 4>::Identity();

    // call Virtual handler
    Initialize();
}

} // namespace gr

#undef MATCH_BASE_TYPE
