#ifndef PARAM_COLLAPSE
#define PARAM_COLLAPSE

// local optimization
#include <vcg/complex/local_optimization.h>
#include <vcg/math/quadric.h>
#include <vcg/complex/local_optimization/tri_edge_collapse_quadric.h>
#include <vcg/complex/local_optimization/tri_edge_flip.h>
#include <set>
#include <vcg/complex/trimesh/append.h>

#include <local_parametrization.h>
#include <mesh_operators.h>
#include <vcg/space/color4.h>
#include <lm.h>
#include <uv_grid.h>
#include "opt_patch.h"
#include "local_optimization.h"

template <class BaseMesh>
class ParamEdgeCollapse: public vcg::tri::TriEdgeCollapse<BaseMesh,ParamEdgeCollapse<BaseMesh> > {
public:
        typedef vcg::tri::TriEdgeCollapse<BaseMesh,ParamEdgeCollapse<BaseMesh> > Super;
	typedef typename BaseMesh::VertexType::EdgeType EdgeType;
        typedef typename BaseMesh::VertexType VertexType;
	typedef typename BaseMesh::VertexType BaseVertex;
	typedef typename BaseMesh::FaceType   BaseFace;
        typedef typename BaseMesh::FaceType FaceType;
        typedef typename BaseMesh::ScalarType   ScalarType;
        typedef typename BaseMesh::CoordType   CoordType;
        typedef BaseMesh TriMeshType;

	static EnergyType &EType(){static EnergyType E;return E;};
	
	inline ParamEdgeCollapse(const EdgeType &p, int mark)
	{    
        Super::localMark = mark;
        Super::pos=p;
        Super::_priority = ComputePriority();
	}

	inline ScalarType Cost()
  {
	std::vector<typename BaseMesh::FaceType*> on_edge,faces1,faces2;
        getSharedFace<BaseMesh>(Super::pos.V(0),Super::pos.V(1),on_edge,faces1,faces2);

	FaceType* edgeF[2];
	edgeF[0]=on_edge[0];
	edgeF[1]=on_edge[1];
    ScalarType costArea=EstimateAreaByParam<BaseMesh>(Super::pos.V(0),Super::pos.V(1),edgeF);
    ScalarType lenght=EstimateLenghtByParam<BaseMesh>(Super::pos.V(0),Super::pos.V(1),edgeF);

	if (costArea<0)
		assert(0);
	assert(lenght>=0);
	return (pow(lenght,2)+costArea);
  }
	
  inline void SetHlevMeshUV(const std::vector<BaseFace*> &LowFace,
                                                    std::vector<BaseFace*> &HiFace,
                                                        std::vector<BaseVertex*> &HiVertex)
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
                        GetUV<BaseMesh>(test_face,bary,brother->T().U(),brother->T().V());
			//printf("%f , %f \n",brother->T().U(),brother->T().V());
			assert(brother!=NULL);
			HiVertex.push_back(brother);
		}
	}

	///add brother of the domain mesh
        std::vector<BaseVertex*> LowVertices;
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
														 std::vector<BaseVertex*> &vertices)
  {
    ///set a vector of pointer to face
        std::vector<BaseFace*> OrdFace;
	for (unsigned int h=0;h<domain.face.size();h++)
		OrdFace.push_back(&domain.face[h]);

	///update parametric positions of vertices
	for (unsigned int i=0;i<vertices.size();i++)
		{
			CoordType bary1;
			FaceType *chosen;
			ScalarType u=vertices[i]->T().U();
			ScalarType v=vertices[i]->T().V();
      GetBaryFaceFromUV<BaseMesh>(domain,u,v,OrdFace,bary1,chosen);
			assert(fabs(bary1.X()+bary1.Y()+bary1.Z()-1.0)<=0.0001);
			/*vertices[i]->father=chosen;
			assert(!chosen->IsD());
			vertices[i]->Bary=bary1;*/
			AssingFather(vertices[i],chosen,bary1,domain);
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
  inline CoordType FindBestPos(BaseMesh &m)
  {
    minInfo0 Minf;
    ///create the submesh
    VertexType *v0=Super::pos.V(0);
        VertexType *v1=Super::pos.V(1);
	

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
        typename std::vector<VertexType*>::iterator iteVP;
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
	{/*
		Minf.HiVertex[i]->father=swap[i].first;
		assert(!swap[i].first->IsD());
		Minf.HiVertex[i]->Bary=swap[i].second;*/
		AssingFather(*Minf.HiVertex[i],swap[i].first,swap[i].second,m);
	}

	return (bestPos);
  }

  inline ScalarType ComputePriority()
  { 
	return (Cost());
	//return( Distance(pos.V(0)->cP(),pos.V(1)->cP()));
  }

  CoordType ComputeMinimal(BaseMesh &m)
  {
	CoordType bestPos=FindBestPos(m);
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
									  BaseMesh &param,std::vector<FaceType*> &orderedFaces,
										BaseMesh &base_mesh)
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
		
		AssingFather(*brother,chosen,bary1,base_mesh);
		/*brother->father=chosen;
		assert(!chosen->IsD());
		brother->Bary=bary1;*/

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
    typedef typename BaseMesh::FaceType FaceType;
    typedef typename BaseMesh::VertexType VertexType;
    typedef typename BaseMesh::ScalarType ScalarType;
    typedef typename BaseMesh::CoordType CoordType;
		
		assert(this->pos.V(0)!=this->pos.V(1));
		assert(!this->pos.V(0)->IsD());
		assert(!this->pos.V(1)->IsD());
    assert(size_t((this->pos.V(0)-&(*m.vert.begin())))<m.vert.size());
    assert(size_t((this->pos.V(1)-&(*m.vert.begin())))<m.vert.size());
				
    std::vector<FaceType*> result;
    std::vector<FaceType*> in_v0;
    std::vector<FaceType*> in_v1;

#ifndef NDEBUG
		getSharedFace<BaseMesh>(this->pos.V(0),this->pos.V(1),result,in_v0,in_v1);
		assert(result.size()==2);
#endif

		///compute new position
		CoordType oldRPos=(this->pos.V(0)->RPos+this->pos.V(1)->RPos)/2.0;
		CoordType newPos;
		newPos=ComputeMinimal(m);//
		//vcg::tri::UpdateTopology<BaseMesh>::TestVertexFace(m); ///TEST

		BaseMesh param0,param1;
		//std::vector<VertexType*> vert_star0,vert_star1;
		std::vector<FaceType*> orderedFaces0,orderedFaces1;
		std::vector<VertexType*> orderedVertex0,orderedVertex1;
	
		///create a parametrized submesh pre-collapse
    CreatePreCollapseSubmesh(Super::pos,param0,orderedVertex0,orderedFaces0);

//---------------------------///
		///update FF topology post-collapse
		UpdateFF(this->pos);
		
		///INITIAL AREA
		ScalarType area0=Area<BaseFace>(orderedFaces0);

		///do the collapse
		DoCollapse(m, this->pos, newPos); // v0 is deleted and v1 take the new position
		//vcg::tri::UpdateTopology<BaseMesh>::TestVertexFace(m); ///TEST
		//---------------------------///
		///create a parametrized submesh post-collapse #1
    CreatePostCollapseSubmesh(this->pos,param1,orderedVertex1,orderedFaces1);

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
    AphaBetaToUV(this->pos,orderedFaces0,param0,HresVert);
		
		//DELETE SONS
		ClearVert_Bary(orderedFaces0);
//---------------------------///
		
		///UPTIMIZE UV
		//OptimizeUV(&param0,&param1,HresVert);
		
//---------------------------///
		///REPROJECT BACK TO ORIGINAL FATHER #3
		
		UVToAlphaBeta(HresVert,param1,orderedFaces1,m);

		
        /*PatchesOptimizer<BaseMesh>::OptimizeUV(this->pos.V(1),m);*/
	
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
		
		this->pos.V(1)->RPos=oldRPos;
		
    /*bool b=*/SmartOptimizeStar<BaseMesh>(this->pos.V(1),m,Accuracy(),EType());
		
		/*int t1=clock();
		time_opt+=(t1-t0);*/
	}

public:
	static int &Accuracy()
	{
		static int _acc;
		return _acc;
	}
	
	static BaseMesh* &HresMesh()
	{
		static BaseMesh* mesh;
		return mesh;
	}

	BaseVertex *getV(int num)
	{
		assert((num>=0)&&(num<2));
		return this->pos.V(num);
	}	
};

#endif
