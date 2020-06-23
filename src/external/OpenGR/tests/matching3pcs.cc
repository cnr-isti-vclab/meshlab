//
// Created by Sandra Alfaro on 31/05/18.
//

#include <gr/algorithms/matchBase.h>
#include <gr/algorithms/match3pcs.h>
#include <gr/algorithms/PointPairFilter.h>
#include "gr/io/io.h"
#include "gr/utils/geometry.h"

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <array>
#include <fstream>
#include <iostream>
#include <string>

#include <boost/filesystem.hpp>

#include "testing.h"

#define WRITE_OUTPUT_FILES

#define sqr(x) ((x) * (x))

using namespace std;
using namespace gr;

using Scalar = Point3D<float>::Scalar;
enum {Dim = 3};
typedef Eigen::Transform<Scalar, Dim, Eigen::Affine> Transform;

const int nbSet = 2;

struct TrVisitorType {
    template <typename Derived>
    inline void operator() (
            float fraction,
            float best_LCP,
            const Eigen::MatrixBase<Derived>& /*transformation*/) {
      if (fraction >= 0)
        {
          printf("done: %d%c best: %f                  \r",
                 static_cast<int>(fraction * 100), '%', best_LCP);
          fflush(stdout);
        }
    }
    constexpr bool needsGlobalTransformation() const { return false; }
};

constexpr Utils::LogLevel loglvl = Utils::Verbose;
Utils::Logger logger(loglvl);

/*!
  Datasets and associated parameters
  In the current state we use a conservative configuration, better timing
  could be achieved by tuning a bit more the parameters.
  */

std::array<std::string, nbSet> confFiles = {
    "./datasets/bunny/data/bun.conf",
    "./datasets/armadillo/ArmadilloSide.conf"
};

std::array<Scalar, nbSet> deltas  = {
    0.004,
    0.003,
};

std::array<Scalar, nbSet> overlaps = {
    0.8,
    0.7,
};

std::array<bool, nbSet> swapPQ {
    true,
    false,
};

std::array<Scalar, nbSet> n_points = {
    200,
    200,
};


// Maximum allowed computation time.
int max_time_seconds = 1e9;


/*!
  Read a configuration file from Standford 3D shape repository and
  output a set of filename and eigen transformations
  */
inline void
extractFilesAndTrFromStandfordConfFile(
        const std::string &confFilePath,
        std::vector<Transform>& transforms,
        std::vector<string>& files
){
    using namespace boost;
    using namespace std;

    VERIFY (filesystem::exists(confFilePath) && filesystem::is_regular_file(confFilePath));

    // extract the working directory for the configuration path
    const string workingDir = filesystem::path(confFilePath).parent_path().native();
    VERIFY (filesystem::exists(workingDir));

    // read the configuration file and call the matching process
    string line;
    ifstream confFile;
    confFile.open(confFilePath);
    VERIFY (confFile.is_open());

    while ( getline (confFile,line) )
    {
        istringstream iss (line);
        vector<string> tokens{istream_iterator<string>{iss},
                              istream_iterator<string>{}};

        // here we know that the tokens are:
        // [0]: keyword, must be bmesh
        // [1]: 3D object filename
        // [2-4]: target translation with previous object
        // [5-8]: target quaternion with previous object

        if (tokens.size() == 9){
            if (tokens[0].compare("bmesh") == 0){

                string inputfile = filesystem::path(confFilePath).parent_path().string()+string("/")+tokens[1];
                VERIFY(filesystem::exists(inputfile) && filesystem::is_regular_file(inputfile));

                // build the Eigen rotation matrix from the rotation and translation stored in the files
                Eigen::Matrix<Scalar, Dim, 1> tr (
                        std::atof(tokens[2].c_str()),
                        std::atof(tokens[3].c_str()),
                        std::atof(tokens[4].c_str()));

                Eigen::Quaternion<Scalar> quat(
                        std::atof(tokens[8].c_str()), // eigen starts by w
                        std::atof(tokens[5].c_str()),
                        std::atof(tokens[6].c_str()),
                        std::atof(tokens[7].c_str()));

                quat.normalize();

                Transform transform (Transform::Identity());
                transform.rotate(quat);
                transform.translate(-tr);

                transforms.push_back(transform);
                files.push_back(inputfile);


            }
        }
    }
    confFile.close();
}

void test_model(const vector<Transform> &transforms,
                const vector<string> &files,
                vector<Point3D<float> > &mergedset,
                int i,
                int param_i){
    using namespace gr;

    const string input1 = files.at(i-1);
    const string input2 = files.at(i);

    cout << "Matching " << input2.c_str() << endl;

    vector<Point3D<float> > set1, set2;
    vector<Eigen::Matrix2f> tex_coords1, tex_coords2;
    vector<typename Point3D<float>::VectorType> normals1, normals2;
    vector<tripple> tris1, tris2;
    vector<std::string> mtls1, mtls2;

    IOManager iomanager;
    VERIFY(iomanager.ReadObject((char *)input1.c_str(), set1, tex_coords1, normals1, tris1, mtls1));
    VERIFY(iomanager.ReadObject((char *)input2.c_str(), set2, tex_coords2, normals2, tris2, mtls2));


    using MatrixType = Eigen::Matrix<typename Point3D<float>::Scalar, 4, 4>;
    // clean only when we have pset to avoid wrong face to point indexation
    if (tris1.size() == 0)
        Utils::CleanInvalidNormals(set1, normals1);
    if (tris2.size() == 0)
        Utils::CleanInvalidNormals(set2, normals2);

    // first transform the first mesh to its gt coordinates:
    // we compare only pairwise matching, so we don't want to
    // accumulate error during the matching process
    // Transforms Q by the new transformation.
    {
        MatrixType transformation = transforms[i-1].inverse().matrix();
        for (int j = 0; j < set1.size(); ++j) {
            set1[j].pos() = (transformation * set1[j].pos().homogeneous()).head<3>();

//            cv::Mat first(4, 1, CV_64F), transformed;
//            first.at<double>(0, 0) = set1[j].x();
//            first.at<double>(1, 0) = set1[j].y();
//            first.at<double>(2, 0) = set1[j].z();
//            first.at<double>(3, 0) = 1;
//            transformed = transformation * first;
//            set1[j].x() = transformed.at<double>(0, 0);
//            set1[j].y() = transformed.at<double>(1, 0);
//            set1[j].z() = transformed.at<double>(2, 0);
        }
    }

    mergedset.insert(mergedset.end(), set1.begin(), set1.end());

    // Our matcher.
    using MatcherType = gr::Match3pcs<gr::Point3D<float>, TrVisitorType, gr::AdaptivePointFilter, gr::AdaptivePointFilter::Options>;
    using OptionType  = typename MatcherType::OptionsType;
    using SamplerType   = gr::UniformDistSampler<gr::Point3D<float> >;


    OptionType options;
    TrVisitorType visitor;
    SamplerType sampler;

    // Set parameters.
    MatrixType mat (MatrixType::Identity());
    VERIFY(options.configureOverlap(overlaps[param_i]));
    options.sample_size = n_points[param_i];
    options.max_time_seconds = max_time_seconds;
    options.delta = deltas[param_i];

    Scalar score = 0.;

    MatcherType matcher(options, logger);

    cout << "./Super4PCS -i "
         << input1.c_str() << " "
         << input2.c_str()
         << " -o " << options.getOverlapEstimation()
         << " -d " << options.delta
         << " -n " << options.sample_size
         << " -a " << options.max_normal_difference
         << " -c " << options.max_color_distance
         << " -t " << options.max_time_seconds
         << endl;
    score = matcher.ComputeTransformation(mergedset, set2, mat, sampler, visitor);

#ifdef WRITE_OUTPUT_FILES
    stringstream iss;
    iss << input2;
    iss << "_aligned.ply";
    Utils::TransformPointCloud(set2, mat);
    iomanager.WriteObject(iss.str().c_str(),
                          set2,
                          tex_coords2,
                          normals2,
                          tris2,
                          mtls2);
#endif

    Transform transformEst (mat);

    cout << "Reference: " << endl << transforms[i].matrix() << endl;
    cout << "Estimation: " << endl << transformEst.matrix() << endl;

    Eigen::Quaternion<Scalar>
            q    (transformEst.rotation()),
            qref (transforms[i].rotation());
    cout << " " << q.x()
         << " " << q.y()
         << " " << q.z()
         << " " << q.w()  << endl;
    cout << " " << qref.x()
         << " " << qref.y()
         << " " << qref.z()
         << " " << qref.w()  << endl;

    Scalar rotDiff = std::abs( ( q.vec().array().abs() - ( qref.vec().array().abs() ) ).abs().sum()) + std::abs(std::abs(q.w()) - std::abs(qref.w()));
    Scalar trDiff  = std::abs(transformEst.translation().dot(transforms[i].translation()));

    cout << "rotDiff = " << rotDiff << endl;
    cout << "trDiff = " << trDiff << endl;

    // use different tests, just to get more verbose output
    VERIFY(rotDiff <= 0.3);
    VERIFY(trDiff <= 0.1);
    VERIFY(rotDiff + trDiff <= 0.3);

#ifdef WRITE_OUTPUT_FILES
    stringstream iss2;
    iss2 << input1;
    iss2 << "_merged.ply";
    iomanager.WriteObject(iss2.str().c_str(),
                          mergedset,
                          vector<Eigen::Matrix2f>(),
                          vector<typename Point3D<float>::VectorType>(),
                          vector<tripple>(),
                          vector<std::string>());
#endif
}

int main(int argc, const char **argv) {
    using std::string;

    const char* custom_argv [1] = {"matching"};

    if(!Testing::init_testing(1, custom_argv))
    {
        return EXIT_FAILURE;
    }

    if(argc < 2) // we accept only 1 argument: the test id
    {
        return EXIT_FAILURE;
    }


    int i = std::atoi(argv[1]);
    if (i > confFiles.size()) return EXIT_FAILURE;

    cout << "Starting to process test " << i << endl;

    vector<Transform> transforms;
    vector<string> files;
    extractFilesAndTrFromStandfordConfFile(confFiles.at(i), transforms, files);

    VERIFY(transforms.size() == files.size());
    const int nbTests = 2;


    // In this test we match each frame to the union of the previously matched
    // frames.
    // In practice we can't use the output of Super4PCS directly, it would require
    // a local ICP to avoid error accumulation. So we sum-up the GT transformations,
    // and check our Super4PCS is working well by comparing the estimated transformation
    // matrix and the GT.
    vector<Point3D<float> > mergedset;


    for (int j = 1; j <= nbTests; ++j){
        CALL_SUBTEST(( test_model(transforms, files, mergedset, j, i) ));
    }

    return EXIT_SUCCESS;
}
