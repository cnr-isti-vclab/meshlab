#include "normalize_unitbox.h"

void normalize_unitbox(
    Eigen::MatrixXd & V)
{
    V = V.rowwise() - V.colwise().minCoeff();
    V /= V.maxCoeff();
}
