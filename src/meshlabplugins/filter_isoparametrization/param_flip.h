#ifndef PARAM_FLIP
#define PARAM_FLIP

#include <vcg/complex/local_optimization/tri_edge_flip.h>

///Flip function
template <class BaseMesh>
class ParamEdgeFlip : public vcg::tri::PlanarEdgeFlip<BaseMesh, ParamEdgeFlip<BaseMesh> >
{
	typedef typename BaseMesh::VertexType::EdgeType EdgeType;
	typedef typename BaseMesh::VertexType BaseVertex;
        typedef typename BaseMesh::VertexType VertexType;
	typedef typename BaseMesh::FaceType   BaseFace;
        typedef typename BaseMesh::FaceType   FaceType;
        typedef typename BaseMesh::CoordType   CoordType;
        typedef typename BaseMesh::ScalarType   ScalarType;
        typedef vcg::tri::PlanarEdgeFlip<BaseMesh, ParamEdgeFlip<BaseMesh> > Super;
	ScalarType diff;

	public:
	
	static EnergyType &EType(){static EnergyType E;return E;};

	bool savedomain;
	
	bool IsFeasible()
	{
		if(!vcg::face::CheckFlipEdge(*this->_pos.F(), this->_pos.E()))
			return false;

		return (this->_priority>0);
	}

	inline ParamEdgeFlip() {}

	/*!
	 *	Constructor with <I>pos</I> type
	 */
  inline ParamEdgeFlip(const typename Super::PosType pos, int mark)
	{
		this->_pos = pos;
		this->_localMark = mark;
		this->_priority = this->ComputePriority();
		savedomain=false;
	}

	///do the effective flip 
	void ExecuteFlip(FaceType &f, const int &edge, BaseMesh *base_domain=NULL)
	{
		std::vector<FaceType*> faces;
		faces.push_back(&f);
		faces.push_back(f.FFp(edge));
		std::vector<VertexType*> HresVert;
		getHresVertex<FaceType>(faces,HresVert);
		///parametrize H_res mesh respect to diamond
		for (unsigned int i=0;i<HresVert.size();i++)
		{
			VertexType* v=HresVert[i];
			///get father & bary
			FaceType* father=v->father;
			CoordType bary=v->Bary;
			assert((father==faces[0])||(father==faces[1]));
			vcg::Point2<ScalarType> t0=father->V(0)->T().P();
			vcg::Point2<ScalarType> t1=father->V(1)->T().P();
			vcg::Point2<ScalarType> t2=father->V(2)->T().P();

			//assert(testBaryCoords(bary));
			if(!testBaryCoords(bary))
		{
			printf("BAry0 :%lf,%lf,%lf",bary.X(),bary.Y(),bary.Z());
			//system("pause");
		}

			GetUV<BaseMesh>(father,bary,v->T().U(),v->T().V());
		}

		///update VF topology
		FaceType *f1=f.FFp(edge);
		FaceType *f0=&f;
		vcg::face::VFDetach(*f1,0);
		vcg::face::VFDetach(*f1,1);
		vcg::face::VFDetach(*f1,2);
		vcg::face::VFDetach(*f0,0);
		vcg::face::VFDetach(*f0,1);
		vcg::face::VFDetach(*f0,2);
		///then do the effective flip

		vcg::face::FlipEdge(f,edge);

		
		
		vcg::face::VFAppend(f1,0);
		vcg::face::VFAppend(f1,1);
		vcg::face::VFAppend(f1,2);
		vcg::face::VFAppend(f0,0);
		vcg::face::VFAppend(f0,1);
		vcg::face::VFAppend(f0,2);
		///edh updating topology


		///set son->father new link
		for (unsigned int i=0;i<HresVert.size();i++)
		{
			VertexType* v=HresVert[i];
			ScalarType U=v->T().U();
			ScalarType V=v->T().V();
			CoordType bary;
			int index;
			bool found=GetBaryFaceFromUV(faces,U,V,bary,index);
			if (!found)
			{
				printf("\n U : %lf; V : %lf \n",U,V);
				//system("pause");
			}
			//assert(found);
			assert(testBaryCoords(bary));
			if (base_domain!=NULL)
				AssingFather(*v,faces[index],bary,*base_domain);
			else
			{
				v->father=faces[index];
				assert(!faces[index]->IsD());
				v->Bary=bary;
			}
		}
		
		

		///set father->son new link
		for (unsigned int i=0;i<faces.size();i++)
			faces[i]->vertices_bary.clear();

		for (unsigned int i=0;i<HresVert.size();i++)
		{
			VertexType *son=HresVert[i];
			FaceType *father=son->father;
			CoordType bary=son->Bary;
			father->vertices_bary.push_back(std::pair<BaseVertex*,vcg::Point3f>(son,bary));
		}
		
	}

	ScalarType EdgeDiff()
	{
		/*
		     1  
		    /|\
		   / | \
		 2 f0|f1 3 
		   \ | /
		    \|/
		     0
		 */
		
		VertexType *v0, *v1, *v2, *v3;
		int edge0 = this->_pos.E();
		v0 = this->_pos.F()->V0(edge0);
		v1 = this->_pos.F()->V1(edge0);
		v2 = this->_pos.F()->V2(edge0);
		v3 = this->_pos.F()->FFp(edge0)->V2(this->_pos.F()->FFi(edge0));
		int edge1=this->_pos.F()->FFi(edge0);
		FaceType* f0=this->_pos.F();
		FaceType* f1=this->_pos.F()->FFp(edge0);

		///parametrize all possible diamonds
		///diam0 & diam1
		///make a copy of the mesh
		std::vector<FaceType*> OrdFace;
		OrdFace.push_back(f0);
		OrdFace.push_back(f1);
		
		BaseMesh Diam;
		BaseMesh DiamHres;

		///create a copy of the domain and of the H resolution
		CopySubMeshLevels(OrdFace,Diam,DiamHres);
		
		///parametrize domains
		ParametrizeDiamondEquilateral(Diam,edge0,edge1);

		///copy parametrization on original mesh

		FaceType* on_edge[2];
		on_edge[0]=&Diam.face[0];
		on_edge[1]=&Diam.face[1];
		assert(Diam.face[0].FFp(edge0)==&Diam.face[1]);///test
		assert(Diam.face[1].FFp(edge1)==&Diam.face[0]);///test

		///Evaluate lenght of shared edge
		ScalarType L0=EstimateLenghtByParam<BaseMesh>(Diam.face[0].V(edge0),Diam.face[0].V((edge0+1)%3),on_edge);

		///do the flip on the copied mesh do not affect the original mesh
		ExecuteFlip(Diam.face[0],edge0);
		
		UpdateTopologies(&Diam);
		
		///get the non border edge of face0
		int NB_edge=-1;
		if (!Diam.face[0].IsB(0))
			NB_edge=0;
		else
		if (!Diam.face[0].IsB(1))
			NB_edge=1;
		else
		if (!Diam.face[0].IsB(2))
			NB_edge=2;
		assert(NB_edge!=-1);

		ScalarType L1=EstimateLenghtByParam<BaseMesh>(Diam.face[0].V(NB_edge),Diam.face[0].V((NB_edge+1)%3),on_edge);
		
		ScalarType value=L0-L1;
		diff=value;
		this->_priority = 1.0/value;
		return (this->_priority);
	}
	
	
	void Execute(BaseMesh &m)
	{
		
		assert(this->_priority>0);
		/*
		     1  
		    /|\
		   / | \
		 2 f0|f1 3 
		   \ | /
		    \|/
		     0
		 */
		VertexType *v0, *v1, *v2, *v3;
		int edge0 = this->_pos.E();
		v0 = this->_pos.F()->V0(edge0);
		v1 = this->_pos.F()->V1(edge0);
		v2 = this->_pos.F()->V2(edge0);
		v3 = this->_pos.F()->FFp(edge0)->V2(this->_pos.F()->FFi(edge0));
		///assing texcoords
		ScalarType h=(sqrt((ScalarType)3.0)/(ScalarType)2.0);
		v0->T().P()=vcg::Point2<ScalarType>(0,(ScalarType)-0.5);
		v1->T().P()=vcg::Point2<ScalarType>(0,(ScalarType)0.5);
		v2->T().P()=vcg::Point2<ScalarType>(-h,0);
		v3->T().P()=vcg::Point2<ScalarType>(h,0);

#ifndef _MESHLAB
		///save domain if need for demos
		if (savedomain)
		{
			BaseMesh hlev_mesh;
			std::vector<FaceType*> faces;
			FaceType* f=this->_pos.F();
			int edge=this->_pos.E();
			faces.push_back(f);
			faces.push_back(f->FFp(edge));
			std::vector<VertexType*> HresVert;
			getHresVertex<FaceType>(faces,HresVert);

			///parametrize H_res mesh respect to diamond
			std::vector<VertexType*> OrderedVertices;
			std::vector<FaceType*> OrderedFaces;
			CopyMeshFromVertices(HresVert,OrderedVertices,OrderedFaces,hlev_mesh);
			//for (int i=0;i<hlev_mesh.vert.size();i++)
				//hlev_mesh.vert[i].C()=hlev_mesh.vert[i].OriginalCol;

			vcg::tri::io::ExporterPLY<BaseMesh>::Save(hlev_mesh,"c:/export_submeshes/FLIPHlev3D.ply",vcg::tri::io::Mask::IOM_VERTCOLOR);
			for (unsigned int i=0;i<hlev_mesh.vert.size();i++)
			{
				hlev_mesh.vert[i].P().X()=hlev_mesh.vert[i].T().U();
				hlev_mesh.vert[i].P().Y()=hlev_mesh.vert[i].T().V();
				hlev_mesh.vert[i].P().Z()=0;
			}
			vcg::tri::io::ExporterPLY<BaseMesh>::Save(hlev_mesh,"c:/export_submeshes/FLIPHlevUV.ply",vcg::tri::io::Mask::IOM_VERTCOLOR);
		}
#endif

		ExecuteFlip(*this->_pos.F(),this->_pos.E(),&m);
		
		UpdateTopologies(&m);
		
		///stars optimization
		/*int t0=clock();*/
		/*OptimizeStar<BaseMesh>(v0);
		OptimizeStar<BaseMesh>(v1);
		OptimizeStar<BaseMesh>(v2);
		OptimizeStar<BaseMesh>(v3);*/
		
		SmartOptimizeStar<BaseMesh>(v0,m,Accuracy(),EType());
		SmartOptimizeStar<BaseMesh>(v1,m,Accuracy(),EType());
		SmartOptimizeStar<BaseMesh>(v2,m,Accuracy(),EType());
		SmartOptimizeStar<BaseMesh>(v3,m,Accuracy(),EType());	
		/*int t1=clock();
		time_opt+=(t1-t0);*/
	}

	ScalarType ComputePriority()
	{
    this->_priority=EdgeDiff();
		return this->_priority;
	}

	BaseFace *getF()
	{return this->_pos.F();}

	int getE()
	{return this->_pos.E();}

	public:
	static int &Accuracy()
	{
		static int _acc;
		return _acc;
	}

};

#endif
