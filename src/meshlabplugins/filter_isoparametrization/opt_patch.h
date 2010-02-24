#include <algorithm>
#include <vcg/container/simple_temporary_data.h>

#ifndef _OPT_PATCHES
#define _OPT_PATCHES

template <class MeshType>
class PatchesOptimizer
{
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;

public:
	typedef struct minInfoUV
	{
	public:

		VertexType* to_optimize;
		std::vector<VertexType*> Hres_vert;
		MeshType *parametrized_domain;
		MeshType *base_domain;
		MeshType hres_mesh;
	};

	ScalarType averageArea;
	ScalarType averageLenght;
	MeshType &base_mesh;
	MeshType &final_mesh;
	int global_mark;
	vcg::SimpleTempData<typename MeshType::VertContainer,int> markers;

	///energy for equilararity and equiareal minimization
	static void Equi_energy(float *p, float *x, int /*m*/, int/* n*/, void *data)
	{ 

		/*const float MaxVal=10000.f;*/
		minInfoUV &inf = *(minInfoUV *)data; 

		///assing coordinate to central vertex
		inf.to_optimize->T().U()=p[0];
		inf.to_optimize->T().V()=p[1];

		///control that the parametrization is non folded
		std::vector<FaceType*> folded;
		bool b=NonFolded<MeshType>(*inf.parametrized_domain,folded);
		if (!b)
		{
			x[0]=std::numeric_limits<float>::max();
			x[1]=std::numeric_limits<float>::max();
			return;
		}

		////set rest positions for survived vertex
		///get the non border one that is the one survived
		CoordType val;
		bool found0,found1;

		///update 3d position of central vertex
		found0=GetCoordFromUV<MeshType>(inf.hres_mesh,inf.to_optimize->T().U(),inf.to_optimize->T().V(),val,true);
		if (!found0)
			found1=GetCoordFromUV<MeshType>(*inf.parametrized_domain,inf.to_optimize->T().U(),inf.to_optimize->T().V(),val,true);

		//assert ((found0)||(found1));
		if ((found0)||(found1))
			inf.to_optimize->RPos=val;

		///clear assigned vertices
		for (unsigned int i=0;i<inf.parametrized_domain->face.size();i++)
			inf.parametrized_domain->face[i].vertices_bary.resize(0);

		///update alphabeta from UV to calculate edge_lenght and area
		bool inside=true;
		for (unsigned int i=0;i<inf.Hres_vert.size();i++)
		{
			VertexType *test=inf.Hres_vert[i];
			ScalarType u=test->T().U();
			ScalarType v=test->T().V();
			CoordType bary;
			int index;
			inside &=GetBaryFaceFromUV(*inf.parametrized_domain,u,v,bary,index);
			FaceType* chosen;
			if (!inside)///ack
			{
				chosen=test->father;
				bary=test->Bary;
			}
			else
			{
				chosen=&inf.parametrized_domain->face[index];
			}
			chosen->vertices_bary.push_back(std::pair<VertexType*,vcg::Point3f>(test,bary));
			test->father=chosen;
			assert(!chosen->IsD());
			test->Bary=bary;
		}

		if (!inside)///ack
		{
			x[0]=std::numeric_limits<float>::max();
			x[1]=std::numeric_limits<float>::max();
			return;
		}

		ScalarType maxEdge=0;
		ScalarType minEdge=std::numeric_limits<float>::max();
		ScalarType maxArea=0;
		ScalarType minArea=std::numeric_limits<float>::max();

		///find minimum and maximum of estimated area
		for (unsigned int i=0;i<inf.parametrized_domain->face.size();i++)
		{
			ScalarType area=EstimateAreaByParam<FaceType>(&inf.parametrized_domain->face[i]);
			if (area<minArea)
				minArea=area;
			if (area>maxArea)
				maxArea=area;
		}

		///find minimum and maximum of edges
		for (unsigned int i=0;i<inf.parametrized_domain->vert.size();i++)
		{
			VertexType *v0=&inf.parametrized_domain->vert[i];
			VertexType *v1=inf.to_optimize;
			if (v0!=v1)
			{
				std::vector<typename MeshType::FaceType*> on_edge,faces1,faces2;
				getSharedFace<MeshType>(v0,v1,on_edge,faces1,faces2);
				FaceType* edgeF[2];
				edgeF[0]=on_edge[0];
				edgeF[1]=on_edge[1];
				ScalarType lenght=EstimateLenghtByParam<MeshType>(v0,v1,edgeF);
				if (lenght<minEdge)
					minEdge=lenght;
				if (lenght>maxEdge)
					maxEdge=lenght;
			}
		}
		//if ((minArea<=0)||(minEdge<=0))

		/*assert(minArea>0);
		assert(minEdge>0);*/
		if (minArea==0)
			minArea=(ScalarType)0.00001;
		if (minEdge==0)
			minEdge=(ScalarType)0.00001;

		x[0]=((maxArea/minArea)*(ScalarType)2.0);
		x[1]=pow(maxEdge/minEdge,(ScalarType)2.0);

	}

	static ScalarType LengthPath(VertexType *v0,VertexType *v1)
	{
		std::vector<FaceType*> on_edge,faces1,faces2;
		getSharedFace<MeshType>(v0,v1,on_edge,faces1,faces2);
		FaceType* edgeF[2];
		edgeF[0]=on_edge[0];
		edgeF[1]=on_edge[1];
		ScalarType lenght=EstimateLenghtByParam<FaceType>(v0,v1,edgeF);
		return lenght;
	}

	/////return the priority of vertex processing
	//ScalarType Priority(VertexType *v)
	//{
	//	std::vector<typename MeshType::VertexType*> star;
	//	getVertexStar<MeshType>(v,star);
	//	ScalarType prior=0;
	//	for (int i=0;i<star.size();i++)
	//	{
	//		VertexType *v1=star[i];
	//		ScalarType lenght=LengthPath(v,v1);//EstimateLenghtByParam<FaceType>(v0,v1,edgeF);
	//		prior+=pow((lenght-averageLenght),(ScalarType)2);
	//	}
	//	std::vector<VertexType*> vertices;
	//	std::vector<FaceType*> faces;
	//	vertices.push_back(v);
	//	getSharedFace<MeshType>(vertices,faces);
	//	for (int i=0;i<faces.size();i++)
	//		prior+=pow((EstimateAreaByParam<FaceType>(faces[i])-averageArea),2);

	//	return prior;
	//}
	
	///return the priority of vertex processing
	ScalarType Priority(VertexType *v)
	{
		std::vector<typename MeshType::VertexType*> star;
		getVertexStar<MeshType>(v,star);
		ScalarType priorL=0,priorA=0;
		std::vector<ScalarType> Lenghts,Areas;
		Lenghts.resize(star.size());
	
		std::vector<VertexType*> vertices;
		std::vector<FaceType*> faces;
		vertices.push_back(v);
		getSharedFace<MeshType>(vertices,faces);
		Areas.resize(faces.size());

		ScalarType aveL=0;
		ScalarType aveA=0;
		for (unsigned int i=0;i<star.size();i++)
		{
			VertexType *v1=star[i];
			ScalarType lenght=LengthPath(v,v1);//EstimateLenghtByParam<FaceType>(v0,v1,edgeF);
			Lenghts[i]=lenght;
			aveL+=lenght;
		}
		aveL/=(ScalarType)star.size();

		for (unsigned int i=0;i<faces.size();i++)
		{
			Areas[i]=EstimateAreaByParam<FaceType>(faces[i]);
			aveA+=Areas[i];
		}
		aveA/=(ScalarType)faces.size();

		for (unsigned int i=0;i<Lenghts.size();i++)
			priorL+=pow((Lenghts[i]-aveL),(ScalarType)2);

		for (unsigned int i=0;i<Areas.size();i++)
			priorA+=pow((Areas[i]-aveA),(ScalarType)2);
		
		return (pow(priorL,(ScalarType)2)/2.0+priorA);
	}

	static void FindVarianceLenghtArea(MeshType &base_mesh,
		const ScalarType &averageLenght,
		const ScalarType &averageArea,
		ScalarType &varianceL,
		ScalarType &varianceA)
	{
                typename MeshType::FaceIterator Fi;
		varianceA=0;
		varianceL=0;
		int num_edge=0;
		for (Fi=base_mesh.face.begin();Fi!=base_mesh.face.end();Fi++)
		{
			ScalarType area=EstimateAreaByParam<FaceType>(&(*Fi));
			varianceA+=pow((area-averageArea),(ScalarType)2.0);
			for (int i=0;i<3;i++)
			{
				VertexType *v0=(*Fi).V(i);
				VertexType *v1=(*Fi).V((i+1)%3);
				/*	std::vector<FaceType*> on_edge,faces1,faces2;
				getSharedFace<MeshType>(v0,v1,on_edge,faces1,faces2);
				FaceType* edgeF[2];
				edgeF[0]=on_edge[0];
				edgeF[1]=on_edge[1];*/
				if (v0>v1)
				{
					ScalarType lenght=LengthPath(v0,v1);//EstimateLenghtByParam<FaceType>(v0,v1,edgeF);
					varianceL+=pow((lenght-averageLenght),(ScalarType)2);
					num_edge++;
				}
			}
		}
		varianceL=sqrt(varianceL/(ScalarType)num_edge);
		varianceA=sqrt(varianceA/(ScalarType)base_mesh.fn);
	}

	///optimize UV of central vertex
   static void OptimizeUV(VertexType *center,MeshType &base_domain)
	{
		///parametrize base domain star and subvertices
		ParametrizeStarEquilateral<MeshType>(center,true);

		///get incident faces
		std::vector<FaceType*> faces;
		std::vector<VertexType*> vertices;
		vertices.push_back(center);
		getSharedFace<MeshType>(vertices,faces);
		MeshType domain;

		///get Hres Vertices
		std::vector<VertexType*> Hres_vert;
    getHresVertex<typename MeshType::FaceType>(faces,Hres_vert);

		///make a copy of base mesh
		std::vector<FaceType*> ordFaces;
		CreateMeshVertexStar<MeshType>(vertices,ordFaces,domain);
		assert(ordFaces.size()==domain.face.size());
		assert(ordFaces.size()==faces.size());
	/*	assert(Test(ordFaces,faces));
		assert(Test1(ordFaces,domain.face));
		assert(Test1(faces,domain.face));*/
		/*assert(Test2(domain,Hres_vert.size()));*/

		UpdateTopologies<MeshType>(&domain);

		///minimization
		minInfoUV Minf;
		///setting parameters for minimization
		//Minf.base_domain=base_domain;
		Minf.parametrized_domain=&domain;
		//Minf.base_domain=&base_mesh;
		Minf.Hres_vert=std::vector<VertexType*>(Hres_vert.begin(),Hres_vert.end());

		///create a copy of hres mesh
		std::vector<VertexType*> OrderedVertices;
		std::vector<FaceType*> OrderedFaces;
		CopyMeshFromVertices<MeshType>(Hres_vert,OrderedVertices,OrderedFaces,Minf.hres_mesh);

		///get the vertex to optimize position
		int i=0;
		while (domain.vert[i].IsB()) i++;
		Minf.to_optimize=&domain.vert[i];

		///texture value of central vertex
		///that should be optimized
		float *p=new float [2];
		p[0]=0;
		p[1]=0;

		///allocate vector of output
		float *x=new float [2];
		x[0]=0;
		x[1]=0;

		float opts[LM_OPTS_SZ], info[LM_INFO_SZ];
		opts[0]=(float)LM_INIT_MU; 
		opts[1]=(float)1E-15; 
		opts[2]=(float)1E-15; 
		opts[3]=(float)1E-20;
		opts[4]=(float)LM_DIFF_DELTA;

		/*int num=*/slevmar_dif(Equi_energy,p,x,2,2,1000,opts,info,NULL,NULL,&Minf);
		
		
		///copy back values

		//clear old values 
		for (unsigned int i=0;i<ordFaces.size();i++)
			ordFaces[i]->vertices_bary.resize(0);

		
		//reassing
		int num=0;
		for (unsigned int i=0;i<domain.face.size();i++)
		{
			for (unsigned int j=0;j<domain.face[i].vertices_bary.size();j++)
			{
				VertexType *vert=domain.face[i].vertices_bary[j].first;
				CoordType bary=domain.face[i].vertices_bary[j].second;
				ordFaces[i]->vertices_bary.push_back(std::pair<VertexType*,vcg::Point3f>(vert,bary));
				/*vert->father=ordFaces[i];
				assert(!ordFaces[i]->IsD());
				vert->Bary=bary;*/
				AssingFather(*vert,ordFaces[i],bary,base_domain);
				num++;
			}
		}
		//assert(num==Minf.Hres_vert.size());
    if (size_t(num)!=Minf.Hres_vert.size())
		{	
			printf("num0 %d \n",num);
      printf("num1 %d \n",int(Minf.Hres_vert.size()));
		}

		center->RPos=Minf.to_optimize->RPos;
		delete(x);
		delete(p);
	}

	typedef struct Elem
	{
	public:
		VertexType *center;
		ScalarType priority;
		int t_mark;

		//bool operator <(Elem* e){return (priority<e.priority);}
		inline const bool operator <(const Elem& e) const 
		{ 
			return (priority<e.priority);
			//return (locModPtr->Priority() < h.locModPtr->Priority());
		}

		Elem(VertexType *_center,ScalarType _priority,int _t_mark)
		{
			center=_center;
			priority=_priority;
			t_mark=_t_mark;
		}
	};
	std::vector<Elem> Operations;


  void Execute(VertexType *center)
	{
		OptimizeUV(center,base_mesh);
		std::vector<typename MeshType::VertexType*> neigh;
		getVertexStar<MeshType>(center,neigh);

		//push back neighbors and update mark
		global_mark++;
		///update mark neighbors
		for(unsigned int i=0;i<neigh.size();i++)
			markers[neigh[i]]=global_mark;

		///push_back neighbors to the heap
		for(unsigned int i=0;i<neigh.size();i++)
		{
			Operations.push_back(Elem(neigh[i],Priority(neigh[i]),global_mark));
			std::push_heap( Operations.begin(), Operations.end());
		}
	}



void OptimizePatches()
	{
		global_mark=0;
		markers.Init(global_mark);

		Operations.clear();

		const ScalarType sqrtsrt3=(ScalarType)1.31607401;

		averageArea=Area(final_mesh)/((ScalarType)base_mesh.fn*(ScalarType)2.0);
		averageLenght=(ScalarType)2.0*sqrt(averageArea)/sqrtsrt3;

		ScalarType varianceL,varianceA;
		FindVarianceLenghtArea(base_mesh,averageLenght,averageArea,varianceL,varianceA);

#ifndef _MESHLAB
		printf("Variance lenght:%f\n",varianceL*100.f/averageLenght);
		printf("Variance area:%f\n",varianceA*100.f/averageArea);
#endif
		//Initialize heap
		for (unsigned int i=0;i<base_mesh.vert.size();i++)
		{
			VertexType *v=&base_mesh.vert[i];
			Operations.push_back(Elem(v,Priority(v),global_mark));
		}
		std::make_heap(Operations.begin(),Operations.end());
		float gap=(ScalarType)0.05;
		int n_oper=0;
		///start Optimization
		ScalarType varianceL0=varianceL;
		ScalarType varianceA0=varianceA;
		ScalarType varianceL1;
		ScalarType varianceA1;
		bool continue_opt=true;
		while (continue_opt)
		{
			int temp_oper=0;
			while (temp_oper<20)
			{
				std::pop_heap(Operations.begin(),Operations.end());
				VertexType* oper=Operations.back().center;
				int t_mark=Operations.back().t_mark;
				Operations.pop_back();
				if (markers[oper]<=t_mark)
				{
					Execute(oper);
					n_oper++;
					temp_oper++;
				}
			}
			FindVarianceLenghtArea(base_mesh,averageLenght,averageArea,varianceL1,varianceA1);
			ScalarType percL=(varianceL0-varianceL1)*100/averageLenght;
			ScalarType percA=(varianceA0-varianceA1)*100/averageArea;
			ScalarType curr_gap=percL+percA;
#ifndef _MESHLAB
			printf("gap:%f\n",curr_gap);
#endif
			varianceL0=varianceL1;
			varianceA0=varianceA1;
			continue_opt=curr_gap>gap;
		}
		FindVarianceLenghtArea(base_mesh,averageLenght,averageArea,varianceL,varianceA);

#ifndef _MESHLAB
		printf("Num Oper:%i\n",n_oper);
		printf("Variance lenght:%f\n",varianceL*100.f/averageLenght);
		printf("Variance area:%f\n",varianceA*100.f/averageArea);
#endif

	}

	PatchesOptimizer(MeshType &_base_mesh,MeshType &_final_mesh):base_mesh(_base_mesh),final_mesh(_final_mesh),markers(_base_mesh.vert){}
	
	static MeshType* &HresMesh()
	{
		static MeshType* mesh;
		return mesh;
	}
	
	static MeshType* &BaseMesh()
	{
		static MeshType* mesh;
		return mesh;
	}

};
#endif
