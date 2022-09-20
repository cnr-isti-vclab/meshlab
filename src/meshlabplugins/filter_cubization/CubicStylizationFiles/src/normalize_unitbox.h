#ifndef NORMALIZE_UNITBOX_H
#define NORMALIZE_UNITBOX_H

#include <iostream>
#include <Eigen/Core>

// Inputs:
//   V        a matrix of vertex positions
// Outputs:
//   V        a matrix of vertex positions (in a unit box)
void normalize_unitbox(
    Eigen::MatrixXd & V);

#endif // NORMALIZE_UNITBOX_H

