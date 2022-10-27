#include "cube_style_precomputation.h"

void cube_style_precomputation(
    const Eigen::MatrixXd & V,
    const Eigen::MatrixXi & F,
    cube_style_data & data)
{
    using namespace Eigen;
    using namespace std;

    data.reset();

    igl::per_vertex_normals(V,F, data.N);

    igl::cotmatrix(V,F,data.L);

    SparseMatrix<double> M;
    igl::massmatrix(V,F,igl::MASSMATRIX_TYPE_BARYCENTRIC,M);
    data.VA = M.diagonal();

    vector<vector<int>> adjFList, VI;
    igl::vertex_triangle_adjacency(V.rows(),F,adjFList,VI);

    igl::arap_rhs(V,F,V.cols(),igl::ARAP_ENERGY_TYPE_SPOKES_AND_RIMS, data.K);

    data.hEList.resize(V.rows());
    data.WVecList.resize(V.rows());
    data.dVList.resize(V.rows());
    vector<int> adjF;
    for (int ii=0; ii<V.rows();  ii++)
    {
        adjF = adjFList[ii];

        data.hEList[ii].resize(adjF.size()*3, 2);
        data.WVecList[ii].resize(adjF.size()*3);
        for (int jj=0; jj<adjF.size(); jj++)
        {
            int v0 = F(adjF[jj],0);
            int v1 = F(adjF[jj],1);
            int v2 = F(adjF[jj],2);

            // compute adjacent half-edge indices of a vertex
            data.hEList[ii](3*jj  ,0) = v0;
            data.hEList[ii](3*jj  ,1) = v1;
            data.hEList[ii](3*jj+1,0) = v1;
            data.hEList[ii](3*jj+1,1) = v2;
            data.hEList[ii](3*jj+2,0) = v2;
            data.hEList[ii](3*jj+2,1) = v0;

            // compute WVec = vec(W)
            data.WVecList[ii](3*jj  ) = data.L.coeff(v0,v1);
            data.WVecList[ii](3*jj+1) = data.L.coeff(v1,v2);
            data.WVecList[ii](3*jj+2) = data.L.coeff(v2,v0);
        }

        // compute [dV] matrix for each vertex
        data.dVList[ii].resize(3, adjF.size()*3);
        MatrixXd V_hE0, V_hE1;
        igl::slice(V,data.hEList[ii].col(0),1,V_hE0);
        igl::slice(V,data.hEList[ii].col(1),1,V_hE1);
        // data.dVList[ii].block(0,0,3,data.hEList[ii].rows()) = (V_hE1 - V_hE0).transpose();
        data.dVList[ii] = (V_hE1 - V_hE0).transpose();
    }

    igl::min_quad_with_fixed_precompute(data.L,data.b,SparseMatrix<double>(),false,data.solver_data);

    data.zAll.resize(3, V.rows()); data.zAll.setRandom();
    data.uAll.resize(3, V.rows()); data.uAll.setRandom();
    data.rhoAll.resize(V.rows()); data.rhoAll.setConstant(data.rhoInit);
}
