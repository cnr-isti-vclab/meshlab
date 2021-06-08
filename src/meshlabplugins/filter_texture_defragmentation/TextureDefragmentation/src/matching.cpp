/*******************************************************************************
    Copyright (c) 2021, Andrea Maggiordomo, Paolo Cignoni and Marco Tarini

    This file is part of TextureDefrag, a reference implementation for
    the paper ``Texture Defragmentation for Photo-Reconstructed 3D Models''
    by Andrea Maggiordomo, Paolo Cignoni and Marco Tarini.

    TextureDefrag is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TextureDefrag is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TextureDefrag. If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/

#include "matching.h"
#include "utils.h"

#include <vector>
#include <numeric>

#include <Eigen/Core>
#include <Eigen/Eigenvalues>

MatchingTransform ComputeMatchingMatrix(const std::vector<vcg::Point2d>& targetVector, const std::vector<vcg::Point2d>& matchingVector)
{
    ensure(targetVector.size() == matchingVector.size());
    ensure(targetVector.size() >= 2);

    vcg::Point2d ct = std::accumulate(targetVector.begin(), targetVector.end(), vcg::Point2d::Zero()) / (double) targetVector.size();
    vcg::Point2d cm = std::accumulate(matchingVector.begin(), matchingVector.end(), vcg::Point2d::Zero()) / (double) matchingVector.size();

    Eigen::MatrixXd A(2 * targetVector.size() + 1, 4);
    Eigen::VectorXd b(2 * targetVector.size() + 1);

    A.setZero();
    b.setZero();

    int n = targetVector.size();
    for (int i = 0; i < n; ++i) {
        vcg::Point2d p = targetVector[i] - ct;
        vcg::Point2d q = matchingVector[i] - cm;
        A(i, 0)   = q.X(); A(i, 1)   = q.Y(); b(i)   = p.X();
        A(i+n, 2) = q.X(); A(i+n, 3) = q.Y(); b(i+n) = p.Y();
    }

    Eigen::VectorXd x = A.fullPivHouseholderQr().solve(b);
    Eigen::Matrix2d SR;
    SR << x[0], x[1],
          x[2], x[3];

    Eigen::Vector2d ect(ct.X(), ct.Y());
    Eigen::Vector2d ecm(cm.X(), cm.Y());

    Eigen::Vector2d t = ect - SR * ecm;


    return { vcg::Point2d(t[0], t[1]), { SR(0, 0), SR(0, 1), SR(1, 0), SR(1, 1) } };
}

MatchingTransform ComputeMatchingSimilarityMatrix(const std::vector<vcg::Point2d>& targetVector, const std::vector<vcg::Point2d>& matchingVector)
{
    ensure(targetVector.size() == matchingVector.size());
    ensure(targetVector.size() >= 2);

    int n = targetVector.size();

    vcg::Point2d ct = std::accumulate(targetVector.begin(), targetVector.end(), vcg::Point2d::Zero()) / (double) targetVector.size();
    vcg::Point2d cm = std::accumulate(matchingVector.begin(), matchingVector.end(), vcg::Point2d::Zero()) / (double) matchingVector.size();

    std::vector<Eigen::Vector2d> pv;
    std::vector<Eigen::Vector2d> qv;

    for (int i = 0; i < n; ++i) {
        vcg::Point2d p = targetVector[i] - ct;
        vcg::Point2d q = matchingVector[i] - cm;
        pv.push_back({p.X(), p.Y()});
        qv.push_back({q.X(), q.Y()});
    }

    double pSquaredNormSum = 0;
    double qSquaredNormSum = 0;
    Eigen::Matrix2d M = Eigen::Matrix2d::Zero();
    for (int i = 0; i < n; ++i) {
        pSquaredNormSum += pv[i].squaredNorm();
        qSquaredNormSum += qv[i].squaredNorm();
        M += pv[i] * qv[i].transpose();
    }

    double scale = std::sqrt(pSquaredNormSum / qSquaredNormSum);

    Eigen::Matrix2d Q = M.transpose() * M;

    Eigen::EigenSolver<Eigen::Matrix2d> es;
    es.compute(Q);

    ensure(es.eigenvalues().imag().squaredNorm() < 1e-8);
    ensure(es.eigenvectors().imag().squaredNorm() < 1e-8);

    Eigen::Vector2d lambda = es.eigenvalues().real();
    Eigen::Matrix2d V = es.eigenvectors().real();

    Eigen::Matrix2d invsqrtQ = (1/std::sqrt(lambda[0])) * (V.col(0) * V.col(0).transpose()) + (1/std::sqrt(lambda[1])) * (V.col(1) * V.col(1).transpose());
    Eigen::Matrix2d R = M * invsqrtQ;
    Eigen::Matrix2d sR = scale * R;

    // does this work?
    if(sR.determinant() < 0) {
        Eigen::Matrix2d U, V;
        Eigen::Vector2d sigma;
        Eigen::JacobiSVD<Eigen::Matrix2d> svd;
        svd.compute(R, Eigen::ComputeFullU | Eigen::ComputeFullV);
        U = svd.matrixU(); V = svd.matrixV(); sigma = svd.singularValues();
        ensure(std::abs(sigma[0] - 1) < 1e-8);
        ensure(std::abs(sigma[1] - 1) < 1e-8);
        U.col(U.cols() - 1) *= -1;
        R = U * V.transpose();
    }

    Eigen::Vector2d ect(ct[0], ct[1]);
    Eigen::Vector2d ecm(cm[0], cm[1]);

    Eigen::Vector2d t = ect - scale * R * ecm;

    return { vcg::Point2d(t[0], t[1]), { sR(0,0), sR(0,1), sR(1,0), sR(1,1) } };
}

MatchingTransform ComputeMatchingRigidMatrix(const std::vector<vcg::Point2d>& targetVector, const std::vector<vcg::Point2d>& matchingVector)
{
    ensure(targetVector.size() == matchingVector.size());
    ensure(targetVector.size() >= 2);

    int n = targetVector.size();

    vcg::Point2d ct = std::accumulate(targetVector.begin(), targetVector.end(), vcg::Point2d::Zero()) / (double) n;
    vcg::Point2d cm = std::accumulate(matchingVector.begin(), matchingVector.end(), vcg::Point2d::Zero()) / (double) n;

    std::vector<Eigen::Vector2d> pv;
    std::vector<Eigen::Vector2d> qv;

    for (int i = 0; i < n; ++i) {
        vcg::Point2d p = targetVector[i] - ct;
        vcg::Point2d q = matchingVector[i] - cm;
        pv.push_back({p.X(), p.Y()});
        qv.push_back({q.X(), q.Y()});
    }

    // compute the covariance matrix
    Eigen::Matrix2d S = Eigen::Matrix2d::Zero();
    for (int i = 0; i < n; ++i) {
        S += qv[i] * pv[i].transpose();
    }

    Eigen::Matrix2d U, V;
    Eigen::JacobiSVD<Eigen::Matrix2d> svd;
    svd.compute(S, Eigen::ComputeFullU | Eigen::ComputeFullV);
    U = svd.matrixU();
    V = svd.matrixV();

    Eigen::Vector2d d(1, 1);
    if (U.determinant() * V.determinant() < 0) {
        d[1] = -1;
    }

    Eigen::Matrix2d R  = V * d.asDiagonal() * U.transpose();

    ensure(R.determinant() > 0);

    Eigen::Vector2d ect(ct[0], ct[1]);
    Eigen::Vector2d ecm(cm[0], cm[1]);

    Eigen::Vector2d t = ect - R * ecm;

    return { vcg::Point2d(t[0], t[1]), { R(0,0), R(0,1), R(1,0), R(1,1) } };
}

double MatchingError(const MatchingTransform& matching, const std::vector<vcg::Point2d>& points1, const std::vector<vcg::Point2d>& points2)
{
    return MatchingErrorAverage(matching, points1, points2);
}

double MatchingErrorAverage(const MatchingTransform& matching, const std::vector<vcg::Point2d>& points1, const std::vector<vcg::Point2d>& points2)
{
    ensure(points1.size() == points2.size());
    double error = 0;
    for (unsigned i = 0; i < points1.size(); ++i) {
        error += (points1[i] - matching.Apply(points2[i])).Norm();
    }

    return error / (double) points1.size();
}

double MatchingErrorTotal(const MatchingTransform& matching, const std::vector<vcg::Point2d>& points1, const std::vector<vcg::Point2d>& points2)
{
    ensure(points1.size() == points2.size());
    double error = 0;
    for (unsigned i = 0; i < points1.size(); ++i) {
        error += (points1[i] - matching.Apply(points2[i])).Norm();
    }

    return error;
}
