// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#include "randperm.h"
#include "colon.h"
#include <algorithm>

template <typename DerivedI, typename URBG>
IGL_INLINE void igl::randperm(
  const int n,
  Eigen::PlainObjectBase<DerivedI> & I,
  URBG && urbg)
{
  Eigen::VectorXi II;
  igl::colon(0,1,n-1,II);
  I = II;

  std::shuffle(I.data(),I.data()+n, urbg);
}

template <typename DerivedI>
IGL_INLINE void igl::randperm(
  const int n,
  Eigen::PlainObjectBase<DerivedI> & I)
{
  return igl::randperm(n, I, std::minstd_rand(std::rand()));
}

#ifdef IGL_STATIC_LIBRARY
// Explicit template instantiation
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::minstd_rand0>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::minstd_rand0 &&);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::minstd_rand0 &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::minstd_rand0 &);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::minstd_rand0>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::minstd_rand0 &&);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::minstd_rand0 &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::minstd_rand0 &);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::minstd_rand>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::minstd_rand &&);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::minstd_rand &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::minstd_rand &);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::minstd_rand>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::minstd_rand &&);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::minstd_rand &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::minstd_rand &);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::mt19937>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::mt19937 &&);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::mt19937 &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::mt19937 &);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::mt19937>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::mt19937 &&);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::mt19937 &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::mt19937 &);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::mt19937_64>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::mt19937_64 &&);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::mt19937_64 &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::mt19937_64 &);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::mt19937_64>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::mt19937_64 &&);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::mt19937_64 &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::mt19937_64 &);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::ranlux24_base>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::ranlux24_base &&);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::ranlux24_base &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::ranlux24_base &);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::ranlux24_base>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::ranlux24_base &&);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::ranlux24_base &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::ranlux24_base &);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::ranlux48_base>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::ranlux48_base &&);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::ranlux48_base &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::ranlux48_base &);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::ranlux48_base>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::ranlux48_base &&);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::ranlux48_base &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::ranlux48_base &);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::ranlux24>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::ranlux24 &&);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::ranlux24 &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::ranlux24 &);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::ranlux24>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::ranlux24 &&);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::ranlux24 &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::ranlux24 &);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::ranlux48>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::ranlux48 &&);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::ranlux48 &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::ranlux48 &);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::ranlux48>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::ranlux48 &&);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::ranlux48 &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::ranlux48 &);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::knuth_b>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::knuth_b &&);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>, std::knuth_b &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&, std::knuth_b &);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::knuth_b>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::knuth_b &&);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>, std::knuth_b &>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, std::knuth_b &);
template void igl::randperm<Eigen::Matrix<int, -1, 1, 0, -1, 1>>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&);
template void igl::randperm<Eigen::Matrix<int, -1, -1, 0, -1, -1>>(int, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&);
#endif
