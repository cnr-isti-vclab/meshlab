#ifndef TRIMESH_HEAT_METHOD
#define TRIMESH_HEAT_METHOD

#include <Eigen/Sparse>
#include <Eigen/SparseCholesky>

#include <vcg/complex/algorithms/update/quality.h>

#include <common/plugins/interfaces/filter_plugin.h>


// foward declarations
inline Eigen::Vector3d toEigen(const vcg::Point3f& p);
inline double cotan(const Eigen::Vector3d& v0, const Eigen::Vector3d& v1);
inline void buildMassMatrix(CMeshO &mesh, Eigen::SparseMatrix<double> &mass);
inline void buildCotanMatrix(CMeshO &mesh, Eigen::SparseMatrix<double> &cotanOperator);
inline double computeAverageEdgeLength(CMeshO &mesh);
inline Eigen::MatrixX3d computeVertexGradient(CMeshO &mesh, const Eigen::VectorXd &heat);
inline Eigen::VectorXd computeVertexDivergence(CMeshO &mesh, const Eigen::MatrixX3d &field);
inline Eigen::MatrixX3d normalizeVectorField(const Eigen::MatrixX3d &field);

inline Eigen::Vector3d toEigen(const vcg::Point3f& p)
{
    return Eigen::Vector3d(p.X(), p.Y(), p.Z());
};


inline double cotan(const Eigen::Vector3d& v0, const Eigen::Vector3d& v1)
{
    // cos(theta) / sin(theta)
    return v0.dot(v1) / v0.cross(v1).norm();
};


inline void buildMassMatrix(CMeshO &mesh, Eigen::SparseMatrix<double> &mass){
    // compute area of all faces
    for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi)
    {
        vcg::Point3f p0 = fi->V(0)->P();
        vcg::Point3f p1 = fi->V(1)->P();
        vcg::Point3f p2 = fi->V(2)->P();
        double e0 = toEigen(p1 - p0).norm();
        double e1 = toEigen(p2 - p0).norm();
        double e2 = toEigen(p2 - p1).norm();
        double s = (e0 + e1 + e2) / 2;
        double area = std::sqrt(s * (s - e0) * (s - e1) * (s - e2));
        // we store face areas in quality field to avoid a hash table
        // this will also be useful for the gradient computation
        fi->Q() = area;
    }
    // compute area of the dual cell for each vertex
    for (int i = 0; i < mesh.VN(); ++i){
        CMeshO::VertexType *vp = &mesh.vert[i];

        std::vector<CMeshO::FaceType*> faces;
        std::vector<int> indices;
        vcg::face::VFStarVF<CMeshO::FaceType>(vp, faces, indices);
        
        double area = 0;
        for (int j = 0; j < faces.size(); ++j)
        {
            area += faces[j]->Q();
        }
        area /= 3;
        mass.coeffRef(i, i) = area;
    }
}


inline void buildCotanMatrix(CMeshO &mesh, Eigen::SparseMatrix<double> &cotanOperator){
    // initialize a hashtable from vertex pointers to ids
    std::unordered_map<CMeshO::VertexType*, int> vertex_ids;
    for (int i = 0; i < mesh.VN(); ++i){
        vertex_ids[&mesh.vert[i]] = i;
    }

    // iterate over all vertices to fill cotan matrix
    for (int i = 0; i < mesh.VN(); ++i){
        CMeshO::VertexType *vp = &mesh.vert[i];
        CMeshO::FaceType *fp = vp->VFp();
        vcg::face::Pos<CMeshO::FaceType> pos(fp, vp);
        vcg::face::Pos<CMeshO::FaceType> start(fp, vp);
        // iterate over all incident edges of vp
        do {
            // get the vertex opposite to vp
            pos.FlipV();
            CMeshO::VertexType *vo = pos.V();
            // move to left vertex
            pos.FlipE();pos.FlipV();
            CMeshO::VertexType *vl = pos.V();
            // move back then to right vertex
            pos.FlipV();pos.FlipE(); // back to vo
            pos.FlipF();pos.FlipE();pos.FlipV();
            CMeshO::VertexType *vr = pos.V();
            pos.FlipV();pos.FlipE();pos.FlipF();pos.FlipV(); // back to vp

            // compute cotan of left edges and right edges
            Eigen::Vector3d elf = toEigen(vo->P() - vl->P()); // far left edge
            Eigen::Vector3d eln = toEigen(vp->P() - vl->P()); // near left edge
            Eigen::Vector3d erf = toEigen(vp->P() - vr->P()); // far right edge
            Eigen::Vector3d ern = toEigen(vo->P() - vr->P()); // near right edge

            double cotan_l = cotan(elf, eln);
            double cotan_r = cotan(ern, erf);

            // add to the matrix
            cotanOperator.coeffRef(vertex_ids[vp], vertex_ids[vo]) = (cotan_l + cotan_r)/2;

            // move to the next edge
            pos.FlipF();pos.FlipE();
        } while (pos != start);
    }

    // compute diagonal entries
    for (int i = 0; i < mesh.VN(); ++i){
        cotanOperator.coeffRef(i, i) = -cotanOperator.row(i).sum();
    }
    
}


inline double computeAverageEdgeLength(CMeshO &mesh){
    // compute total length of all edges
    double total_length = 0;
    for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi)
    {
        vcg::Point3f p0 = fi->V(0)->P();
        vcg::Point3f p1 = fi->V(1)->P();
        vcg::Point3f p2 = fi->V(2)->P();
        double e2 = toEigen(p1 - p0).norm();
        double e1 = toEigen(p2 - p0).norm();
        double e0 = toEigen(p2 - p1).norm();
        total_length += (e0 + e1 + e2) / 2;
    }
    return total_length / (3./2. * mesh.FN());
}


inline Eigen::MatrixX3d computeVertexGradient(CMeshO &mesh, const Eigen::VectorXd &heat){
    Eigen::MatrixX3d heatGradientField(mesh.FN(), 3);
    // initialize a hashtable from vertex pointers to ids
    std::unordered_map<CMeshO::VertexType*, int> vertex_ids;
    for (int i = 0; i < mesh.VN(); ++i){
        vertex_ids[&mesh.vert[i]] = i;
    }
    // compute gradient of heat function at each vertex
    for (int i = 0; i < mesh.FN(); ++i){
        CMeshO::FaceType *fp = &mesh.face[i];

        vcg::Point3f p0 = fp->V(0)->P();
        vcg::Point3f p1 = fp->V(1)->P();
        vcg::Point3f p2 = fp->V(2)->P();

        // normal unit vector
        Eigen::Vector3d n = toEigen(fp->N());
        n /= n.norm();
        // face area
        double faceArea = fp->Q();
        // (ORDERING): edge unit vectors (assuming counter-clockwise ordering)
        // note if the ordering is clockwise, the gradient will point in the opposite direction
        Eigen::Vector3d e0 = toEigen(p2 - p1);
        e0 /= e0.norm();
        Eigen::Vector3d e1 = toEigen(p0 - p2);
        e1 /= e1.norm();
        Eigen::Vector3d e2 = toEigen(p1 - p0);
        e2 /= e2.norm();
        // gradient unit vectors
        Eigen::Vector3d g0 = n.cross(e0); //v0 grad
        Eigen::Vector3d g1 = n.cross(e1); //v1 grad
        Eigen::Vector3d g2 = n.cross(e2); //v2 grad

        // add vertex gradient contributions
        Eigen::Vector3d total_grad = (
            g0 * heat(vertex_ids[fp->V(0)]) + 
            g1 * heat(vertex_ids[fp->V(1)]) + 
            g2 * heat(vertex_ids[fp->V(2)])
        ) / (2 * faceArea);

        heatGradientField.row(i) = total_grad;
    }
    return heatGradientField;
}


inline Eigen::MatrixX3d normalizeVectorField(const Eigen::MatrixX3d &field){
    Eigen::MatrixX3d normalizedField(field.rows(), 3);
    normalizedField.setZero();
    // normalize vector field at each vertex
    for (int i = 0; i < field.rows(); ++i){
        Eigen::Vector3d v = field.row(i);
        normalizedField.row(i) = v / v.norm();
    }
    return normalizedField;
}


inline Eigen::VectorXd computeVertexDivergence(CMeshO &mesh, const Eigen::MatrixX3d &field){
    Eigen::VectorXd divergence(mesh.VN());
    divergence.setZero();
    // initialize a hashtable from face pointers to ids
    std::unordered_map<CMeshO::FaceType*, int> face_ids;
    for (int i = 0; i < mesh.FN(); ++i){
        face_ids[&mesh.face[i]] = i;
    }

    // compute divergence of vector field at each vertex
    for (int i = 0; i < mesh.VN(); ++i){
        CMeshO::VertexType *vp = &mesh.vert[i];

        std::vector<CMeshO::FaceType*> faces;
        std::vector<int> indices;
        vcg::face::VFStarVF<CMeshO::FaceType>(vp, faces, indices);
        for (int j = 0; j < faces.size(); ++j)
        {
            CMeshO::FaceType *fp = faces[j];
            int index = indices[j];
            vcg::Point3f p0 = fp->V(0)->P();
            vcg::Point3f p1 = fp->V(1)->P();
            vcg::Point3f p2 = fp->V(2)->P();
            // (ORDERING) edge vectors
            Eigen::Vector3d el, er, eo; //left, right, opposite to vp
            if (index == 0){
                el = toEigen(p2 - p0); //-e1
                er = toEigen(p1 - p0); //e2
                eo = toEigen(p2 - p1); //e0
            } else if (index == 1){
                el = toEigen(p0 - p1); //-e2
                er = toEigen(p2 - p1); //e0
                eo = toEigen(p0 - p2); //e1
            } else if (index == 2){ 
                el = toEigen(p1 - p2); //-e0
                er = toEigen(p0 - p2); //e1
                eo = toEigen(p1 - p0); //e2
            }
            // compute left and right cotangents
            double cotl = cotan(-el, eo); // -el -> angle between el and eo
            double cotr = cotan(er, eo);
            // normalize edge vectors after cotangent computation
            el /= el.norm();
            er /= er.norm();
            // add divergence contribution of given face
            Eigen::Vector3d x = field.row(face_ids[fp]);
            divergence(i) += (cotl * er.dot(x) + cotr * el.dot(x)) / 2;
        }
    }
    return divergence;
}


#endif
