// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2015 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#include "edge_flaps.h"
#include "unique_edge_map.h"
#include <vector>
#include <cassert>

IGL_INLINE void igl::edge_flaps(
  const Eigen::MatrixXi & F,
  const Eigen::MatrixXi & uE,
  const Eigen::VectorXi & EMAP,
  Eigen::MatrixXi & EF,
  Eigen::MatrixXi & EI)
{
  // Initialize to boundary value
  EF.setConstant(uE.rows(),2,-1);
  EI.setConstant(uE.rows(),2,-1);
  // loop over all faces
  for(int f = 0;f<F.rows();f++)
  {
    // loop over edges across from corners
    for(int v = 0;v<3;v++)
    {
      // get edge id
      const int e = EMAP(v*F.rows()+f);
      // See if this is left or right flap w.r.t. edge orientation
      if( F(f,(v+1)%3) == uE(e,0) && F(f,(v+2)%3) == uE(e,1))
      {
        EF(e,0) = f;
        EI(e,0) = v;
      }else
      {
        assert(F(f,(v+1)%3) == uE(e,1) && F(f,(v+2)%3) == uE(e,0));
        EF(e,1) = f;
        EI(e,1) = v;
      }
    }
  }
}

IGL_INLINE void igl::edge_flaps(
  const Eigen::MatrixXi & F,
  Eigen::MatrixXi & uE,
  Eigen::VectorXi & EMAP,
  Eigen::MatrixXi & EF,
  Eigen::MatrixXi & EI)
{
  Eigen::MatrixXi allE;
  igl::unique_edge_map(F,allE,uE,EMAP);
  // Const-ify to call overload
  const auto & cuE = uE;
  const auto & cEMAP = EMAP;
  return edge_flaps(F,cuE,cEMAP,EF,EI);
}
