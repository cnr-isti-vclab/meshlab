#ifndef TRIMESH_HEAT_METHOD
#define TRIMESH_HEAT_METHOD

#include <Eigen/Sparse>
#include <Eigen/SparseCholesky>

#include <vcg/complex/algorithms/update/quality.h>

#include <common/plugins/interfaces/filter_plugin.h>

#include <memory>


// LOCAL FUNCTIONS

namespace {
    inline Eigen::Vector3d toEigen(const vcg::Point3f& p)
    {
        return Eigen::Vector3d(p.X(), p.Y(), p.Z());
    };


    inline double cotan(const Eigen::Vector3d& v0, const Eigen::Vector3d& v1)
    {
        // cos(theta) / sin(theta)
        return v0.dot(v1) / v0.cross(v1).norm();
    };
}

// simple struct to track progress bar status
struct ProgressBarCallbackT {
    vcg::CallBackPos* callback;
    int current_progress;
    int allocated_progress;
};


inline void buildMassMatrix(CMeshO &mesh, Eigen::SparseMatrix<double> &mass){
    mass.resize(mesh.VN(), mesh.VN());
    mass.reserve(Eigen::VectorXi::Constant(mesh.VN(),1));

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
        mass.insert(i, i) = area;
    }

    mass.makeCompressed();
}


inline void buildCotanLowerTriMatrix(CMeshO &mesh, Eigen::SparseMatrix<double> &cotanOperator){
    cotanOperator.resize(mesh.VN(), mesh.VN());

    typedef Eigen::Triplet<double> T;
    std::vector<T> tripletList;
    tripletList.reserve(3*mesh.VN() + 3*mesh.FN());

    // compute cotan weights
    for (int i = 0; i < mesh.FN(); ++i){
        CMeshO::FaceType* fi = &mesh.face[i];

        CMeshO::VertexType* v0 = fi->V(0);
        CMeshO::VertexType* v1 = fi->V(1);
        CMeshO::VertexType* v2 = fi->V(2);

        vcg::Point3f p0 = v0->P();
        vcg::Point3f p1 = v1->P();
        vcg::Point3f p2 = v2->P();

        Eigen::Vector3d e0 = toEigen(p2 - p1);
        Eigen::Vector3d e1 = toEigen(p0 - p2);
        Eigen::Vector3d e2 = toEigen(p1 - p0);

        // first edge is inverted to get correct orientation
        double alpha0 = cotan(-e1, e2) / 2;
        double alpha1 = cotan(-e2, e0) / 2;
        double alpha2 = cotan(-e0, e1) / 2;

        int i0 = vcg::tri::Index(mesh,v0);
        int i1 = vcg::tri::Index(mesh,v1);
        int i2 = vcg::tri::Index(mesh,v2);

        // save only lower triangular part
        if (i0 > i1)
            tripletList.push_back(T(i0,i1,alpha2));
        else
            tripletList.push_back(T(i1,i0,alpha2));
        if (i0 > i2)
            tripletList.push_back(T(i0,i2,alpha1));
        else
            tripletList.push_back(T(i2,i0,alpha1));
        if (i1 > i2)
            tripletList.push_back(T(i1,i2,alpha0));
        else
            tripletList.push_back(T(i2,i1,alpha0));

        tripletList.push_back(T(i0,i0,-(alpha1 + alpha2)));
        tripletList.push_back(T(i1,i1,-(alpha0 + alpha2)));
        tripletList.push_back(T(i2,i2,-(alpha0 + alpha1)));
    }

    cotanOperator.setFromTriplets(tripletList.begin(), tripletList.end());
    cotanOperator.makeCompressed();
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
    // compute gradient of heat function at each vertex
    for (int i = 0; i < mesh.FN(); ++i){
        CMeshO::FaceType *fp = &mesh.face[i];

        CMeshO::VertexType* v0 = fp->V(0);
        CMeshO::VertexType* v1 = fp->V(1);
        CMeshO::VertexType* v2 = fp->V(2);

        vcg::Point3f p0 = v0->P();
        vcg::Point3f p1 = v1->P();
        vcg::Point3f p2 = v2->P();

        int i0 = vcg::tri::Index(mesh,v0);
        int i1 = vcg::tri::Index(mesh,v1);
        int i2 = vcg::tri::Index(mesh,v2);

        // normal unit vector
        Eigen::Vector3d n = toEigen(fp->N());
        n /= n.norm();
        // face area
        double faceArea = fp->Q();
        // edge unit vectors (counter-clockwise)
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
        Eigen::Vector3d tri_grad = (g0 * heat(i0) + g1 * heat(i1) + g2 * heat(i2)) / (2 * faceArea);

        heatGradientField.row(i) = tri_grad;
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
            double cotl = cotan(-el, -eo);
            double cotr = cotan(-er, eo);
            // normalize edge vectors after cotangent computation
            el /= el.norm();
            er /= er.norm();
            // add divergence contribution of given face
            Eigen::Vector3d x = field.row(vcg::tri::Index(mesh,fp));
            divergence(i) += (cotl * er.dot(x) + cotr * el.dot(x)) / 2;
        }
    }
    return divergence;
}


class HeatMethodSolver {
public:
    HeatMethodSolver(CMeshO& mesh, float m, vcg::CallBackPos* cb = NULL)
        : m(m)
    {
        if(cb != NULL)  cb(10, "Updating Topology...");
        // update topology and face normals
        vcg::tri::UpdateTopology<CMeshO>::VertexFace(mesh);
        vcg::tri::UpdateTopology<CMeshO>::FaceFace(mesh);
        vcg::tri::UpdateNormal<CMeshO>::PerFaceNormalized(mesh);

        // state variables
        Eigen::SparseMatrix<double> massMatrix;
        Eigen::SparseMatrix<double> cotanMatrix;
        double averageEdgeLength;

        buildMassMatrix(mesh, massMatrix);
        // this step is very slow (95% of total time) hence we pass the callback
        // to update progress and avoid freezes
        if(cb != NULL)  cb(20, "Building Cotan Matrix...");
        buildCotanLowerTriMatrix(mesh, cotanMatrix);
        averageEdgeLength = computeAverageEdgeLength(mesh);

        // initialize pointers
        solver1 = std::shared_ptr<Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>>>(new Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>>);
        solver2 = std::shared_ptr<Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>>>(new Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>>);

        // build factorization
        double timestep = m * averageEdgeLength * averageEdgeLength;
        if(cb != NULL)  cb(50, "Matrix Factorization...");
        solver1->compute(massMatrix - timestep * cotanMatrix);
        if(cb != NULL)  cb(75, "Matrix Factorization...");
        solver2->compute(cotanMatrix);
        if((solver1->info() != Eigen::Success) || (solver2->info() != Eigen::Success))
            factorizationFailed = true;
        else
            factorizationFailed = false;
    }

    HeatMethodSolver() = delete;

    HeatMethodSolver& operator=(const HeatMethodSolver&) = delete;

    Eigen::VectorXd solve(CMeshO &mesh, Eigen::VectorXd &sourcePoints){
        Eigen::VectorXd heatflow = solver1->solve(sourcePoints); // (VN)
        Eigen::MatrixX3d heatGradient = computeVertexGradient(mesh, heatflow); // (FN, 3)
        Eigen::MatrixX3d unitVectorField = normalizeVectorField(-heatGradient); // (FN, 3)
        Eigen::VectorXd divergence = computeVertexDivergence(mesh, unitVectorField); // (VN)
        Eigen::VectorXd geodesicDistance = solver2->solve(divergence); // (VN)
        if((solver1->info() != Eigen::Success) || (solver2->info() != Eigen::Success))
            solverFailed = true;
        else
            solverFailed = false;
        // shift to impose dist(source) = 0
        geodesicDistance.array() -= geodesicDistance.minCoeff();
        return geodesicDistance;
    }

    double get_m(){return m;}

    bool factorization_failed(){return factorizationFailed;}

    bool solver_failed(){return solverFailed;}
private:
    // Eigen::SimplicialLDLT has no copy/move constructor
    std::shared_ptr<Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>>> solver1;
    std::shared_ptr<Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>>> solver2;
    double m;
    bool factorizationFailed;
    bool solverFailed;
};

#endif
