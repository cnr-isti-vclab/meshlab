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

#ifndef ARAP_H
#define ARAP_H

#include "mesh.h"

#include <Eigen/Core>
#include <Eigen/Sparse>


struct ARAPSolveInfo {
    double initialEnergy;
    double finalEnergy;
    int iterations;
    bool numericalError;
};

class ARAP {

public:

    struct Cot {
        double v[3];
    };

private:

    Mesh& m;

    std::vector<int> fixed_i;
    std::vector<vcg::Point2d> fixed_pos;

    int max_iter;

    void ComputeSystemMatrix(Mesh& m, const std::vector<Cot>& cotan, Eigen::SparseMatrix<double>& L);
    void ComputeRHS(Mesh& m, const std::vector<Eigen::Matrix2d>& rotations, const std::vector<Cot>& cotan, Eigen::VectorXd& bu, Eigen::VectorXd& bv);

public:

    ARAP(Mesh& mesh);

    double CurrentEnergy();
    void FixVertex(Mesh::ConstVertexPointer vp, const vcg::Point2d& pos);
    void FixBoundaryVertices();
    int FixSelectedVertices();
    int FixRandomEdgeWithinTolerance(double tol);
    void SetMaxIterations(int n);

    ARAPSolveInfo Solve();

    static double ComputeEnergyFromStoredWedgeTC(Mesh& m, double *num, double *denom);
    static double ComputeEnergyFromStoredWedgeTC(const std::vector<Mesh::FacePointer>& fpVec, Mesh& m, double *num, double *denom);
    static double ComputeEnergy(const vcg::Point2d& x10, const vcg::Point2d& x20,
                                const vcg::Point2d& u10, const vcg::Point2d& u20,
                                double *area);
};


#endif // ARAP_H
