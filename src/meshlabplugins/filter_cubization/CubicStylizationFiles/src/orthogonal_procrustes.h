#ifndef ORTHOGONAL_PROCRUSTES_H
#define ORTHOGONAL_PROCRUSTES_H

#include <iostream>
#include <Eigen/Core>
#include <igl/svd3x3.h>
#include <igl/polar_svd.h>

void orthogonal_procrustes(
    const Eigen::Matrix3d & S,
    Eigen::Matrix3d & R);
#endif // ORTHOGONAL_PROCRUSTES_H
