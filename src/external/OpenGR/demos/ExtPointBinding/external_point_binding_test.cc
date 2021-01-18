#include "gr/io/io.h"
#include "gr/utils/geometry.h"
#include "gr/sampling.h"
#include "gr/algorithms/match4pcsBase.h"
#include "gr/algorithms/Functor4pcs.h"
#include "gr/algorithms/FunctorSuper4pcs.h"
#include "gr/algorithms/FunctorBrute4pcs.h"
#include <gr/algorithms/PointPairFilter.h>

#include <Eigen/Dense>

#include <fstream>
#include <iostream>
#include <string>

#include "../demo-utils.h"

#include "ext/point_conversion.h"
#include "ext/point_extlib1.hpp"
#include "ext/point_extlib2.hpp"
#include "ext/pointadapter_extlib1.hpp"
#include "ext/pointadapter_extlib2.hpp"

#define sqr(x) ((x) * (x))

using namespace std;
using namespace gr;
using namespace gr::Demo;

// data IO
IOManager ioManager;

static inline void printExtPBindingParameterList(){ // TODO: Change name
    fprintf(stderr, "\t[ -p point_type (%d)]\n", point_type);
    fprintf(stderr, "\t[ -r result_file_name (%s) ]\n", output.c_str());
    fprintf(stderr, "\t[ -m output matrix file (%s) ]\n", outputMat.c_str());
    fprintf(stderr, "\t[ --sampled1 (output sampled cloud 1 -- debug only) ]\n");
    fprintf(stderr, "\t[ --sampled2 (output sampled cloud 2 -- debug only) ]\n");
}
struct TransformVisitor {
    template <typename Derived>
    inline void operator()(
            float fraction,
            float best_LCP,
            const Eigen::MatrixBase<Derived>& /*transformation*/) const {
      if (fraction >= 0)
        {
          printf("done: %d%c best: %f                  \r",
               static_cast<int>(fraction * 100), '%', best_LCP);
          fflush(stdout);
        }
    }
    constexpr bool needsGlobalTransformation() const { return false; }
};

/**
  * If desired (by setting outputSampled1 and outputSampled2), the clouds could be
  * exported. The function takes the matcher as parameter, and exports the sampled
  * clouds of the matcher.
  */ 
template<typename Matcher>
void exportSampledClouds(const Matcher& matcher, const Utils::Logger& logger)
{
  if(! outputSampled1.empty() ){
    logger.Log<Utils::Verbose>( "Exporting Sampled cloud 1 to ",
                                outputSampled1.c_str(),
                                " ..." );
    ioManager.WriteObject((char *)outputSampled1.c_str(),
                           matcher.getFirstSampled(),
                           vector<Eigen::Matrix2f>(),
                           vector<typename Point3D<float>::VectorType>(), // dummy
                           vector<tripple>(),
                           vector<string>());
    logger.Log<Utils::Verbose>( "Export DONE" );
  }
  if(! outputSampled2.empty() ){
    logger.Log<Utils::Verbose>( "Exporting Sampled cloud 2 to ",
                                outputSampled2.c_str(),
                                " ..." );
    ioManager.WriteObject((char *)outputSampled2.c_str(),
                           matcher.getSecondSampled(),
                           vector<Eigen::Matrix2f>(),
                           vector<typename Point3D<float>::VectorType>(), // dummy
                           vector<tripple>(),
                           vector<string>());
    logger.Log<Utils::Verbose>( "Export DONE" );
  }
}

template <
    typename Matcher,
    typename PointType,
    typename Options,
    typename Range,
    template<typename> typename Sampler,
    typename TransformVisitor>
typename PointType::Scalar computeAlignment (
    const Options& options,
    const Utils::Logger& logger,
    const Range& P,
    const Range& Q,
    Eigen::Ref<Eigen::Matrix<typename PointType::Scalar, 4, 4>> mat,
    const Sampler<PointType>& sampler,
    TransformVisitor& visitor
    ) {
  Matcher matcher (options, logger);
  logger.Log<Utils::Verbose>( "Starting registration" );
  typename PointType::Scalar score = matcher.ComputeTransformation(P, Q, mat, sampler, visitor);


  logger.Log<Utils::Verbose>( "Score: ", score );
  logger.Log<Utils::Verbose>( "(Homogeneous) Transformation from ",
                              input2.c_str(),
                              " to ",
                              input1.c_str(),
                              ": \n",
                              mat);

  if(! outputSampled1.empty() ){
      logger.Log<Utils::Verbose>( "Exporting Sampled cloud 1 to ",
                                  outputSampled1.c_str(),
                                  " ..." );
      ioManager.WriteObject((char *)outputSampled1.c_str(),
                             matcher.getFirstSampled(),
                             vector<Eigen::Matrix2f>(),
                             vector<typename Point3D<float>::VectorType>(), // dummy
                             vector<tripple>(),
                             vector<string>());
      logger.Log<Utils::Verbose>( "Export DONE" );
  }
  if(! outputSampled2.empty() ){
      logger.Log<Utils::Verbose>( "Exporting Sampled cloud 2 to ",
                                  outputSampled2.c_str(),
                                  " ..." );
      ioManager.WriteObject((char *)outputSampled2.c_str(),
                             matcher.getSecondSampled(),
                             vector<Eigen::Matrix2f>(),
                             vector<typename Point3D<float>::VectorType>(), // dummy
                             vector<tripple>(),
                             vector<string>());
      logger.Log<Utils::Verbose>( "Export DONE" );
  }

  return score;
}

int main(int argc, char **argv) {
  using namespace gr;
  using Scalar = float;

  /**
   * Point clouds are read as gr::Point3D, then converted to other types to emulate
   * PointAdapter usage. This way, we can make use of IOManager to read point cloud
   * files, and lightweight point converter functions to convert gr points to example
   * external point type
   */
  vector<Point3D<Scalar> > set1, set2;
  vector<Eigen::Matrix2f> tex_coords1, tex_coords2;
  vector<typename Point3D<Scalar>::VectorType> normals1, normals2;
  vector<tripple> tris1, tris2;
  vector<std::string> mtls1, mtls2;

  // Match and return the score (estimated overlap or the LCP).
  typename Point3D<Scalar>::Scalar score = 0;

  constexpr Utils::LogLevel loglvl = Utils::Verbose;

  using TrVisitorType = typename std::conditional <loglvl==Utils::NoLog,
                            DummyTransformVisitor,
                            TransformVisitor>::type;
  using PairFilter = gr::AdaptivePointFilter;

  TrVisitorType visitor;
  Utils::Logger logger(loglvl);

  /// TODO Add proper error codes
  if(argc < 4){
    Demo::printUsage(argc, argv);
    printExtPBindingParameterList();
    exit(-2);
  }
  if(int c = Demo::getArgs(argc, argv) != 0)
  {
    Demo::printUsage(argc, argv);
    printExtPBindingParameterList();
    exit(std::max(c,0));
  }

  using MatrixType = Eigen::Matrix<Scalar, 4, 4>;
  MatrixType mat (MatrixType::Identity());

  // Read the inputs.
  if (!ioManager.ReadObject((char *)input1.c_str(), set1, tex_coords1, normals1, tris1,
                  mtls1)) {
    logger.Log<Utils::ErrorReport>("Can't read input set1");
    exit(-1);
  }

  if (!ioManager.ReadObject((char *)input2.c_str(), set2, tex_coords2, normals2, tris2,
                  mtls2)) {
    logger.Log<Utils::ErrorReport>("Can't read input set2");
    exit(-1);
  }

  // clean only when we have pset to avoid wrong face to point indexation
  if (tris1.size() == 0)
    Utils::CleanInvalidNormals(set1, normals1);
  if (tris2.size() == 0)
    Utils::CleanInvalidNormals(set2, normals2);

  try {
    if(point_type == 0) // gr::Point3D, directly pass the read sets
    {
      logger.Log<Utils::Verbose>("Registration on std::vector of gr::Point3D<float>"
                                 " instances - a point adapter is not required."    );

      using PointType    = gr::Point3D<Scalar>;
      /**
       * Here, PointAdapter does not really serve as an adapter since the point type
       * that we have as the type of the instances of points already comply with
       * the PointConcept. Therefore, we just pass the point type as the adapter type. 
       * */
      using PointAdapter = gr::Point3D<Scalar>;
      using MatcherType  = gr::Match4pcsBase<gr::FunctorSuper4PCS, PointAdapter, 
                                             TrVisitorType, gr::AdaptivePointFilter,
                                             gr::AdaptivePointFilter::Options>;
      using OptionType   = typename MatcherType::OptionsType;

      UniformDistSampler<PointAdapter> sampler;

      OptionType options;
      if(! Demo::setOptionsFromArgs(options, logger))
        exit(-2); /// \FIXME use status codes for error reporting

      // Create a matcher instance
      MatcherType matcher (options, logger);

      logger.Log<Utils::Verbose>( "Starting registration" );

      // Compute transformation: put transformation to mat, return registration score
      Scalar score = matcher.ComputeTransformation(set1, set2, mat, sampler, visitor);

      logger.Log<Utils::Verbose>( "Score: ", score );
      logger.Log<Utils::Verbose>( "(Homogeneous) Transformation from ",
                              input2.c_str(),
                              " to ",
                              input1.c_str(),
                              ": \n",
                              mat);

      // Export sampled point clouds - if paths were set
      exportSampledClouds(matcher, logger);
    }
    else if(point_type == 1) /* extlib1::PointType1, convert read sets to vector
                              * of PointType1 to emulate PointAdapter usage */
    {
      logger.Log<Utils::Verbose>("Registration on std::vector of extlib1::PointType1"
                                 "instances using extlib1::PointAdapter"             );

      /**
       * Convert the vectors of gr::Point3D<float> instances to some containers of 
       * extlib1::PointType1 instances, so that, we can assume that we have some
       * containers of extlib1::PointType1 instances in hand to use as input to
       * gr registration methods.
       */
//! [Creating a vector of extlib1::PointType1]
      std::vector<extlib1::PointType1> set1_point1 = getExtlib1Points(set1);
      std::vector<extlib1::PointType1> set2_point1 = getExtlib1Points(set2);
//! [Creating a vector of extlib1::PointType1]
      using PointType    = extlib1::PointType1;
      /**
       * Here, extlib1::PointType1 is not a model of gr::PointConcept. Hence, an adapter
       * type is needed to have registration on this external point type. extlib1::PointAdapter
       * serves as the adapter. Therefore, whenever gr requires a point type to be passed,
       * the adapter type will be passed, which it will use to wrap the extlib1::PointType1
       * instances on-the-fly to reach the information.
       */
//! [Point adapter and matcher type]
      using PointAdapter = extlib1::PointAdapter;
      using MatcherType = gr::Match4pcsBase<gr::FunctorSuper4PCS,
                                            PointAdapter, // here is the adapter!
                                            TrVisitorType,
                                            gr::AdaptivePointFilter,
                                            gr::AdaptivePointFilter::Options>;
//! [Point adapter and matcher type]
      using OptionType  = typename MatcherType::OptionsType;
//! [Sampler type]
      UniformDistSampler<PointAdapter> sampler;
//! [Sampler type]
      OptionType options;
      if(! Demo::setOptionsFromArgs(options, logger))
        exit(-2); /// \FIXME use status codes for error reporting

      logger.Log<Utils::Verbose>( "Starting registration" );
//! [Matcher instantiation and computation]
      // Create a matcher instance
      MatcherType matcher (options, logger);
      // Compute transformation: put transformation to mat, return registration score
      Scalar score = matcher.ComputeTransformation(set1_point1, set2_point1, mat, sampler, visitor);
//! [Matcher instantiation and computation]
      logger.Log<Utils::Verbose>( "Score: ", score );
      logger.Log<Utils::Verbose>( "(Homogeneous) Transformation from ",
                              input2.c_str(),
                              " to ",
                              input1.c_str(),
                              ": \n",
                              mat);

      // Export sampled point clouds - if paths were set
      exportSampledClouds(matcher, logger);
    }
    else if(point_type == 2) /* extlib2::PointType2, convert read sets to lists 
                              * of PointType2 to emulate PointAdapter usage */
    {
      logger.Log<Utils::Verbose>("Registration on std::list of extlib2::PointType2"
                                 " instances using extlib2::PointAdapter");

      /**
       * Convert the vectors of gr::Point3D<float> instances to some containers of 
       * extlib2::PointType2 instances, so that, we can assume that we have some
       * containers of extlib2::PointType2 instances in hand to use as input to
       * gr registration methods.
       */
      std::list<extlib2::PointType2> set1_point2 = getExtlib2Points(set1);
      std::list<extlib2::PointType2> set2_point2 = getExtlib2Points(set2);

      using PointType    = extlib2::PointType2;
      /**
       * Here, extlib2::PointType2 is not a model of gr::PointConcept. Hence, an adapter
       * type is needed to have registration on this external point type. extlib2::PointAdapter
       * serves as the adapter. Therefore, whenever gr requires a point type to be passed,
       * the adapter type will be passed, which it will use to wrap the extlib2::PointType2
       * instances on-the-fly to reach the information.
       */
      using PointAdapter = extlib2::PointAdapter;
      using MatcherType = gr::Match4pcsBase<gr::FunctorSuper4PCS, PointAdapter, TrVisitorType, gr::AdaptivePointFilter, gr::AdaptivePointFilter::Options>;
      using OptionType  = typename MatcherType::OptionsType;

      UniformDistSampler<PointAdapter> sampler;

      OptionType options;
      if(! Demo::setOptionsFromArgs(options, logger))
        exit(-2); /// \FIXME use status codes for error reporting

      // Create a matcher instance
      MatcherType matcher (options, logger);

      logger.Log<Utils::Verbose>( "Starting registration" );

      // Compute transformation: put transformation to mat, return registration score
      Scalar score = matcher.ComputeTransformation(set1_point2, set2_point2, mat, sampler, visitor);

      logger.Log<Utils::Verbose>( "Score: ", score );
      logger.Log<Utils::Verbose>( "(Homogeneous) Transformation from ",
                              input2.c_str(),
                              " to ",
                              input1.c_str(),
                              ": \n",
                              mat);
        
      // Export sampled point clouds - if paths were set
      exportSampledClouds(matcher, logger);

      // deallocate memory for points2: needed due to internals of extlib2::PointType2
      if(!set1_point2.empty()) {
        delete[] set1_point2.front().posBuffer;
        delete[] set1_point2.front().nBuffer;
        delete[] set1_point2.front().colorBuffer;
        set1_point2.clear();
      }

      if(!set2_point2.empty()) {
        delete[] set2_point2.front().posBuffer;
        delete[] set2_point2.front().nBuffer;
        delete[] set2_point2.front().colorBuffer;
        set2_point2.clear();
      }
    }
    else
    {
      std::cerr << "Invalid point type" << std::endl;
      exit(-1);
    }
  }
  catch (const std::exception& e) {
      logger.Log<Utils::ErrorReport>( "[Error]: " , e.what() );
      logger.Log<Utils::ErrorReport>( "Aborting with code -3 ..." );
      return -3;
  }
  catch (...) {
      logger.Log<Utils::ErrorReport>( "[Unknown Error]: Aborting with code -4 ..." );
      return -4;
  }

  if(! outputMat.empty() ){
      logger.Log<Utils::Verbose>( "Exporting Matrix to ",
                                  outputMat.c_str(),
                                  "..." );
      ioManager.WriteMatrix(outputMat, mat.cast<double>(), IOManager::POLYWORKS);
      logger.Log<Utils::Verbose>( "Export DONE" );
  }

  if (! output.empty() ){

      logger.Log<Utils::Verbose>( "Exporting Registered geometry to ",
                                  output.c_str(),
                                  "..." );
      Utils::TransformPointCloud(set2, mat);
      ioManager.WriteObject((char *)output.c_str(),
                             set2,
                             tex_coords2,
                             normals2,
                             tris2,
                             mtls2);
      logger.Log<Utils::Verbose>( "Export DONE" );
  }

  return 0;
}
