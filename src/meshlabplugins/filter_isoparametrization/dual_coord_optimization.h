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
	//typedef typename vcg::tri::MIPSTexCoordOptimization<MeshType> OptType1;


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
	/*std::vector<std::vector<VertexType*> > HVert;*/

	/*///subdomain in witch a vertex falls into
	std::vector<param_domain*> subdomain;*/

	///map fo correspondences between domain & entities
	//std::map<VertexType*,param_domain*> starMap;
	//std::map<std::pair<FaceType*,FaceType*>,param_domain*> diamondMap;
	//std::map<FaceType*,param_domain*> faceMap;

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

				/*///insert in search structure
				starMap.insert(std::pair<VertexType*,param_domain*>(&domain->vert[i],&star_meshes[index]));*/

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
						/*diamondMap.insert(std::pair<std::pair<FaceType*,FaceType*> ,param_domain*>(entry,&diamond_meshes[index]));*/
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

/////subdivide the mesh into Star subdomain
//void InitStarSubdivision()
//{
//	
//	vcg::SimpleTempData<typename MeshType::VertContainer, param_domain* > domainVert(h_res_mesh->vert);
//
//	for (int i=0;i<h_res_mesh->vert.size();i++)
//	{
//		assert(!h_res_mesh->vert[i].IsD());
//		///get father and bary coordinate
//		FaceType *father=h_res_mesh->vert[i].father;
//		CoordType bary=h_res_mesh->vert[i].Bary;
//
//		///then get the substar falling into
//
//		///get nearest vertex
//		VertexType* chosen;
//		CoordType proj=Warp(&h_res_mesh->vert[i]);		
//		int index=getVertexStar(proj,father);
//		///chosen is the half-star center
//		chosen=father->V(index);
//
//		///then get corresponding half-star reference parametrized mesh
//		std::map<VertexType*,param_domain*>::iterator iteStar=starMap.find(chosen);
//		assert(iteStar!=starMap.end());
//		///end parametrize it using that star
//
//		///get corresponding parametrizad face
//		param_domain* pdomain=(*iteStar).second;
//		FaceType *param_face=NULL;
//		int f=0;
//		while ((f<pdomain->ordered_faces.size())&&(param_face==NULL))
//		{
//			if (pdomain->ordered_faces[f]==father)
//				param_face=&pdomain->domain.face[f];
//			f++;
//		}
//		assert(param_face!=NULL);
//		///then translate barycentric into UV over the star
//		ScalarType u,v;
//		GetUV<MeshType>(param_face,bary,u,v);
//
//		///set UV values
//		h_res_mesh->vert[i].T().U()=u;
//		h_res_mesh->vert[i].T().V()=v;
//		h_res_mesh->vert[i].ClearS();
//
//		///set corresponding domain
//		domainVert[i]=pdomain;
//		subdomain[i]=pdomain;
//	}
//
//	///then set as selected border vertices
//	for (int i=0;i<h_res_mesh->face.size();i++)
//	{
//		VertexType *v0=h_res_mesh->face[i].V(0);
//		VertexType *v1=h_res_mesh->face[i].V(1);
//		VertexType *v2=h_res_mesh->face[i].V(2);
//		if (!((domainVert[v0]==domainVert[v1])&&(domainVert[v1]==domainVert[v2])))
//		{
//			v0->SetS();
//			v1->SetS();
//			v2->SetS();
//		}
//	}
//}


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

/////subdivide the mesh into subdomains
//void InitDiamondSubdivision()
//{
//	
//	vcg::SimpleTempData<typename MeshType::VertContainer, param_domain* > domainVert(h_res_mesh->vert);
//
//	for (int i=0;i<h_res_mesh->vert.size();i++)
//	{
//		/*h_res_mesh->vert[i].C()=vcg::Color4b(255,255,255,255);*/
//
//		assert(!h_res_mesh->vert[i].IsD());
//		///get father and bary coordinate
//		FaceType *father=h_res_mesh->vert[i].father;
//		CoordType bary=h_res_mesh->vert[i].Bary;
//
//		///get nearest edge
//		int chosen;
//		CoordType proj=Warp(&h_res_mesh->vert[i]);
//		
//		chosen=getEdgeDiamond(proj,father);
//		///then get corresponding star
//		FaceType *fadj=father->FFp(chosen);
//		FaceType *f0=(father>fadj)? father:fadj;
//		FaceType *f1=(father<fadj)? father:fadj;
//
//		std::pair<FaceType*,FaceType*> keyF=std::pair<FaceType*,FaceType*>(f0,f1);
//		std::map<std::pair<FaceType*,FaceType*> ,param_domain*>::iterator iteDiam=diamondMap.find(keyF);
//		assert(iteDiam!=diamondMap.end());
//
//		///end parametrize it using that star
//		param_domain* pdomain=(*iteDiam).second;
//		FaceType *param_face=NULL;
//		int f=0;
//		while ((f<pdomain->ordered_faces.size())&&(param_face==NULL))
//		{
//			if (pdomain->ordered_faces[f]==father)
//				param_face=&pdomain->domain.face[f];
//			f++;
//		}
//		assert(param_face!=NULL);
//		///then translate barycentric into UV over the star
//		ScalarType u,v;
//		GetUV<MeshType>(param_face,bary,u,v);
//
//		///set UV values
//		h_res_mesh->vert[i].T().U()=u;
//		h_res_mesh->vert[i].T().V()=v;
//		h_res_mesh->vert[i].ClearS();
//
//		///set corresponding domain
//		domainVert[i]=pdomain;
//		subdomain[i]=pdomain;
//	}
//
//	///then set as selected border vertices
//	for (int i=0;i<h_res_mesh->face.size();i++)
//	{
//		VertexType *v0=h_res_mesh->face[i].V(0);
//		VertexType *v1=h_res_mesh->face[i].V(1);
//		VertexType *v2=h_res_mesh->face[i].V(2);
//		if (!((domainVert[v0]==domainVert[v1])&&(domainVert[v1]==domainVert[v2])))
//		{
//			v0->SetS();
//			v1->SetS();
//			v2->SetS();
//		}
//	}
//}

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

/////subdivide the mesh into subdomains
//void InitFaceSubdivision()
//{
//	
//	vcg::SimpleTempData<typename MeshType::VertContainer, param_domain* > domainVert(h_res_mesh->vert);
//
//	for (int i=0;i<h_res_mesh->vert.size();i++)
//	{
//	
//		assert(!h_res_mesh->vert[i].IsD());
//		///get father and bary coordinate
//		FaceType *father=h_res_mesh->vert[i].father;
//		CoordType bary=h_res_mesh->vert[i].Bary;
//
//		std::map<FaceType*,param_domain*>::iterator iteFace=faceMap.find(father);
//		assert(iteFace!=faceMap.end());
//
//		///end parametrize it using that star
//		param_domain* pdomain=(*iteFace).second;
//		FaceType *param_face=&pdomain->domain.face[0];
//		assert(father==pdomain->ordered_faces[0]);
//
//		///then translate barycentric into UV over the star
//		ScalarType u,v;
//		GetUV<MeshType>(param_face,bary,u,v);
//
//		///set UV values
//		h_res_mesh->vert[i].T().U()=u;
//		h_res_mesh->vert[i].T().V()=v;
//		h_res_mesh->vert[i].ClearS();
//
//		///set corresponding domain
//		domainVert[i]=pdomain;
//		subdomain[i]=pdomain;
//	}
//
//	///then set as selected border vertices
//	for (int i=0;i<h_res_mesh->face.size();i++)
//	{
//		VertexType *v0=h_res_mesh->face[i].V(0);
//		VertexType *v1=h_res_mesh->face[i].V(1);
//		VertexType *v2=h_res_mesh->face[i].V(2);
//		if (!((domainVert[v0]==domainVert[v1])&&(domainVert[v1]==domainVert[v2])))
//		{
//			v0->SetS();
//			v1->SetS();
//			v2->SetS();
//		}
//	}
//}

//void GetDomainHVert(param_domain*  domain,std::vector<VertexType*> &HresVert)
//{
//	HresVert.clear();
//	for (int i=0;i<h_res_mesh->vert.size();i++)
//		if (subdomain[i]==domain)
//			HresVert.push_back(&h_res_mesh->vert[i]);
//}

////test current domains and unfold when is necessary and needed
//bool UnfoldDomain()
//{
//	std::set<param_domain*> folded_domains;
//
//	///set all vertices as non-visited
//	for (int i=0;i<h_res_mesh->vert.size();i++)
//		h_res_mesh->vert[i].ClearV();
//
//	///collect folded vertices
//	bool folded=false;
//	for (int i=0;i<h_res_mesh->face.size();i++)
//	{
//		if (!(h_res_mesh->face[i].V(0)->IsS()&&
//			h_res_mesh->face[i].V(1)->IsS()&&
//			h_res_mesh->face[i].V(2)->IsS()))
//		{
//			FaceType *f=&h_res_mesh->face[i];
//			vcg::Point2<ScalarType> tex0=vcg::Point2<ScalarType>(f->V(0)->T().U(),f->V(0)->T().V());
//			vcg::Point2<ScalarType> tex1=vcg::Point2<ScalarType>(f->V(1)->T().U(),f->V(1)->T().V());
//			vcg::Point2<ScalarType> tex2=vcg::Point2<ScalarType>(f->V(2)->T().U(),f->V(2)->T().V());
//			vcg::Triangle2<MeshType::ScalarType> t2d=vcg::Triangle2<MeshType::ScalarType>(tex0,tex1,tex2);
//			ScalarType area=(tex1-tex0)^(tex2-tex0);
//			if (area<0)
//			{
//				h_res_mesh->face[i].V(0)->SetV();
//				h_res_mesh->face[i].V(1)->SetV();
//				h_res_mesh->face[i].V(2)->SetV();
//				folded=true;
//			}
//		}
//	}
//
//	if (!folded)
//	{
//		//printf("nothing to unfold\n");
//		return true;
//	}
//
//	///collect all domains in wich needed optimization
//	for (int i=0;i<h_res_mesh->vert.size();i++)
//		if (h_res_mesh->vert[i].IsV())
//			folded_domains.insert(subdomain[i]);
//
//	///clear flags
//	for (int i=0;i<h_res_mesh->vert.size();i++)
//		h_res_mesh->vert[i].ClearV();
//
//	///for each one try to unfold
//	std::set<param_domain*>::iterator iteParam;
//	for (iteParam=folded_domains.begin();iteParam!=folded_domains.end();iteParam++)
//	{
//		std::vector<VertexType*> HresVert;
//		std::vector<VertexType*> ordVert;
//		//HresVert.clear();
//		///get h resolution vertex
//		MeshType folded;
//		//getHresVertex<FaceType>((*iteParam)->ordered_faces,HresVert);*/
//		/*for (int i=0;i<h_res_mesh->vert.size();i++)
//			if (subdomain[i]==(*iteParam))
//				HresVert.push_back(&h_res_mesh->vert[i]);*/
//
//		GetDomainHVert((*iteParam),HresVert);
//
//		CopyMeshFromVertices<MeshType>(HresVert,ordVert,folded);
//		//bool done=true;
//		bool done=UnFold<MeshType>(folded,(*iteParam)->ordered_faces.size());
//		///copy back values
//		
//		if (done)
//		{
//			for (int i=0;i<folded.vert.size();i++)
//				ordVert[i]->T().P()=folded.vert[i].T().P();
//		}
//	}
//	return true;
//}

//void MinimizeStep(char *phasename,
//				  const ScalarType &conv_interval=0.00001,
//				  const int &max_step=500)
//{
//	//////statistics
//	UnfoldDomain();
//	int opt_type=0;
//	std::vector<CoordType> oldPos;
//	std::vector<FaceType*> oldFather;
//	oldPos.resize(h_res_mesh->vert.size());
//	oldFather.resize(h_res_mesh->vert.size());
//
//	for (int i=0;i<h_res_mesh->vert.size();i++)
//	{
//		oldPos[i]=h_res_mesh->vert[i].Bary;
//		oldFather[i]=h_res_mesh->vert[i].father;
//	}
//	
//
//	///fix selected vertices 
//	optimizer[0]->SetNothingAsFixed();
//	for (int i=0;i<h_res_mesh->vert.size();i++)
//		if (h_res_mesh->vert[i].IsS())
//			optimizer[opt_type]->FixVertex(&h_res_mesh->vert[i]);
//
//	
//
//	//ScalarType speed=2.0/(sqrt((ScalarType)h_res_mesh->vn/(ScalarType)domain->vn))*optimize_speed_fact;
//	//optimizer[opt_type]->SetSpeed(speed);
//
//	/////then iterate until convergence
//	//for (int i=0;i<max_step;i++)
//	//	optimizer[opt_type]->IterateBlind();
//	ScalarType speed0=sqrt((ScalarType)domain->fn/(ScalarType)h_res_mesh->vn) * optimize_speed_fact;
//	//ScalarType speed0=optimize_speed_fact;
//	optimizer[opt_type]->SetSpeed(speed0);
//	///END SETTINS SPEED
//	//if ((!bi)&&(b))
//	//	opt.IterateUntilConvergence(0.001);//,itenum);    
//	//else
//	int ite=optimizer[opt_type]->IterateUntilConvergence(speed0/10.0);//,itenum);
//
//	/*optimizer[opt_type]->IterateUntilConvergence(conv_interval,max_step);*/
//	bool inside=true;
//	
//	//reassing fathers and bary coordinates
//	for (int i=0;i<h_res_mesh->vert.size();i++)
//	{
//		VertexType *vert=&h_res_mesh->vert[i];
//		param_domain* pdomain=subdomain[i];
//		ScalarType u=vert->T().U();
//		ScalarType v=vert->T().V();
//		///then get face falling into and estimate (alpha,beta,gamma)
//		CoordType bary;
//		BaseFace* chosen;
//		inside &=GetBaryFaceFromUV(pdomain->domain,u,v,pdomain->ordered_faces,bary,chosen);
//		vert->father=chosen;
//		vert->Bary=bary;
//	}
//	//assert(inside);
//	////statistics
//	int changed=0;
//	ScalarType max_diff=0;
//	for (int i=0;i<h_res_mesh->vert.size();i++)
//	{
//		//ScalarType displ=(oldPos[i]-ProjectPos(h_res_mesh->vert[i])).Norm();
//		if (oldFather[i]!=h_res_mesh->vert[i].father)
//			changed++;
//
//		else {
//			ScalarType displ = (oldPos[i]-h_res_mesh->vert[i].Bary).SquaredNorm();
//			if (displ>max_diff) max_diff=displ;
//		}
//	}
//	printf("%s : \n",phasename);
//	printf(" diff:%6.5f \n",sqrt(max_diff) );
//	printf("migr:%i \n",changed);
//	printf("AREA distorsion:%lf \n",ApproxAreaDistortion<MeshType>(*h_res_mesh,domain->fn));
//	printf("ANGLE distorsion:%lf \n",ApproxAngleDistortion<MeshType>(*h_res_mesh));
//	/*if (!inside)
//		printf("Point out of parametrization during optimization \n");*/
//
//}

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

		OptType opt(*currMesh);
		opt.TargetCurrentGeometry();
		opt.SetBorderAsFixed();

		///save previous values
		InitDampRestUV(*currMesh);
		
		

		///NEW SETTING SPEED
		/*ScalarType edge_esteem=GetSmallestUVEdgeSize<MeshType>(*currMesh);
		ScalarType speed0=edge_esteem*0.01;
		ScalarType conv=edge_esteem*0.0005;*/
		ScalarType edge_esteem=GetSmallestUVHeight(*currMesh);
		

		ScalarType speed0=edge_esteem*0.1;
		ScalarType conv=edge_esteem*0.002;

		if (accuracy>1)
			conv*=1.0/(ScalarType)((accuracy-1)*10.0);

		opt.SetSpeed(speed0);
		/*int ite=*/opt.IterateUntilConvergence(conv);

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
//#ifndef _MESHLAB
//				printf("error in minimization... recovering...\n");
//#endif
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
			OrigVert->father=chosen;
			OrigVert->Bary=bary;
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

//void EndOptimization()
//{
//	///clear father and bary
//	for (unsigned int i=0;i<domain->face.size();i++)
//		domain->face[i].vertices_bary.resize(0);
//		
//	///set face-vertex link
//	for (unsigned int i=0;i<h_res_mesh->vert.size();i++)
//	{
//		BaseVertex *v=&h_res_mesh->vert[i];
//		BaseFace *f=v->father;
//		CoordType bary=v->Bary;
//		f->vertices_bary.push_back(std::pair<VertexType*,CoordType>(v,bary));
//	}	
//
//}

int accuracy;
vcg::CallBackPos *cb;
int step;

public:


	void Init(MeshType &_domain,
			  MeshType &_h_res_mesh,
			  vcg::CallBackPos *_cb,
			  int _accuracy=1)
	{
		step=0;
		cb=_cb;		
		accuracy=_accuracy;

		vcg::tri::UpdateNormals<MeshType>::PerFaceNormalized(_domain);

		domain=&_domain;
		h_res_mesh=&_h_res_mesh;
		
		/*subdomain.resize(h_res_mesh->vert.size());*/

		///get the average area per triangle
		/*ScalarType area=Area<MeshType>(_h_res_mesh);*/

		///get esteemation of average area per triangle
		//ScalarType average_area=area/(ScalarType)_h_res_mesh.vn;

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
		/*ScalarType distArea=ApproxAreaDistortion<BaseMesh>(*h_res_mesh,domain->fn);
		ScalarType distAngle=ApproxAngleDistortion<BaseMesh>(*h_res_mesh);
		ScalarType distAggregate=geomAverage<ScalarType>(distArea+1.0,distAngle+1.0,3,1)-1;
		printf("\n AREA  distorsion:%lf;\n ANGLE distorsion:%lf;\n AGGREGATE distorsion:%lf \n\n",distArea,distAngle,distAggregate);*/
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

	void Optimize(ScalarType gap=0.5)
	{
		/*int t0=clock();*/
		int k=0;
		/*int opt_type=0;*/
		ScalarType distArea=ApproxAreaDistortion<BaseMesh>(*h_res_mesh,domain->fn);
		ScalarType distAngle=ApproxAngleDistortion<BaseMesh>(*h_res_mesh);
		ScalarType distAggregate0=geomAverage<ScalarType>(distArea+1.0,distAngle+1.0,3,1)-1;
		PrintAttributes();
		bool ContinueOpt=true;
		PrintAttributes();
		PatchesOptimizer<BaseMesh> DomOpt(*domain,*h_res_mesh);
		step++;
		PrintAttributes();

		DomOpt.OptimizePatches();

	

		while (ContinueOpt)
		{
			///domain Optimization	
			k++;
			/*#ifndef _MESHLAB
			printf("\n DOING STAR\n");
			#endif*/
			InitStarSubdivision();
			MinimizeStep(0);
			//PrintAttributes();
		/*	#ifndef _MESHLAB
			printf("\n DOING DIAMOND\n");
			#endif*/
			InitDiamondSubdivision();
			MinimizeStep(1);
			//PrintAttributes();
			/*#ifndef _MESHLAB
			printf("\n DOING FACES\n");
			#endif*/
			InitFaceSubdivision();
			MinimizeStep(2);
			step++;
			PrintAttributes();

			distArea=ApproxAreaDistortion<BaseMesh>(*h_res_mesh,domain->fn);
			distAngle=ApproxAngleDistortion<BaseMesh>(*h_res_mesh);
			ScalarType distAggregate1=geomAverage<ScalarType>(distArea+1.0,distAngle+1.0,3,1)-1;
			ScalarType NewGap=((distAggregate0-distAggregate1)*100.0)/distAggregate0;
			/*#ifndef _MESHLAB
			PrintAttributes();
			printf("\n Step %d Gap %lf \n",k,NewGap);
			#endif*/
			if (NewGap<gap)
				ContinueOpt=false;
			distAggregate0=distAggregate1;
		}
		///*EndOptimization();*/
		//int t1=clock();
		//#ifndef _MESHLAB
		//printf("TIME OPTIMIZATION:%d \n",(t1-t0)/1000);
		//#endif
	}
	
	
};
#endif