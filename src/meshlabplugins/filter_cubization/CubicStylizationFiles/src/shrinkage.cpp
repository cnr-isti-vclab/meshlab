#include "shrinkage.h"

void shrinkage(
    const Eigen::VectorXd & x,
    const double & k,
    Eigen::VectorXd & z)
{
    using namespace Eigen;
    using namespace std;

    VectorXd tmp1 = x.array() - k;
    VectorXd posMax = tmp1.array().max(0.0);

    VectorXd tmp2 = -x.array() - k;
    VectorXd negMax = tmp2.array().max(0.0);

    // VectorXd posMax = x.array() - k;
    // for (int ii=0; ii<posMax.size(); ii++)
    // {
    //     if (posMax(ii)<0)
    //         posMax(ii) = 0.0;
    // }

    // VectorXd negMax = -x.array() - k;
    // for (int ii=0; ii<negMax.size(); ii++)
    // {
    //     if (negMax(ii)<0)
    //         negMax(ii) = 0.0;
    // }

    z = posMax - negMax;
}
