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

#define sqr(x) ((x) * (x))

using namespace std;
using namespace gr;
using namespace gr::Demo;

// data IO
IOManager ioManager;

static inline void printS4PCSParameterList(){
    fprintf(stderr, "\t[ -r result_file_name (%s) ]\n", output.c_str());
    fprintf(stderr, "\t[ -m output matrix file (%s) ]\n", outputMat.c_str());
    fprintf(stderr, "\t[ -x (use 4pcs: false by default) ]\n");
    fprintf(stderr, "\t[ --sampled1 (output sampled cloud 1 -- debug+super4pcs only) ]\n");
    fprintf(stderr, "\t[ --sampled2 (output sampled cloud 2 -- debug+super4pcs only) ]\n");
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
  // Point clouds are read as gr::Point3D, then converted to other types if necessary to
  // emulate PointAdapter usage
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
    printS4PCSParameterList();
    exit(-2);
  }
  if(int c = Demo::getArgs(argc, argv) != 0)
  {
    Demo::printUsage(argc, argv);
    printS4PCSParameterList();
    exit(std::max(c,0));
  }

  // prepare matcher ressourcesoutputSampled2
  using MatrixType = Eigen::Matrix<typename Point3D<Scalar>::Scalar, 4, 4>;
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
    if (use_super4pcs) {
      using PointType    = gr::Point3D<Scalar>;
      using MatcherType  = gr::Match4pcsBase<gr::FunctorSuper4PCS, PointType, 
                                             TrVisitorType, gr::AdaptivePointFilter,
                                             gr::AdaptivePointFilter::Options>;
      using OptionType   = typename MatcherType::OptionsType;

      UniformDistSampler<PointType> sampler;

      OptionType options;
      if(! Demo::setOptionsFromArgs(options, logger))
        exit(-2); /// \FIXME use status codes for error reporting

      score = computeAlignment<MatcherType, PointType> (options, logger, set1, set2,
                                                        mat, sampler, visitor);
    }
    else {
      // 4PCS
      using PointType    = gr::Point3D<Scalar>;
      using MatcherType  = gr::Match4pcsBase<gr::Functor4PCS, PointType,
                                             TrVisitorType, gr::AdaptivePointFilter,
                                             gr::AdaptivePointFilter::Options>;
      using OptionType   = typename MatcherType::OptionsType;

      UniformDistSampler<PointType> sampler;

      OptionType options;
      if(! Demo::setOptionsFromArgs(options, logger))
        exit(-2); /// \FIXME use status codes for error reporting

      score = computeAlignment<MatcherType, PointType> (options, logger, set1, set2,
                                                        mat, sampler, visitor);
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
