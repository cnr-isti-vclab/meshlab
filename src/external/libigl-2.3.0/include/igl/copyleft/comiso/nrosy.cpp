// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2014 Daniele Panozzo <daniele.panozzo@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.

#include "nrosy.h"

#include <igl/copyleft/comiso/nrosy.h>
#include <igl/triangle_triangle_adjacency.h>
#include <igl/edge_topology.h>
#include <igl/per_face_normals.h>

#include <stdexcept>
#include "../../PI.h"

#include <Eigen/Geometry>
#include <Eigen/Sparse>
#include <queue>
#include <vector>

#include <gmm/gmm.h>
#include <CoMISo/Solver/ConstrainedSolver.hh>
#include <CoMISo/Solver/MISolver.hh>
#include <CoMISo/Solver/GMM_Tools.hh>

namespace igl
{
namespace copyleft
{

namespace comiso
{
class NRosyField
{
public:
  // Init
  IGL_INLINE NRosyField(const Eigen::MatrixXd& _V, const Eigen::MatrixXi& _F);

  // Generate the N-rosy field
  // N degree of the rosy field
  // round separately: round the integer variables one at a time, slower but higher quality
  IGL_INLINE void solve(int N = 4);

  // Set a hard constraint on fid
  // fid: face id
  // v: direction to fix (in 3d)
  IGL_INLINE void setConstraintHard(int fid, const Eigen::Vector3d& v);

  // Set a soft constraint on fid
  // fid: face id
  // w: weight of the soft constraint, clipped between 0 and 1
  // v: direction to fix (in 3d)
  IGL_INLINE void setConstraintSoft(int fid, double w, const Eigen::Vector3d& v);

  // Set the ratio between smoothness and soft constraints (0 -> smoothness only, 1 -> soft constr only)
  IGL_INLINE void setSoftAlpha(double alpha);

  // Reset constraints (at least one constraint must be present or solve will fail)
  IGL_INLINE void resetConstraints();

  // Return the current field
  IGL_INLINE Eigen::MatrixXd getFieldPerFace();

  // Compute singularity indexes
  IGL_INLINE void findCones(int N);

  // Return the singularities
  IGL_INLINE Eigen::VectorXd getSingularityIndexPerVertex();

private:
  // Compute angle differences between reference frames
  IGL_INLINE void computek();

  // Remove useless matchings
  IGL_INLINE void reduceSpace();

  // Prepare the system matrix
  IGL_INLINE void prepareSystemMatrix(int N);

  // Solve with roundings using CoMIso
  IGL_INLINE void solveRoundings();

  // Convert a vector in 3d to an angle wrt the local reference system
  IGL_INLINE double convert3DtoLocal(unsigned fid, const Eigen::Vector3d& v);

  // Convert an angle wrt the local reference system to a 3d vector
  IGL_INLINE Eigen::Vector3d convertLocalto3D(unsigned fid, double a);

  // Compute the per vertex angle defect
  IGL_INLINE Eigen::VectorXd angleDefect();

  // Temporary variable for the field
  Eigen::VectorXd angles;

  // Hard constraints
  Eigen::VectorXd hard;
  std::vector<bool> isHard;

  // Soft constraints
  Eigen::VectorXd soft;
  Eigen::VectorXd wSoft;
  double softAlpha;

  // Face Topology
  Eigen::MatrixXi TT, TTi;

  // Edge Topology
  Eigen::MatrixXi EV, FE, EF;
  std::vector<bool> isBorderEdge;

  // Per Edge information
  // Angle between two reference frames
  Eigen::VectorXd k;

  // Jumps
  Eigen::VectorXi p;
  std::vector<bool> pFixed;

  // Mesh
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;

  // Normals per face
  Eigen::MatrixXd N;

  // Singularity index
  Eigen::VectorXd singularityIndex;

  // Reference frame per triangle
  std::vector<Eigen::MatrixXd> TPs;

  // System stuff
  Eigen::SparseMatrix<double> A;
  Eigen::VectorXd b;
  Eigen::VectorXi tag_t;
  Eigen::VectorXi tag_p;

};

} // NAMESPACE COMISO
} // NAMESPACE COPYLEFT
} // NAMESPACE IGL

igl::copyleft::comiso::NRosyField::NRosyField(const Eigen::MatrixXd& _V, const Eigen::MatrixXi& _F)
{
  V = _V;
  F = _F;

  assert(V.rows() > 0);
  assert(F.rows() > 0);

  // Generate topological relations
  igl::triangle_triangle_adjacency(F,TT,TTi);
  igl::edge_topology(V,F, EV, FE, EF);

  // Flag border edges
  isBorderEdge.resize(EV.rows());
  for(unsigned i=0; i<EV.rows(); ++i)
    isBorderEdge[i] = (EF(i,0) == -1) || ((EF(i,1) == -1));

  // Generate normals per face
  igl::per_face_normals(V, F, N);

  // Generate reference frames
  for(unsigned fid=0; fid<F.rows(); ++fid)
  {
    // First edge
    Eigen::Vector3d e1 = V.row(F(fid,1)) - V.row(F(fid,0));
    e1.normalize();
    Eigen::Vector3d e2 = N.row(fid);
    e2 = e2.cross(e1);
    e2.normalize();

    Eigen::MatrixXd TP(2,3);
    TP << e1.transpose(), e2.transpose();
    TPs.push_back(TP);
  }

  // Alloc internal variables
  angles = Eigen::VectorXd::Zero(F.rows());
  p = Eigen::VectorXi::Zero(EV.rows());
  pFixed.resize(EV.rows());
  k = Eigen::VectorXd::Zero(EV.rows());
  singularityIndex = Eigen::VectorXd::Zero(V.rows());

  // Reset the constraints
  resetConstraints();

  // Compute k, differences between reference frames
  computek();
  softAlpha = 0.5;
}

void igl::copyleft::comiso::NRosyField::setSoftAlpha(double alpha)
{
  assert(alpha >= 0 && alpha < 1);
  softAlpha = alpha;
}


void igl::copyleft::comiso::NRosyField::prepareSystemMatrix(const int N)
{
  double Nd = N;

  // Minimize the MIQ energy
  // Energy on edge ij is
  //     (t_i - t_j + kij + pij*(2*pi/N))^2
  // Partial derivatives:
  //   t_i: 2     ( t_i - t_j + kij + pij*(2*pi/N)) = 0
  //   t_j: 2     (-t_i + t_j - kij - pij*(2*pi/N)) = 0
  //   pij: 4pi/N ( t_i - t_j + kij + pij*(2*pi/N)) = 0
  //
  //          t_i      t_j         pij       kij
  // t_i [     2       -2           4pi/N      2    ]
  // t_j [    -2        2          -4pi/N     -2    ]
  // pij [   4pi/N   -4pi/N    2*(2pi/N)^2   4pi/N  ]

  // Count and tag the variables
  tag_t = Eigen::VectorXi::Constant(F.rows(),-1);
  std::vector<int> id_t;
  size_t count = 0;
  for(unsigned i=0; i<F.rows(); ++i)
    if (!isHard[i])
    {
      tag_t(i) = count++;
      id_t.push_back(i);
    }

  size_t count_t = id_t.size();

  tag_p = Eigen::VectorXi::Constant(EF.rows(),-1);
  std::vector<int> id_p;
  for(unsigned i=0; i<EF.rows(); ++i)
  {
    if (!pFixed[i])
    {
      // if it is not fixed then it is a variable
      tag_p(i) = count++;
    }

    // if it is not a border edge,
    if (!isBorderEdge[i])
    {
      // and it is not between two fixed faces
      if (!(isHard[EF(i,0)] && isHard[EF(i,1)]))
      {
          // then it participates in the energy!
          id_p.push_back(i);
      }
    }
  }

  size_t count_p = count - count_t;
  // System sizes: A (count_t + count_p) x (count_t + count_p)
  //               b (count_t + count_p)

  b.resize(count_t + count_p);
  b.setZero();

  std::vector<Eigen::Triplet<double> > T;
  T.reserve(3 * 4 * count_p);

  for(auto eid : id_p)
  {
    int i = EF(eid, 0);
    int j = EF(eid, 1);
    bool isFixed_i = isHard[i];
    bool isFixed_j = isHard[j];
    bool isFixed_p = pFixed[eid];
    int row;
    // (i)-th row: t_i [     2       -2           4pi/N      2    ]
    if (!isFixed_i)
    {
      row = tag_t[i];
      T.emplace_back(row, tag_t[i], 2);
      if (isFixed_j)
        b(row) +=  2 * hard[j];
      else
        T.emplace_back(row, tag_t[j], -2);
      if (isFixed_p)
        b(row) += -((4. * igl::PI) / Nd) * p[eid];
      else
        T.emplace_back(row, tag_p[eid], ((4. * igl::PI) / Nd));
      b(row) += -2 * k[eid];
      assert(hard[i] == hard[i]);
      assert(hard[j] == hard[j]);
      assert(p[eid] == p[eid]);
      assert(k[eid] == k[eid]);
      assert(b(row) == b(row));
    }
    // (j)+1 -th row: t_j [    -2        2          -4pi/N     -2    ]
    if (!isFixed_j)
    {
      row = tag_t[j];
      T.emplace_back(row, tag_t[j], 2);
      if (isFixed_i)
        b(row) += 2 * hard[i];
      else
        T.emplace_back(row, tag_t[i], -2);
      if (isFixed_p)
        b(row) += ((4. * igl::PI) / Nd) * p[eid];
      else
        T.emplace_back(row, tag_p[eid], -((4. * igl::PI) / Nd));
      b(row) += 2 * k[eid];
      assert(k[eid] == k[eid]);
      assert(b(row) == b(row));
    }
    // (r*3)+2 -th row: pij [   4pi/N   -4pi/N    2*(2pi/N)^2   4pi/N  ]
    if (!isFixed_p)
    {
      row = tag_p[eid];
      T.emplace_back(row, tag_p[eid], (2. * pow(((2. * igl::PI) / Nd), 2)));
      if (isFixed_i)
        b(row) += -(4. * igl::PI) / Nd * hard[i];
      else
        T.emplace_back(row, tag_t[i], (4. * igl::PI) / Nd);
      if (isFixed_j)
        b(row) += (4. * igl::PI) / Nd * hard[j];
      else
        T.emplace_back(row,tag_t[j], -(4. * igl::PI) / Nd);
      b(row) += - (4 * igl::PI)/Nd * k[eid];
      assert(k[eid] == k[eid]);
      assert(b(row) == b(row));
    }
  }

  A.resize(count_t + count_p, count_t + count_p);
  A.setFromTriplets(T.begin(), T.end());

  // Soft constraints
  bool addSoft = false;

  for(unsigned i=0; i<wSoft.size();++i)
    if (wSoft[i] != 0)
      addSoft = true;

  if (addSoft)
  {
    Eigen::VectorXd bSoft = Eigen::VectorXd::Zero(count_t + count_p);
    std::vector<Eigen::Triplet<double> > TSoft;
    TSoft.reserve(2 * count_p);

    for(unsigned i=0; i<F.rows(); ++i)
    {
      int varid = tag_t[i];
      if (varid != -1) // if it is a variable in the system
      {
        TSoft.emplace_back(varid, varid, wSoft[i]);
        bSoft[varid] += wSoft[i] * soft[i];
      }
    }
    Eigen::SparseMatrix<double> ASoft(count_t + count_p, count_t + count_p);
    ASoft.setFromTriplets(TSoft.begin(), TSoft.end());

    A = (1.0 - softAlpha) * A + softAlpha * ASoft;
    b = b * (1.0 - softAlpha) + bSoft * softAlpha;
  }
}

void igl::copyleft::comiso::NRosyField::solveRoundings()
{
  unsigned n = A.rows();

  gmm::col_matrix< gmm::wsvector< double > > gmm_A(n, n);
  std::vector<double> gmm_b(n);
  std::vector<int> ids_to_round;
  std::vector<double> x(n);

  // Copy A
  for (int k=0; k<A.outerSize(); ++k)
    for (Eigen::SparseMatrix<double>::InnerIterator it(A, k); it; ++it)
    {
      gmm_A(it.row(),it.col()) += it.value();
    }

  // Copy b
  for(unsigned int i = 0; i < n;++i)
    gmm_b[i] = b[i];

  // Set variables to round
  ids_to_round.clear();
  for(unsigned i=0; i<tag_p.size();++i)
    if(tag_p[i] != -1)
      ids_to_round.push_back(tag_p[i]);

  // Empty constraints
  gmm::row_matrix< gmm::wsvector< double > > gmm_C(0, n);

  COMISO::ConstrainedSolver cs;
  cs.solve(gmm_C, gmm_A, x, gmm_b, ids_to_round, 0.0, false, true);

  // Copy the result back
  for(unsigned i=0; i<F.rows(); ++i)
    if (tag_t[i] != -1)
      angles[i] = x[tag_t[i]];
    else
      angles[i] = hard[i];

  for(unsigned i=0; i<EF.rows(); ++i)
    if(tag_p[i]  != -1)
      p[i] = (int)std::round(x[tag_p[i]]);
}


void igl::copyleft::comiso::NRosyField::solve(const int N)
{
  // Reduce the search space by fixing matchings
  reduceSpace();

  // Build the system
  prepareSystemMatrix(N);

  // Solve with integer roundings
  solveRoundings();

  // Find the cones
  findCones(N);
}

void igl::copyleft::comiso::NRosyField::setConstraintHard(const int fid, const Eigen::Vector3d& v)
{
  isHard[fid] = true;
  hard(fid) = convert3DtoLocal(fid, v);
}

void igl::copyleft::comiso::NRosyField::setConstraintSoft(const int fid, const double w, const Eigen::Vector3d& v)
{
  wSoft(fid) = w;
  soft(fid) = convert3DtoLocal(fid, v);
}

void igl::copyleft::comiso::NRosyField::resetConstraints()
{
  isHard.resize(F.rows());
  for(unsigned i = 0; i < F.rows(); ++i)
    isHard[i] = false;
  hard   = Eigen::VectorXd::Zero(F.rows());
  wSoft = Eigen::VectorXd::Zero(F.rows());
  soft = Eigen::VectorXd::Zero(F.rows());
}

Eigen::MatrixXd igl::copyleft::comiso::NRosyField::getFieldPerFace()
{
  Eigen::MatrixXd result(F.rows(),3);
  for(unsigned int i = 0; i < F.rows(); ++i)
    result.row(i) = convertLocalto3D(i, angles(i));
  return result;
}

void igl::copyleft::comiso::NRosyField::computek()
{
  // For every non-border edge
  for (unsigned eid = 0; eid < EF.rows(); ++eid)
  {
    if (!isBorderEdge[eid])
    {
      int fid0 = EF(eid,0);
      int fid1 = EF(eid,1);

      Eigen::Vector3d N0 = N.row(fid0);
      Eigen::Vector3d N1 = N.row(fid1);

      // find common edge on triangle 0 and 1
      int fid0_vc = -1;
      int fid1_vc = -1;
      for (unsigned i=0;i<3;++i)
      {
        if (EV(eid,0) == F(fid0,i))
          fid0_vc = i;
        if (EV(eid,1) == F(fid1,i))
          fid1_vc = i;
      }
      assert(fid0_vc != -1);
      assert(fid1_vc != -1);

      Eigen::Vector3d common_edge = V.row(F(fid0,(fid0_vc+1)%3)) - V.row(F(fid0,fid0_vc));
      common_edge.normalize();

      // Map the two triangles in a new space where the common edge is the x axis and the N0 the z axis
      Eigen::MatrixXd P(3,3);
      Eigen::VectorXd o = V.row(F(fid0,fid0_vc));
      Eigen::VectorXd tmp = N0.cross(common_edge);
      P << common_edge, tmp, N0;
      P.transposeInPlace();


      Eigen::MatrixXd V0(3,3);
      V0.row(0) = V.row(F(fid0,0)).transpose() -o;
      V0.row(1) = V.row(F(fid0,1)).transpose() -o;
      V0.row(2) = V.row(F(fid0,2)).transpose() -o;

      V0 = (P*V0.transpose()).transpose();

      assert(V0(0,2) < 1e-10);
      assert(V0(1,2) < 1e-10);
      assert(V0(2,2) < 1e-10);

      Eigen::MatrixXd V1(3,3);
      V1.row(0) = V.row(F(fid1,0)).transpose() -o;
      V1.row(1) = V.row(F(fid1,1)).transpose() -o;
      V1.row(2) = V.row(F(fid1,2)).transpose() -o;
      V1 = (P*V1.transpose()).transpose();

      assert(V1(fid1_vc,2) < 1e-10);
      assert(V1((fid1_vc+1)%3,2) < 1e-10);

      // compute rotation R such that R * N1 = N0
      // i.e. map both triangles to the same plane
      double alpha = -std::atan2(-V1((fid1_vc + 2) % 3, 2), -V1((fid1_vc + 2) % 3, 1));

      Eigen::MatrixXd R(3,3);
      R << 1,          0,            0,
           0, std::cos(alpha), -std::sin(alpha),
           0, std::sin(alpha),  std::cos(alpha);
      V1 = (R*V1.transpose()).transpose();

      assert(V1(0,2) < 1e-10);
      assert(V1(1,2) < 1e-10);
      assert(V1(2,2) < 1e-10);

      // measure the angle between the reference frames
      // k_ij is the angle between the triangle on the left and the one on the right
      Eigen::VectorXd ref0 = V0.row(1) - V0.row(0);
      Eigen::VectorXd ref1 = V1.row(1) - V1.row(0);

      ref0.normalize();
      ref1.normalize();
      
      double ktemp = - std::atan2(ref1(1), ref1(0)) + std::atan2(ref0(1), ref0(0));
      
      // make sure kappa is in corret range 
      auto pos_fmod = [](double x, double y){
        return (0 == y) ? x : x - y * floor(x/y);
      };
      ktemp = pos_fmod(ktemp, 2*igl::PI);
      if (ktemp > igl::PI) 
        ktemp -= 2*igl::PI;
      
      // just to be sure, rotate ref0 using angle ktemp...
      Eigen::MatrixXd R2(2,2);
      R2 << std::cos(-ktemp), -std::sin(-ktemp), std::sin(-ktemp), std::cos(-ktemp);

      tmp = R2*ref0.head<2>();

      assert(tmp(0) - ref1(0) < 1e-10);
      assert(tmp(1) - ref1(1) < 1e-10);

      k[eid] = ktemp;
    }
  }

}

void igl::copyleft::comiso::NRosyField::reduceSpace()
{
  // All variables are free in the beginning
  for(unsigned int i = 0; i < EV.rows(); ++i)
    pFixed[i] = false;

  std::vector<bool> visited(EV.rows(), false);
  std::vector<bool> starting(EV.rows(), false);

  std::queue<int> q;
  for(unsigned int i = 0; i < F.rows(); ++i)
    if (isHard[i] || wSoft[i] != 0)
    {
      q.push(i);
      starting[i] = true;
    }

  // Reduce the search space (see MI paper)
  while (!q.empty())
  {
    int c = q.front();
    q.pop();

    visited[c] = true;
    for(int i=0; i<3; ++i)
    {
      int eid = FE(c,i);
      int fid = TT(c,i);

      // skip borders
      if (fid != -1)
      {
        assert((EF(eid,0) == c && EF(eid,1) == fid) || (EF(eid,1) == c && EF(eid,0) == fid));
        // for every neighbouring face
        if (!visited[fid] && !starting[fid])
        {
          pFixed[eid] = true;
          p[eid] = 0;
          visited[fid] = true;
          q.push(fid);
        }
      }
      else
      {
        // fix borders
        pFixed[eid] = true;
        p[eid] = 0;
      }
    }
  }

  // Force matchings between fixed faces
  for(unsigned int i = 0; i < F.rows();++i)
  {
    if (isHard[i])
    {
      for(unsigned int j = 0; j < 3; ++j)
      {
        int fid = TT(i,j);
        if ((fid!=-1) && (isHard[fid]))
        {
          // i and fid are adjacent and fixed
          int eid = FE(i,j);
          int fid0 = EF(eid,0);
          int fid1 = EF(eid,1);

          pFixed[eid] = true;
          p[eid] = (int)std::round(2.0 / igl::PI * (hard(fid1) - hard(fid0) - k(eid)));
        }
      }
    }
  }
}

double igl::copyleft::comiso::NRosyField::convert3DtoLocal(unsigned fid, const Eigen::Vector3d& v)
{
  // Project onto the tangent plane
  Eigen::Vector2d vp = TPs[fid] * v;

  // Convert to angle
  return std::atan2(vp(1), vp(0));
}

Eigen::Vector3d igl::copyleft::comiso::NRosyField::convertLocalto3D(unsigned fid, double a)
{
  Eigen::Vector2d vp(std::cos(a), std::sin(a));
  return vp.transpose() * TPs[fid];
}

Eigen::VectorXd igl::copyleft::comiso::NRosyField::angleDefect()
{
  Eigen::VectorXd A = Eigen::VectorXd::Constant(V.rows(), 2*igl::PI);

  for (unsigned int i = 0; i < F.rows(); ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      Eigen::VectorXd a = V.row(F(i,(j+1)%3)) - V.row(F(i,j));
      Eigen::VectorXd b = V.row(F(i,(j+2)%3)) - V.row(F(i,j));
      double t = a.transpose() * b;
      if(a.norm() > 0. && b.norm() > 0.)
        t /= (a.norm() * b.norm());
      else
        throw std::runtime_error("igl::copyleft::comiso::NRosyField::angleDefect: Division by zero!");
      A(F(i, j)) -= std::acos(std::max(std::min(t, 1.), -1.));
    }
  }

  return A;
}

void igl::copyleft::comiso::NRosyField::findCones(int N)
{
  // Compute I0, see http://www.graphics.rwth-aachen.de/media/papers/bommes_zimmer_2009_siggraph_011.pdf for details

  singularityIndex = Eigen::VectorXd::Zero(V.rows());

  // first the k
  for (unsigned i = 0; i < EV.rows(); ++i)
  {
    if (!isBorderEdge[i])
    {
      singularityIndex(EV(i, 0)) += k(i);
      singularityIndex(EV(i, 1)) -= k(i);
    }
  }

  // then the A
  Eigen::VectorXd A = angleDefect();
  singularityIndex += A;
  // normalize
  singularityIndex /= (2 * igl::PI);

  // round to integer (remove numerical noise)
  for (unsigned i = 0; i < singularityIndex.size(); ++i)
    singularityIndex(i) = round(singularityIndex(i));

  for (unsigned i = 0; i < EV.rows(); ++i)
  {
    if (!isBorderEdge[i])
    {
      singularityIndex(EV(i, 0)) += double(p(i)) / double(N);
      singularityIndex(EV(i, 1)) -= double(p(i)) / double(N);
    }
  }

  // Clear the vertices on the edges
  for (unsigned i = 0; i < EV.rows(); ++i)
  {
    if (isBorderEdge[i])
    {
      singularityIndex(EV(i,0)) = 0;
      singularityIndex(EV(i,1)) = 0;
    }
  }
}

Eigen::VectorXd igl::copyleft::comiso::NRosyField::getSingularityIndexPerVertex()
{
  return singularityIndex;
}

IGL_INLINE void igl::copyleft::comiso::nrosy(
  const Eigen::MatrixXd& V,
  const Eigen::MatrixXi& F,
  const Eigen::VectorXi& b,
  const Eigen::MatrixXd& bc,
  const Eigen::VectorXi& b_soft,
  const Eigen::VectorXd& w_soft,
  const Eigen::MatrixXd& bc_soft,
  const int N,
  const double soft,
  Eigen::MatrixXd& R,
  Eigen::VectorXd& S
  )
{
  // Init solver
  igl::copyleft::comiso::NRosyField solver(V, F);

  // Add hard constraints
  for (unsigned i = 0; i < b.size(); ++i)
    solver.setConstraintHard(b(i), bc.row(i));

  // Add soft constraints
  for (unsigned i = 0; i < b_soft.size(); ++i)
    solver.setConstraintSoft(b_soft(i), w_soft(i), bc_soft.row(i));

  // Set the soft constraints global weight
  solver.setSoftAlpha(soft);

  // Interpolate
  solver.solve(N);

  // Copy the result back
  R = solver.getFieldPerFace();

  // Extract singularity indices
  S = solver.getSingularityIndexPerVertex();
}


IGL_INLINE void igl::copyleft::comiso::nrosy(
                           const Eigen::MatrixXd& V,
                           const Eigen::MatrixXi& F,
                           const Eigen::VectorXi& b,
                           const Eigen::MatrixXd& bc,
                           const int N,
                           Eigen::MatrixXd& R,
                           Eigen::VectorXd& S
                           )
{
  // Init solver
  igl::copyleft::comiso::NRosyField solver(V, F);

  // Add hard constraints
  for (unsigned i= 0; i < b.size(); ++i)
    solver.setConstraintHard(b(i), bc.row(i));

  // Interpolate
  solver.solve(N);

  // Copy the result back
  R = solver.getFieldPerFace();

  // Extract singularity indices
  S = solver.getSingularityIndexPerVertex();
}
