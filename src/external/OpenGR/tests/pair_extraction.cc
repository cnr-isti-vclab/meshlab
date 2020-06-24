// Copyright 2014 Nicolas Mellado
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -------------------------------------------------------------------------- //
//
// Authors: Nicolas Mellado
//
// This test check the validity of the pair extraction subroutine on random data
// of different dimensions (2,3 and 4).
//
//
// This test is part of the implementation of the Super 4-points Congruent Sets
// (Super 4PCS) algorithm presented in:
//
// Super 4PCS: Fast Global Pointcloud Registration via Smart Indexing
// Nicolas Mellado, Dror Aiger, Niloy J. Mitra
// Symposium on Geometry Processing 2014.
//
// Data acquisition in large-scale scenes regularly involves accumulating
// information across multiple scans. A common approach is to locally align scan
// pairs using Iterative Closest Point (ICP) algorithm (or its variants), but
// requires static scenes and small motion between scan pairs. This prevents
// accumulating data across multiple scan sessions and/or different acquisition
// modalities (e.g., stereo, depth scans). Alternatively, one can use a global
// registration algorithm allowing scans to be in arbitrary initial poses. The
// state-of-the-art global registration algorithm, 4PCS, however has a quadratic
// time complexity in the number of data points. This vastly limits its
// applicability to acquisition of large environments. We present Super 4PCS for
// global pointcloud registration that is optimal, i.e., runs in linear time (in
// the number of data points) and is also output sensitive in the complexity of
// the alignment problem based on the (unknown) overlap across scan pairs.
// Technically, we map the algorithm as an ‘instance problem’ and solve it
// efficiently using a smart indexing data organization. The algorithm is
// simple, memory-efficient, and fast. We demonstrate that Super 4PCS results in
// significant speedup over alternative approaches and allows unstructured
// efficient acquisition of scenes at scales previously not possible. Complete
// source code and datasets are available for research use at
// http://geometry.cs.ucl.ac.uk/projects/2014/super4PCS/.

#include "gr/algorithms/matchBase.h"
#include "gr/algorithms/match4pcsBase.h"
#include "gr/algorithms/Functor4pcs.h"
#include "gr/algorithms/FunctorSuper4pcs.h"
#include "gr/algorithms/FunctorBrute4pcs.h"
#include "gr/accelerators/pairExtraction/bruteForceFunctor.h"
#include "gr/accelerators/pairExtraction/intersectionFunctor.h"
#include "gr/accelerators/pairExtraction/intersectionPrimitive.h"
#include "gr/utils/timer.h"
#include "gr/algorithms/PointPairFilter.h"
#include "gr/sampling.h"

#include <Eigen/Dense>

#include <fstream>
#include <iostream>
#include <string>

#include <stdlib.h>
#include <utility> // pair

#include "testing.h"

#define TRACE

using namespace gr;

struct MyPairCreationFunctor{
  typedef std::pair<unsigned int, unsigned int>ResPair;
  std::vector< ResPair >pairs;

  std::vector<unsigned int> ids;

  inline void beginPrimitiveCollect(int primId){
  }

  inline void endPrimitiveCollect(int primId){
  }
  inline void process(int primId, int pointId){
    //if(pointId >= 10)
      if (primId>pointId)
        pairs.emplace_back(pointId, primId);
  }
};

struct TrVisitorType {
    template <typename Derived>
    inline void operator() (
            float fraction,
            float best_LCP,
            const Eigen::MatrixBase<Derived>& /*transformation*/) {
#ifdef TRACE
        std::cout << "New LCP: "
                  << static_cast<int>(fraction * 100)
                  << '%'
                  << best_LCP
                  <<std::endl;
#else
      void(fraction);
      void(best_LCP);
#endif
    }
    constexpr bool needsGlobalTransformation() const { return false; }
};

constexpr Utils::LogLevel loglvl = Utils::Verbose;
Utils::Logger logger(loglvl);


/*!
 * \brief Generate a set of random points and spheres, and test the pair extraction

   Two tests are operated here:
    - Check the validity of the sphere to point intersection test
    - Check the rendering

   Note here that rendering timings are not optimal because we have a volume
   uniformly sampled and not a surface.

 */
template<typename Scalar, typename Point, typename Primitive, typename Functor>
void testFunction( Scalar r, Scalar epsilon,
                   unsigned int nbPoints,
                   unsigned int minNodeSize){

  // Init required structures
  gr::Utils::Timer t;
  std::vector< std::pair<unsigned int, unsigned int> > p2;
  p2.reserve(nbPoints*nbPoints);

  MyPairCreationFunctor functor;
  functor.ids.clear();
  for(unsigned int i = 0; i < nbPoints; i++)
    functor.ids.push_back(i);
  functor.pairs.reserve(nbPoints*nbPoints);

  //std::cout << "**************************" << std::endl;
  //std::cout << "Epsilon = " << epsilon << std::endl;

  // Init Random Positions
  std::vector<Point> points;
  std::vector<Primitive> primitives;

  //std::cout << "Points: " << std::endl;
  Point half (Point::Ones()/2.f);
  for(unsigned int i = 0; i != nbPoints; i++){
    // generate points on a sphere
    Point p (0.5f*Point::Random().normalized() + half);
    points.push_back(p);
    primitives.emplace_back(p, r);
    //std::cout << p.transpose() << std::endl;
  }

  // Test test intersection procedure
  // Here we compare the brute force pair extraction and the sphere to point
  // intersection procedure
  {
    Primitive& sphere = primitives.front();
    for(unsigned int i = 0; i != nbPoints; i++){
      const Point&p = points[i];
      VERIFY( sphere.intersectPoint(p, epsilon) ==
              SQR((p - sphere.center()).norm()- sphere.radius()) < SQR(epsilon));
    }
  }


  // Test Rendering process
  {
    Functor IF;

    // Extract pairs using rendering process
    t.reset();
    IF.process(primitives, points, epsilon, minNodeSize, functor);
#ifdef TRACE
    const auto IFtimestep = t.elapsed();
#endif


    // Extract pairs using brute force
    t.reset();
    for(unsigned int i = 0; i != nbPoints; i++)
      for(unsigned int j = i+1; j < nbPoints; j++)
         if (primitives[j].intersectPoint(points[i], epsilon))
          p2.emplace_back(i,j);

#ifdef TRACE
    const auto BFtimestep = t.elapsed();

    std::cout << "Timers (" << (IFtimestep.count() < BFtimestep.count()
                                ? "PASSED" : "NOT PASSED")
              << "): \t Functor: " << IFtimestep.count()/1000
              << "\t BruteForce: " << BFtimestep.count()/1000 << std::endl;

    // Check we get the same set size
    std::cout << "Size check (" << (functor.pairs.size() == p2.size()
                                ? "PASSED" : "NOT PASSED")
              << "): \t Functor: " << functor.pairs.size()
              << " \t BruteForce: " << p2.size() << std::endl;
#endif

    // sort to ensure containers consistency
    std::sort(functor.pairs.begin(), functor.pairs.end());
    std::sort(p2.begin(), p2.end());


//    std::cout << "Functor: " << std::endl;
//    for (const auto&p0 : functor.pairs)
//        std::cout << "\t" << p0.first << " - " << p0.second << std::endl;
//    std::cout << "Brute Force: " << std::endl;
//    for (const auto&p0 : p2)
//        std::cout << "\t" << p0.first << " - " << p0.second << std::endl;


    VERIFY( functor.pairs.size() == p2.size() );
    VERIFY( std::equal(functor.pairs.begin(), functor.pairs.end(), p2.begin()));

  }
}


template<typename Scalar,
         int Dim,
         template <typename,typename,int,typename> class _Functor>
void callSubTests()
{
    using namespace gr::Accelerators::PairExtraction;

    typedef  Eigen::Matrix<Scalar, Dim, 1> EigenPoint;
    typedef  HyperSphere< EigenPoint, Dim, Scalar > Sphere;
    typedef _Functor<Sphere, EigenPoint, Dim, Scalar> Functor;

    Scalar   r = 0.5; // radius of the spheres
    Scalar eps = GetRoundedEpsilonValue(0.125/16.); // epsilon value
    unsigned int nbPoint = 2500;  // size of Q point cloud
    int minNodeSize = 50;

#pragma omp parallel for
    for(int i = 0; i < Testing::g_repeat; ++i)
    {
        CALL_SUBTEST(( testFunction<Scalar,
                                    EigenPoint,
                                    Sphere,
                                    Functor>(r, eps, nbPoint, minNodeSize) ));
    }
}

template <template <typename, typename, typename> typename FunctorType>
void callMatch4SubTestsWithFunctor()
{
    using MatcherType = gr::Match4pcsBase<FunctorType, gr::Point3D<float>, TrVisitorType, gr::DummyPointFilter, gr::DummyPointFilter::Options>;
    using Scalar = typename MatcherType::Scalar;
    using PairsVector = typename MatcherType::PairsVector;
    using OptionType  = typename MatcherType::OptionsType;
    using SamplerType = gr::UniformDistSampler<gr::Point3D<float> >;

    SamplerType sampler;

    OptionType opt;
    opt.delta = 0.1;
    opt.dummyFilteringResponse = true;
    VERIFY(opt.configureOverlap(0.5));

    const size_t nbPointP = 200;
    const size_t nbPointQ = 150;
    // Computes distance between pairs.
    Scalar distance1 = 0.3;
    Scalar distance2 = 0.5;
    Scalar normal_angle1 = 0.6;
    Scalar normal_angle2 = 0.4;

    Scalar pair_distance_epsilon = MatcherType::distance_factor * opt.delta;

#pragma omp parallel for
    for(int i = 0; i < Testing::g_repeat; ++i)
    {

        // generate input point cloud
        std::vector<Point3D<float> > P, Q;
        Testing::generateSphereCloud(P, nbPointP);
        Testing::generateSphereCloud(Q, nbPointQ);

        std::vector<std::pair<int, int>> gtpairs1, gtpairs2;
        Testing::extractPairs(distance1, pair_distance_epsilon, Q, gtpairs1);
        Testing::extractPairs(distance2, pair_distance_epsilon, Q, gtpairs2);

        std::sort(gtpairs1.begin(), gtpairs1.end());
        std::sort(gtpairs2.begin(), gtpairs2.end());


        // extract point using matcher
        Testing::TestMatcher<MatcherType> match (opt, logger);
        match.init(P, Q, sampler);
        // Init base (not interested in what base is, therefore, use dummy ref)
        typename MatcherType::CongruentBaseType dummy;
        match.initBase(dummy);

        std::vector<std::pair<int, int>> pairs1, pairs2;
        match.getFunctor().ExtractPairs(distance1,
                           normal_angle1,
                           pair_distance_epsilon,
                           0,
                           1,
                           &pairs1);
        match.getFunctor().ExtractPairs(distance2,
                           normal_angle2,
                           pair_distance_epsilon,
                           2,
                           3,
                           &pairs2);

        std::sort(pairs1.begin(), pairs1.end());
        std::sort(pairs2.begin(), pairs2.end());

#ifdef TRACE

        // Check we get the same set size
        std::cout << "Size check 1 (" << (pairs1.size() == gtpairs1.size()
                                          ? "PASSED" : "NOT PASSED")
                  << "): \t Functor: " << pairs1.size()
                  << " \t GT: " << gtpairs1.size() << std::endl;
        std::cout << "Size check 2 (" << (pairs2.size() == gtpairs2.size()
                                          ? "PASSED" : "NOT PASSED")
                  << "): \t Functor: " << pairs2.size()
                  << " \t GT: " << gtpairs2.size() << std::endl;
#endif

        VERIFY( gtpairs1.size() == pairs1.size() );
        VERIFY( std::equal(pairs1.begin(), pairs1.end(), gtpairs1.begin()));

        VERIFY( gtpairs2.size() == pairs2.size() );
        VERIFY( std::equal(pairs2.begin(), pairs2.end(), gtpairs2.begin()));
    }

}

void callMatch4SubTests() {

  using std::cout;
  using std::endl;

  cout << "Extract pairs using Functor4PCS" << endl;
  callMatch4SubTestsWithFunctor<Functor4PCS>();
  cout << "Ok..." << endl;

  cout << "Extract pairs using Functor4PCS" << endl;
  callMatch4SubTestsWithFunctor<FunctorBrute4PCS>();
  cout << "Ok..." << endl;

  cout << "Extract pairs using FunctorSuper4PCS" << endl;
  callMatch4SubTestsWithFunctor<FunctorSuper4PCS>();
  cout << "Ok..." << endl;
}


int main(int argc, const char **argv) {
    if(!Testing::init_testing(argc, argv))
    {
        return EXIT_FAILURE;
    }

    using std::cout;
    using std::endl;
    using namespace gr::Accelerators::PairExtraction;


    cout << "Extract pairs in 2 dimensions (BRUTE FORCE)..." << endl;
    callSubTests<float, 2, BruteForceFunctor>();
    callSubTests<double, 2, BruteForceFunctor>();
    callSubTests<long double, 2, BruteForceFunctor>();
    cout << "Ok..." << endl;

    cout << "Extract pairs in 2 dimensions (RENDERING)..." << endl;
    callSubTests<float, 2, IntersectionFunctor>();
    callSubTests<double, 2, IntersectionFunctor>();
    callSubTests<long double, 2, IntersectionFunctor>();
    cout << "Ok..." << endl;

    cout << "Extract pairs in 3 dimensions (BRUTE FORCE)..." << endl;
    callSubTests<float, 3, BruteForceFunctor>();
    callSubTests<double, 3, BruteForceFunctor>();
    callSubTests<long double, 3, BruteForceFunctor>();
    cout << "Ok..." << endl;

    cout << "Extract pairs in 3 dimensions (RENDERING)..." << endl;
    callSubTests<float, 3, IntersectionFunctor>();
    callSubTests<double, 3, IntersectionFunctor>();
    callSubTests<long double, 3, IntersectionFunctor>();
    cout << "Ok..." << endl;

    cout << "Extract pairs in 4 dimensions (BRUTE FORCE)..." << endl;
    callSubTests<float, 4, BruteForceFunctor>();
    callSubTests<double, 4, BruteForceFunctor>();
    callSubTests<long double, 4, BruteForceFunctor>();
    cout << "Ok..." << endl;

    cout << "Extract pairs in 4 dimensions (RENDERING)..." << endl;
    callSubTests<float, 4, IntersectionFunctor>();
    callSubTests<double, 4, IntersectionFunctor>();
    callSubTests<long double, 4, IntersectionFunctor>();
    cout << "Ok..." << endl;

    callMatch4SubTests();

    return EXIT_SUCCESS;
}
