#ifndef DUAL_OPTIMIZER
#define DUAL_OPTIMIZER
#include <wrap/callback.h>

template <class MeshType>
class BaryOptimizatorDual
{
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType   FaceType;
	typedef typename MeshType::CoordType  CoordType;
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename vcg::tri::AreaPreservingTexCoordOptimization<MeshType> OptType;
	typedef typename vcg::tri::MeanValueTexCoordOptimization<MeshType> OptType1;
	//typedef typename vcg::tri::MeanValueTexCoordOptimization<MeshType> OptType;

	 EnergyType EType;

public:
	struct param_domain{
		MeshType *domain;
		std::vector<FaceType*> ordered_faces;
	};

	//OptType *optimizer;
	
	///set of star meshes to optimize barycentryc coords locally
	std::vector<param_domain> star_meshes;
	std::vector<param_domain> diamond_meshes;
	std::vector<param_domain> face_meshes;
	///structures for the optimization
	std::vector<MeshType*> HRES_meshes;
	std::vector<std::vector<VertexType*> > Ord_HVert;
	
	///hight resolution mesh and domain mesh
	MeshType *domain;
	MeshType *h_res_mesh;
	
	///initialize star parametrization
	void InitStarEquilateral()//const ScalarType &average_area=1)
	{
		///for each vertex
		int index=0;
		for (unsigned int i=0;i<domain->vert.size();i++)
		{
			if (!(domain->vert[i].IsD()))
			{
				std::vector<VertexType*> starCenter;
				starCenter.push_back(&domain->vert[i]);

				star_meshes[index].domain=new MeshType();

				///create star
				CreateMeshVertexStar(starCenter,star_meshes[index].ordered_faces,*star_meshes[index].domain);
				
				///and parametrize it
				ParametrizeStarEquilateral<MeshType>(*star_meshes[index].domain,1.0);

				index++;
			}
		}		
	}
	
	void InitDiamondEquilateral(const ScalarType &edge_len=1.0)
	{
		
		///for each vertex
		int index=0;
		for (unsigned int i=0;i<domain->face.size();i++)
		{
			if (!(domain->face[i].IsD()))
			{
				FaceType *f0=&domain->face[i];
				//for each edge
				for (int j=0;j<3;j++)
				{
					FaceType * f1=f0->FFp(j);
					if (f1<f0)
					{
						///add to domain map
						std::pair<FaceType*,FaceType*> entry=std::pair<FaceType*,FaceType*>(f0,f1);
						///end domain mapping

						int num0=j;
						int num1=f0->FFi(j);

						///copy the mesh
						std::vector<FaceType*> faces;
						faces.push_back(f0);
						faces.push_back(f1);

						diamond_meshes[index].domain=new MeshType();

						///create a copy of the mesh
						std::vector<VertexType*> orderedVertex;
						CopyMeshFromFaces<MeshType>(faces,orderedVertex,*diamond_meshes[index].domain);
						UpdateTopologies<MeshType>(diamond_meshes[index].domain);

						///set other components
						diamond_meshes[index].ordered_faces.resize(2);
						diamond_meshes[index].ordered_faces[0]=f0;
						diamond_meshes[index].ordered_faces[1]=f1;
						///parametrize locally
						ParametrizeDiamondEquilateral<MeshType>(*diamond_meshes[index].domain,num0,num1,edge_len);
					
						index++;
					}
				}
			}
		}
	}
	

	void InitFaceEquilateral(const ScalarType &edge_len=1)
	{
		///for each vertex
		int index=0;
		for (unsigned int i=0;i<domain->face.size();i++)
		{
			if (!(domain->face[i].IsD()))
			{
				FaceType *f0=&domain->face[i];

				std::vector<FaceType*> faces;
				faces.push_back(f0);

				///create the mesh
				face_meshes[index].domain=new MeshType();
				std::vector<VertexType*> orderedVertex;
				CopyMeshFromFaces<MeshType>(faces,orderedVertex,*face_meshes[index].domain);

				assert(face_meshes[index].domain->vn==3);
				assert(face_meshes[index].domain->fn==1);

				///initialize auxiliary structures
				face_meshes[index].ordered_faces.resize(1);
				face_meshes[index].ordered_faces[0]=f0;
				
				///parametrize it
				ParametrizeFaceEquilateral<MeshType>(*face_meshes[index].domain,edge_len);
				///add to search structures
				/*faceMap.insert(std::pair<FaceType*,param_domain*>(f0,&face_meshes[index]));*/
				index++;
			}
		}
	}

///given a point and a face return the half-star in witch it falls
int getVertexStar(const CoordType &point,FaceType *f)
{
	CoordType edge0=(f->P(0)+f->P(1))/2.0;
	CoordType edge1=(f->P(1)+f->P(2))/2.0;
	CoordType edge2=(f->P(2)+f->P(0))/2.0;
	CoordType Center=(f->P(0)+f->P(1)+f->P(2))/3.0;
	CoordType vect0=edge0-point;
	CoordType vect1=Center-point;
	CoordType vect2=edge2-point;
	CoordType Norm=f->N();
	ScalarType in0=(vect0^vect1)*Norm;
	ScalarType in1=(vect1^vect2)*Norm;
	if ((in0>=0)&&(in1>=0))
		return 0;

	vect0=edge0-point;
	vect1=Center-point;
	vect2=edge1-point;
	in0=(vect1^vect0)*Norm;
	in1=(vect2^vect1)*Norm;
	if ((in0>=0)&&(in1>=0))
		return 1;

	vect0=edge1-point;
	vect1=Center-point;
	vect2=edge2-point;
	in0=(vect1^vect0)*Norm;
	in1=(vect2^vect1)*Norm;
	assert((in0>=0)&&(in1>=0));
	return 2;
}

///given a point and a face return the half-diamond edge index in witch it falls
int getEdgeDiamond(const CoordType &point,FaceType *f)
{
	CoordType Center=(f->P(0)+f->P(1)+f->P(2))/3.0;
	CoordType vect0=f->P(1)-point;
	CoordType vect1=Center-point;
	CoordType vect2=f->P(0)-point;
	CoordType Norm=f->N();
	ScalarType in0=(vect0^vect1)*Norm;
	ScalarType in1=(vect1^vect2)*Norm;
	if ((in0>=0)&&(in1>=0))
		return 0;

	vect0=f->P(2)-point;
	vect1=Center-point;
	vect2=f->P(1)-point;
	in0=(vect0^vect1)*Norm;
	in1=(vect1^vect2)*Norm;
	if ((in0>=0)&&(in1>=0))
		return 1;

	vect0=f->P(0)-point;
	vect1=Center-point;
	vect2=f->P(2)-point;
	in0=(vect0^vect1)*Norm;
	in1=(vect1^vect2)*Norm;
	assert((in0>=0)&&(in1>=0));
	return 2;
}


///initialize Star Submeshes
void InitStarSubdivision()
{

	int index=0;
	HRES_meshes.clear();
	Ord_HVert.clear();
	///initialilze vector of meshes
	HRES_meshes.resize(star_meshes.size());
	Ord_HVert.resize(star_meshes.size());
	/*HVert.resize(star_meshes.size());*/
	for (unsigned int i=0;i<HRES_meshes.size();i++)
		HRES_meshes[i]=new MeshType();

	///for each vertex of base domain
	for (unsigned int i=0;i<domain->vert.size();i++)
	{
		VertexType *center=&domain->vert[i];
		if (!center->IsD())
		{
			///copy current parametrization of star 
			for (unsigned int k=0;k<star_meshes[index].ordered_faces.size();k++)
			{
				FaceType *param=&star_meshes[index].domain->face[k];
				FaceType *original=star_meshes[index].ordered_faces[k];
				for (int v=0;v<3;v++)
					original->V(v)->T().P()=param->V(v)->T().P();
			}

			///get h res vertex on faces composing the star
			std::vector<VertexType*> Hres,inDomain;
			getHresVertex<FaceType>(star_meshes[index].ordered_faces,Hres);

			///find out the vertices falling in the substar
			/*HVert[index].reserve(Hres.size()/2);*/
			for (unsigned int k=0;k<Hres.size();k++)
			{
				VertexType* chosen;
				VertexType* test=Hres[k];
				CoordType proj=Warp(test);
				FaceType * father=test->father;
				CoordType bary=test->Bary;
				///get index of half-star
				int index=getVertexStar(proj,father);
				chosen=father->V(index);
				///if is part of current half star
				if (chosen==center)
				{
					inDomain.push_back(test);
					///parametrize it
					GetUV<MeshType>(father,bary,test->T().U(),test->T().V());
				}
			}
			///create Hres mesh already parametrized
			std::vector<FaceType*> OrderedFaces;
			CopyMeshFromVertices<MeshType>(inDomain,Ord_HVert[index],OrderedFaces,*HRES_meshes[index]);
			index++;
		}
	}
}

///initialize Star Submeshes
void InitDiamondSubdivision()
{
	
	int index=0;
	HRES_meshes.clear();
	Ord_HVert.clear();
	///initialilze vector of meshes
	HRES_meshes.resize(diamond_meshes.size());
	Ord_HVert.resize(diamond_meshes.size());
	/*HVert.resize(star_meshes.size());*/
	for (unsigned int i=0;i<HRES_meshes.size();i++)
		HRES_meshes[i]=new MeshType();

	///for each edge of base domain
	for (unsigned int i=0;i<domain->face.size();i++)
	{
		FaceType *f0=&domain->face[i];
		if (f0->IsD())
			break;
		//for each edge
		for (int eNum=0;eNum<3;eNum++)
		{
			FaceType * f1=f0->FFp(eNum);
			if (f1<f0)
			{
				///copy current parametrization of diamond 
				for (unsigned int k=0;k<diamond_meshes[index].ordered_faces.size();k++)
				{
					FaceType *param=&diamond_meshes[index].domain->face[k];
					FaceType *original=diamond_meshes[index].ordered_faces[k];
					for (int v=0;v<3;v++)
						original->V(v)->T().P()=param->V(v)->T().P();
				}

				///get h res vertex on faces composing the diamond
				std::vector<VertexType*> Hres,inDomain;
				getHresVertex<FaceType>(diamond_meshes[index].ordered_faces,Hres);

				///find out the vertices falling in the half-diamond
				/*HVert[index].reserve(Hres.size()/2);*/
				for (unsigned int k=0;k<Hres.size();k++)
				{
					//VertexType* chosen;
					VertexType* test=Hres[k];
					CoordType proj=Warp(test);
					FaceType * father=test->father;
					CoordType bary=test->Bary;
					///get index of half-star
					int index=getEdgeDiamond(proj,father);
					///if is part of current half star
					if (index==eNum)
					{
						inDomain.push_back(test);
						///parametrize it
						GetUV<MeshType>(father,bary,test->T().U(),test->T().V());
					}
				}
				///create Hres mesh already parametrized
				std::vector<FaceType*> OrderedFaces;
				CopyMeshFromVertices<MeshType>(inDomain,Ord_HVert[index],OrderedFaces,*HRES_meshes[index]);
				index++;
			}
		}
	}
}

///initialize Star Submeshes
void InitFaceSubdivision()
{

	int index=0;
	HRES_meshes.clear();
	Ord_HVert.clear();
	///initialilze vector of meshes
	HRES_meshes.resize(face_meshes.size());
	Ord_HVert.resize(face_meshes.size());

	for (unsigned int i=0;i<HRES_meshes.size();i++)
		HRES_meshes[i]=new MeshType();

	///for each face of base domain
	for (unsigned int i=0;i<domain->face.size();i++)
	{
		FaceType *f0=&domain->face[i];
		if (f0->IsD())
			break;
		///copy current parametrization of face 
		FaceType *param=&face_meshes[index].domain->face[0];
		FaceType *original=face_meshes[index].ordered_faces[0];

		assert(face_meshes[index].domain->vn==3);
		assert(face_meshes[index].domain->fn==1);
		assert(face_meshes[index].ordered_faces.size()==1);
		assert(original==f0);

		for (int v=0;v<3;v++)
			original->V(v)->T().P()=param->V(v)->T().P();

		///get h res vertex on faces composing the diamond
		std::vector<VertexType*> inDomain;
		getHresVertex<FaceType>(face_meshes[index].ordered_faces,inDomain);

		///transform in UV
		for (unsigned int k=0;k<inDomain.size();k++)
		{
			VertexType* test=inDomain[k];
			FaceType * father=test->father;
			assert(father==f0);
			CoordType bary=test->Bary;
			GetUV<MeshType>(father,bary,test->T().U(),test->T().V());
		}
		///create Hres mesh already parametrized
		std::vector<FaceType*> OrderedFaces;
		CopyMeshFromVertices<MeshType>(inDomain,Ord_HVert[index],OrderedFaces,*HRES_meshes[index]);
		index++;
	}
 }


void MinimizeStep(const int &phaseNum)
{
	

	//Ord_HVert[index]
	for (unsigned int i=0;i<HRES_meshes.size();i++)
	{

		MeshType *currMesh=HRES_meshes[i];
		if (currMesh->fn>0)
		{
		UpdateTopologies<MeshType>(currMesh);
		
		///on star
		int numDom=1;
		switch (phaseNum)
		{
		case 0:numDom=6;break;//star
		case 1:numDom=2;break;//diam
		case 2:numDom=1;break;//face
		}
		///save previous values
		InitDampRestUV(*currMesh);
		bool b=UnFold<MeshType>(*currMesh,numDom);
		bool isOK=testParamCoords<MeshType>(*currMesh);
		if ((!b)||(!isOK))
			RestoreRestUV<MeshType>(*currMesh);

		/*OptType opt(*currMesh);
		opt.TargetCurrentGeometry();
		opt.SetBorderAsFixed();*/

		///save previous values
		InitDampRestUV(*currMesh);
		
		

		///NEW SETTING SPEED
		
		ScalarType edge_esteem=GetSmallestUVHeight(*currMesh);
		

		ScalarType speed0=edge_esteem*0.1;
		ScalarType conv=edge_esteem*0.002;

		if (accuracy>1)
			conv*=1.0/(ScalarType)((accuracy-1)*10.0);

		if (EType==EN_EXTMips)
		{
			OptType opt(*currMesh);
			opt.TargetCurrentGeometry();
			opt.SetBorderAsFixed();
			opt.SetSpeed(speed0);
			opt.IterateUntilConvergence(conv);
		}
		else
		if (EType==EN_MeanVal)
		{
			OptType1 opt(*currMesh);
			opt.TargetCurrentGeometry();
			opt.SetBorderAsFixed();
			opt.SetSpeed(speed0);
			opt.IterateUntilConvergence(conv);
		}

		//opt.IterateUntilConvergence();

		///test for uv errors
		bool IsOK=true;
		for (unsigned int j=0;j<currMesh->vert.size();j++)
		{
			VertexType *ParamVert=&currMesh->vert[j];
			ScalarType u=ParamVert->T().U();
			ScalarType v=ParamVert->T().V();
			if ((!((u<=1.001)&&(u>=-1.001)))||
			   (!(v<=1.001)&&(v>=-1.001)))
			{
				IsOK=false;

				for (unsigned int k=0;k<currMesh->vert.size();k++)
					currMesh->vert[k].T().P()=currMesh->vert[k].RestUV;
				break;
			}
		}
		//reassing fathers and bary coordinates
		for (unsigned int j=0;j<currMesh->vert.size();j++)
		{
			VertexType *ParamVert=&currMesh->vert[j];
			VertexType *OrigVert=Ord_HVert[i][j];
			ScalarType u=ParamVert->T().U();
			ScalarType v=ParamVert->T().V();
			///then get face falling into and estimate (alpha,beta,gamma)
			CoordType bary;
			BaseFace* chosen;
			param_domain *currDom;
			switch (phaseNum)
			{
			case 0:currDom=&star_meshes[i];break;//star
			case 1:currDom=&diamond_meshes[i];break;//diam
			case 2:currDom=&face_meshes[i];break;//face
			}
			/*assert(currDom->domain->vn==3);
			assert(currDom->domain->fn==1);*/
			bool inside=GetBaryFaceFromUV(*currDom->domain,u,v,currDom->ordered_faces,bary,chosen);
			if (!inside)
			{
				/*#ifndef _MESHLAB*/
				printf("\n OUTSIDE %f,%f \n",u,v);
				/*#endif*/
				vcg::Point2<ScalarType> UV=vcg::Point2<ScalarType>(u,v);
				ForceInParam<MeshType>(UV,*currDom->domain);
				u=UV.X();
				v=UV.Y();
				inside=GetBaryFaceFromUV(*currDom->domain,u,v,currDom->ordered_faces,bary,chosen);
				//assert(0);
			}
			assert(inside);
			//OrigVert->father=chosen;
			//OrigVert->Bary=bary;
			AssingFather(*OrigVert,chosen,bary,*domain);
		}
		}
			///delete current mesh
			delete(HRES_meshes[i]);
	}

	///clear father and bary
	for (unsigned int i=0;i<domain->face.size();i++)
		domain->face[i].vertices_bary.clear();

	///set face-vertex link
	for (unsigned int i=0;i<h_res_mesh->vert.size();i++)
	{
		BaseVertex *v=&h_res_mesh->vert[i];
		if (!v->IsD())
		{
			BaseFace *f=v->father;
			CoordType bary=v->Bary;
			f->vertices_bary.push_back(std::pair<VertexType*,CoordType>(v,bary));
		}
	}	
}


int accuracy;
vcg::CallBackPos *cb;
int step;

public:


	void Init(MeshType &_domain,
			  MeshType &_h_res_mesh,
			  vcg::CallBackPos *_cb,
			  int _accuracy=1,
			  EnergyType _EType=EN_EXTMips)
	{
		EType=_EType;

		step=0;
		cb=_cb;		
		accuracy=_accuracy;

		vcg::tri::UpdateNormals<MeshType>::PerFaceNormalized(_domain);

		domain=&_domain;
		h_res_mesh=&_h_res_mesh;
		
		///initialize STARS
		star_meshes.resize(domain->vn);
		InitStarEquilateral();
		/*InitStarSubdivision();*/

		///initialize DIAMONDS
		int num_edges=0;
		for (unsigned int i=0;i<domain->face.size();i++)
		{
			if (!(domain->face[i].IsD()))
			{
				FaceType *f0=&domain->face[i];
				//for each edge
				for (int j=0;j<3;j++)
				{
					FaceType * f1=f0->FFp(j);
					if (f1<f0)
						num_edges++;
				}
			}
		}
		diamond_meshes.resize(num_edges);
		InitDiamondEquilateral();
		
		///initialize FACES
		face_meshes.resize(domain->fn);
		InitFaceEquilateral();
		
		///init minimizer
		for (unsigned int i=0;i<h_res_mesh->vert.size();i++)
			h_res_mesh->vert[i].P()=h_res_mesh->vert[i].RPos;

		//InitDampRestUV(*h_res_mesh);
	}

	
	void PrintAttributes()
	{
		
		int done=step;
		int total=6;
		ScalarType ratio=(ScalarType)done/total;
		int percent=(int)(ratio*(ScalarType)100);
		ScalarType distArea=ApproxAreaDistortion<BaseMesh>(*h_res_mesh,domain->fn);
		ScalarType distAngle=ApproxAngleDistortion<BaseMesh>(*h_res_mesh);
		char ret[200];
		sprintf(ret," PERFORM GLOBAL OPTIMIZATION  Area distorsion:%4f ; ANGLE distorsion:%4f ",distArea,distAngle);
		(*cb)(percent,ret);
	}

	void Optimize(ScalarType gap=0.5,int max_step=10)
	{
		int k=0;
		ScalarType distArea=ApproxAreaDistortion<BaseMesh>(*h_res_mesh,domain->fn);
		ScalarType distAngle=ApproxAngleDistortion<BaseMesh>(*h_res_mesh);
		ScalarType distAggregate0=geomAverage<ScalarType>(distArea+1.0,distAngle+1.0,3,1)-1;
		bool ContinueOpt=true;
		PatchesOptimizer<BaseMesh> DomOpt(*domain,*h_res_mesh);
		step++;
		

		DomOpt.OptimizePatches();
		PrintAttributes();

		while (ContinueOpt)
		{
			///domain Optimization	
			k++;
			
			InitStarSubdivision();
			MinimizeStep(0);

			InitDiamondSubdivision();
			MinimizeStep(1);

			InitFaceSubdivision();
			MinimizeStep(2);
			step++;
			PrintAttributes();

			distArea=ApproxAreaDistortion<BaseMesh>(*h_res_mesh,domain->fn);
			distAngle=ApproxAngleDistortion<BaseMesh>(*h_res_mesh);
			ScalarType distAggregate1=geomAverage<ScalarType>(distArea+1.0,distAngle+1.0,3,1)-1;
			ScalarType NewGap=((distAggregate0-distAggregate1)*100.0)/distAggregate0;
			if ((NewGap<gap)||(k>max_step))
				ContinueOpt=false;
			distAggregate0=distAggregate1;
		}
	}
};
#endif