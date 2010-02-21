#include <algorithm>
#include <time.h>
#include <vcg/complex/trimesh/refine.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/space/color4.h>
#include <vcg/space/intersection2.h>

#ifndef _DIAMONDPARA
#define _DIAMONDPARA

class DiamondParametrizator
{
	typedef IsoParametrization::CoordType CoordType;
	typedef IsoParametrization::ScalarType ScalarType;

	IsoParametrization *isoParam;

	///data used for splitting
	typedef std::pair<int,int> EdgeKey;

	///interpolation data
	typedef struct InterpData
	{
		float alpha;
		int I;
		vcg::Point2<float> UV;
	};

	std::map<EdgeKey,InterpData> alphaMap;

	///data used to store an retrieve edges
	//typedef std::pair<AbstractFace*,int> TriEdge;
	//std::vector<DiamondPatch> DDAdiacency;
	//std::map<TriEdge,int> edgeMap;
	//int sampleSize;

	template <class FaceType>
	int AssignDiamond(FaceType *face)
	{
		ScalarType val=(ScalarType)1.0/(ScalarType)3.0;
		CoordType bary3d(val,val,val);
		int I_interp;
		vcg::Point2<ScalarType> UV_interp;
		isoParam->Phi(face,bary3d,I_interp,UV_interp);
		int D_interp=isoParam->getHDiamIndex(I_interp,UV_interp);
		face->WT(0).N()=D_interp;
		face->WT(1).N()=D_interp;
		face->WT(2).N()=D_interp;
		return D_interp;
	}

	///associate the diamond in which a face belongs to
	void AssociateDiamond()
	{
		ParamMesh *to_param=isoParam->ParaMesh();
		typedef ParamMesh::FaceType FaceType;
		
		///first step first associating initial faces to diamond
		for (unsigned int i=0;i<to_param->face.size();i++)
		{
			FaceType *curr=&to_param->face[i];
			AssignDiamond(curr);
			curr->C()=colorDiam[curr->WT(0).N()];
		}
	}

	void InterpEdge(const ParamFace *f,const int &index_edge,
					const float &alpha,int &I,vcg::Point2<ScalarType> &UV)
	{
		#ifndef NDEBUG
		float eps=0.00001f;
		#endif
		int index0=index_edge;
		int index1=(index_edge+1)%3;
		CoordType bary=CoordType(0,0,0);
		assert((alpha>=0)&&(alpha<=1));
		bary.V(index0)=alpha;
		bary.V(index1)=((ScalarType)1.0-alpha);
		isoParam->Phi(f,bary,I,UV);
		assert((UV.X()>=0)&&(UV.Y()>=0)&&(UV.X()<=1)&&(UV.Y()<=1)&&(UV.X()+UV.Y()<=1+eps));
	}

template <class FaceType>
void QuadCoord(FaceType * curr,const int &vert_num,vcg::Point2f &UVQuad,int &indexQuad)
{
	typedef typename FaceType::VertexType VertexType;
	typedef typename FaceType::ScalarType ScalarType;

	VertexType* v=curr->V(vert_num);
	
	int DiamIndex=curr->WT(0).N(); ///get index of diamond associated to the face
	assert((curr->WT(0).N()==curr->WT(1).N())&&(curr->WT(1).N()==curr->WT(2).N()));

	///transform to quad coordinates
	int I=v->T().N();
	vcg::Point2f UV=v->T().P();
	
	///transform in diamond coordinates
	vcg::Point2f UVDiam/*,UVQuad*/;
	isoParam->GE1(I,UV,DiamIndex,UVDiam);
	
	///transform in quad coordinates
	isoParam->GE1Quad(DiamIndex,UVDiam,UVQuad);

	indexQuad=DiamIndex;
	/*return (UVQuad);*/
}

template <class FaceType>
	bool To_Split(FaceType * curr,const float	&border,
				  bool to_split[3],InterpData Idata[3])
	{
		to_split[0]=false;
		to_split[1]=false;
		to_split[2]=false;

		typedef typename FaceType::VertexType VertexType;
		typedef typename FaceType::ScalarType ScalarType;

		/*ParamMesh *to_param=isoParam->ParaMesh();*/

		/*int DiamIndex=curr->WT(0).N(); *////get index of diamond associated to the face
		assert((curr->WT(0).N()==curr->WT(1).N())&&(curr->WT(1).N()==curr->WT(2).N()));
		
		vcg::Point2f UVQuad[3];
		int index[3];

		QuadCoord(curr,0,UVQuad[0],index[0]);
		QuadCoord(curr,1,UVQuad[1],index[1]);
		QuadCoord(curr,2,UVQuad[2],index[2]);

		///outern border
		vcg::Box2<ScalarType> bbox,bbox0;
		bbox.Add(vcg::Point2f(-border,-border));
		bbox.Add(vcg::Point2f(1+border,1+border));
		bbox0.Add(vcg::Point2f(0,0));
		bbox0.Add(vcg::Point2f(1,1));
		
		if (bbox.IsIn(UVQuad[0])&&bbox.IsIn(UVQuad[1])&&bbox.IsIn(UVQuad[2]))
			return false; ///no intersection is possible

		///else test which edges must be splitted		
		//vcg::Segment2<float> border_seg[4];
		vcg::Line2<float> border_seg[4];
		border_seg[0].Set(vcg::Point2f(0,0),vcg::Point2f(1,0));
		border_seg[1].Set(vcg::Point2f(1,0),vcg::Point2f(0,1));
		border_seg[2].Set(vcg::Point2f(0,1),vcg::Point2f(1,0));
		border_seg[3].Set(vcg::Point2f(0,0),vcg::Point2f(0,1));
		bool intersected=false;
		for (int edge=0;edge<3;edge++)
		{	
			vcg::Segment2<float> curr_edge=vcg::Segment2<float>(UVQuad[edge],UVQuad[(edge+1)%3]);
			float dist_medium=1.0;
			for (int j=0;j<4;j++)
			{
				vcg::Point2f p_inters;
				vcg::Line2<float> curr_border=border_seg[j];
				bool intersect=LineSegmentIntersection(curr_border,curr_edge,p_inters);

				float l_test0=(curr_edge.P0()-p_inters).Norm();
				float l_test1=(curr_edge.P1()-p_inters).Norm();
				float l_test=std::min(l_test0,l_test1);
	
				const ScalarType _EPS=(ScalarType)0.0001;
				if ((intersect)&&(l_test>=_EPS))
				{
					float lenght=curr_edge.Length();
					float dist=((curr_edge.P0()-p_inters).Norm());
					float Ndist=dist/lenght;
					float alpha=1.0-Ndist;
					float dist_medium1=fabs(alpha-0.5);
					
					if (dist_medium1<dist_medium)
					{
						dist_medium=dist_medium1;
						int I;
						vcg::Point2f UV;
						InterpEdge(curr,edge,alpha,I,UV);
						Idata[edge].alpha=alpha;
						Idata[edge].I=I;
						Idata[edge].UV=UV;
						to_split[edge]=true;
						intersected=true;
					}
				}
			}
		}
		if(!intersected)
			assert(0);
		return(intersected);
	}

	// Basic subdivision class
	// This class must provide methods for finding the position of the newly created vertices
	// In this implemenation we simply put the new vertex in the MidPoint position.
	// Color and TexCoords are interpolated accordingly.
	template<class MESH_TYPE>
	struct SplitMidPoint : public   std::unary_function<vcg::face::Pos<typename MESH_TYPE::FaceType> ,  typename MESH_TYPE::CoordType >
	{
		typedef typename MESH_TYPE::VertexType VertexType;
		typedef typename MESH_TYPE::FaceType FaceType;
		typedef typename MESH_TYPE::CoordType CoordType;

		std::map<EdgeKey,InterpData> *alphaMap;
		IsoParametrization *isoParam;



		void operator()(typename MESH_TYPE::VertexType &nv, vcg::face::Pos<typename MESH_TYPE::FaceType>  ep)
		{
			//printf("DONE\n");

			ParamMesh *to_param=isoParam->ParaMesh();

			///get eth value on which the edge must be splitted
			VertexType* v0=ep.f->V(ep.z);
			VertexType* v1=ep.f->V1(ep.z);

			int i0=IndexFromPointer(v0,to_param);
			int i1=IndexFromPointer(v1,to_param);

			assert(v0!=v1);
			EdgeKey k;
			int index0=ep.z;
			int index1=(ep.z+1)%3;

			if (i0>i1)
			{
				std::swap(v0,v1);
				std::swap(i0,i1);
				std::swap(index0,index1);
			}

			k=EdgeKey(i0,i1);
			std::map<EdgeKey,InterpData>::iterator ItE=alphaMap->find(k);
			assert(ItE!=alphaMap->end());
			InterpData interp=(*ItE).second;
			float alpha=interp.alpha;
			assert((alpha>=0)&&(alpha<=1));
			nv.P()= v0->P()*alpha+v1->P()*(1.0-alpha);
			nv.RPos= v0->RPos*alpha+v1->RPos*(1.0-alpha);

			if( MESH_TYPE::HasPerVertexNormal())
				nv.N()=v0->N()*alpha+v1->N()*((ScalarType)1.0-alpha);
			if( MESH_TYPE::HasPerVertexColor())
			{
				CoordType color=CoordType(v0->C().X(),v0->C().Y(),v0->C().Z());
				color=color*alpha+color*((ScalarType)1.0-alpha);
				nv.C()=vcg::Color4b((unsigned char)color.X(),(unsigned char)color.Y(),(unsigned char)color.Z(),(unsigned char)255);
			}

			nv.T().N()=interp.I;
			nv.T().P()=interp.UV;
		}

		vcg::TexCoord2<float> WedgeInterp(vcg::TexCoord2<float> &t0, vcg::TexCoord2<float> &t1)
		{
			vcg::TexCoord2<float> tmp;
			assert(t0.n()== t1.n());
			tmp.n()=t0.n(); 
			tmp.t()=(t0.t()+t1.t())/2.0;
			return tmp;
		}
	};



	template <class MESH_TYPE>//, class FLT>
	class EdgePredicate
	{
		typedef typename MESH_TYPE::VertexType VertexType;
		typedef typename MESH_TYPE::FaceType FaceType;
	public:
		std::map<EdgeKey,InterpData> *alphaMap;
		IsoParametrization *isoParam;

		bool operator()(vcg::face::Pos<typename MESH_TYPE::FaceType> ep) const
		{
			ParamMesh *to_param=isoParam->ParaMesh();

			VertexType* v0=ep.f->V(ep.z);
			VertexType* v1=ep.f->V1(ep.z);

			int i0=IndexFromPointer(v0,to_param);
			int i1=IndexFromPointer(v1,to_param);

			assert(v0!=v1);
			EdgeKey k;

			if (i0>i1)
			{
				std::swap(v0,v1);
				std::swap(i0,i1);
			}

			k=EdgeKey(i0,i1);
			std::map<EdgeKey,InterpData>::iterator ItE=alphaMap->find(k);
			bool to_split=(ItE!=alphaMap->end());
			return (to_split);
		}
	};

	static int IndexFromPointer(ParamVertex * v,ParamMesh *mesh)
	{
		int index=v-&(*mesh->vert.begin());
		return (index);
	}

	void InsertInterpData(ParamFace *curr,const int &edge,
						  ParamMesh *to_param,InterpData &Idata)
	{
		ParamVertex *v0=curr->V(edge);
		ParamVertex *v1=curr->V1(edge);
		int i0=IndexFromPointer(v0,to_param);
		int i1=IndexFromPointer(v1,to_param);
		if (i0>i1)
		{
			std::swap(v0,v1);
			std::swap(i0,i1);
			Idata.alpha=(ScalarType)1.0-Idata.alpha;
			assert((Idata.alpha>=0)&&(Idata.alpha<=1));
		}	
		EdgeKey k=EdgeKey(i0,i1);
		std::map<EdgeKey,InterpData>::iterator ItE=alphaMap.find(k);
		if(ItE!=alphaMap.end())
		{
			if (fabs((*ItE).second.alpha-0.5)>fabs(Idata.alpha-0.5))
			{
				(*ItE).second.alpha=Idata.alpha;
				(*ItE).second.I=Idata.I;
				(*ItE).second.UV=Idata.UV;
			}	
		}
		else
			alphaMap.insert(std::pair<EdgeKey,InterpData>(k,Idata));
	}
	
	bool Split(const ScalarType &border)
	{
		alphaMap.clear();

		ParamMesh *to_param=isoParam->ParaMesh();
		///copy paramesh
		

		

		typedef ParamMesh::VertexType VertexType;
		typedef ParamMesh::FaceType FaceType;
		SplitMidPoint<ParamMesh> splMd;
		EdgePredicate<ParamMesh> eP;

		///second step.. test the split if needed
		for (unsigned int i=0;i<to_param->face.size();i++)
		{
			///get the current face and test the edges
			FaceType * curr=&to_param->face[i];
			InterpData Idata[3];
			bool to_split[3];
			bool is_out=To_Split<FaceType>(curr,border,to_split,Idata);
			if (is_out){
				for (int edge=0;edge<3;edge++)
					if (to_split[edge])
						InsertInterpData(curr,edge,to_param,Idata[edge]);
			}
			
		}
		splMd.isoParam=isoParam;
		splMd.alphaMap=&alphaMap;
		eP.isoParam=isoParam;
		eP.alphaMap=&alphaMap;

		#ifndef _MESHLAB
		int f0=to_param->fn;
		#endif
		bool done=vcg::RefineE<ParamMesh,SplitMidPoint<ParamMesh>,EdgePredicate<ParamMesh> >(*to_param,splMd,eP);
		#ifndef _MESHLAB
		printf("FACE ADDED  %d \n",to_param->fn-f0);
		#endif
		return done;
		
	}
	
	void SetWedgeCoords(const ScalarType &border)
	{
		typedef ParamMesh::VertexType VertexType;
		typedef ParamMesh::FaceType FaceType;
	
		ParamMesh *to_param=isoParam->ParaMesh();
		int edge_size=(int)ceil(sqrt((ScalarType)num_diamonds));
		ScalarType edgedim=1.0/(ScalarType)edge_size;
		for (unsigned int i=0;i<to_param->face.size();i++)
		{
			///get the current face and test the edges
			FaceType * curr=&to_param->face[i];
			for (int j=0;j<3;j++)
			{
				vcg::Point2f QCoord;
				vcg::Point2i IntCoord;
				int index;
				QuadCoord(curr,j,QCoord,index);
				IntCoord.X()=index/edge_size;
				IntCoord.Y()=index%edge_size;
				///transform from [ -border,1 + border] to [0,1]
				QCoord+=vcg::Point2f(border,border);
				QCoord/=(ScalarType)1.0+(ScalarType)2.0*border;
				assert((QCoord.X()>=0)&&(QCoord.X()<=1)&&(QCoord.Y()>=0)&&(QCoord.Y()<=1));

				QCoord*=edgedim;
				QCoord.X()+=edgedim*(ScalarType)IntCoord.X();
				QCoord.Y()+=edgedim*(ScalarType)IntCoord.Y();
				assert(QCoord.X()<=1);
				assert(QCoord.Y()<=1);
				curr->WT(j).P()=QCoord;
				///and finally set for global texture coords
			}
		}
	}

	int num_diamonds;

public:
	
	std::vector<vcg::Color4b > colorDiam;

	///initialize the parameterization
	void Init(IsoParametrization *_isoParam)
	{
	
		isoParam=_isoParam;

		///COUNT THE NUMBER OF EDGES
		num_diamonds=0;
		for (unsigned int i=0;i<isoParam->AbsMesh()->face.size();i++)
		{
			AbstractFace *f=&isoParam->AbsMesh()->face[i];
			for (int j=0;j<3;j++)
				if (f->FFp(j)<f)
					num_diamonds++;
		}

		colorDiam.resize(num_diamonds);
		srand(clock());
		for (unsigned int i=0;i<colorDiam.size();i++)
			colorDiam[i]=vcg::Color4b(rand()%255,rand()%255,rand()%255,255);

	}

	//void Draw()
	//{
	//	ParamMesh *to_param=isoParam->ParaMesh();
	//	//vcg::tri::UpdateNormals<ParamMesh>::PerFaceNormalized(*to_param);
	//	glDepthRange(0.01,1.0);
	//	glEnable(GL_LIGHTING);
	//	glEnable(GL_LIGHT0);
	//	glEnable(GL_NORMALIZE);
	//	glBegin(GL_TRIANGLES);
	//	for (int i=0;i<to_param->face.size();i++)
	//	{

	//		/*vcg::glColor(to_param->face[i].C());*/

	//		CoordType p0=to_param->face[i].V(0)->P();
	//		CoordType p1=to_param->face[i].V(1)->P();
	//		CoordType p2=to_param->face[i].V(2)->P();
	//		CoordType norm=(p1-p0)^(p2-p0);
	//		norm.Normalize();
	//		vcg::glNormal(norm);
	//		vcg::Point2f t0=to_param->face[i].WT(0).P();
	//		vcg::Point2f t1=to_param->face[i].WT(1).P();
	//		vcg::Point2f t2=to_param->face[i].WT(2).P();
	//		vcg::Color4b c0,c1,c2;
	//		if ((t0.X()< 0)||(t0.Y()< 0)||(t0.X()>1.0)||(t0.Y()>1.0))
	//		c0=vcg::Color4b(0,0,255,255);
	//		else
	//		c0=vcg::Color4b(t0.X()*255.0,t0.Y()*255.0,0,255);
	//		if ((t1.X()< 0)||(t1.Y()< 0)||(t1.X()>1.0)||(t1.Y()>1.0))
	//		c1=vcg::Color4b(0,0,255,255);
	//		else
	//		c1=vcg::Color4b(t1.X()*255.0,t1.Y()*255.0,0,255);
	//		if ((t2.X()< 0)||(t2.Y()< 0)||(t2.X()>1.0)||(t2.Y()>1.0))
	//		c2=vcg::Color4b(0,0,255,255);
	//		else
	//		c2=vcg::Color4b(t2.X()*255.0,t2.Y()*255.0,0,255);
	//		vcg::glColor(c0);
	//		vcg::glVertex(to_param->face[i].V(0)->P());
	//		vcg::glColor(c1);
	//		vcg::glVertex(to_param->face[i].V(1)->P());
	//		vcg::glColor(c2);
	//		vcg::glVertex(to_param->face[i].V(2)->P());
	//	}
	//	glEnd();

	//	/*glDepthRange(0,0.99999);
	//	glDisable(GL_LIGHTING);
	//	glDisable(GL_LIGHT0);
	//	glDisable(GL_NORMALIZE);
	//	glLineWidth(1);
	//	glColor3d(0,0,0);
	//	for (int i=0;i<to_param->face.size();i++)
	//	{
	//		glBegin(GL_LINE_LOOP);
	//		CoordType p0=to_param->face[i].V(0)->P();
	//		CoordType p1=to_param->face[i].V(1)->P();
	//		CoordType p2=to_param->face[i].V(2)->P();
	//		vcg::glVertex(to_param->face[i].V(0)->P());
	//		vcg::glVertex(to_param->face[i].V(1)->P());
	//		vcg::glVertex(to_param->face[i].V(2)->P());
	//		glEnd();
	//	}*/
	//	
	//	glDepthRange(0.0,1.0);
	//}

	

	///set the vertex coordinates
	template <class MeshType>
	void SetCoordinates(MeshType &mesh,const ScalarType &border=0.01)
	{
		std::vector<vcg::Color4b > colorDiam;
		
		//ParamMesh *to_param=isoParam->ParaMesh();

		typedef ParamMesh::FaceType FaceType;
		typedef ParamMesh::VertexType VertexType;

		bool done=true;
		/*int n0=to_param->fn;*/
		while (done)
		{
			AssociateDiamond();
			done=Split(border);
			isoParam->Update();
		}

		AssociateDiamond();
		SetWedgeCoords(border);

		///copy parametrization to the new mesh
		mesh.Clear();
		vcg::tri::Append<MeshType,ParamMesh>::Mesh(mesh,*isoParam->ParaMesh(),false,true);
		
	}

};
#endif