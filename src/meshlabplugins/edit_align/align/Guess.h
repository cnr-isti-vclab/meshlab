/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#ifndef __VCG_GUESS
#define __VCG_GUESS

#include <vcg/space/point3.h>
#include <vcg/math/matrix44.h>
#include <vcg/space/index/grid_static_obj.h>
#include <vcg/space/index/kdtree/kdtree.h>

#include <vcg/complex/algorithms/point_sampling.h>
namespace vcg
{
namespace tri
{

// Class for Brute force alignment.
// given two meshes, Fix and Mov, it build a set of coarser and coarser occupancy grids.
// Put Fix into the grids, and then try to rotate a subset of the Mov points to see how much
// they cover the same space of the other one.

class Guess
{
public:
  // Parameter of the automatic alighment approach
  class Param
  {
  public:
    Param()
    {

      GridSize=200000;
      MatrixNum=1000;
      Range=8;
      SampleNum=10000;
      UGLevels=3;
    }

    int MatrixNum; // Number of Direction to be tested.

    int Range;     // numero
    int GridSize;
    int SampleNum; // Number of points of the Movable mesh that has to be used in the SearchTranslate
    int UGLevels;
  };

    class Result
        {
        public:

            Result(const Matrix44f &_m, int _score,int _id, const Point3f &BestTransvec)
            {
                m=_m;
                id=_id;
                score=_score;
                BestTr=BestTransvec;
            }
            Matrix44f m;
            int id; // the id of the in the RotMVec;
            int score;
            Point3f BestTr; //Best Translation vector found.
            bool operator <  (const Result & rr) const {return (score< rr.score);}
            };

/***********************************/
    // The internal state:

    std::vector<Matrix44f> RotMVec;
    Param pp;

    // the array of finer and finer grids
    GridStaticObj< bool,float> u[4];

    // the mesh are represented by just two (well sampled) point clouds.
    std::vector< Point3f > movVertBase;
    std::vector< Point3f > movNormBase;

    Point3f movBarycenterBase;
    Point3f fixBarycenterBase;

/***********************************/

    Guess(){}

    template <class MeshType>
    void Init(MeshType &fixm, MeshType &movm)
    {
        std::vector<MeshType *> FixV;
        std::vector<Matrix44f> MV;
        Matrix44f tm; tm.SetIdentity();
        MV.push_back(tm);
        FixV.push_back(&fixm);
        Init(FixV,MV,movm,tm);
    }

    template <class MeshType>
    void GenerateOccupancyMesh(MeshType &m, int lev, const Matrix44f &res)
    {
      VectorConstDataWrapper<std::vector<Point3f> > ww(this->movVertBase);
      Matrix44f invRes = Inverse(res);
      KdTree<float> surfTree(ww);
      surfTree.setMaxNofNeighbors(1);
      float scaleFact=0;
      float squaredVoxDiag = u[lev].voxel.SquaredNorm();
      m.Clear();
      int cnt =0,crossCnt=0;
      for(int i=0;i<u[lev].siz[0];++i) {
        for(int j=0;j<u[lev].siz[1];++j) {
          for(int k=0;k<u[lev].siz[2];++k) {
            if(u[lev].Grid(i,j,k))
            {
              cnt++;
              Point3f p;
              u[lev].IPfToPf(Point3f(i+0.5f,j+0.5f,k+0.5f),p);
              surfTree.doQueryK(invRes*p);
              if(surfTree.getNeighborSquaredDistance(0) < squaredVoxDiag)
              {
                scaleFact = 0.5;
                crossCnt++;
              }
              else scaleFact = 0.2;
              MeshType t;
              tri::Hexahedron(t);
              tri::UpdatePosition<MeshType>::Scale(t,scaleFact);
              tri::UpdatePosition<MeshType>::Translate(t,Point3f(0.5f,0.5f,0.5f));
              tri::UpdatePosition<MeshType>::Translate(t,Point3f(float(i),float(j),float(k)));
              tri::Append<MeshType,MeshType>::Mesh(m,t);
            }
          }
        }
      }
      tri::UpdatePosition<MeshType>::Matrix(m,u[lev].IPtoPfMatrix());
      qDebug("Found %i (%i) filled on %i",cnt, crossCnt, u[lev].siz[0]*u[lev].siz[1]*u[lev].siz[2]);
    }
    template <class MeshType>
    void Init( std::vector<MeshType*> &FixMV, std::vector<Matrix44f> &FixTrV, MeshType &MovM, const Matrix44f & MovTr)
    {
          Box3f tbb;
            // Step 1; Compute the Global BBox and init all the ug's
            for(int i=0;i<FixMV.size();++i)
                tbb.Add(FixTrV[i],FixMV[i]->bbox);

            // the UG are coaser and coarser;
            // u[0] is the finest
            // u[1] has 1/8 of the cells of u[0] etc etc...
            for(int kk=0;kk<pp.UGLevels;++kk)
            {
                u[kk].Create(tbb,pp.GridSize/pow(8.0f,float(kk)),false);
                qDebug("Created grid of %i (%i %i %i)\n",u[kk].size(),u[kk].siz[0],u[kk].siz[1],u[kk].siz[2]);
            }

            // Step 2: add all the Vertexes of Fix Meshes to the UG;
            typename MeshType::VertexIterator vi;
            Point3f tp;
            for(int i=0;i<FixMV.size();++i)
                {
                    for(vi=FixMV[i]->vert.begin();vi!=FixMV[i]->vert.end();++vi)
                        {
                            tp=FixTrV[i]*(*vi).P();
                            for(int kk=0;kk<pp.UGLevels;++kk)
                                        u[kk].Grid(tp)=true;
                        }

                }

      // step 3; subsample the vertex of mov mesh
            TrivialSampler<MeshType> ts(movVertBase);
            SurfaceSampling<MeshType, TrivialSampler<MeshType> >::Montecarlo(MovM, ts, pp.SampleNum);

            movBarycenterBase=Point3f(0,0,0);
            for(size_t i=0;i<movVertBase.size();++i)
                    movBarycenterBase += movVertBase[i];
            movBarycenterBase = movBarycenterBase/float(movVertBase.size());
            qDebug("mov barycenter %f %f %f\n",movBarycenterBase[0],movBarycenterBase[1],movBarycenterBase[2]);

            fixBarycenterBase=tbb.Center();
            qDebug("fix barycenter %f %f %f\n",fixBarycenterBase[0],fixBarycenterBase[1],fixBarycenterBase[2]);

          // step 4; prepare the vector of the rotation matrixes.
            GenRotMatrix( );

    }

    void ComputeTrans(std::vector<Result> &rv);
    void ComputeAlign(std::vector<Result> &rv);
    void AddVert(std::vector<Point3f> &vv, Matrix44d &Tr);
    void ApplyTransformation(const Point3f &baseTranslation, const Matrix44f &BaseRot,
                             std::vector< Point3f > &movVert, std::vector< Point3f > &movNorm, Box3f &movBox) const;

    int SearchBestTranslation(GridStaticObj<bool,float> &U,
                                        const Matrix44f &BaseRot,
                                        const int range,
                                        Point3f &StartTrans,
                                        Point3f &BestTrans  // miglior vettore di spostamento
                                        );


    int SearchTranslate(const int UGLev,
                                        const Matrix44d &BaseRot,
                                        const int range,
                                        const int step,
                                        Point3d &BestTransV,  // miglior vettore di spostamento
                                        bool Verbose);

    int SearchTranslate2(const int UGLev,
                                        const Matrix44d &BaseRot,
                                        const int range,
                                        const int step,
                                        Point3d &BestTransV,  // miglior vettore di spostamento
                                        bool Verbose);

    void GenRotMatrix();
    Point3f ComputeBaseTranslation(Matrix44f &baseRot) const;
    Matrix44f BuildTransformation(const Matrix44f &baseRot, const Point3f &BaseTrans) const ;
    Matrix44f BuildResult(const Matrix44f &baseRot, const Point3f &BaseTrans, const Point3f &BestTrans) const;

private :

};



/*
First Step: Precompute a set of well Distributed Normals
- Numero di orientamenti da testare
  funziona sempre
    con 66 (GenNormal(1,2) ottaedro suddiviso 2 volte ) per un totale di 858 orientamenti
  con 42 (GenNormal(2,1) icosaedro suddiviso una volta) per un totale di 462 orientamenti
*/
} // end namespace tri;
} // end namespace vcg;
#endif
