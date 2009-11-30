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
#include <vcg/complex/trimesh/point_sampling.h>
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
				Step=1;
				SampleNum=10000;
				UGLevels=3;
			}
			
			int MatrixNum; // Number of Direction to be tested. 

      int Range;     // numero
			int Step;
			int GridSize;
			int SampleNum; // Number of points of the Movable mesh that has to be used in the SearchTranslate
			int UGLevels;
		};

	class Result
		{
		public:
			
			Result(Matrix44d &_m, double _err,int _id, const Point3d &btv)
			{
				m=_m;
				id=_id;
				err=_err;
				BestTr=btv;
			}
			Matrix44d m;
			int id; // identificativo della rotazione da cui si era partiti
			double err;
			Point3d BestTr; //Best Translation vector found. 
			bool operator <  (const Result & rr) const {return (err< rr.err);}
			};

/***********************************/
// The internal state:

  std::vector<Matrix44f> MV;
	Param pp;

	//Matrix44f BaseRot;
	
	// the array of finer and finer grids
	GridStaticObj< bool,float> u[4];
	
	std::vector< Point3f > movVertBase;
	std::vector< Point3f > movNormBase;
	Point3f movBarycenterBase;
		
	
/***********************************/

	Guess(){	
	};

	template <class MSH> 
	void Init(MSH &fixm, MSH &movm)
	{
		std::vector<MSH *> FixV;
		std::vector<Matrix44f> MV;
		Matrix44f tm; tm.SetIdentity();
		MV.push_back(tm);
		FixV.push_back(&fixm);
		Init(FixV,MV,movm,tm);
	}


	template <class MSH>
	void Init( std::vector<MSH*> &FixMV, std::vector<Matrix44f> &FixTV, MSH &MovM, const Matrix44f & MovTr)
	{
		  Box3f tbb;
			// Step 1; Compute the Global BBox and init all the ug's
			for(int i=0;i<FixMV.size();++i)
				tbb.Add(FixTV[i],FixMV[i]->bbox);
			
			// the UG are coaser and coarser;
			// u[0] is the finest
			// u[1] has 1/8 of the cells of u[0] etc etc...
			for(int kk=0;kk<pp.UGLevels;++kk)
			{
                u[kk].Create(tbb,pp.GridSize/pow(8.0f,float(kk)),false);
				printf("Created grid of %i (%i %i %i)\n",u[kk].size(),u[kk].siz[0],u[kk].siz[1],u[kk].siz[2]);
			}
			
			// Step 2: add all the Vertexes of Fix Meshes to the UG;
			typename MSH::VertexIterator vi;
			Point3f tp;
			for(int i=0;i<FixMV.size();++i)
				{
					for(vi=FixMV[i]->vert.begin();vi!=FixMV[i]->vert.end();++vi)
						{
							tp=FixTV[i]*(*vi).P();
							for(int kk=0;kk<pp.UGLevels;++kk)
										u[kk].Grid(tp)=true;
						}
						
				}
		
      // step 3; subsample the vertex of mov mesh
			TrivialSampler<MSH> ts(movVertBase);
			SurfaceSampling<MSH, TrivialSampler<MSH> >::Montecarlo(MovM, ts, pp.SampleNum);
								
			movBarycenterBase=Point3f(0,0,0);
			for(size_t i=0;i<movVertBase.size();++i)
					movBarycenterBase += movVertBase[i];
			movBarycenterBase = movBarycenterBase/float(movVertBase.size());
			
		  // step 4; prepare the basic rotation matrixes.
			GenRotMatrix( );
			
	}

	void ComputeTrans(std::vector<Result> &rv);
	void ComputeAlign(std::vector<Result> &rv);
	void AddVert(std::vector<Point3f> &vv, Matrix44d &Tr);
	void ApplyTransformation(const Point3f &baseTranslation, const Matrix44f &BaseRot, std::vector< Point3f > &movVert, std::vector< Point3f > &movNorm, Box3f &movBox);

	int SearchBestTranslation(	GridStaticObj<bool,float> &U,
										const Matrix44f &BaseRot,
										const int range, 
										const int step, 
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

	void GetM(Result &rr, Matrix44d &Res);
  void GenRotMatrix();
	Point3f InitBaseTranslation(Matrix44f &baseRot);
	Matrix44f BuildTransformation(Matrix44f &baseRot, Point3f &BaseTrans);

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
