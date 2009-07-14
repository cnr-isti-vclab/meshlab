#ifndef PARAM_COLLAPSE
#define PARAM_COLLAPSE

// local optimization
#include <vcg/complex/local_optimization.h>
#include <vcg/math/quadric.h>
#include <vcg/complex/local_optimization/tri_edge_collapse_quadric.h>
#include <vcg/complex/local_optimization/tri_edge_flip.h>
#include <set>

#include <local_parametrization.h>
#include <mesh_operators.h>
#include <vcg/space/color4.h>
#include <lm.h>
#include <uv_grid.h>

template <class BaseMesh>
class ParamEdgeCollapse: public vcg::tri::TriEdgeCollapse<BaseMesh,ParamEdgeCollapse<BaseMesh> > {
//#endif
public:
	typedef typename BaseMesh::VertexType::EdgeType EdgeType;
	typedef typename BaseMesh::VertexType BaseVertex;
	typedef typename BaseMesh::FaceType   BaseFace;

	inline ParamEdgeCollapse(const EdgeType &p, int mark)
	{    
		localMark = mark;
		pos=p;
		_priority = ComputePriority();
		//savedomain=false;
	}

	inline ScalarType Cost()
  {
	std::vector<typename BaseMesh::FaceType*> on_edge,faces1,faces2;
	getSharedFace<BaseMesh>(pos.V(0),pos.V(1),on_edge,faces1,faces2);

	/*const ScalarType sqr3=sqrt(3.0);*/
	FaceType* edgeF[2];
	edgeF[0]=on_edge[0];
	edgeF[1]=on_edge[1];
	ScalarType costArea=EstimateAreaByParam<BaseMesh>(pos.V(0),pos.V(1),edgeF);
	ScalarType lenght=EstimateLenghtByParam<BaseMesh>(pos.V(0),pos.V(1),edgeF);

	//ScalarType lenght=(Distance(pos.V(0)->cP(),pos.V(1)->cP()));
	//return (pow(lenght,2)+costArea*4);
	return (pow(lenght,2)+costArea);
  }
	
  inline void SetHlevMeshUV(const std::vector<FaceType*> &LowFace,
						    std::vector<FaceType*> &HiFace,
							std::vector<VertexType*> &HiVertex)
  {

	///interpolate parametric u & v values into the face
	for (unsigned int index=0;index<LowFace.size();index++)
	{
		FaceType *test_face=LowFace[index];
		assert(!test_face->IsD());
		for (unsigned int i=0;i<test_face->vertices_bary.size();i++)
		{
			VertexType *brother=test_face->vertices_bary[i].first;
			CoordType bary=test_face->vertices_bary[i].second;
			GetUV<TriMeshType>(test_face,bary,brother->T().U(),brother->T().V());
			//printf("%f , %f \n",brother->T().U(),brother->T().V());
			assert(brother!=NULL);
			HiVertex.push_back(brother);
		}
	}

	///add brother of the domain mesh
	std::vector<VertexType*> LowVertices;
	getSharedVertex<BaseMesh>(LowFace,LowVertices);
	for (unsigned int index=0;index<LowVertices.size();index++)
	{
		assert(!LowVertices[index]->IsD());
		if (LowVertices[index]->brother!=NULL)
		{
			VertexType* b=LowVertices[index]->brother;
			assert(b!=NULL);
			HiVertex.push_back(b);
			b->T().P()=LowVertices[index]->T().P();
		}
	}

	///then return pointer to hight level faces
	getSharedFace<BaseMesh>(HiVertex,HiFace);
  }

  static void  SetBaryFromUV(BaseMesh &domain,
					 std::vector<VertexType*> &vertices)
  {
    ///set a vector of pointer to face
	std::vector<FaceType*> OrdFace;
	for (unsigned int h=0;h<domain.face.size();h++)
		OrdFace.push_back(&domain.face[h]);

	///update parametric positions of vertices
	for (unsigned int i=0;i<vertices.size();i++)
		{
			CoordType bary1;
			FaceType *chosen;
			ScalarType u=vertices[i]->T().U();
			ScalarType v=vertices[i]->T().V();
			GetBaryFaceFromUV<TriMeshType>(domain,u,v,OrdFace,bary1,chosen);
			assert(fabs(bary1.X()+bary1.Y()+bary1.Z()-1.0)<=0.0001);
			vertices[i]->father=chosen;
			vertices[i]->Bary=bary1;
		}
  }

 
  struct minInfo0
  {
  public:
	BaseMesh *domain;
	BaseMesh *collapsed;
	BaseVertex *central;
	CoordType middle;
	std::vector<BaseFace*> HiFace;
	std::vector<BaseVertex*> HiVertex;
	ScalarType original_area;
  };
  
 
  static void energy0(double *p, double *x, int/* m*/, int /*n*/, void *data)
  { 
	minInfo0 &inf = *(minInfo0 *)data; 

	///assing coordinate to the face
	inf.central->P().X()=p[0];
	inf.central->P().Y()=p[1];
	inf.central->P().Z()=p[2];

	///find aspect ratio
	x[0]=(1.0/AspectRatio<BaseMesh>(*inf.collapsed));

	ScalarType areadelta=0;
	for (unsigned int i=0;i<inf.domain->face.size();i++)
		areadelta+=inf.domain->face[i].areadelta;

	ScalarType area0=Area(*inf.collapsed);
	ScalarType area1=Area(*inf.domain)+areadelta;
	
	x[1]=pow(area0/area1+area1/area0,2);//2.0*pow((area0-area1)/area1,2);

	x[2]=AreaDispersion<BaseMesh>(*inf.collapsed);
	x[3]=0;//(inf.middle-inf.central->P()).SquaredNorm()/area1;
  }

   ///find best position
  inline CoordType FindBestPos()
  {
    minInfo0 Minf;
    ///create the submesh
    VertexType *v0=pos.V(0);
	VertexType *v1=pos.V(1);
	

	std::vector<typename BaseMesh::VertexType*> star;
	std::vector<typename BaseMesh::FaceType*> orderedFaces;
	star.push_back(v0);
	star.push_back(v1);
	BaseMesh created,domain;

	///DISTORSION
	BaseMesh HLcreated;
	///DISTORSION

	CreateMeshVertexStar<BaseMesh>(star,orderedFaces,created);
	/*if (created.face[1].areadelta>1)
		assert(0);*/
	UpdateTopologies<BaseMesh>(&created);
	
	//ParametrizeLocally(created);

	/////copy uv values on ordered faces
	//for (int i=0;i<created.face.size();i++)
	//	for (int j=0;j<3;j++)
	//		orderedFaces[i]->V(j)->T().P()=created.face[i].V(j)->T().P();

	/////set parametric position respect to new part of submesh
	///*std::vector<FaceType*> HiFace;
	//std::vector<VertexType*> HiVertex;*/
	//SetHlevMeshUV(orderedFaces,Minf.HiFace,Minf.HiVertex);

	///save previous values
	std::vector<std::pair<FaceType*,CoordType> > swap;
	std::vector<VertexType*>::iterator iteVP;
	for (iteVP=Minf.HiVertex.begin();iteVP!=Minf.HiVertex.end();iteVP++)
		swap.push_back(std::pair<FaceType*,CoordType> ((*iteVP)->father,(*iteVP)->Bary));
	///DISTORSION
	
	///create pos
	EdgeType posEdge;
	std::vector<typename FaceType::VertexType*> vertEdge;
	FindNotBorderVertices<BaseMesh>(created,vertEdge);
	posEdge.V(0)=std::max<typename FaceType::VertexType*>(vertEdge[1],vertEdge[0]);
	posEdge.V(1)=std::min<typename FaceType::VertexType*>(vertEdge[1],vertEdge[0]);

	///simulate collapse
	CoordType newPos=(v0->P()+v1->P())/2.0;

	///copy domain
	vcg::tri::Append<BaseMesh,BaseMesh>::Mesh(domain,created);
	for (unsigned int i=0;i<created.face.size();i++)
		domain.face[i].areadelta=created.face[i].areadelta;

	DoCollapse(created,posEdge, newPos); // v0 is deleted and v1 take the new position
	UpdateTopologies<BaseMesh>(&created);

	/////parametrize domain
	//ParametrizeLocally(created);
	//ParametrizeLocally(domain);
	
	///minimization
	double p[3];
	p[0]=newPos.X();
	p[1]=newPos.Y();
	p[2]=newPos.Z();

	/*minInfo Minf;*/
	Minf.domain=&domain;
	Minf.collapsed=&created;
	Minf.central=posEdge.V(1);
	Minf.middle=newPos;

	double x[4];
	x[0]=0;
	x[1]=0;
	x[2]=0;
	x[3]=0;
	/*x[4]=0;*/

	double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
	opts[0]=LM_INIT_MU; opts[1]=1E-15; opts[2]=1E-15; opts[3]=1E-20;
	opts[4]=LM_DIFF_DELTA;

	//energy0(p,x,3,4,Minf);
	
	/*int num=*/dlevmar_dif(energy0,p,x,3,4,1000,opts,info,NULL,NULL,&Minf);

	///find back the value
	
	CoordType bestPos;
	bestPos.X()=p[0];
	bestPos.Y()=p[1];
	bestPos.Z()=p[2];


	for (unsigned int i=0;i<Minf.HiVertex.size();i++)
	{
		Minf.HiVertex[i]->father=swap[i].first;
		Minf.HiVertex[i]->Bary=swap[i].second;
	}

	return (bestPos);
  }

  inline ScalarType ComputePriority()
  { 
	return (Cost());
	//return( Distance(pos.V(0)->cP(),pos.V(1)->cP()));
  }

  CoordType ComputeMinimal()
  {
	CoordType bestPos=FindBestPos();
	return bestPos;
  }

  void UpdateFF(EdgeType &posEdge)
	{
		std::vector<typename TriMeshType::FaceType*> shared;
		std::vector<typename TriMeshType::FaceType*> in_v0;
		std::vector<typename TriMeshType::FaceType*> in_v1;
		///then reupdate topology
		getSharedFace<TriMeshType>(posEdge.V(0),posEdge.V(1),shared,in_v0,in_v1);

		///find the edge shared between them
		for (unsigned int j=0;j<shared.size();j++)
		{
			int iedge=-1;
			FaceType* face=shared[j];
			if (((face->V(0)==posEdge.V(0))&&(face->V(1)==posEdge.V(1)))||
				 (face->V(0)==posEdge.V(1))&&(face->V(1)==posEdge.V(0)))
				 iedge=0;
			else
			if (((face->V(1)==posEdge.V(0))&&(face->V(2)==posEdge.V(1)))||
				 (face->V(1)==posEdge.V(1))&&(face->V(2)==posEdge.V(0)))
				 iedge=1;
			else
			if (((face->V(2)==posEdge.V(0))&&(face->V(0)==posEdge.V(1)))||
				 (face->V(2)==posEdge.V(1))&&(face->V(0)==posEdge.V(0)))
				 iedge=2;
			assert (iedge!=-1);

			///then update topology for the two other faces 
			int edge0=(iedge+1)%3;
			int edge1=(iedge+2)%3;

			///get opposEdgeite faces and indexes
			FaceType* f0=face->FFp(edge0);
			FaceType* f1=face->FFp(edge1);
			
			int indexopp0=face->FFi(edge0);
			int indexopp1=face->FFi(edge1);
			///control if they are border 
			if ((f0==face)&&(f1==face))
			{
				printf("border");
			}
			else
			if (f0==face)
			{
				f1->FFp(indexopp1)=f1;
				f1->FFi(indexopp1)=-1;
				printf("border");
			}
			else
			if (f1==face)
			{
				f0->FFp(indexopp0)=f0;
				f0->FFi(indexopp0)=-1;
				printf("border");
			}
			else///otherwise attache two other faces
			{
				///the reassing adiacency
				f0->FFp(indexopp0)=f1;
				f1->FFp(indexopp1)=f0;
				f0->FFi(indexopp0)=indexopp1;
				f1->FFi(indexopp1)=indexopp0;
				assert( f0->FFp(indexopp0)->FFp(f0->FFi(indexopp0))==f0 );
				assert( f1->FFp(indexopp1)->FFp(f1->FFi(indexopp1))==f1 );
			}
			
		}	
	}

	//struct minInfo1
	//{
	//	public:

	//	BaseVertex* to_optimize;
	//	std::vector<BaseVertex*> Hres_vert;
	//	BaseMesh *parametrized_domain;
	//	BaseMesh *base_domain;
	//	BaseMesh hres_mesh;
	//	UVGrid<BaseMesh> UVGr;
	//};
 // 
	//
	// ///energy for equilararity and equiareal minimization
	//static void energy1(float *p, float *x, int m, int n, void *data)
	//{ 
	//	
	//	const float MaxVal=10000.f;
	//	minInfo1 &inf = *(minInfo1 *)data; 
	//	
	//	///assing coordinate to central vertex
	//	inf.to_optimize->T().U()=p[0];
	//	inf.to_optimize->T().V()=p[1];
	//	
	//	///control that the parametrization is non folded
	//	std::vector<BaseFace*> folded;
	//	bool b=NonFolded<BaseMesh>(*inf.parametrized_domain,folded);
	//	if (!b)
	//	{
	//		x[0]=std::numeric_limits<float>::max();
	//		x[1]=std::numeric_limits<float>::max();
	//		return;
	//	}

	//	////set rest positions for survived vertex
	//	///get the non border one that is the one survived
	//	CoordType val,valtest;
	//	bool found0=false;
	//	bool found1;

	//	//found0=GetCoordFromUV<BaseMesh>(inf.hres_mesh,inf.to_optimize->T().U(),inf.to_optimize->T().V(),val,true);
	//	//if (inf.hres_mesh.fn>0)
	//	found0=inf.UVGr.CoordinatesPointUnique(inf.to_optimize->T().P(),val);
	//	
	//	if (!found0)
	//	  found1=GetCoordFromUV<BaseMesh>(*inf.base_domain,inf.to_optimize->T().U(),inf.to_optimize->T().V(),val,true);

	//	assert ((found0)||(found1));
	//	inf.to_optimize->RPos=val;

	//	///clear assigned vertices
	//	for (unsigned int i=0;i<inf.parametrized_domain->face.size();i++)
	//		inf.parametrized_domain->face[i].vertices_bary.resize(0);
	//	
	//	///update alphabeta from UV to calculate edge_lenght and area
	//	bool inside=true;
	//	for (unsigned int i=0;i<inf.Hres_vert.size();i++)
	//	{
	//		BaseVertex *test=inf.Hres_vert[i];
	//		ScalarType u=test->T().U();
	//		ScalarType v=test->T().V();
	//		CoordType bary;
	//		int index;
	//		inside &=GetBaryFaceFromUV(*inf.parametrized_domain,u,v,bary,index);
	//		if (!inside)///ack
	//		{
	//			x[0]=std::numeric_limits<float>::max();
	//			x[1]=std::numeric_limits<float>::max();
	//			return;
	//		}
	//		BaseFace* chosen=&inf.parametrized_domain->face[index];
	//		chosen->vertices_bary.push_back(std::pair<BaseVertex*,vcg::Point3f>(test,bary));
	//		test->father=chosen;
	//		test->Bary=bary;
	//	}
	//	
	//	
	//	/*///ack
	//	if (!inside)
	//	{
	//		x[0]=std::numeric_limits<float>::max();
	//		x[1]=std::numeric_limits<float>::max();
	//		return;
	//	}*/
	//	//assert(inside);
	//	
	//	ScalarType maxEdge=0;
	//	ScalarType minEdge=std::numeric_limits<float>::max();
	//	ScalarType maxArea=0;
	//	ScalarType minArea=std::numeric_limits<float>::max();
	//	
	//	///find minimum and maximum of estimated area
	//	for (unsigned int i=0;i<inf.parametrized_domain->face.size();i++)
	//	{
	//		ScalarType area=EstimateAreaByParam<BaseFace>(&inf.parametrized_domain->face[i]);
	//		if (area<minArea)
	//			minArea=area;
	//		if (area>maxArea)
	//			maxArea=area;
	//	}

	//	///find minimum and maximum of edges
	//	for (unsigned int i=0;i<inf.parametrized_domain->vert.size();i++)
	//	{
	//		BaseVertex *v0=&inf.parametrized_domain->vert[i];
	//		BaseVertex *v1=inf.to_optimize;
	//		if (v0!=v1)
	//		{
	//			std::vector<typename BaseMesh::FaceType*> on_edge,faces1,faces2;
	//			getSharedFace<BaseMesh>(v0,v1,on_edge,faces1,faces2);
	//			BaseFace* edgeF[2];
	//			edgeF[0]=on_edge[0];
	//			edgeF[1]=on_edge[1];
	//			ScalarType lenght=EstimateLenghtByParam<BaseMesh>(v0,v1,edgeF);
	//			if (lenght<minEdge)
	//				minEdge=lenght;
	//			if (lenght>maxEdge)
	//				maxEdge=lenght;
	//		}
	//	}
	//	//if ((minArea<=0)||(minEdge<=0))
	//	
	//	assert(minArea>0);
	//	assert(minEdge>0);
	//	x[0]=(maxArea/minArea);
	//	x[1]=pow(maxEdge/minEdge,2);
	//	
	//	/*if (x[0]>MaxVal)
	//		x[0]=MaxVal;
	//	if (x[1]>MaxVal)
	//		x[1]=MaxVal;*/
	//	/*x[0]=(maxArea-minArea);
	//	x[1]=pow(maxEdge-minEdge,2);*/
	//}

	//void  OptimizeUV(BaseMesh *base_domain,
	//				BaseMesh *new_domain,
	//				const std::vector<VertexType*> &Hres_vert)
	//{
	//	minInfo1 Minf;
	//	//BaseMesh hres_mesh;
	//	///setting parameters for minimization
	//	Minf.base_domain=base_domain;
	//	Minf.parametrized_domain=new_domain;
	//	Minf.Hres_vert=std::vector<VertexType*>(Hres_vert.begin(),Hres_vert.end());
	//	///new
	//	std::vector<VertexType*> OrderedVertices;
	//	std::vector<FaceType*> OrderedFaces;
	//	CopyMeshFromVertices<BaseMesh>(Hres_vert,OrderedVertices,OrderedFaces,Minf.hres_mesh);
	//	///initialize grid
	//	int grid_size=(int)sqrt((ScalarType)Minf.hres_mesh.face.size());
	//	if (grid_size<5)
	//		grid_size=5;
	//	if (Minf.hres_mesh.fn>0)
	//	{
	//		Minf.UVGr.Init(Minf.hres_mesh,grid_size);
	//		/*printf("box %f \n",Minf.UVGr.bbox2.DimX());*/
	//	}
	//	///end new

	//	///get the vertex to optimize position
	//	int i=0;
	//	while (new_domain->vert[i].IsB()) i++;
	//	Minf.to_optimize=&new_domain->vert[i];
	//	
	//	///texture value of central vertex
	//	///that should be optimized
	//	float *p=new float [2];
	//	p[0]=0;
	//	p[1]=0;

	//	///allocate vector of output
	//	float *x=new float [2];
	//	x[0]=0;
	//	x[1]=0;
	//	
	//	float opts[LM_OPTS_SZ], info[LM_INFO_SZ];
	//	opts[0]=LM_INIT_MU; opts[1]=1E-15; opts[2]=1E-15; opts[3]=1E-20;
	//	opts[4]=LM_DIFF_DELTA;
	//	
	//	/*energy1(p,x,2,2,&Minf);*/
	//	
	//		
	//	int num=slevmar_dif(energy1,p,x,2,2,1000,opts,info,NULL,NULL,&Minf);
	//	
	//		
	//	//printf("%d \n",num);
	//	/*if (num<0) 
	//	{
	//		printf("fail levelmark %d \n",info[6]);
	//	}*/
	//	///copy back values
	//	Minf.to_optimize->T().U()=p[0];
	//	Minf.to_optimize->T().V()=p[1];

	//	delete(x);
	//	delete(p);
	//	
	//}	


///create a copy the submesh for a collapse and parameterize it
void CreatePreCollapseSubmesh(EdgeType &pos,
							  BaseMesh &param,
							  std::vector<VertexType*> &orderedVertex,
							  std::vector<FaceType*> &orderedFaces)
{
	std::vector<VertexType*> vert_star;
	vert_star.push_back(pos.V(0));
	vert_star.push_back(pos.V(1));

	///get a copy of the mesh
	CreateMeshVertexStar(vert_star,orderedFaces,orderedVertex,param);
	UpdateTopologies(&param);
	InitDampRestUV(param);

	ParametrizeLocally<BaseMesh>(param);

	///store UV coordinates in original vertex to copy 
	///for post collapsed surface parametrization
	for (unsigned int i=0;i<orderedVertex.size();i++)
		orderedVertex[i]->T().P()=param.vert[i].T().P();
}

///create a copy the submesh after the collapse that is already parameterized
/// only the central vertex has to be set to (0,0)
void CreatePostCollapseSubmesh(EdgeType &pos,
							   BaseMesh &param_post,
							  std::vector<VertexType*> &orderedVertex,
							  std::vector<FaceType*> &orderedFaces)
{
	std::vector<VertexType*> vert_star;
	vert_star.push_back(pos.V(1));

	CreateMeshVertexStar(vert_star,orderedFaces,orderedVertex,param_post);
	UpdateTopologies(&param_post);
	InitDampRestUV(param_post);

	///set to zero the star center (non border)
	bool found=false;
	unsigned int i=0;
	while ((i<param_post.vert.size())&&(!found))
	{
		if (!param_post.vert[i].IsB())
			found=true;
		else
			i++;
	}
	assert(found);
	param_post.vert[i].T().P()=vcg::Point2<ScalarType>(0,0);
		
}

void AphaBetaToUV(EdgeType &pos,
				  std::vector<FaceType*> &orderedFaces,
				  BaseMesh &param,
			      std::vector<VertexType*> &HresVert)
{
	/*const ScalarType eps=(ScalarType)0.00001;*/
	///VERTEX ON FACE REPROJECTING
	//transform from Alpha Beta to UV
	for (unsigned int index=0;index<orderedFaces.size();index++)
	{
		FaceType *test_face=orderedFaces[index];
		FaceType *parametric_face=&param.face[index];

		///fro each vertex belonging to such face
		for (unsigned int i=0;i<test_face->vertices_bary.size();i++)
		{
			///get brother vertex
			VertexType *brother=test_face->vertices_bary[i].first;
			assert(brother!=NULL);

			///get his barycentric coordinates
			CoordType bary=test_face->vertices_bary[i].second;

			///transform to UV
			ScalarType u,v;
			GetUV<BaseMesh>(parametric_face,bary,u,v);
			///and assing
			brother->T().U()=u;
			brother->T().V()=v;

			/*///testing coordinates
			if (!((u>-1.0-eps)&&(u<1.0+eps)&&
			   (v>-1.0-eps)&&(v<1.0+eps)))
			{
				printf("Error 0 Uv :%f,%f \n",u,v);
				printf("Bary :%f,%f %f \n",bary.X(),bary.Y(),bary.Z());
				system("pause");
			}*/

			///save high res vertices
			HresVert.push_back(brother);
		}
		/*///clear 
		test_face->vertices_bary.clear();*/
	}
	
	///add the two collapsed vertex
	///and set UV coordinates
	for (int i=0;i<2;i++)
	{ 
		if (pos.V(i)->brother!=NULL)
		{
			HresVert.push_back(pos.V(i)->brother);
			///set his coordinates 
			pos.V(i)->brother->T().P()=pos.V(i)->T().P();

			//param_pos.push_back(vert_star0[i]->T().P());
			///color for flip test

			///set his brother as null
			pos.V(i)->brother=NULL;
		}
	}

}

void UVToAlphaBeta(std::vector<VertexType*> &HresVert,
				   BaseMesh &param,std::vector<FaceType*> &orderedFaces)
{
	///for each parametrized vertex
	for (unsigned int i=0;i<HresVert.size();i++)
	{
		///get his brother
		CoordType bary1;
		FaceType *chosen;
		VertexType *brother=HresVert[i];
		assert(brother!=NULL);
		///and its barycentric coords
		ScalarType u=HresVert[i]->T().U();
		ScalarType v=HresVert[i]->T().V();
		int index;

		////found the face and its barycentric coordinates form the parametrized mesh
		bool found=GetBaryFaceFromUV(param,u,v,bary1,index);

		///test for approximation errors
		if (!found)
		{
			printf("Error 1\n");
			printf("Old Uv :%f,%f \n",u,v);
			while (!found)
			{
				///put next to the center
				u*=(ScalarType)0.9;
				v*=(ScalarType)0.9;
				found=GetBaryFaceFromUV(param,u,v,bary1,index);//<BaseMesh>(param1,u,v,orderedFaces1,bary1,chosen);
			}
			printf("New Uv %f,%f \n",u,v);
		}
		
		///get the face from original mesh
		assert(found);
		chosen=orderedFaces[index];
		//if (!(fabs(bary1.X()+bary1.Y()+bary1.Z()-1.0)<=0.0001))
		//{
		//	printf("Error 2 Uv :%f,%f \n",u,v);
		//	system("pause");
		//	bary1=CoordType(0.3333,0.3333,0.3333);
		//	chosen=orderedFaces[0];
		//	//assert(0);
		//}
		///set father-son relation
		chosen->vertices_bary.push_back(std::pair<BaseVertex*,vcg::Point3f>(brother,bary1));
		brother->father=chosen;
		brother->Bary=bary1;

		///set new parametrization value
		GetUV<BaseMesh>(&param.face[index],bary1,u,v);
		HresVert[i]->T().U()=u;
		HresVert[i]->T().V()=v;
	}
}

void ClearVert_Bary(std::vector<FaceType*> &orderedFaces)
{
	for (unsigned int index=0;index<orderedFaces.size();index++)
	{
		FaceType *test_face=orderedFaces[index];
		test_face->vertices_bary.clear();
	}
}

void AssignRPos(VertexType* &to_assign,
				ScalarType &U,
				ScalarType &V,
				std::vector<FaceType*> &orderedFaces,
				BaseMesh &param)
{
	CoordType val;

	////set rest positions for survived vertex
	///create h resolution mesh 
	BaseMesh hlev_mesh;
	std::vector<BaseVertex*> ord_vertex;
	CopyHlevMesh(orderedFaces,hlev_mesh,ord_vertex);
	///first interpolate in h_res then proceed to lowres if doesn't find
	bool found;
	found=GetCoordFromUV(hlev_mesh,U,V,val,true);   
	if (!found)
		found=GetCoordFromUV(param,U,V,val,true);   
	
	assert (found);
	
	to_assign->RPos=val;
}

void Execute(BaseMesh &m)
	{	
		typedef BaseMesh::FaceType FaceType;
		typedef BaseMesh::VertexType VertexType;
		typedef BaseMesh::ScalarType ScalarType;
		typedef BaseMesh::CoordType CoordType;
		
		///compute new position
		CoordType newPos;
		newPos=ComputeMinimal();//(pos.V(0)->RPos+pos.V(1)->RPos)/2.0;//
		

		BaseMesh param0,param1;
		//std::vector<VertexType*> vert_star0,vert_star1;
		std::vector<FaceType*> orderedFaces0,orderedFaces1;
		std::vector<VertexType*> orderedVertex0,orderedVertex1;
	
		///create a parametrized submesh pre-collapse
		CreatePreCollapseSubmesh(pos,param0,orderedVertex0,orderedFaces0);

//---------------------------///
		///update FF topology post-collapse
		UpdateFF(this->pos);
		
		///INITIAL AREA
		ScalarType area0=Area<BaseFace>(orderedFaces0);
		
		///do the collapse
		DoCollapse(m, this->pos, newPos); // v0 is deleted and v1 take the new position
		
		//---------------------------///
		///create a parametrized submesh post-collapse #1
		CreatePostCollapseSubmesh(pos,param1,orderedVertex1,orderedFaces1);

		//---------------------------///
		///FINAL AREA
		ScalarType area1=Area<BaseFace>(orderedFaces1);
		
		////save error
		ScalarType areadelta=(area0-area1)/(ScalarType)orderedFaces1.size();
		for (unsigned int i=0;i<orderedFaces1.size();i++)
			orderedFaces1[i]->areadelta=areadelta;
		
		///VERTEX ON FACE REPROJECTING #2
		//---------------------------///
		/////collect all vertices with respective param coords
		std::vector<VertexType*> HresVert;
	
		//TRANSFORM TO UV
		AphaBetaToUV(pos,orderedFaces0,param0,HresVert);

		//DELETE SONS
		ClearVert_Bary(orderedFaces0);
//---------------------------///
		
		///UPTIMIZE UV
		//OptimizeUV(&param0,&param1,HresVert);
		
//---------------------------///
		///REPROJECT BACK TO ORIGINAL FATHER #3
		UVToAlphaBeta(HresVert,param1,orderedFaces1);

		///UPTIMIZE UV
		PatchesOptimizer<BaseMesh>::OptimizeUV(pos.V(1));

//---------------------------///
		///get the non border one that is the one survived
		unsigned int k=0;
		while ((param1.vert[k].IsB())&&(k<param1.vert.size()))
			k++;
		assert(k<param1.vert.size());
		CoordType val;

//---------------------------///
		///ASSIGN REST POSITION CENTRAL VERTEX
		//AssignRPos(pos.V(1),param1.vert[k].T().U(),param1.vert[k].T().V(),orderedFaces1,param0);
//---------------------------///

		///FINAL OPTIMIZATION
		/*int t0=clock();*/
		SmartOptimizeStar<BaseMesh>(pos.V(1),Accuracy());
		/*int t1=clock();
		time_opt+=(t1-t0);*/
	}

public:
	static int &Accuracy()
	{
		static int _acc;
		return _acc;
	}

	BaseVertex *getV(int num)
	{
		assert((num>=0)&&(num<2));
		return this->pos.V(num);
	}	
};

#endif