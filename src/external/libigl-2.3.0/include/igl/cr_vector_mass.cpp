// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2020 Oded Stein <oded.stein@columbia.edu>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#include "cr_vector_mass.h"

#include <vector>

#include "orient_halfedges.h"

#include "doublearea.h"
#include "squared_edge_lengths.h"


template <typename DerivedV, typename DerivedF, typename DerivedE,
typename DerivedOE, typename ScalarM>
IGL_INLINE void
igl::cr_vector_mass(
  const Eigen::MatrixBase<DerivedV>& V,
  const Eigen::MatrixBase<DerivedF>& F,
  const Eigen::MatrixBase<DerivedE>& E,
  const Eigen::MatrixBase<DerivedOE>& oE,
  Eigen::SparseMatrix<ScalarM>& M)
{
  Eigen::Matrix<typename DerivedV::Scalar, Eigen::Dynamic, Eigen::Dynamic>
  l_sq;
  squared_edge_lengths(V, F, l_sq);
  cr_vector_mass_intrinsic(F, l_sq, E, oE, M);
}


template <typename DerivedV, typename DerivedF, typename DerivedE,
typename DerivedOE, typename ScalarM>
IGL_INLINE void
igl::cr_vector_mass(
  const Eigen::MatrixBase<DerivedV>& V,
  const Eigen::MatrixBase<DerivedF>& F,
  Eigen::PlainObjectBase<DerivedE>& E,
  Eigen::PlainObjectBase<DerivedOE>& oE,
  Eigen::SparseMatrix<ScalarM>& M)
{
  if(E.rows()!=F.rows() || E.cols()!=F.cols() || oE.rows()!=F.rows() ||
   oE.cols()!=F.cols()) {
    orient_halfedges(F, E, oE);
  }

  const Eigen::PlainObjectBase<DerivedE>& cE = E;
  const Eigen::PlainObjectBase<DerivedOE>& coE = oE;
  cr_vector_mass(V, F, cE, coE, M);
}


template <typename DerivedF, typename DerivedL_sq, typename DerivedE,
typename DerivedOE, typename ScalarM>
IGL_INLINE void
igl::cr_vector_mass_intrinsic(
  const Eigen::MatrixBase<DerivedF>& F,
  const Eigen::MatrixBase<DerivedL_sq>& l_sq,
  const Eigen::MatrixBase<DerivedE>& E,
  const Eigen::MatrixBase<DerivedOE>& oE,
  Eigen::SparseMatrix<ScalarM>& M)
{
  Eigen::Matrix<typename DerivedL_sq::Scalar, Eigen::Dynamic, Eigen::Dynamic>
  dA;
  DerivedL_sq l_sqrt = l_sq.array().sqrt().matrix();
  doublearea(l_sqrt, dA);
  cr_vector_mass_intrinsic(F, l_sq, dA, E, oE, M);
}


template <typename DerivedF, typename DerivedL_sq, typename DeriveddA,
typename DerivedE, typename DerivedOE, typename ScalarM>
IGL_INLINE void
igl::cr_vector_mass_intrinsic(
 const Eigen::MatrixBase<DerivedF>& F,
 const Eigen::MatrixBase<DerivedL_sq>& l_sq,
 const Eigen::MatrixBase<DeriveddA>& dA,
 const Eigen::MatrixBase<DerivedE>& E,
 const Eigen::MatrixBase<DerivedOE>& oE,
 Eigen::SparseMatrix<ScalarM>& M)
{
  assert(F.cols()==3 && "Faces have three vertices");
  assert(E.rows()==F.rows() && E.cols()==F.cols() && oE.rows()==F.rows() &&
   oE.cols()==F.cols() && "Wrong dimension in edge vectors");

  const Eigen::Index m = F.rows();
  const typename DerivedE::Scalar nE = E.maxCoeff() + 1;

  std::vector<Eigen::Triplet<ScalarM> > tripletList;
  tripletList.reserve(2*3*m);
  for(Eigen::Index f=0; f<m; ++f) {
    for(int e=0; e<3; ++e) {
      const typename DerivedF::Scalar v1=F(f,(e+1)%3), v2=F(f,(e+2)%3);
      //Scaled
      const ScalarM entry = dA(f) / 6;
      tripletList.emplace_back(E(f,e), E(f,e), entry);
      tripletList.emplace_back(E(f,e)+nE, E(f,e)+nE, entry);
    }
  }
  M.resize(2*nE, 2*nE);
  M.setFromTriplets(tripletList.begin(), tripletList.end());
}


#ifdef IGL_STATIC_LIBRARY
// Explicit template instantiation
template void igl::cr_vector_mass<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, double>(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&, Eigen::SparseMatrix<double, 0, int>&);
template void igl::cr_vector_mass_intrinsic<Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, double>(Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::SparseMatrix<double, 0, int>&);
#endif
