#ifndef CUBIC_STYLIZING_H
#define CUBIC_STYLIZING_H

#include <CubicStylizationFiles/src/cube_style_data.h>
#include <CubicStylizationFiles/src/cube_style_precomputation.h>
#include <CubicStylizationFiles/src/cube_style_single_iteration.h>
#include <CubicStylizationFiles/src/normalize_unitbox.h>

#include <vcg/complex/algorithms/mesh_to_matrix.h>
#include <vcg/complex/allocate.h>

#include <common/utilities/eigen_mesh_conversions.h>

#include <CubicStylizationFiles/conversionMeshes.h>

namespace vcg{
namespace tri{

template<class MeshType >
class Cubization{
    public:
        static void Init(MeshType& m, Eigen::MatrixXd& V, Eigen::MatrixXd& U, Eigen::MatrixXi& F, cube_style_data& data){
    	    
            // check requirements
            vcg::tri::VertexVectorHasPerVertexTexCoord( m.vert );
            vcg::tri::VertexVectorHasPerVertexFlags( m.vert );

            Mesh2Matrix(m, V, F);
    	    
            //normalize_unitbox(V);
            U = V;
	    
            data.bc.resize(1,3);
            data.bc << V.row(F(0,0));

            data.b.resize(1);
            data.b << F(0,0);

            // precomputation ARAP and initialize ADMM parameters
            cube_style_precomputation(V,F,data);   	
    	}
    	
        static double Stylize(Eigen::MatrixXd &V, Eigen::MatrixXd &U, Eigen::MatrixXi &F, cube_style_data& data, Eigen::VectorXd& energy_verts, double& energyCubic)
        {
            // apply cubic stylization
            cube_style_single_iteration(V,U,data, energy_verts);
            energyCubic = data.objVal;

            return data.reldV;
        }
};
}}
#endif // CUBIC_STYLIZING_H
