#ifndef SHRINKAGE_H
#define SHRINKAGE_H

#include <Eigen/Core>
#include <iostream>

void shrinkage(
    const Eigen::VectorXd & x,
    const double & k,
    Eigen::VectorXd & z);
#endif // SHRINKAGE_H
