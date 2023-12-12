#include "fit_rotations_l1.h"

void fit_rotations_l1(
    const Eigen::MatrixXd & V,
    Eigen::MatrixXd & U,
    Eigen::MatrixXd & RAll,
    cube_style_data & data,
    Eigen::VectorXd & energy_vects)
{
    using namespace Eigen;
    using namespace std;

    data.objValVec.setZero(V.rows());
    igl::parallel_for(
        V.rows(),
        [&data, &RAll, &U](const int ii)
        {
            // warm start parameters
            VectorXd z = data.zAll.col(ii);
            VectorXd u = data.uAll.col(ii);
            VectorXd n = data.N.row(ii).transpose();
            double rho = data.rhoAll(ii);
            Matrix3d R;

            // get energy parameters
            // Note: dVn = [dV n], dUn = [dU z-u]
            MatrixXi hE = data.hEList[ii];
            MatrixXd dU(3,hE.rows());
            {
                MatrixXd U_hE0, U_hE1;
                igl::slice(U,hE.col(0),1,U_hE0);
                igl::slice(U,hE.col(1),1,U_hE1);
                dU = (U_hE1 - U_hE0).transpose();
            }

            // Note:
            // S = [dV n] * [W 0; 0 rho] * [dU (z-u)]'
            //   = dV * W * dU' + n * rho * (z-u)'
            //   = Spre + n * rho * (z-u)'
            MatrixXd dV = data.dVList[ii];
            VectorXd WVec = data.WVecList[ii];
            Matrix3d Spre = dV * WVec.asDiagonal() * dU.transpose();

            // ADMM
            for (int k=0; k<data.maxIter_ADMM; k++)
            {
                // R step
                Matrix3d S = Spre + (rho * n * (z-u).transpose());
                // S /= S.norm();
                orthogonal_procrustes(S, R);

                // z step
                VectorXd zOld = z;
                shrinkage(R*n+u, data.lambda* data.VA(ii)/rho, z);

                // u step
                u.noalias() += R*n - z;

                // compute residual
                double r_norm = (z - R*n).norm();
                double s_norm = (-rho * (z - zOld)).norm();

                // rho step
                if (r_norm > data.mu * s_norm)
                {
                    rho = data.tao * rho;
                    u = u / data.tao;
                }
                else if (s_norm > data.mu * r_norm)
                {
                    rho = rho / data.tao;
                    u = u * data.tao;
                }

                // stopping criteria
                double nz = double(z.size());
                double eps_pri = sqrt(2.0*nz)*data.ABSTOL + data.RELTOL*max( (R*n).norm(),z.norm() );
                double eps_dual = sqrt(1.0*nz)*data.ABSTOL + data.RELTOL* ((rho*u).norm());
                if ( (r_norm<eps_pri)  && (s_norm<eps_dual) )
                {
                    // save parameters
                    data.zAll.col(ii) = z;
                    data.uAll.col(ii) = u;
                    data.rhoAll(ii) = rho;
                    RAll.block(0,3*ii,3,3) = R;

                    // save objective
                    double objVal =
                        0.5*((R*dV-dU)*WVec.asDiagonal()*(R*dV-dU).transpose()).trace()
                        + data.lambda * data.VA(ii) * (R*n).cwiseAbs().sum();
                    data.objValVec(ii) = objVal;
                    break;
                }
            } // ADMM end
        }
    ,1000);

    energy_vects = data.objValVec;
    data.objVal = data.objValVec.sum();
}
