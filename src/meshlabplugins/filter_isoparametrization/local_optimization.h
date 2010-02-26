#ifndef _LOCAL_OPTIMIZATION
#define _LOCAL_OPTIMIZATION

#include "statistics.h"

typedef enum EnergyType{EN_EXTMips,EN_MeanVal};

template <class MeshType>
bool UnFold(MeshType &mesh,int /*num_faces*/,bool fix_selected=false)
{
  typedef typename MeshType::ScalarType ScalarType;
  //static int folds=0;
  std::vector<typename MeshType::FaceType*> folded_faces;
  bool unfolded=NonFolded<MeshType>(mesh,folded_faces);
  if (unfolded)return (true);
  typedef typename vcg::tri::MIPSTexCoordFoldHealer<MeshType> UnfoldType;	
  UnfoldType opt(mesh);
	
  opt.TargetCurrentGeometry();
  opt.SetBorderAsFixed();
  if (fix_selected)
  for (unsigned int i=0;i<mesh.vert.size();i++)
	if (mesh.vert[i].IsS())	  
		opt.FixVertex(&mesh.vert[i]);
		
  
  
  //ScalarType speed=(2.0/(sqrt((ScalarType)mesh.vn/num_faces)))*0.008;
  ScalarType edge_esteem=GetSmallestUVHeight(mesh);
  //ScalarType speed=edge_esteem*0.005;*/
  //ScalarType edge_esteem=GetSmallestUVEdgeSize<MeshType>(mesh);
	
  ScalarType speed=edge_esteem*0.05;
  opt.SetSpeed(speed);
  ///then iterate until unfolding
  /*int iter=*/opt.IterateUntilConvergence();
//#ifndef _MESHLAB
//  printf("[F:%d]",iter);
//#endif
  return true;
}

///return true if UV coordinates are OK
template <class MeshType>
bool testCoords(MeshType &m)
{
        typedef typename MeshType::ScalarType ScalarType;
	for (int i=0;i<m.vert.size();i++)
	{
		ScalarType u=m.vert[i].T().U();
		ScalarType v=m.vert[i].T().V();
		if (!((u>-1.0)&&(u<1.0)&&(v>-1.0)&&(v<1.0)))
			return false;
	}
	return true;
}

template <class MeshType>
typename MeshType::ScalarType StarDistorsion(typename MeshType::VertexType *v)
{
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;

	std::vector<VertexType*> starCenter;
	starCenter.push_back(v);
	std::vector<FaceType*> ordered_faces;
	std::vector<VertexType*> HresVert;
	MeshType star_domain,hlev_mesh;

	///create star
	CreateMeshVertexStar(starCenter,ordered_faces,star_domain);
	ParametrizeStarEquilateral<MeshType>(star_domain);

	///get all the vertices in H definition that shuld be optimized
	///and initialize vertices with u,v coordinates
	for (unsigned int index=0;index<ordered_faces.size();index++)
	{
		FaceType *parametric_face=&star_domain.face[index];
		FaceType *test_face=ordered_faces[index];

		///parametrize HlevMesh
		for (unsigned int i=0;i<test_face->vertices_bary.size();i++)
		{
			///interpolate U V per vertex
			CoordType bary=test_face->vertices_bary[i].second;
			ScalarType u,v;
			GetUV<MeshType>(parametric_face,bary,u,v);
			
			VertexType* to_parametrize=test_face->vertices_bary[i].first;
			to_parametrize->T().U()=u;
			to_parametrize->T().V()=v;
		
			HresVert.push_back(to_parametrize);
		}
	}

	///get a copy of submesh
	std::vector<typename MeshType::VertexType*> ordered_vertex;
	CopyHlevMesh(ordered_faces,hlev_mesh,ordered_vertex);
	UpdateTopologies<MeshType>(&hlev_mesh);
        ScalarType val0=ApproxAreaDistortion<MeshType>(hlev_mesh,star_domain.fn);
        ScalarType val1=ApproxAngleDistortion<MeshType>(hlev_mesh);
	ScalarType val2=geomAverage<ScalarType>(val0+(ScalarType)1.0,val1+(ScalarType)1.0,3,1)-(ScalarType)1;
	return val2;
}

///optimize a single star
template <class MeshType>
void OptimizeStar(typename MeshType::VertexType *v,MeshType &domain,int accuracy=1,EnergyType En=EN_EXTMips)
{
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename vcg::tri::AreaPreservingTexCoordOptimization<MeshType> OptType;
	typedef typename vcg::tri::MeanValueTexCoordOptimization<MeshType> OptType1;
	
	
	std::vector<VertexType*> starCenter;
	starCenter.push_back(v);
	std::vector<FaceType*> ordered_faces;
	std::vector<VertexType*> HresVert;
	MeshType star_domain,hlev_mesh;

	///create star
	CreateMeshVertexStar(starCenter,ordered_faces,star_domain);
	ParametrizeStarEquilateral<MeshType>(star_domain);

	///get all the vertices in H definition that shuld be optimized
	///and initialize vertices with u,v coordinates
	for (unsigned int index=0;index<ordered_faces.size();index++)
	{
		FaceType *parametric_face=&star_domain.face[index];
		FaceType *test_face=ordered_faces[index];

		///parametrize HlevMesh
		for (unsigned int i=0;i<test_face->vertices_bary.size();i++)
		{
			///interpolate U V per vertex
			CoordType bary=test_face->vertices_bary[i].second;
			ScalarType u,v;
			GetUV<MeshType>(parametric_face,bary,u,v);
			
			VertexType* to_parametrize=test_face->vertices_bary[i].first;
			to_parametrize->T().U()=u;
			to_parametrize->T().V()=v;
		
			HresVert.push_back(to_parametrize);
		}
	}

	/*for (int i=0;i<10;i++)*/
	if (En==EN_MeanVal)
		for (int i=0;i<4;i++)
			vcg::tri::SmoothTexCoords(hlev_mesh);
	else
		vcg::tri::SmoothTexCoords(hlev_mesh);

	///get a copy of submesh
	std::vector<typename MeshType::VertexType*> ordered_vertex;
	CopyHlevMesh(ordered_faces,hlev_mesh,ordered_vertex);
	UpdateTopologies<MeshType>(&hlev_mesh);
	InitDampRestUV(hlev_mesh);

	assert(testParamCoords(hlev_mesh));
	
	

	if (hlev_mesh.vn==0)
		return;
	if (hlev_mesh.fn==0)
		return;

	bool b=UnFold<MeshType>(hlev_mesh,star_domain.fn);
	bool isOK=testParamCoords(hlev_mesh);

	if ((!b)||(!isOK))
		RestoreRestUV(hlev_mesh);
	
	bool b0=NonFolded<MeshType>(hlev_mesh);
	

	

	///initialize optimization
	if (En==EN_EXTMips)
	{
		OptType opt(hlev_mesh);
		opt.TargetCurrentGeometry();
		opt.SetBorderAsFixed();

		////SETTING SPEED
		ScalarType edge_esteem=GetSmallestUVHeight(hlev_mesh);

		ScalarType speed0=edge_esteem*0.5;//0.05;
		ScalarType conv=edge_esteem*0.2;//edge_esteem*0.05;
		if (accuracy>1)
			conv*=1.0/(ScalarType)((accuracy-1)*10.0);

		opt.SetSpeed(speed0);
		opt.IterateUntilConvergence(conv);
	}
	else
		if (En==EN_MeanVal)
		{
			OptType1 opt(hlev_mesh);
			opt.TargetCurrentGeometry();
			opt.SetBorderAsFixed();

			////SETTING SPEED
			ScalarType edge_esteem=GetSmallestUVHeight(hlev_mesh);

			ScalarType speed0=edge_esteem*0.5;//0.05;
			ScalarType conv=edge_esteem*0.2;//edge_esteem*0.05;
			if (accuracy>1)
				conv*=1.0/(ScalarType)((accuracy-1)*10.0);

			opt.SetSpeed(speed0);
			opt.IterateUntilConvergence(conv);
		}
	/*opt.IterateN(100);*/
	/*int ite=*/

	if (!testParamCoords(hlev_mesh))
		return;///no modifications problems with optimization 

	///folded during optimization
	bool b1=NonFolded<MeshType>(hlev_mesh);
	if ((b0)&&(!b1))
	{
#ifndef _MESHLAB
		printf("@");
#endif
		//return;
		/*RestoreRestUV(hlev_mesh);*/
	}

	bool inside=true;

	//test barycentric coords
	std::vector<FaceType*> oldFath;
	std::vector<CoordType> oldBary;
	oldFath.reserve(hlev_mesh.vert.size());
	oldBary.reserve(hlev_mesh.vert.size());

	for (unsigned int i=0;i<hlev_mesh.vert.size();i++)
	{
		VertexType *parametrized=&hlev_mesh.vert[i];
		///save previous values
		VertexType *to_reassing=ordered_vertex[i];
		oldFath.push_back(to_reassing->father);
		oldBary.push_back(to_reassing->Bary);
		
		///get UV coords
		ScalarType U=parametrized->T().U();
		ScalarType V=parametrized->T().V();
		///then get face falling into and estimate (alpha,beta,gamma)
		CoordType bary;
                FaceType* chosen;
		inside=GetBaryFaceFromUV(star_domain,U,V,ordered_faces,bary,chosen);
		if ((!inside)||(!testBaryCoords(bary)))
		{
			///restore old coordinates and return
			for (unsigned int k=0;k<oldFath.size();k++)
			{
				/*hlev_mesh.vert[k].father=oldFath[k];
				assert(!oldFath[k]->IsD());
				hlev_mesh.vert[k].Bary=oldBary[k];*/
				AssingFather(hlev_mesh.vert[k],oldFath[k],oldBary[k],domain);
			}
			CoordType val;
			bool found1=GetCoordFromUV(hlev_mesh,0,0,val,true);   
			if (found1)
				v->RPos=val;
			return;
		}

		//to_reassing->father=chosen;
		//assert(!chosen->IsD());
		//to_reassing->Bary=bary;
		AssingFather(*to_reassing,chosen,bary,domain);
	}

	///clear father and bary
	for (unsigned int i=0;i<ordered_faces.size();i++)
		ordered_faces[i]->vertices_bary.resize(0);

	///set face-vertex link
	for (unsigned int i=0;i<HresVert.size();i++)
	{
                VertexType *v=HresVert[i];
                FaceType *f=v->father;
		CoordType bary=v->Bary;
		/*if(!testBaryCoords(bary))
		{
			printf("BAry0 :%lf,%lf,%lf",bary.X(),bary.Y(),bary.Z());
			system("pause");
		}*/
		f->vertices_bary.push_back(std::pair<VertexType*,CoordType>(v,bary));
	}
	
	///update Rpos of V if it can
	///get UV coords of V that is (0,0)
	//vcg::Point2f UVCenter=vcg::Point2f(0,0);
	CoordType val;
	bool found1=GetCoordFromUV(hlev_mesh,0,0,val,true);   
	if (found1)
		v->RPos=val;
}


template <class MeshType>
bool SmartOptimizeStar(typename MeshType::VertexType *center,MeshType &base_domain,int accuracy=1,EnergyType En=EN_EXTMips)
{
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;
	std::vector<FaceType*> faces;
	std::vector<VertexType*> centers;
	centers.push_back(center);
	///find number of vertices falling into
	getSharedFace<MeshType>(centers,faces);
	centers.clear();
	int sizeH=0;
	for (unsigned int i=0;i<faces.size();i++)
		sizeH+=faces[i]->vertices_bary.size();

	ScalarType ratio=(ScalarType)sizeH/(ScalarType)faces.size();

	if (ratio<=1)
		return false;
	else
		OptimizeStar<MeshType>(center,base_domain,accuracy,En);
	return true;
}
#endif
