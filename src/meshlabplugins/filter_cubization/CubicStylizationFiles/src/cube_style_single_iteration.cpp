#include "cube_style_single_iteration.h"

void cube_style_single_iteration(
    const Eigen::MatrixXd & V,
    Eigen::MatrixXd & U,
    cube_style_data & data,
    Eigen::VectorXd & energyVects)
{
    using namespace Eigen;
    using namespace std;

    // local step
    MatrixXd RAll(3,V.rows()*3);
    {
        fit_rotations_l1(V, U, RAll, data, energyVects);
    }

    //R.setZero();
    //R = RAll;

    // global step
    MatrixXd Upre = U;
    {
        VectorXd Rcol;
        igl::columnize(RAll, V.rows(), 2, Rcol);
        VectorXd Bcol = data.K * Rcol;
        for(int dim=0; dim<V.cols(); dim++)
        {
            VectorXd Uc,Bc,bcc;
            Bc = Bcol.block(dim*V.rows(),0,V.rows(),1);
            bcc = data.bc.col(dim);
            min_quad_with_fixed_solve(
                data.solver_data,Bc,bcc,VectorXd(),Uc);
            U.col(dim) = Uc;
        }
    }

    // print optimization date
    data.reldV = (U-Upre).cwiseAbs().maxCoeff() / (U-V).cwiseAbs().maxCoeff();
    //cout << "reldV:" << scientific << data.reldV << ", obj:" << data.objVal << endl;
}
