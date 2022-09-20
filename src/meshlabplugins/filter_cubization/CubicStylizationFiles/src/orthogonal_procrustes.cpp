#include "orthogonal_procrustes.h"

void orthogonal_procrustes(
    const Eigen::Matrix3d & S,
    Eigen::Matrix3d & R)
{
    using namespace Eigen;
    using namespace std;

    // using svd3x3 which has problems for double precision
    // Matrix3d SU, SV;
    // Matrix<double,3,1> SS;
    // igl::svd3x3(S, SU, SS, SV);
    // R = SV * SU.transpose();
    // if (R.determinant() < 0)
    // {
    //     SU.col(2) = -SU.col(2);
    //     R = SV * SU.transpose();
    // }

    // using Eigen svd
    JacobiSVD<Matrix3d> svd;
    svd.compute(S, Eigen::ComputeFullU | Eigen::ComputeFullV );
    Matrix3d SU = svd.matrixU();
    Matrix3d SV = svd.matrixV();
    R = SV * SU.transpose();
    if (R.determinant() < 0)
    {
        SU.col(2) = -SU.col(2);
        R = SV * SU.transpose();
    }

    assert(R.determinant() > 0);
}
