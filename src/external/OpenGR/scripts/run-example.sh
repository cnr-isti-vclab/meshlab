#! /bin/sh

# Copyright 2014 Nicolas Mellado
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# -------------------------------------------------------------------------- ::
#
# Authors: Nicolas Mellado
#
# An implementation of the Super 4-points Congruent Sets (Super 4PCS)
# algorithm presented in:
#
# Super 4PCS: Fast Global Pointcloud Registration via Smart Indexing
# Nicolas Mellado, Dror Aiger, Niloy J. Mitra
# Symposium on Geometry Processing 2014.
#
# Data acquisition in large-scale scenes regularly involves accumulating
# information across multiple scans. A common approach is to locally align scan
# pairs using Iterative Closest Point (ICP) algorithm (or its variants), but
# requires static scenes and small motion between scan pairs. This prevents
# accumulating data across multiple scan sessions and/or different acquisition
# modalities (e.g., stereo, depth scans). Alternatively, one can use a global
# registration algorithm allowing scans to be in arbitrary initial poses. The
# state-of-the-art global registration algorithm, 4PCS, however has a quadratic
# time complexity in the number of data points. This vastly limits its
# applicability to acquisition of large environments. We present Super 4PCS for
# global pointcloud registration that is optimal, i.e., runs in linear time (in
# the number of data points) and is also output sensitive in the complexity of
# the alignment problem based on the (unknown) overlap across scan pairs.
# Technically, we map the algorithm as an 'instance problem' and solve it
# efficiently using a smart indexing data organization. The algorithm is
# simple, memory-efficient, and fast. We demonstrate that Super 4PCS results in
# significant speedup over alternative approaches and allows unstructured
# efficient acquisition of scenes at scales previously not possible. Complete
# source code and datasets are available for research use at
# http://geometry.cs.ucl.ac.uk/projects/2014/super4PCS/.
#
#
#
# This file is supposed to be ran from super4pcs_install_dir/scripts/
#
# run both 4pcs and super4pcs on the hippo model.
# parameters are optimised to get an accurate alignement of the two models
# -o input estimated overlap in [0,1]
# -d delta, used to compute the LCP between the two models
# -t maximum computation time in seconds
# -n number of samples used for the matching
#
# When available, also runs the registration with the PCL wrapper

export LD_LIBRARY_PATH=../lib/

SPACER="------------------------------"

echo ${SPACER}
echo "Running Super4PCS"
time -p ../bin/Super4PCS -i ../assets/hippo1.obj ../assets/hippo2.obj -o 0.7 -d 0.01 -t 1000 -n 200 -r super4pcs_fast.obj -m mat_super4pcs_fast.txt


echo ${SPACER}
echo "Running 4PCS"
time -p ../bin/Super4PCS -i ../assets/hippo1.obj ../assets/hippo2.obj -o 0.7 -d 0.01 -t 1000 -n 200 -r      4pcs_fast.obj -m      mat_4pcs_fast.txt -x


if [ -f ../bin/OpenGR-PCLWrapper ]; then
    echo ${SPACER}
    echo "Running PCLWrapper"
    ../bin/OpenGR-PCLWrapper ../assets/hippo1.obj ../assets/hippo2.obj -o 0.7 -d 0.01 -t 1000 -n 200
fi
