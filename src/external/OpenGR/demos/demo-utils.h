// Copyright 2017 Nicolas Mellado
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
// This file is part of the implementation of the 4-points Congruent Sets (4PCS)
// algorithm presented in:
//
// Super 4PCS: Fast Global Pointcloud Registration via Smart Indexing
// Nicolas Mellado, Dror Aiger, Niloy J. Mitra
// Symposium on Geometry Processing 2014.
//
// Given two sets of points in 3-space, P and Q, the algorithm applies RANSAC
// in roughly O(n^2) time instead of O(n^3) for standard RANSAC, using an
// efficient method based on invariants, to find the set of all 4-points in Q
// that can be matched by rigid transformation to a given set of 4-points in P
// called a base. This avoids the need to examine all sets of 3-points in Q
// against any base of 3-points in P as in standard RANSAC.
// The algorithm can use colors and normals to speed-up the matching
// and to improve the quality. It can be easily extended to affine/similarity
// transformation but then the speed-up is smaller because of the large number
// of congruent sets. The algorithm can also limit the range of transformations
// when the application knows something on the initial pose but this is not
// necessary in general (though can speed the runtime significantly).

// Home page of the 4PCS project (containing the paper, presentations and a
// demo): http://graphics.stanford.edu/~niloy/research/fpcs/fpcs_sig_08.html
// Use google search on "4-points congruent sets" to see many related papers
// and applications.


#ifndef _OPENGR_DEMO_UTILS_H_
#define _OPENGR_DEMO_UTILS_H_

#include <fstream>
#include <iostream>
#include <string>

#include <gr/shared.h>
#include <gr/utils/logger.h>

namespace gr {
namespace Demo {
// First input.
static std::string input1 = "input1.obj";

// Second input.
static std::string input2 = "input2.obj";

// Output. The transformed second input.
static std::string output = "";
// Default name for the '.obj' output file
static std::string defaultObjOutput = "output.obj";
// Default name for the '.ply' output file
static std::string defaultPlyOutput = "output.ply";

// Transformation matrice.
static std::string outputMat = "";

// Sampled cloud 1
static std::string outputSampled1 = "";

// Sampled cloud 2
static std::string outputSampled2 = "";

// Delta (see the paper).
static double delta = 5.0;

// Estimated overlap (see the paper).
static double overlap = 0.2;

// Threshold of the computed overlap for termination. 1.0 means don't terminate
// before the end.
static double thr = 1.0;

// Maximum norm of RGB values between corresponded points. 1e9 means don't use.
static double max_color = -1;

// Number of sampled points in both files. The 4PCS allows a very aggressive
// sampling.
static int n_points = 200;

// Maximum angle (degrees) between corresponded normals.
static double norm_diff = -1;

// Maximum allowed computation time.
static int max_time_seconds = 10;

// Point type to use - ExtPointBinding demo
static int point_type = 0;
static int max_point_type = 2; // 0: gr::Point3D, 1: extlib1::PointType1, 2: extlib2::PointType2

static bool use_super4pcs = true;

static inline void printParameterList(){
    fprintf(stderr, "Parameter list:\n");
    fprintf(stderr, "\t[ -o overlap (%2.2f) ]\n", overlap);
    fprintf(stderr, "\t[ -d delta (%2.2f) ]\n", delta);
    fprintf(stderr, "\t[ -n n_points (%d) ]\n", n_points);
    fprintf(stderr, "\t[ -a norm_diff (%f) ]\n", norm_diff);
    fprintf(stderr, "\t[ -c max_color_diff (%f) ]\n", max_color);
    fprintf(stderr, "\t[ -t max_time_seconds (%d) ]\n", max_time_seconds);
}

static inline void printUsage(int /*argc*/, char **argv){
    fprintf(stderr, "\nUsage: %s -i input1 input2\n", argv[0]);
    printParameterList();
}

static inline int getArgs(int argc,
                           char **argv,
                           const Utils::Logger& logger = Utils::Logger()) {
  int i = 1;
  while (i < argc) {
    if (!strcmp(argv[i], "-i")) {
      input1 = std::string(argv[++i]);
      input2 = std::string(argv[++i]);
    } else if (!strcmp(argv[i], "-o")) {
      overlap = atof(argv[++i]);
    } else if (!strcmp(argv[i], "-d")) {
      delta = atof(argv[++i]);
    } else if (!strcmp(argv[i], "-c")) {
      max_color = atof(argv[++i]);
    } else if (!strcmp(argv[i], "-t")) {
      max_time_seconds = atoi(argv[++i]);
    } else if (!strcmp(argv[i], "-a")) {
      norm_diff = atof(argv[++i]);
    } else if (!strcmp(argv[i], "-n")) {
      n_points = atoi(argv[++i]);
    } else if (!strcmp(argv[i], "-r")) {
      output = argv[++i];
    } else if (!strcmp(argv[i], "-m")) {
      outputMat = argv[++i];
    } else if (!strcmp(argv[i], "-x")) {
      use_super4pcs = false;
    } else if (!strcmp(argv[i], "--sampled1")) {
      outputSampled1 = argv[++i];
    } else if (!strcmp(argv[i], "--sampled2")) {
      outputSampled2 = argv[++i];
    } else if (!strcmp(argv[i], "-p")) {
      point_type = atoi(argv[++i]);
      if(point_type < 0 || point_type > max_point_type) {
        std::cerr << "Invalid point type: setting to default (0)" << std::endl;
        point_type = 0;
      } 
    } else if (!strcmp(argv[i], "-h")) {
      return 1;
    } else if (argv[i][0] == '-') {
      std::cerr << "Unknown flag\n";
      return -1;
    };
    i++;
  }

  // if no output file (geometry/matrix) is set, force 3d mesh
  if (output.empty() && outputMat.empty()) output = defaultObjOutput;

  return 0;
}

template <typename OptionType>
static inline bool setOptionsFromArgs( OptionType &options,
                                       const Utils::Logger& logger = Utils::Logger())
{
    bool overlapOk = options.configureOverlap(overlap);
    if(! overlapOk )  {
        logger.Log<Utils::ErrorReport>("Invalid overlap configuration. ABORT");
        return false;
    }
    options.sample_size = n_points;
    options.max_normal_difference = norm_diff;
    options.max_color_distance = max_color;
    options.max_time_seconds = max_time_seconds;
    options.delta = delta;

    return true;
}

} // namespace Demo
} // namespace gr
#endif
