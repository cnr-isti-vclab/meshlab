#include "fieldinterpolator.h"
#include <iostream>
#include <fstream>
#include <vcg/Eigen/Eigen>

using namespace std;
using namespace Eigen;
using namespace vcg;

// Solving cholesky with laplacian constraints simply requires the construction
// of a SQUARE matrix with the appropriate mesh coefficients. Constraints on vertices
// are added by the function AddConstraint
void test_cholmod2();
void FieldInterpolator::Init(CMeshO* mesh, LAPLACIAN laptype){
    // Testing function
    // test_cholmod2(); exit(0);

    //--- Activate topology support for mesh
    mesh->face.EnableFFAdjacency();
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(*mesh);
    // CHECK: are these two really required? especially the second
    mesh->vert.EnableVFAdjacency();
    mesh->face.EnableVFAdjacency();
    vcg::tri::UpdateTopology<CMeshO>::VertexFace(*mesh);

    // Store mesh pointer & master OMEGA
    this->mesh = mesh;

    // Allocate memory: square matrix to feed cholesky
    A_dyn = new DynamicSparseMatrix<FieldType>(mesh->vn,mesh->vn);
    xb = new Matrix<FieldType, Dynamic, 1>(mesh->vn);
    xb->setZero(mesh->vn);


    // Fill in the entries of A_dyn and xb
    if( laptype == COMBINATORIAL ){
        // Iterate throgh every vertice
        CVertexO* v;
        size_t v_i = 0;
        CVertexO* vNeigh;
        size_t vNeigh_i;
        for(unsigned int vi=0; vi<mesh->vert.size(); vi++){
            v = &mesh->vert[vi];
            v_i = vcg::tri::Index(*mesh,v);
            // Iterate one-ring
            int numNeighs=0;
            vcg::face::Pos<CFaceO> pos(v->VFp(), v);
            CVertexO* firstV = pos.VFlip();
            do{
                pos.NextE(); // looks weird but is correct here
                vNeigh = pos.VFlip();
                vNeigh_i = vcg::tri::Index(*mesh,vNeigh);
                // Off-diagonal entries (symmetric)
                // Only lower diagonal
                if(v_i>vNeigh_i)
                    A_dyn->coeffRef(v_i,vNeigh_i) = -1;
                else
                    A_dyn->coeffRef(vNeigh_i,v_i) = -1;
                numNeighs++;
            }
            while(vNeigh != firstV);
            // Diagonal entries
            A_dyn->coeffRef(v_i,v_i) = numNeighs;
            // Was I doing it right?
            assert(pos.NumberOfIncidentVertices() == numNeighs);
        }
    }
    else if( laptype == COTANGENT ){
        // The four vertices involved into cotangent weight computation
        CVertexO *vI, *vJ, *vA, *vB;
        // The integer indexes of i and j (to address laplacian matrix)
        size_t vI_i, vJ_i;
        // The spoke vectors
        Point3f v1,v2;
        // The angles used for the cotangent computation
        float alpha, beta, cotcoeff, totcoeff;

        // Iterate throgh every vertice
        for(unsigned int vi=0; vi<mesh->vert.size(); vi++){
            vI   = &mesh->vert[vi];
            vI_i = vcg::tri::Index(*mesh,vI);

            // Zero the total coefficient
            totcoeff = 0;

            // Iterate one-ring
            vcg::face::Pos<CFaceO> pos(vI->VFp(), vI);
            vcg::face::Pos<CFaceO> A_pos; // lies on face alpha
            vcg::face::Pos<CFaceO> B_pos; // lies on face beta
            CVertexO* firstV = pos.VFlip();
            do{
                // Next spoke vertex
                pos.NextE();
                vJ   = pos.VFlip();
                vJ_i = vcg::tri::Index(*mesh,vJ);

                // alpha-spoke
                A_pos = pos;
                A_pos.FlipE();
                vA = A_pos.VFlip();
                v1 = vI->P()-vA->P();
                v2 = vJ->P()-vA->P();
                alpha = acos( v1.dot(v2) / (v1.Norm()*v2.Norm()) );

                // beta-spoke
                B_pos = pos;
                B_pos.FlipF();
                B_pos.FlipE();
                vB = B_pos.VFlip();
                v1 = vI->P()-vB->P();
                v2 = vJ->P()-vB->P();
                beta = acos( v1.dot(v2) / (v1.Norm()*v2.Norm()) );

                // Compute cotangent coefficient
                cotcoeff = .5 * ( cos(alpha)/sin(alpha) + cos(beta)/sin(beta) );
                totcoeff += cotcoeff;

                // Off-diagonal entries (symmetric) Only lower diagonal
                if(vI_i > vJ_i)
                    A_dyn->coeffRef(vI_i,vJ_i) = -cotcoeff;
                else
                    A_dyn->coeffRef(vJ_i,vI_i) = -cotcoeff;
            }
            while(vJ != firstV);
            // Diagonal entries
            A_dyn->coeffRef(vI_i,vI_i) = totcoeff;
        }
    }
}

void FieldInterpolator::AddConstraint( unsigned int vidx, FieldType omega, FieldType val){
    assert( !math::IsNAN(val) );
    assert( omega != 0 );
    A_dyn->coeffRef(vidx,vidx) += omega;
    xb->coeffRef(vidx) += omega*val;
}
void FieldInterpolator::Solve(){
    
    //--- DEBUG: Damp the matrix on file, so that matlab can read it and compute solution
    // You need to remove the first few lines from the matrix.txt before being able to import
    // the matrix successfully!!
    #if 0
        ofstream myfile;
        myfile.open ("/Users/ata2/workspace/workspace_vase/cholmod_verify_posdefi/matrix.txt");
        myfile << *A_dyn;
        cout << *A_dyn;
        myfile.close();
        myfile.open("/Users/ata2/workspace/workspace_vase/cholmod_verify_posdefi/vector.txt");
        myfile << *xb;
        cout <<  *xb;
        myfile.close();
    #endif

    //--- Construct the cholesky factorization
    SparseLLT<SparseMatrix<FieldType> ,Cholmod> llt(*A_dyn);
    //--- Solve the linear system
    llt.solveInPlace(*xb);
    //-- Copy the results back in Q() field of surface
    // cout << "X: ";
    // bool hasnan = false;
    for(unsigned int vi=0; vi<mesh->vert.size(); vi++){
        (mesh->vert[vi]).Q() = (*xb)[vi];
        // hasnan = hasnan || math::IsNAN( (*xb)[vi] );
        // cout << (*xb)[vi] << " ";
    }
    // cout << endl;

    //--- DEBUG: Read the matlab computed solution from file
    #if 0
        ifstream infile("/Users/ata2/workspace/workspace_vase/cholmod_verify_posdefi/vector.txt");
        for(int vi=0; vi<mesh->vert.size(); vi++)
        infile >> (mesh->vert[vi]).Q();
    #endif
}






/*

//-----------------------------------------------------------------------------------------//
//
//                                     TESTING FUNCTIONS
//
//-----------------------------------------------------------------------------------------//
void test_cholmod2(){
    SparseMatrix<double> A(6, 6);
    VectorXd xs = VectorXd::Zero(6);
    VectorXd xf = VectorXd::Zero(6);

    typedef double FieldType;

    // These are how I defined them
    Eigen::DynamicSparseMatrix<FieldType>* A_dyn;
    A_dyn = new DynamicSparseMatrix<FieldType>(6,6);
    // Equivalent to VectorXd or VectorXf (http://eigen.tuxfamily.org/dox/TutorialCore.html#TutorialCoreMatrixTypes)
    Eigen::Matrix<FieldType, Eigen::Dynamic, 1>* xb;
    xb = new Matrix<FieldType, Dynamic, 1>(6);
    xb->setZero(6);

    // MOD
    A.insert(0,0) = 4;   A_dyn->coeffRef(0,0) =  4;
    A.insert(1,0) = -1;  A_dyn->coeffRef(1,0) = -1;
    A.insert(2,0) = -1;  A_dyn->coeffRef(2,0) = -1;
    A.insert(4,0) = -1;  A_dyn->coeffRef(4,0) = -1;
    A.insert(5,0) = -1;  A_dyn->coeffRef(5,0) = -1;
    A.insert(1,1) = 104; A_dyn->coeffRef(1,1) =  104;
    A.insert(2,1) = -1;  A_dyn->coeffRef(2,1) = -1;
    A.insert(3,1) = -1;  A_dyn->coeffRef(3,1) = -1;
    A.insert(4,1) = -1;  A_dyn->coeffRef(4,1) = -1;
    A.insert(2,2) = 4;   A_dyn->coeffRef(2,2) =  4;
    A.insert(3,2) = -1;  A_dyn->coeffRef(3,2) = -1;
    A.insert(5,2) = -1;  A_dyn->coeffRef(5,2) = -1;
    A.insert(3,3) = 4;   A_dyn->coeffRef(3,3) = 4;
    A.insert(4,3) = -1;  A_dyn->coeffRef(4,3) = -1;
    A.insert(5,3) = -1;  A_dyn->coeffRef(5,3) = -1;
    A.insert(4,4) = 4;   A_dyn->coeffRef(4,4) = 4;
    A.insert(5,4) = -1;  A_dyn->coeffRef(5,4) = -1;
    A.insert(5,5) = 104; A_dyn->coeffRef(5,5) = 104;
    A.finalize();

    // MOD
    std::cerr << MatrixXd(*A_dyn) << "\n\n";

    // Create a full copy of the matrix
    MatrixXd AFULL(A);
    std::cerr << AFULL << "\n\n";

    xs << 0, -99.8595, 0, 0, 0, 99.9204;
    xf = xs;

    // MOD
    xb->coeffRef(0) = 0;
    xb->coeffRef(1) = -99.8595;
    xb->coeffRef(2) = 0;
    xb->coeffRef(3) = 0;
    xb->coeffRef(4) = 0;
    xb->coeffRef(5) = 99.9204;
    std::cout << (*xb).transpose() << endl;

    // using a LLT factorization of the full matrix
    AFULL.llt().solveInPlace(xf);
    std::cout << "FULL: " << xf.transpose()  << endl;

    // Using sparse cholesky
    SparseLLT<SparseMatrix<double> ,Cholmod> llt(A);
    llt.solveInPlace(xs);
    std::cout << "SPAR: " << xs.transpose()  << endl;

    // Using my way of declaring
    SparseLLT<SparseMatrix<FieldType> ,Cholmod> llt2(*A_dyn);
    llt2.solveInPlace(*xb);
    std::cout << "SPAR2: " << (*xb).transpose()  << endl;

    delete A_dyn;
    delete xb;
}

void test_cholmod(){
    using namespace Eigen;
    //--- Input data
    int numel = 10;
    double omega = 1e8;
    float Y[]  = {0.00, 0.11, 0.22, 0.33, 0.44,
                  0.44, 0.33, 0.22, 0.11, 0.00};
    int cond[] = { 1, 1, 1, 0, 0, 0, 0, 1, 1, 1};

    //--- Allocates the data for A x=b
    DynamicSparseMatrix<double> A(numel, numel); // Adyn
    VectorXd xb = VectorXd::Zero(numel);  // x and b

    //--- Create a simple graph laplacian
    for(int i=0; i<numel; i++){
        // Off-diagonal entries
        if(i>0)       A.insert(i,i-1) = -1.0;
        if(i<numel-1) A.insert(i,i+1) = -1.0;
        // Diagonal entries (no cyclic boundary)
        A.insert(i,i) = ((i==0)||(i==numel-1))?
                        1+omega*cond[i] : 2+omega*cond[i];
        // Coefficient b entries
        xb[i] = omega*cond[i]*Y[i];
    }
    std::cout << A;

    //--- Solve using sparse cholesky and output
    SparseMatrix<float> Asm(A);
    SparseLLT<SparseMatrix<double> ,Cholmod> llt(Asm);
    llt.solveInPlace(xb);
    std::cout << "x: " << xb.transpose()  << endl;
    exit(0);
}

void mesh_onering_iterate( CMeshO* mesh ){
    // Activate/update topology structures
    mesh->face.EnableFFAdjacency();
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(*mesh);
    mesh->vert.EnableVFAdjacency();
    mesh->face.EnableVFAdjacency();
    vcg::tri::UpdateTopology<CMeshO>::VertexFace(*mesh);

    // Iterate throgh every vertice
    for(unsigned int vi=0; vi<mesh->vert.size(); vi++){
        CVertexO* v = &mesh->vert[vi];
        qDebug() << "onering of vertex: " << vcg::tri::Index(*mesh, v);

        // Create a POS iterator
        vcg::face::Pos<CFaceO> pos(v->VFp(), v);
        CVertexO* firstV = pos.VFlip();
        do{
            pos.NextE();
            v = pos.VFlip();
            qDebug() << "--> " << vcg::tri::Index(*mesh, v);
        }
        while(v != firstV);
        qDebug() << endl;
    }
}

*/
