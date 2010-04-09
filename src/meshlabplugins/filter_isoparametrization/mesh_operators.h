#ifndef MESH_OPERATORS
#define MESH_OPERATORS
#include <vcg/simplex/face/pos.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/edges.h>
#include <vector>
#include <map>

template <class MeshType>
void UpdateStructures(MeshType *mesh)
{
	vcg::tri::UpdateBounding<MeshType>::Box(*mesh);
	vcg::tri::UpdateNormals<MeshType>::PerFaceNormalized(*mesh);
	vcg::tri::UpdateNormals<MeshType>::PerVertexNormalized(*mesh);
	vcg::tri::UpdateTopology<MeshType>::FaceFace(*mesh);
  vcg::tri::UpdateTopology<MeshType>::TestFaceFace(*mesh);
	vcg::tri::UpdateTopology<MeshType>::VertexFace(*mesh);
	vcg::tri::UpdateEdges<MeshType>::Set(*mesh);
	vcg::tri::UpdateFlags<MeshType>::FaceBorderFromFF(*mesh);
	vcg::tri::UpdateFlags<MeshType>::VertexBorderFromFace(*mesh);
}

template <class MeshType>
void UpdateTopologies(MeshType *mesh)
{
	vcg::tri::UpdateTopology<MeshType>::FaceFace(*mesh);
	vcg::tri::UpdateTopology<MeshType>::VertexFace(*mesh);
	vcg::tri::UpdateFlags<MeshType>::FaceBorderFromFF(*mesh);
	vcg::tri::UpdateFlags<MeshType>::VertexBorderFromFace(*mesh);
}

template <class MeshType>
void FindNotBorderVertices(MeshType &mesh, 
													 std::vector<typename MeshType::VertexType*> &vertices)
{
	typename MeshType::VertexIterator Vi;
	for (Vi=mesh.vert.begin();Vi!=mesh.vert.end();Vi++)
		if ((!(*Vi).IsD())&&(!(*Vi).IsB()))
			vertices.push_back(&(*Vi));
}

//return moltiplication of aspect ratio of faces of the mesh
///to see if add + normalize is better
template <class MeshType>
typename MeshType::ScalarType AspectRatio(const MeshType &mesh)
{
	typedef typename MeshType::ScalarType ScalarType;
	ScalarType res=0;
	typename MeshType::ConstFaceIterator Fi;
	for (Fi=mesh.face.begin();Fi!=mesh.face.end();Fi++)
		if ((!(*Fi).IsD()))
			res+=vcg::QualityRadii((*Fi).P(0),(*Fi).P(1),(*Fi).P(2));
	//return res;
	return (res/(ScalarType)mesh.fn);
}

//template <class FaceType>
//ScalarType Area(const FaceType &f)
//{return ((f.P(1)-f.P(0))^(f.P(2)-f.P(0))).Norm();}

template <class FaceType>
typename FaceType::ScalarType AreaUV(const FaceType &f)
{
	typedef typename FaceType::ScalarType ScalarType;
	vcg::Point2<ScalarType> tex0=vcg::Point2<ScalarType>(f.V(0)->Bary.X(),f.V(0)->Bary.Y());
	vcg::Point2<ScalarType> tex1=vcg::Point2<ScalarType>(f.V(1)->Bary.X(),f.V(1)->Bary.Y());
	vcg::Point2<ScalarType> tex2=vcg::Point2<ScalarType>(f.V(2)->Bary.X(),f.V(2)->Bary.Y());
	ScalarType area=((tex1-tex0)^(tex2-tex0));
	return (area);
}

//return area
template <class MeshType>
typename MeshType::ScalarType Area(MeshType &mesh)
{
	typedef typename MeshType::ScalarType ScalarType;
	ScalarType res=0.0;
	for (unsigned int i=0;i<mesh.face.size();i++)
	{
		typename MeshType::FaceType *f=&mesh.face[i];
		if (!f->IsD())
		{
			ScalarType area=((f->P(1)-f->P(0))^(f->P(2)-f->P(0))).Norm();
			//ScalarType area=((f->V(1)->RPos-f->V(0)->RPos)^(f->V(2)->RPos-f->V(0)->RPos)).Norm();
			res+=area;
		}
	}
	return (res);
}

//return area 
template <class FaceType>
typename FaceType::ScalarType Area(std::vector<FaceType*> &faces)
{
	typedef typename FaceType::ScalarType ScalarType;
	ScalarType res=0.0;
	for (unsigned int i=0;i<faces.size();i++)
	{
		FaceType *f=faces[i];
		if (!f->IsD())
		{
			ScalarType area=((f->P(1)-f->P(0))^(f->P(2)-f->P(0))).Norm();
			res+=area;
		}
	}
	return (res);
}

////return dispersion as highest ratio between areas
//template <class MeshType>
//ScalarType Dispersion(MeshType &mesh)
//{
//	ScalarType res=0.0;
//	for (int i=0;i<mesh.face.size();i++)
//		for (int j=0;j<mesh.face.size();j++)
//		{
//			MeshType::FaceType *f0=&mesh.face[i];
//			MeshType::FaceType *f1=&mesh.face[j];
//			if ((i!=j)&&(!f0->IsD())&&(!f1->IsD()))
//			{
//				ScalarType area0=((f0->P(1)-f0->P(0))^(f0->P(2)-f0->P(0))).Norm();
//				ScalarType area1=((f1->P(1)-f1->P(0))^(f1->P(2)-f1->P(0))).Norm();
//				if ((area0/area1)>res)
//					res=(area0/area1);
//			}
//		}
//	return (res);
//}

//return dispersion as highest ratio between areas
template <class MeshType>
typename MeshType::ScalarType AreaDispersion(MeshType &mesh)
{
	typedef typename MeshType::ScalarType ScalarType;
	ScalarType area_tot=Area(mesh);
	ScalarType average=area_tot/(ScalarType)mesh.fn;
	ScalarType res=0;
	for (unsigned int i=0;i<mesh.face.size();i++)
	{
		typename MeshType::FaceType *f=&mesh.face[i];
		if ((!f->IsD()))
		{
			ScalarType area=((f->P(1)-f->P(0))^(f->P(2)-f->P(0))).Norm();
			//res+=std::max((area/average),(average/area));
			res+=pow((area-average),2);
		}
	}
	//return (res/(ScalarType)mesh.fn);
	return (res/pow(area_tot,2));
}

template <class FaceType>
void FindVertices(const std::vector<FaceType*> &faces, 
									std::vector<typename FaceType::VertexType*> &vertices)
{
	typedef typename FaceType::VertexType VertexType;

	typename std::vector<FaceType*>::const_iterator iteF;
	for (iteF=faces.begin();iteF!=faces.end();iteF++)
	{
		assert(!(*iteF)->IsD());
		for (int i=0;i<3;i++)
		{
			assert(!(*iteF)->V(i)->IsD());
			vertices.push_back((*iteF)->V(i));
		}
	}
	std::sort(vertices.begin(),vertices.end());
	typename std::vector<VertexType*>::iterator new_end=std::unique(vertices.begin(),vertices.end());
	int dist=distance(vertices.begin(),new_end);
	vertices.resize(dist);
}

template <class MeshType>
void FindSortedBorderVertices(const MeshType &/*mesh*/,
															typename MeshType::VertexType *Start,
															std::vector<typename MeshType::VertexType*> &vertices)
{
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;

	///find first half edge border
	vcg::face::VFIterator<FaceType> vfi(Start);
	FaceType *f=(vfi.F());
	int edge=(vfi.I());
	assert(f->V(edge)==Start);

	vcg::face::Pos<FaceType> pos=vcg::face::Pos<FaceType>(f,edge,Start);

	do
	pos.NextE();
	while(!pos.IsBorder());

	///then follow the border and put vertices into the vector
	do {
		assert(!pos.V()->IsD());
		vertices.push_back(pos.V());
		pos.NextB();
	}
	while (pos.V()!=Start);
}

template <class MeshType>
void CopyMeshFromFaces(const std::vector<typename MeshType::FaceType*> &faces,
											 std::vector<typename MeshType::VertexType*> &orderedVertex,
											 MeshType & new_mesh)
{
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;

	///get set of faces
	std::map<VertexType*,VertexType*> vertexmap;
	std::vector<typename FaceType::VertexType*> vertices;
	FindVertices(faces,vertices);

	///initialization of new mesh
	new_mesh.Clear();
	new_mesh.vn=0;
	new_mesh.fn=0;
	new_mesh.face.resize(faces.size());
	new_mesh.vert.resize(vertices.size());
	new_mesh.vn=vertices.size();
	new_mesh.fn=faces.size();

	///add new vertices
	typename std::vector<VertexType*>::const_iterator iteV;
	int i=0;
	for (iteV=vertices.begin();iteV!=vertices.end();iteV++)
	{
		///copy position
		assert(!(*iteV)->IsD());
		new_mesh.vert[i].P()=(*iteV)->P();
		new_mesh.vert[i].RPos=(*iteV)->RPos;
		new_mesh.vert[i].T().P()=(*iteV)->T().P();
		new_mesh.vert[i].N()=(*iteV)->N();
		/*assert(new_mesh.vert[i].brother!=NULL);*/
		//if (MeshType::Has_Auxiliary())
		new_mesh.vert[i].brother=(*iteV)->brother;
		new_mesh.vert[i].ClearFlags();

		orderedVertex.push_back((*iteV));
		vertexmap.insert(std::pair<VertexType*,VertexType*>((*iteV),&new_mesh.vert[i]));
		i++;
	}

	///setting of new faces
	typename std::vector<FaceType*>::const_iterator iteF;
	typename std::vector<FaceType>::iterator iteF1;
	for (iteF=faces.begin(),iteF1=new_mesh.face.begin()
		;iteF!=faces.end();iteF++,iteF1++)
	{
		(*iteF1).areadelta=(*iteF)->areadelta;
		/*	if ((*iteF1).areadelta>1)
		assert(0);*/
		///for each vertex get new reference
		///and associate face-vertex
		for (int j=0;j<3;j++)
		{
			VertexType* v=(*iteF)->V(j);
			typename std::map<VertexType*,VertexType*>::iterator iteMap=vertexmap.find(v);
			assert(iteMap!=vertexmap.end());
			(*iteF1).V(j)=(*iteMap).second;
		}
	}
}
template <class FaceType>
inline void getHresVertex(std::vector<FaceType*> &domain,
													std::vector<typename FaceType::VertexType*> &Hres)
{
	for (unsigned int i=0;i<domain.size();i++)
	{
		FaceType* f=domain[i];
		for (unsigned int j=0;j<f->vertices_bary.size();j++)
			if (f->vertices_bary[j].first->father==f)
				Hres.push_back(f->vertices_bary[j].first);
	}
}

///copy mesh low level & high level
///putting the link between them toghether
template <class MeshType>
void CopySubMeshLevels(std::vector<typename MeshType::FaceType*> &faces,
											 MeshType &Domain,MeshType &hlevMesh)
{
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;

	///create a copy of the domain
	std::vector<VertexType*> ordVertexD,ordVertexH;
	CopyMeshFromFaces<MeshType>(faces,ordVertexD,Domain);

	///update topologies
	UpdateTopologies(&Domain);

	///get the high resolution mesh
	std::vector<VertexType*> HresVert;
	getHresVertex<FaceType>(faces,HresVert);

	///copy mesh from vertices
	std::vector<FaceType*> OrderedFaces;
	CopyMeshFromVertices(HresVert,ordVertexH,OrderedFaces,hlevMesh);
	UpdateTopologies(&hlevMesh);

	///then set link son->father
	for (unsigned int i=0;i<hlevMesh.vert.size();i++)
	{
		FaceType *father=hlevMesh.vert[i].father;
		CoordType bary=hlevMesh.vert[i].Bary;
		///find position of father to map on the new domain
		typename std::vector<FaceType*>::iterator iteFath;
		iteFath=std::find(faces.begin(),faces.end(),father);
		if (iteFath!=faces.end())
		{
			int position=std::distance(faces.begin(),iteFath);
			AssingFather(hlevMesh.vert[i],&Domain.face[position],bary,Domain);
			///associate new father
			//hlevMesh.vert[i].father=&Domain.face[position];
			//assert(!Domain.face[position].IsD());

		}
	}

	///and set father to son link
	for (unsigned int i=0;i<Domain.face.size();i++)
		Domain.face[i].vertices_bary.clear();

	for (unsigned int i=0;i<hlevMesh.vert.size();i++)
	{
		VertexType *son=&hlevMesh.vert[i];
		FaceType *father=son->father;
		CoordType bary=son->Bary;
		father->vertices_bary.push_back(std::pair<VertexType *,vcg::Point3f>(son,bary));
	}
}
///return false if the the two vertices has no common faces,
/// it stores in <result> the intersection of the faces in v0 and v1, while in_v0 and in_v1 it returns
///the faces shared by each vertex
template <class MeshType>
inline bool getSharedFace(typename MeshType::VertexType *v0,
													typename MeshType::VertexType *v1,
													std::vector<typename MeshType::FaceType*> &result,
													std::vector<typename MeshType::FaceType*> &in_v0,
													std::vector<typename MeshType::FaceType*> &in_v1)
{
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;

	result.clear();
	result.reserve(2);
	vcg::face::VFIterator<FaceType> vfi0(v0); //initialize the iterator to the first vertex
	vcg::face::VFIterator<FaceType> vfi1(v1); //initialize the iterator to the first vertex
	vcg::face::VFIterator<FaceType> vfi2(v0); //initialize the iterator to the first vertex

	std::set<FaceType*> faces0;

  ///put faces in v0 in a <set>
	for(;!vfi0.End();++vfi0)
		faces0.insert(vfi0.F());

  ///put faces exclusively in v1 in <in_v1> in and build up the vector <result> containing the  intersection between both vertices
	for(;!vfi1.End();++vfi1)
		if (faces0.count(vfi1.F())!=0)
			result.push_back(vfi1.F());
		else
			in_v1.push_back(vfi1.F());

	///faces in v0
	bool non_shared=(result.size()==0);
	if (non_shared)
		return false;
	bool border=(result.size()==1);
	for(;!vfi2.End();++vfi2)
	{
			if ((!border)&&((result[0]!=vfi2.F())&&(result[1]!=vfi2.F())))
				in_v0.push_back(vfi2.F());
			else
				if ((border)&&((result[0]!=vfi2.F())))
					in_v0.push_back(vfi2.F());
	}
  return true;
}


template  <class MeshType>
inline void getSharedFace(std::vector<typename MeshType::VertexType*> &vertices,
													std::vector<typename MeshType::FaceType*> &faces)
{
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;

	typename std::vector<VertexType*>::const_iterator vi;

	for (vi=vertices.begin();vi!=vertices.end();vi++)
	{
		assert(!(*vi)->IsD());
		int num=0;
		vcg::face::VFIterator<FaceType> vfi(*vi);
		while (!vfi.End())
		{
			assert(!vfi.F()->IsD());
			faces.push_back(vfi.F());
			num++;
			++vfi;
		}
	}

	///sort and unique
	std::sort(faces.begin(),faces.end());
	typename std::vector<FaceType*>::iterator new_end=std::unique(faces.begin(),faces.end());
	int dist=distance(faces.begin(),new_end);
	faces.resize(dist);
}


///create a mesh considering just the faces that share all three vertex
template <class MeshType>
void CopyMeshFromVertices(std::vector<typename MeshType::VertexType*> &vertices,
													std::vector<typename MeshType::VertexType*> &OrderedVertices,
													std::vector<typename MeshType::FaceType*> &OrderedFaces,
													MeshType & new_mesh)
{
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;

	typename std::vector<VertexType*>::const_iterator iteV;
	for (iteV=vertices.begin();iteV!=vertices.end();iteV++)
		(*iteV)->ClearV();

	OrderedVertices.clear();

	///vertex-vertex reference
	std::map<VertexType*,VertexType*> vertexmap;

	///get set of faces
	std::vector<FaceType*> faces;

	getSharedFace<MeshType>(vertices,faces);

	///initialization of new mesh
	new_mesh.Clear();
	new_mesh.vn=0;
	new_mesh.fn=0;

	///set vertices as selected

	for (iteV=vertices.begin();iteV!=vertices.end();iteV++)
		(*iteV)->SetV();

	///getting inside faces
	typename std::vector<FaceType*>::const_iterator iteF;
	for (iteF=faces.begin();iteF!=faces.end();iteF++)
	{
		///for each vertex get new reference
		///if there isn't one reference means the face does not appartain to group
		VertexType* v0=(*iteF)->V(0);
		VertexType* v1=(*iteF)->V(1);
		VertexType* v2=(*iteF)->V(2);
		bool inside=((*v0).IsV()&&(*v1).IsV()&&(*v2).IsV());
		if (inside)
			OrderedFaces.push_back((*iteF));
	}

	///find internal vertices
	FindVertices(OrderedFaces,OrderedVertices);

	///setting size
	new_mesh.face.resize(OrderedFaces.size());
	new_mesh.vert.resize(OrderedVertices.size());
	new_mesh.vn=OrderedVertices.size();
	new_mesh.fn=OrderedFaces.size();

	///setting of internal vertices
	int i=0;
	typename std::vector<typename MeshType::VertexType*>::iterator iteVI;
	for (iteVI=OrderedVertices.begin();iteVI!=OrderedVertices.end();iteVI++)
	{
		///copy position
		assert(!(*iteVI)->IsD());
		new_mesh.vert[i].P()=(*iteVI)->P();
		new_mesh.vert[i].RPos=(*iteVI)->RPos;
		new_mesh.vert[i].T().P()=(*iteVI)->T().P();
		new_mesh.vert[i].father=(*iteVI)->father;
		assert(!(*iteVI)->father->IsD());
		new_mesh.vert[i].Bary=(*iteVI)->Bary;
		//new_mesh.vert[i].Damp=(*iteVI)->Damp;
		new_mesh.vert[i].RestUV=(*iteVI)->RestUV;
		new_mesh.vert[i].N()=(*iteVI)->N();
		new_mesh.vert[i].C()=(*iteVI)->C();
		new_mesh.vert[i].OriginalCol=(*iteVI)->OriginalCol;
		new_mesh.vert[i].ClearFlags();
		///map setting
		vertexmap.insert(std::pair<VertexType*,VertexType*>((*iteVI),&new_mesh.vert[i]));
		i++;
	}

	///setting of new faces
	typename std::vector<FaceType>::iterator iteF1;
	for (iteF=OrderedFaces.begin(),iteF1=new_mesh.face.begin()
		;iteF!=OrderedFaces.end();iteF++,iteF1++)
	{
		///for each vertex get new reference
		///and associate face-vertex
		for (int j=0;j<3;j++)
		{
			VertexType* v=(*iteF)->V(j);
			typename std::map<VertexType*,VertexType*>::iterator iteMap=vertexmap.find(v);
			assert(iteMap!=vertexmap.end());
			(*iteF1).V(j)=(*iteMap).second;
		}
	}

	///clear flags
	for (iteV=vertices.begin();iteV!=vertices.end();iteV++)
		(*iteV)->ClearV();

}

/////create a mesh considering the faces that share at leasts one vertex
//template <class MeshType>
//void CopyExtendedMeshFromVertices(const std::vector<typename MeshType::VertexType*> &vertices,
//								  std::vector<typename MeshType::VertexType*> &OrderedVertices,
//								  MeshType & new_mesh)
//{
//	std::vector<typename MeshType::FaceType*> faces;
//	getSharedFace(vertices,faces);
//	CopyMeshFromFaces(faces,new_mesh);
//}




template  <class MeshType>
inline void getSharedVertex(const std::vector<typename MeshType::FaceType*> &faces,
														std::vector<typename MeshType::VertexType*> &vertices)
{
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;

	typename std::vector<FaceType*>::const_iterator fi;
	for (fi=faces.begin();fi!=faces.end();fi++)
	{
		assert(!(*fi)->IsD());
		for (int j=0;j<3;j++)
			vertices.push_back((*fi)->V(j));
	}

	///sort and unique
	std::sort(vertices.begin(),vertices.end());
	typename std::vector<VertexType*>::iterator new_end=std::unique(vertices.begin(),vertices.end());
	int dist=distance(vertices.begin(),new_end);
	vertices.resize(dist);
}
//
//template <class MeshType>
//inline ScalarType GeoDesic()
//{
//
//
//}
template <class FaceType>
inline int EdgeIndex(const FaceType* test_face,
										 const typename FaceType::VertexType* v0,
										 const typename FaceType::VertexType* v1)
{
	///get edge index
	int edge_index=0;
	if (((test_face->cV(1)==v0)&&(test_face->cV(2)==v1))||
		((test_face->cV(2)==v0)&&(test_face->cV(1)==v1)))
		edge_index=1;
	else
		if (((test_face->cV(2)==v0)&&(test_face->cV(0)==v1))||
			((test_face->cV(0)==v0)&&(test_face->cV(2)==v1)))
			edge_index=2;
		else
			assert(((test_face->cV(0)==v0)&&(test_face->cV(1)==v1))||
			((test_face->cV(1)==v0)&&(test_face->cV(0)==v1)));
	return edge_index;
}

////ATTENTIOn to change if v0 is border
template <class MeshType>
inline void getVertexStar(typename MeshType::VertexType *v,
													std::vector<typename MeshType::VertexType*> &star)
{
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;

	assert(!v->IsB());
	vcg::face::VFIterator<FaceType> vfi(v);
	///get a face and an edge
	FaceType *f=vfi.F();
	int edge=vfi.I();
	vcg::face::Pos<FaceType> pos=vcg::face::Pos<FaceType>(f,edge,v);
	do
	{
		pos.FlipV();
		if (!pos.V()->IsD())
			star.push_back(pos.V());
		pos.FlipV();

		pos.NextE();
	}
	while (pos.F()!=f);
}

////ATTENTIOn to change if v0 is border
template <class MeshType>
inline void getSharedVertexStar(typename MeshType::VertexType *v0,
																typename MeshType::VertexType *v1,
																std::vector<typename MeshType::VertexType*> &shared)
{
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;
	typedef typename std::vector<VertexType*>::iterator iteVert;
	std::vector<VertexType*> star0;
	std::vector<VertexType*> star1;
	getVertexStar<MeshType>(v0,star0);
	getVertexStar<MeshType>(v1,star1);
	std::sort<iteVert>(star0.begin(),star0.end());
	std::sort<iteVert>(star1.begin(),star1.end());
	shared.resize(std::max(star0.size(),star1.size()));
	iteVert intersEnd=std::set_intersection<iteVert>(star0.begin(),star0.end(),star1.begin(),star1.end(),shared.begin());
	int dist=distance(shared.begin(),intersEnd);
	shared.resize(dist);
}

template <class MeshType>
inline typename MeshType::ScalarType StarAspectRatio(const std::vector<typename MeshType::VertexType*> &starCenters)
{
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;

	std::vector<typename MeshType::FaceType*> orderedFaces;
	getSharedFace<MeshType>(starCenters,orderedFaces);

	ScalarType res=0.0;

	typename std::vector<FaceType*>::iterator Fi;
	for (Fi=orderedFaces.begin();Fi!=orderedFaces.end();Fi++)
		res+=vcg::QualityRadii((*Fi)->P(0),(*Fi)->P(1),(*Fi)->P(2));

	return (res/(ScalarType)orderedFaces.size());
}

template <class MeshType>
inline typename MeshType::ScalarType StarDispersion(const std::vector<typename MeshType::VertexType*> &starCenters)
{
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;

	std::vector<typename MeshType::FaceType*> orderedFaces;
	getSharedFace<MeshType>(starCenters,orderedFaces);

	ScalarType average_area=0.0;

	typename std::vector<FaceType*>::iterator Fi;
	for (Fi=orderedFaces.begin();Fi!=orderedFaces.end();Fi++)
		average_area+=(((*Fi)->P(1)-(*Fi)->P(0))^((*Fi)->P(2)-(*Fi)->P(0))).Norm();

	average_area/=(ScalarType)orderedFaces.size();

	ScalarType res=0;
	for (Fi=orderedFaces.begin();Fi!=orderedFaces.end();Fi++)
	{
		ScalarType area=(((*Fi)->P(1)-(*Fi)->P(0))^((*Fi)->P(2)-(*Fi)->P(0))).Norm();
		res+=std::max((area/average_area),(average_area/area));
	}
	return (res/(ScalarType)orderedFaces.size());
}

template <class MeshType>
inline void CreateMeshVertexStar(std::vector<typename MeshType::VertexType*> &starCenters,
																 std::vector<typename MeshType::FaceType*> &orderedFaces,
																 MeshType &created)
{
	///get faces referenced by vertices
	std::vector<typename MeshType::VertexType*> orderedVertex;
	getSharedFace<MeshType>(starCenters,orderedFaces);
	CopyMeshFromFaces<MeshType>(orderedFaces,orderedVertex,created);
}

template <class MeshType>
inline void CreateMeshVertexStar(std::vector<typename MeshType::VertexType*> &starCenters,
																 std::vector<typename MeshType::FaceType*> &orderedFaces,
																 std::vector<typename MeshType::VertexType*> &orderedVertex,
																 MeshType &created)
{
	///get faces referenced by vertices
	getSharedFace<MeshType>(starCenters,orderedFaces);
	CopyMeshFromFaces<MeshType>(orderedFaces,orderedVertex,created);
}

template <class MeshType>
inline void getAroundFaceVertices(typename MeshType::VertexType *v0,
																	typename MeshType::VertexType *v1,
																	std::vector<typename MeshType::VertexType*> &result,
																	std::vector<typename MeshType::FaceType*> &on_edge,
																	std::vector<typename MeshType::FaceType*> &in_v0,
																	std::vector<typename MeshType::FaceType*> &in_v1)
{
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;

	getSharedFace(v0,v1,on_edge,in_v0,in_v1);

	std::set<VertexType*> Added;

	CoordType Center=CoordType(0,0,0);
	int num=0;

	///get all vertices around the collapse 
	for (int i=0;i<in_v0.size();i++)
	{
		for (int j=0;j<3;j++)
			if ((in_v0[i].V(j)!=v0)&&(in_v0[i].V(j)!=v1))
			{
				std::pair< std::set<VertexType*>, bool > done=Added.Add(in_v0[i].V(j));
				if (done.second)
					result.push_back(in_v0[i].V(j));
			}
	}

	///get all vertices around the collapse 
	for (int i=0;i<in_v0.size();i++)
	{
		for (int j=0;j<3;j++)
			if ((in_v1[i].V(j)!=v0)&&(in_v1[i].V(j)!=v1))
			{
				std::pair< std::set<VertexType*>, bool > done=Added.Add(in_v1[i].V(j));
				if (done.second)
					result.push_back(in_v1[i].V(j));
			}
	}
}

template <class MeshType>
inline void CopyHlevMesh(std::vector<typename MeshType::FaceType*> &faces,
												 MeshType &hlev_mesh,
												 std::vector<typename MeshType::VertexType*> &ordered_vertex)
{
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::CoordType CoordType;
	std::vector<VertexType*> vertices;

	///collect vertices to create the sub mesh
	for (unsigned int i=0;i<faces.size();i++)
	{
		FaceType *f=faces[i];
		for (unsigned int j=0;j<f->vertices_bary.size();j++)
		{
			VertexType *v=f->vertices_bary[j].first;
			vertices.push_back(v);
		}
	}
	std::vector<FaceType*> OrderedFaces;
	CopyMeshFromVertices<MeshType>(vertices,ordered_vertex,OrderedFaces,hlev_mesh);
}


#endif
