#ifndef FIT_ROTATIONS_L1_H
#define FIT_ROTATIONS_L1_H

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <CubicStylizationFiles/src/cube_style_data.h>
#include <CubicStylizationFiles/src/orthogonal_procrustes.h>
#include <CubicStylizationFiles/src/shrinkage.h>
#include <igl/slice.h>
#include <igl/parallel_for.h>
#include <math.h>

void fit_rotations_l1(
    const Eigen::MatrixXd & V,
    Eigen::MatrixXd & U,
    Eigen::MatrixXd & RAll,
    cube_style_data & data,
    Eigen::VectorXd & energy_vects);
#endif // FIT_ROTATIONS_L1_H
