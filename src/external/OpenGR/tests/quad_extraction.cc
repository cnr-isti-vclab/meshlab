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
// This test check the validity of the quad extraction subroutine on random data
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

#include "algorithms/4pcs.h"
#include "algorithms/super4pcs.h"

#include "Eigen/Dense"

#include <fstream>
#include <iostream>
#include <string>

#include "testing.h"

using namespace match_4pcs;
using namespace Super4PCS;


template <typename MatchRef, typename MatchType>
void callSubTests()
{
    using Scalar = typename MatchType::Scalar;
    using PairsVector = typename MatchType::PairsVector;

    match_4pcs::Match4PCSOptions opt;
    opt.delta = 0.1;
    opt.overlap_estimation = 0.9;

    const size_t nbPointP = 10;
    const size_t nbPointQ = 15;

    Scalar pair_distance_epsilon = MatchType::distance_factor * opt.delta;

    for(int i = 0; i < Testing::g_repeat; ++i)
    {
        // generate input point cloud
        std::vector<Point3D<float> > P, Q;
        Testing::generateSphereCloud(P, nbPointP);
        Testing::generateSphereCloud(Q, nbPointQ);

        Scalar inv1, inv2;
        int b1, b2, b3, b4;

        ///////////////////
        /// prepare matcher and init 4-points basis
        MatchRef  ref(opt);
        MatchType match (opt);
        match.init(P, Q); ref.init(P, Q);
        int maxTry = 5;
        while( maxTry-- >= 0 &&
               ! match.SelectQuadrilateral(inv1, inv2, b1, b2, b3, b4) );
        VERIFY (maxTry >= 0);

        const auto& base = match.base3D();

        // Computes distance between pairs.
        const Scalar distance1 = (base[0].pos()- base[1].pos()).norm();
        const Scalar distance2 = (base[2].pos()- base[3].pos()).norm();

        ///////////////////
        /// extract pairs
        std::vector<std::pair<int, int>> pairs1, pairs2;
        Testing::extractPairs(distance1, pair_distance_epsilon, Q, pairs1);
        Testing::extractPairs(distance2, pair_distance_epsilon, Q, pairs2);

        ///////////////////
        /// extract quads
        std::vector<match_4pcs::Quadrilateral> congruent_ref, congruent_quads;
        if (ref.FindCongruentQuadrilaterals(inv1, inv2,
                                            MatchType::distance_factor * opt.delta,
                                            MatchType::distance_factor * opt.delta,
                                            pairs1,
                                            pairs2,
                                            &congruent_ref)) {
            VERIFY (match.FindCongruentQuadrilaterals(inv1, inv2,
                                                      MatchType::distance_factor * opt.delta,
                                                      MatchType::distance_factor * opt.delta,
                                                      pairs1,
                                                      pairs2,
                                                      &congruent_quads));

            std::cout << congruent_ref.size() << std::endl;
            std::cout << congruent_quads.size() << std::endl;

            std::sort(congruent_ref.begin(), congruent_ref.end());
            std::sort(congruent_quads.begin(), congruent_quads.end());

//            for(const Quadrilateral& q : congruent_ref)
//                std::cout << "[" << q[0] << " " << q[1] << " " << q[2] << " " << q[3] << "]  ";
//            std::cout << std::endl;

//            for(const Quadrilateral& q : congruent_quads)
//                std::cout << "[" << q[0] << " " << q[1] << " " << q[2] << " " << q[3] << "]  ";
//            std::cout << std::endl;

            // we check that all the quads found by Match exist in ref superset
//            for(const Quadrilateral& q : congruent_quads){
//                auto it =
//                std::find(congruent_ref.cbegin(), congruent_ref.cend(), q);
//                VERIFY (it != congruent_ref.cend());
//            }

            // TODO: Add more tests to be sure that Match will find the real
            // congruent set in ref superset.
            //   - implement quads filtering, and compare filtered sets.
        }
    }
}

int main(int argc, const char **argv) {
    if(!Testing::init_testing(argc, argv))
    {
        return EXIT_FAILURE;
    }

    using std::cout;
    using std::endl;

    cout << "Compare quad extraction between 4PCS and Super4PCS" << endl;
    callSubTests<Match4PCS, MatchSuper4PCS>();
    cout << "Ok..." << endl;


    return EXIT_SUCCESS;
}
