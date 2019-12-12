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
#include <vcg/math/gen_normal.h>
#include <vcg/complex/algorithms/point_sampling.h>
namespace vcg
{
namespace tri
{

// Class for Brute force alignment.
// given two meshes, Fix and Mov, it build a set of coarser and coarser occupancy grids.
// Put Fix into the grids, and then try to rotate a subset of the Mov points to see how much
// they cover the same space of the other one.

template<class ScalarType>
class Guess
{
  typedef typename vcg::Point3<ScalarType> Point3x;
  typedef typename vcg::Matrix44<ScalarType> Matrix44x;
  typedef typename vcg::Box3<ScalarType> Box3x;
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

            Result(const Matrix44x &_m, int _score,int _id, const Point3x &BestTransvec)
            {
                m=_m;
                id=_id;
                score=_score;
                BestTr=BestTransvec;
            }
            Matrix44x m;
            int id; // the id of the in the RotMVec;
            int score;
            Point3x BestTr; //Best Translation vector found.
            bool operator <  (const Result & rr) const {return (score< rr.score);}
            };

/***********************************/
    // The internal state:

    std::vector<Matrix44x> RotMVec;
    Param pp;

    // the array of finer and finer grids
    GridStaticObj< bool,ScalarType> u[4];

    // the mesh are represented by just two (well sampled) point clouds.
    std::vector< Point3x > movVertBase;
    std::vector< Point3x > movNormBase;

    Point3x movBarycenterBase;
    Point3x fixBarycenterBase;

/***********************************/

    Guess(){}

    template <class MeshType>
    void Init(MeshType &fixm, MeshType &movm)
    {
        std::vector<MeshType *> FixV;
        std::vector<Matrix44x> MV;
        Matrix44x tm; tm.SetIdentity();
        MV.push_back(tm);
        FixV.push_back(&fixm);
        Init(FixV,MV,movm,tm);
    }

    template <class MeshType>
    void GenerateOccupancyMesh(MeshType &m, int lev, const Matrix44x &res)
    {
      typedef typename MeshType::CoordType CoordType;
      VectorConstDataWrapper<std::vector<CoordType> > ww(this->movVertBase);
      Matrix44x invRes = Inverse(res);
      KdTree<ScalarType> surfTree(ww);
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
              Point3x p;
              u[lev].IPfToPf(Point3x(i+0.5f,j+0.5f,k+0.5f),p);
              unsigned int ind;
              ScalarType sqdist;
              surfTree.doQueryClosest(invRes*p,ind,sqdist);
              if(sqdist < squaredVoxDiag)
              {
                scaleFact = 0.5;
                crossCnt++;
              }
              else scaleFact = 0.2;
              MeshType t;
              tri::Hexahedron(t);
              tri::UpdatePosition<MeshType>::Scale(t,scaleFact);
              tri::UpdatePosition<MeshType>::Translate(t,CoordType(0.5f,0.5f,0.5f));
              tri::UpdatePosition<MeshType>::Translate(t,CoordType(float(i),float(j),float(k)));
              tri::Append<MeshType,MeshType>::Mesh(m,t);
            }
          }
        }
      }
      tri::UpdatePosition<MeshType>::Matrix(m,u[lev].IPtoPfMatrix());
      qDebug("Found %i (%i) filled on %i",cnt, crossCnt, u[lev].siz[0]*u[lev].siz[1]*u[lev].siz[2]);
    }
    template <class MeshType>
    void Init( std::vector<MeshType*> &FixMV, std::vector<Matrix44x> &FixTrV, MeshType &MovM, const Matrix44x & MovTr)
    {
          Box3x tbb;
            // Step 1; Compute the Global BBox and init all the ug's
            for(size_t i=0;i<FixMV.size();++i)
                tbb.Add(FixTrV[i],FixMV[i]->bbox);

            // the UG are coaser and coarser;
            // u[0] is the finest
            // u[1] has 1/8 of the cells of u[0] etc etc...
            for(int kk=0;kk<pp.UGLevels;++kk)
            {
                u[kk].Create(tbb,pp.GridSize/pow(8.0f,float(kk)),false);
                qDebug("Created grid of %i (%i %i %i)\n",u[kk].size(),u[kk].siz[0],u[kk].siz[1],u[kk].siz[2]);
            }

            // Step 2: add all the Vertices of Fix Meshes to the UG;
            typename MeshType::VertexIterator vi;
            Point3x tp;
            for(size_t i=0;i<FixMV.size();++i)
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

            movBarycenterBase=Point3x(0,0,0);
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
    void AddVert(std::vector<Point3x> &vv, Matrix44d &Tr);
    void ApplyTransformation(const Point3x &BaseTranslation, const Matrix44x &BaseRot,
                             std::vector< Point3x > &movVert, std::vector< Point3x > &movNorm, Box3x &movBox) const
{
    movBox.SetNull();
    movVert.clear();
    const Matrix44x baseTransf = BuildTransformation(BaseRot,BaseTranslation);

    typename std::vector< Point3x >::const_iterator vi;
    for(vi=this->movVertBase.begin();vi!=this->movVertBase.end();++vi)
        {
             movVert.push_back(baseTransf * (*vi));
             movBox.Add(movVert.back());
        }
}

    int SearchBestTranslation(GridStaticObj<bool, ScalarType> &U,
                                        const Matrix44x &BaseRot,
                                        const int range,
                                        Point3x &StartTrans,
                                        Point3x &BestTrans  // miglior vettore di spostamento
                                        ){

  const int wide1=(range*2+1);
  const int wide2=wide1*wide1;


  // the rotated and translated) set of vertices;
  std::vector< Point3x > movVert;
  std::vector< Point3x > movNorm;
  Box3x movBox;

  ApplyTransformation(StartTrans, BaseRot,movVert,movNorm, movBox);

  int t0=clock();

//  qDebug("Start searchTranslate movVert %i(grid sz %i %i %i",movVert.size(), U.siz[0],U.siz[1],U.siz[2]);
//  qDebug(" bbox ug  = %6.2f %6.2f %6.2f - %6.2f %6.2f %6.2f",U.bbox.min[0],U.bbox.min[1],U.bbox.min[2],U.bbox.max[1],U.bbox.max[1],U.bbox.max[2]);
//  qDebug(" bbox mov = %6.2f %6.2f %6.2f - %6.2f %6.2f %6.2f",movBox.min[0],movBox.min[1],movBox.min[2],movBox.max[1],movBox.max[1],movBox.max[2]);
  Point3i ip;
  int i,ii,jj,kk;
  std::vector<int> test((range*2+1)*(range*2+1)*(range*2+1),0);
  //int bx,by,bz,ex,ey,ez;
  Point3i b,e;
  int testposii,testposjj;
  // First Loop, for each point, attempt all the possible translations (2*range)^3 and
  // if the translated point hits something increment the counter for that "translation"

  for(i=0;i<movVert.size();++i)
  {
    Point3x tp; tp.Import(movVert[i]);
    if(U.bbox.IsIn(tp)){
      U.PToIP(tp,ip);
      b[0] = std::max(0,ip[0]-range);
      b[1] = std::max(0,ip[1]-range);
      b[2] = std::max(0,ip[2]-range);
      e[0] = std::min(U.siz[0],ip[0]+range);
      e[1] = std::min(U.siz[1],ip[1]+range);
      e[2] = std::min(U.siz[2],ip[2]+range);

      for(ii=b[0];ii<e[0];++ii)
      {
        testposii=(ii-ip[0]+range)*wide2;
        for(jj=b[1];jj<e[1];++jj)
        {
          testposjj=testposii+(jj-ip[1]+range)*wide1-ip[2]+range;
          for(kk=b[2];kk<e[2];++kk)
          {
            if(U.Grid(ii,jj,kk))
              ++test[testposjj+kk];
            assert(ii >=0 && ii < U.siz[0]);
            assert(jj >=0 && jj < U.siz[1]);
            assert(kk >=0 && kk < U.siz[2]);
          }
        }
      }
    }
  }
  int maxfnd=0;
  Point3i BestI;
  for(ii=-range;ii<=range;++ii)
    for(jj=-range;jj<=range;++jj)
      for(kk=-range;kk<=range;++kk)
      {
        const int pos = (range+ii)*wide2+(range+jj)*wide1+range+kk;
        if(test[pos]>maxfnd)
        {
          BestI=Point3i(ii,jj,kk);
          BestTrans=Point3x(ii*U.voxel[0], jj*U.voxel[1], kk*U.voxel[2]);
          maxfnd=test[pos];
        }
        //printf("Found %i su %i in %i\n",test[testcnt],movvert.size(),t1-t0);
      }

  int t1=clock();
  //if(Verbose)
  qDebug("BestTransl (%4i in %i msec) is %8.4f %8.4f %8.4f (%3i %3i %3i)",maxfnd, (1000*(t1-t0))/CLOCKS_PER_SEC,BestTrans[0],BestTrans[1],BestTrans[2],BestI[0],BestI[1],BestI[2]);
  return maxfnd;
}

    void GenRotMatrix()
    {
        vector<Point3x> NV;
        GenNormal<ScalarType>::Fibonacci(sqrt(float(pp.MatrixNum)),NV);

        ScalarType AngleStep;
        int AngleNum;
        ComputeStep(NV.size(),AngleStep,AngleNum);
        RotMVec.resize(NV.size()*AngleNum);
        qDebug("Generated %li normals and %li rotations",NV.size(),RotMVec.size());
        for(size_t i=0;i<NV.size();++i)
            for(size_t j=0;j<AngleNum;++j)
                    GenMatrix(RotMVec[i*AngleNum+j],NV[i],j*AngleStep);
    }

    // After the init of the grids and of the sampled point vector
    // we have to find a starting translation
    // we choose the sune such that the baricenter of T*R*Mov is in the middle of the grid bbox.

    Point3x ComputeBaseTranslation(Matrix44x &baseRot) const
    {
        Point3x movBarycenter = baseRot * this->movBarycenterBase;
        return this->movBarycenterBase - movBarycenter ;
    }

    Matrix44x BuildTransformation(const Matrix44x &baseRot, const Point3x &BaseTrans) const
    {
     Matrix44x Tr;
     Tr.SetTranslate(BaseTrans);
     return Tr*baseRot;
    }

    Matrix44x BuildResult(const Matrix44x &baseRot, const Point3x &BaseTrans, const Point3x &BestTrans) const
{
  Matrix44x Tr;
  Tr.SetTranslate(BestTrans);
  return Tr*BuildTransformation(baseRot, BaseTrans);
}
private :


    // Genera la matrice di rotazione che porta l'asse z a coincidere con Axis
    // e poi ruota di angle intorno a quell'asse.


    // If i have k uniformly distributed directions, each of them subtends a solid angle of 4Pi/k steradian;
    // A given cone with an apex angle of <a>  subtends a solid angle of c = (1 - cos(a/2))*2Pi
    // (think to the cylindrical projection of a cone...)
    // examples:
    //  a=0 -> c= 0
    //  a=pi/2 c= 2pi
    //  a=120 = 2/3 pi -> (1-cos(pi/3)*2pi -> pi cioe' 1/4 di sfera.
    // quindi se ogni normale sottende un angolo solido di s ottengo che
    // s = (1 -  cos(a/2)) *2pi
    // s/2pi -1 = -cos (a/2)
    // 1 - s/2pi  = cos (a/2)
    // a/2 = acos( 1- s/2pi))
    // a = 2*acos(( 1 - s/2pi))
    void GenMatrix(Matrix44x &a, Point3x Axis, ScalarType angleRad)
    {
        const ScalarType eps=1e-10;
        Point3x RotAx   = Axis ^ Point3x(0,0,1);
      float RotAngleRad = Angle(Axis,Point3x(0,0,1));

        if(math::Abs(RotAx.Norm())<eps) { // in questo caso Axis e' collineare con l'asse z
                RotAx=Point3x(0,1,0);
            }
      //printf("Rotating around (%5.3f %5.3f %5.3f) %5.3f\n",RotAx[0],RotAx[1],RotAx[2],RotAngle);
        a.SetRotateRad(RotAngleRad,RotAx);
        Matrix44x rr;
        rr.SetRotateRad( angleRad, Point3x(0,0,1));

        a=rr*a;
    }


    void ComputeStep(const int directionNum, ScalarType &StepAngle, int &StepNum)
    {
        ScalarType s = (4.0*M_PI)/directionNum; // solid angle subtended by each direction
      ScalarType a = 2 * acos(1-s*2.0/M_PI );
        a/=2;
      StepNum= (int)ceil(2*M_PI/a);
      StepAngle = 2.0*M_PI/ceil(2*M_PI/a);
      //printf("%i normals\nSolid Angle sotteso %f\n angolo step %f (%f deg)\nInterized %f (%f deg) %i steps", k , s ,a,ToDeg(a),StepAngle,ToDeg(StepAngle),StepNum);
    }

};



} // end namespace tri;
} // end namespace vcg;
#endif
