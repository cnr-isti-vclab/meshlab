

#include "defines.h"
///fitting
#include <vcg/space/fitting3.h>
#include <vcg/math/matrix33.h>
#include <vcg/space/triangle2.h>
#include "texcoord_optimization.h"
#include "mesh_operators.h"

#ifndef LOCAL_PARAMETRIZATION
#define LOCAL_PARAMETRIZATION

//#include <vcg/complex/trimesh/point_sampling.h>

//#define samples_area 80

template <class MeshType>
void ParametrizeExternal(MeshType &to_parametrize)
{
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::VertexType VertexType;

        std::vector<VertexType*> vertices;
	
	///find first border vertex
        VertexType* Start=NULL;
        typename MeshType::VertexIterator Vi=to_parametrize.vert.begin();
	
	while ((Start==NULL)&&(Vi<to_parametrize.vert.end()))
	{
		if (((*Vi).IsB())&&(!(*Vi).IsD()))
			Start=&(*Vi);
		Vi++;
	}
	if (Vi==to_parametrize.vert.end())
	{
		assert(0);
	}
	///get sorted border vertices
	
	FindSortedBorderVertices<MeshType>(to_parametrize,Start,vertices);

	//assert(vertices.size()>=4);
	
	///find perimeter
	ScalarType perimeter=0;
	int size=vertices.size();
	for (int i=0;i<size;i++)
		perimeter+=(vertices[i]->P()-vertices[(i+1)%size]->P()).Norm();
	
	///find scaling factor
	/*ScalarType Sperimeter=(2.0*M_PI)/perimeter;*/
	
	///set default texCoords
	for (Vi=to_parametrize.vert.begin();Vi!=to_parametrize.vert.end();Vi++)
	{
		(*Vi).T().U()=-2;
		(*Vi).T().V()=-2;
	}

	///set border vertices
        typename std::vector<VertexType*>::iterator iteV;
	/*ScalarType curr_perim=0;*/
	ScalarType curr_angle=0;
	vertices[0]->T().U()=cos(curr_angle);
	vertices[0]->T().V()=sin(curr_angle);
	//for (int i=1;i<vertices.size();i++)
	//{
	//	curr_perim+=(vertices[i]->P()-vertices[(i-1)]->P()).Norm();
	//	//curr_perim+=perimeter/(ScalarType)size;
	//	curr_angle=curr_perim*Sperimeter;
	//	vertices[i]->T().U()=cos(curr_angle);
	//	vertices[i]->T().V()=sin(curr_angle);
	//	assert((vertices[i]->T().U()>=-1)&&(vertices[i]->T().U()<=1));
	//	assert((vertices[i]->T().V()>=-1)&&(vertices[i]->T().V()<=1));
	//}
	ScalarType anglediv=(2.0*M_PI)/(ScalarType)(vertices.size());
	curr_angle=0;
	for (unsigned int i=1;i<vertices.size();i++)
	{
		curr_angle+=anglediv;
		vertices[i]->T().U()=cos(curr_angle);
		vertices[i]->T().V()=sin(curr_angle);
		assert((vertices[i]->T().U()>=-1)&&(vertices[i]->T().U()<=1));
		assert((vertices[i]->T().V()>=-1)&&(vertices[i]->T().V()<=1));
	}
}

template <class MeshType>
void ParametrizeInternal(MeshType &to_parametrize)
{
	typedef typename MeshType::ScalarType ScalarType;
	const ScalarType Eps=(ScalarType)0.0001;
	///set internal vertices
        for (typename MeshType::VertexIterator Vi=to_parametrize.vert.begin();Vi!=to_parametrize.vert.end();Vi++)
	{
		//assert(!Vi->IsD());
		if ((!Vi->IsB())&&(!Vi->IsD()))
		{
			///find kernel
                        std::vector<typename MeshType::VertexType*> star;
			getVertexStar<MeshType>(&(*Vi),star);
			ScalarType kernel=0;
			for (unsigned int k=0;k<star.size();k++)
				if (star[k]->IsB())
				{
					ScalarType dist=((*Vi).P()-star[k]->P()).Norm();
					if (dist<Eps)
						dist=Eps;
					//ScalarType peso=1.0/(dist);
					ScalarType peso=(dist)/star.size();
					kernel+=(peso);//*dist);
				}
			assert(kernel>0);
			///then find factor
			kernel=1.0/kernel;       

			(*Vi).T().U()=0;
			(*Vi).T().V()=0;
			
			int num=0;
			///find weighted media
			for (unsigned int k=0;k<star.size();k++)
				if (star[k]->IsB())
				{
					ScalarType dist=((*Vi).P()-star[k]->P()).Norm();
					if (dist<Eps)
						dist=Eps;
					//ScalarType peso=1.0/(dist);
					ScalarType peso=(dist)/star.size();
					ScalarType kval=(peso)*kernel;
					assert(kval>0);
					(*Vi).T().U()+=kval*star[k]->T().U();
					(*Vi).T().V()+=kval*star[k]->T().V();
					num++;
				}
			////on border case 2 neighbors
			///go next to the center
			/*if (num==2)
			{
				(*Vi).T().U()/=2.0;
				(*Vi).T().V()/=2.0;
			}*/
			/*ScalarType u=(*Vi).T().U();
			ScalarType v=(*Vi).T().V();*/
			assert(((*Vi).T().U()>=-1)&&((*Vi).T().U()<=1));
			assert(((*Vi).T().V()>=-1)&&((*Vi).T().V()<=1));
		}
	}
	///smoothing of txcoords
	InitDampRestUV(to_parametrize);
        for (typename MeshType::VertexIterator Vi=to_parametrize.vert.begin();Vi!=to_parametrize.vert.end();Vi++)
	{
		if ((!Vi->IsB())&&(!Vi->IsD()))
		{
                        std::vector<typename MeshType::VertexType*> star;
			getVertexStar<MeshType>(&(*Vi),star);
			vcg::Point2<ScalarType> UV=vcg::Point2<ScalarType>(0,0);
			for (unsigned int k=0;k<star.size();k++)
				UV+=star[k]->RestUV;
			UV/=(ScalarType)star.size();
			(*Vi).T().P()=UV;
		}
	}
}


template <class FaceType>
typename FaceType::CoordType InterpolatePos
(FaceType* f, const typename FaceType::CoordType &bary)
{return (f->V(0)->P()*bary.X()+f->V(1)->P()*bary.Y()+f->V(2)->P()*bary.Z());}

template <class FaceType>
typename FaceType::CoordType InterpolateRPos
(FaceType* f,const typename FaceType::CoordType &bary)
{	
	return (f->V(0)->RPos*bary.X()+f->V(1)->RPos*bary.Y()+f->V(2)->RPos*bary.Z());
}

template <class FaceType>
typename FaceType::CoordType InterpolateNorm
(FaceType* f, const typename FaceType::CoordType &bary)
{	
        typedef typename FaceType::CoordType CoordType;
	CoordType n0=f->V(0)->N();
	CoordType n1=f->V(1)->N();
	CoordType n2=f->V(2)->N();
	return (n0*bary.X()+n1*bary.Y()+n2*bary.Z());
}

template <class ScalarType>
int Approx(const ScalarType &value)
{
	ScalarType val0=floor(value);
	ScalarType val1=ceil(value);
	if (fabs(val0-value)<fabs(val1-value))
		return ((int)val0);
	else
		return ((int)val1);
}

template <class FaceType>
vcg::Point3i InterpolateColor
(FaceType* f,const typename FaceType::CoordType &bary)
{	
        typedef typename FaceType::ScalarType ScalarType;
	vcg::Color4b c0=f->V(0)->C();
	vcg::Color4b c1=f->V(1)->C();
	vcg::Color4b c2=f->V(2)->C();
	double R=(ScalarType)c0.X()*bary.X()+(ScalarType)c1.X()*bary.Y()+(ScalarType)c2.X()*bary.Z();
	double G=(ScalarType)c0.Y()*bary.X()+(ScalarType)c1.Y()*bary.Y()+(ScalarType)c2.Y()*bary.Z();
	double B=(ScalarType)c0.Z()*bary.X()+(ScalarType)c1.Z()*bary.Y()+(ScalarType)c2.Z()*bary.Z();

	vcg::Point3i p=vcg::Point3i(Approx(R),Approx(G),Approx(B));

	assert(p.X()<=255);
	assert(p.Y()<=255);
	assert(p.Z()<=255);

	return (p);
}

template <class VertexType>
typename VertexType::CoordType ProjectPos(const VertexType &v)
{
	typedef typename VertexType::FaceType FaceType;
	typedef typename VertexType::CoordType CoordType;

	FaceType *f=v.father;
	CoordType b=v.Bary;
	return (InterpolatePos<FaceType>(f,b));
}

template <class VertexType>
typename VertexType::CoordType Warp(const VertexType* v)
{
        typename VertexType::FaceType * father=v->father;
        typename VertexType::CoordType proj=father->P(0)*v->Bary.X()+father->P(1)*v->Bary.Y()+father->P(2)*v->Bary.Z();
	return proj;
}

template <class VertexType>
typename VertexType::CoordType WarpRpos(const VertexType* v)
{
        typename VertexType::FaceType * father=v->father;
        typename VertexType::CoordType proj=father->V(0)->RPos*v->Bary.X()+father->V(1)->RPos*v->Bary.Y()+father->V(2)->RPos*v->Bary.Z();
	return proj;
}

template <class MeshType>
typename MeshType::ScalarType EstimateLenghtByParam
								(const typename MeshType::VertexType* v0,
								 const typename MeshType::VertexType* v1,
								 typename MeshType::FaceType* on_edge[2])
{
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::ScalarType ScalarType;

//	assert((on_edge.size()==0)||(on_edge.size()==1));
	ScalarType estimated[2]={0,0};
	int num[2]={0,0};
	
	for (int i=0;i<2;i++)
	{
		FaceType *test_face=on_edge[i];

		int edge_index=EdgeIndex(test_face,v0,v1);
		FaceType *Fopp=test_face->FFp(edge_index);

		if (test_face->vertices_bary.size()<2)
		{
			ScalarType dist=Distance(v0->RPos,v1->RPos);
			//#pragma omp atomic
			estimated[i]+=dist;
			num[i]=0;
			continue;
		}

		///collect vertices
		std::vector<VertexType*> vertices;
		vertices.reserve(test_face->vertices_bary.size());
		for (unsigned int k=0;k<test_face->vertices_bary.size();k++)
			vertices.push_back(test_face->vertices_bary[k].first);

		
		///collect faces
		std::vector<FaceType*> faces;
		getSharedFace<MeshType>(vertices,faces);

		///get border edges
		std::vector<std::pair<VertexType*,VertexType*> > edges;
		for (unsigned int j=0;j<faces.size();j++)
		{
			FaceType*f=faces[j];
			///find if there is an on-edge edge
			bool found=false;
			int k=0;
			while  ((k<3)&&(!found))
			{
				if ((f->V0(k)->father==test_face)&&
					(f->V1(k)->father==test_face)&&
					(f->V2(k)->father==Fopp))
				{
					edges.push_back(std::pair<VertexType*,VertexType*>(f->V0(k),f->V1(k)));
					found=true;
				}
				k++;
			}
		}
		
		///find if there's ant edge return inital lenght
		if (edges.size()==0)
		{
			estimated[i]+=(Distance(v0->RPos,v1->RPos));
			num[i]=0;
			continue;
		}
		else
		{
			//get edge direction
			///store the two nearest for each vertex
			/*VertexType *n0=edges[0].first;
			VertexType *n1=edges[0].second;
			ScalarType d0=(Warp(n0)-v0->P()).Norm();
			ScalarType d1=(Warp(n1)-v1->P()).Norm();*/

			//CoordType edgedir=v0->cP()-v1->cP();
			CoordType edgedir=v0->RPos-v1->RPos;
			edgedir.Normalize();
			num[i]=edges.size();
			for (unsigned int e=0;e<edges.size();e++)
			{
				VertexType *vH0=edges[e].first;
				VertexType *vH1=edges[e].second;

				///project points over the plane 
				/*CoordType proj0=Warp(vH0);
				CoordType proj1=Warp(vH1);*/
				CoordType proj0=WarpRpos(vH0);
				CoordType proj1=WarpRpos(vH1);
				
				CoordType dirproj=proj0-proj1;
				dirproj.Normalize();
				//estimated[i]+=fabs(dirproj*edgedir)*((vH0->P()-vH1->P()).Norm());
				//#pragma omp atomic
				estimated[i]+=fabs(dirproj*edgedir)*((vH0->RPos-vH1->RPos).Norm());

			}
		}
	}

	///media of estimated values
	ScalarType alpha0,alpha1;
	ScalarType max_num=abstraction_num;

	if (num[0]>=max_num)
		alpha0=1;
	else
		alpha0=num[0]/max_num;

	if (num[1]>=max_num)
		alpha1=1;
	else
		alpha1=num[1]/max_num;

	estimated[0]=alpha0*estimated[0]+(1.0-alpha0)*(Distance(v0->RPos,v1->RPos));
	estimated[1]=alpha1*estimated[1]+(1.0-alpha1)*(Distance(v0->RPos,v1->RPos));
	return((estimated[0]+estimated[1])/2.0);
}

template <class MeshType>
void MeanVal(const std::vector<vcg::Point2<typename MeshType::ScalarType> > &Points,
			 std::vector<typename MeshType::ScalarType> &Lamda,
			 typename MeshType::CoordType &p)
{
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::ScalarType ScalarType;
        typedef typename MeshType::CoordType CoordType;

	int size=Points.size();
	Lamda.resize(size);

	ScalarType sum=0;	
	for (int i=0;i<size;i++)
	{
		int size=Points.size()-1;
		vcg::Point2<ScalarType> Pcurr=Points[i];
		vcg::Point2<ScalarType> Pprev=Points[(i+(size-1))%size];
		vcg::Point2<ScalarType> Pnext=Points[(i+1)%size];

		CoordType v0=Pprev-p;
		CoordType v1=Pcurr-p;
		CoordType v2=Pnext-p;
		ScalarType l=v1.Norm();
		v0.Normalize();
		v1.Normalize();
		v2.Normalize();

		ScalarType Alpha0=acos(v0*v1);
		ScalarType Alpha1=acos(v1*v2);

                Lamda[i]=(tan(Alpha0/2.0)+tan(Alpha1/2.0))/l;
                sum+=Lamda[i];
	}

	///normalization
	for (int i=0;i<size;i++)
                Lamda[i]/=sum;
}

template <class FaceType>
typename FaceType::ScalarType EstimateAreaByParam(const FaceType* f)
{
	typedef typename FaceType::VertexType VertexType;
	typedef typename FaceType::ScalarType ScalarType;
	
	int num=0;
	ScalarType estimated=0;
	for (unsigned int k=0;k<f->vertices_bary.size();k++)
	{
		VertexType *HresVert=f->vertices_bary[k].first;
		estimated+=HresVert->area;
		num++;
	}

	///media of estimated values
	ScalarType alpha;
	ScalarType max_num=abstraction_num;

	if (num>=max_num)
		alpha=1;
	else
		alpha=num/max_num;

	ScalarType Rarea=((f->V(1)->RPos-f->V(0)->RPos)^(f->V(2)->RPos-f->V(0)->RPos)).Norm()/2.0;
	estimated=alpha*estimated+(1.0-alpha)*Rarea;

	return(estimated);
}

template <class MeshType>
typename MeshType::ScalarType EstimateAreaByParam
							  (const typename MeshType::VertexType* v0,
							   const typename MeshType::VertexType* v1,
							   typename MeshType::FaceType* on_edge[2])
{
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::ScalarType ScalarType;

	//MeshType::PerVertexAttributeHandle<AuxiliaryVertData> handle = vcg::tri::Allocator<MeshType>::GetPerVertexAttribute<AuxiliaryVertData>(mesh,"AuxiliaryVertData");

	ScalarType estimated[2]={0,0};
	int num[2]={0,0};
	VertexType *v2[2];
	for (int i=0;i<2;i++)
	{
		FaceType *test_face=on_edge[i];

		for (int k=0;k<3;k++)
			if ((test_face->V(k)!=v0)&&(test_face->V(k)!=v1))
				v2[i]=test_face->V(k);
		
		for (unsigned int k=0;k<test_face->vertices_bary.size();k++)
		{
			VertexType *brother=test_face->vertices_bary[k].first;
			estimated[i]+=brother->area;
			//estimated[i]+=handle[brother].area;
			num[i]++;
		}
	}

	///media of estimated values
	ScalarType alpha0,alpha1;
	ScalarType max_num=abstraction_num;//20

	if (num[0]>=max_num)
		alpha0=1;
	else
		alpha0=num[0]/max_num;

	if (num[1]>=max_num)
		alpha1=1;
	else
		alpha1=num[1]/max_num;
	ScalarType Rarea0=((on_edge[0]->V(1)->RPos-on_edge[0]->V(0)->RPos)^(on_edge[0]->V(2)->RPos-on_edge[0]->V(0)->RPos)).Norm()/2.0;
	ScalarType Rarea1=((on_edge[1]->V(1)->RPos-on_edge[1]->V(0)->RPos)^(on_edge[1]->V(2)->RPos-on_edge[1]->V(0)->RPos)).Norm()/2.0;
	estimated[0]=alpha0*estimated[0]+(1.0-alpha0)*Rarea0;
	estimated[1]=alpha1*estimated[1]+(1.0-alpha1)*Rarea1;
	return((estimated[0]+estimated[1])/2.0);
}

///template class used to sample surface
template <class FaceType>
class VertexSampler{
	typedef typename FaceType::CoordType CoordType;
public: 
	std::vector<CoordType> points;

	void AddFace(const FaceType &f,const CoordType & bary)
	{points.push_back(f.P(0)*bary.X()+f.P(1)*bary.Y()+f.P(2)*bary.Z());}
};


///sample 3d vertex possible's position
///using area criterion
//template <class MeshType>
//void SamplingPoints(MeshType &mesh,
//					std::vector<typename MeshType::CoordType> &pos)
//{
//	typedef typename MeshType::CoordType CoordType;
//	typedef VertexSampler<MeshType::FaceType> Sampler;
//	pos.reserve(samples_area);
//	Sampler ps;
//	ps.points.reserve(samples_area);
//
//	vcg::tri::SurfaceSampling<MeshType,Sampler>::Montecarlo(mesh,ps,samples_area);
//	pos=std::vector<CoordType>(ps.points.begin(),ps.points.end());
//}

template <class MeshType> 
void InitDampRestUV(MeshType &m)
{
	for (unsigned int i=0;i<m.vert.size();i++)
		m.vert[i].RestUV=m.vert[i].T().P();
}


template <class MeshType> 
void RestoreRestUV(MeshType &m)
{
	for (unsigned int i=0;i<m.vert.size();i++)
		m.vert[i].T().P()=m.vert[i].RestUV;
}


///parametrize a submesh from trinagles that are incident on vertices 
template <class MeshType>
void ParametrizeLocally(MeshType &parametrized,
						bool fix_boundary=true,
						bool init_border=true)
{
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::CoordType CoordType;

	//const ScalarType epsilon=(ScalarType)0.0001;
	

	///save old positions

	std::vector<CoordType> positions;
	positions.resize(parametrized.vert.size());
	///set rest position
	for (unsigned int i=0;i<parametrized.vert.size();i++)
	{
		positions[i]=parametrized.vert[i].P();
		parametrized.vert[i].P()=parametrized.vert[i].RPos;
	}

	UpdateTopologies(&parametrized);

	if (init_border)
		ParametrizeExternal(parametrized);

	ParametrizeInternal(parametrized);
	
	//for (int i=0;i<parametrized.vert.size();i++)
	//	parametrized.vert[i].RPos=parametrized.vert[i].P();

	vcg::tri::MeanValueTexCoordOptimization<MeshType> opt(parametrized);
	//vcg::tri::WachspressTexCoordOptimization<MeshType> opt(parametrized);
	vcg::tri::AreaPreservingTexCoordOptimization<MeshType> opt1(parametrized);
	opt.SetSpeed((ScalarType)0.0005);
	InitDampRestUV(parametrized);
	if (fix_boundary)
	{
		opt.TargetEquilateralGeometry();
		//opt.TargetCurrentGeometry();
		opt.SetBorderAsFixed();
		opt.IterateUntilConvergence((ScalarType)0.000001,100);
		/*opt.Iterate();
		opt.Iterate();*/
	}
	else
	{
		opt1.TargetCurrentGeometry();
		//opt1.SetBorderAsFixed();
		opt1.IterateUntilConvergence((ScalarType)0.000001,200);
	}

	///assert parametrization
	for (unsigned int i=0;i<parametrized.face.size();i++)
	{
		FaceType *f=&parametrized.face[i];
		vcg::Point2<ScalarType> tex0=vcg::Point2<ScalarType>(f->V(0)->T().U(),f->V(0)->T().V());
		vcg::Point2<ScalarType> tex1=vcg::Point2<ScalarType>(f->V(1)->T().U(),f->V(1)->T().V());
		vcg::Point2<ScalarType> tex2=vcg::Point2<ScalarType>(f->V(2)->T().U(),f->V(2)->T().V());
		vcg::Triangle2<typename MeshType::ScalarType> t2d=vcg::Triangle2<typename MeshType::ScalarType>(tex0,tex1,tex2);
#ifndef NDEBUG
		ScalarType area=(tex1-tex0)^(tex2-tex0);
		assert(area>0);
#endif
//#ifndef _MESHLAB
//		if (area<0){
//			vcg::tri::io::ExporterPLY<BaseMesh>::Save(parametrized,"case0.ply");
//
//			for (int j=0;j<parametrized.vert.size();j++)
//			{
//				parametrized.vert[j].P().V(0)=parametrized.vert[j].T().U();
//				parametrized.vert[j].P().V(1)=parametrized.vert[j].T().V();
//				parametrized.vert[j].P().V(2)=0;
//			}
//			vcg::tri::io::ExporterPLY<BaseMesh>::Save(parametrized,"case1.ply");
//			assert(0);
//		}
//#endif
	}
	///restore position
	for (unsigned int i=0;i<parametrized.vert.size();i++)	
		parametrized.vert[i].P()=positions[i];
}

template <class MeshType>
void ForceInParam(vcg::Point2<typename MeshType::ScalarType> &UV,MeshType &domain)
{
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::VertexType VertexType;

	ScalarType minDist=(ScalarType)1000.0;
	vcg::Point2<ScalarType> closest;
	vcg::Point2<ScalarType> center=vcg::Point2<ScalarType>(0,0);
	for (unsigned int i=0;i<domain.face.size();i++)	
	{
		FaceType *f=&domain.face[i];
		vcg::Point2<ScalarType> tex0=vcg::Point2<ScalarType>(f->V(0)->T().U(),f->V(0)->T().V());
		vcg::Point2<ScalarType> tex1=vcg::Point2<ScalarType>(f->V(1)->T().U(),f->V(1)->T().V());
		vcg::Point2<ScalarType> tex2=vcg::Point2<ScalarType>(f->V(2)->T().U(),f->V(2)->T().V());
		center+=tex0;
		center+=tex1;
		center+=tex2;
		vcg::Triangle2<ScalarType> t2d=vcg::Triangle2<ScalarType>(tex0,tex1,tex2);
		ScalarType dist;
		vcg::Point2<ScalarType> temp;
		t2d.PointDistance(UV,dist,temp);
		if (dist<minDist)
		{
			minDist=dist;
			closest=temp;
		}
	}
	center/=(ScalarType)(domain.face.size()*3);
	UV=closest*(ScalarType)0.95+center*(ScalarType)0.05;
}


template <class VertexType>
bool testParamCoords(VertexType *v)
{
	typedef typename VertexType::ScalarType ScalarType;
	ScalarType eps=(ScalarType)0.00001;
	if (!(((v->T().P().X()>=-1-eps)&&(v->T().P().X()<=1+eps)&&
		   (v->T().P().Y()>=-1-eps)&&(v->T().P().Y()<=1+eps))))
			return (false);

	return true;
}

template <class MeshType>
bool testParamCoords(MeshType &domain)
{
	for (unsigned int i=0;i<domain.vert.size();i++)
	{
                typename MeshType::VertexType *v=&domain.vert[i];
                bool b=testParamCoords<typename MeshType::VertexType>(v);
		if (!b)
		{
			#ifndef _MESHLAB
			printf("\n position %lf,%lf \n",v->T().U(),v->T().V());
			#endif
			return false;
		}
	}
	return true;
}

template <class CoordType>
bool testBaryCoords(CoordType &bary)
{
	typedef typename CoordType::ScalarType ScalarType;
	///test
	float eps=(ScalarType)0.0001;
	if(!(fabs(bary.X()+bary.Y()+bary.Z()-1.0)<eps))
		return false;
	if(!((bary.X()<=1.0)&&(bary.X()>=-eps)&&(bary.Y()<=1.0)&&(bary.Y()>=-eps)&&(bary.Z()<=1.0)&&(bary.Z()>=-eps)))
		return false;
	return true;
}

template <class CoordType>
bool NormalizeBaryCoords(CoordType &bary)
{
	bool isOK=testBaryCoords(bary);
	if (!isOK)
		return false;

	typedef typename CoordType::ScalarType ScalarType;

	///test <0
	if (bary.X()<0)
		bary.X()=0;
	if (bary.Y()<0)
		bary.Y()=0;
	if (bary.Z()<0)
		bary.Z()=0;

	///test >1
	if (bary.X()>1.0)
		bary.X()=1.0;
	if (bary.Y()>1.0)
		bary.Y()=1.0;
	if (bary.Z()>1.0)
		bary.Z()=1.0;
	
	///test sum
	ScalarType diff=bary.X()+bary.Y()+bary.Z()-1.0;
	bary.X()-=diff;

	if (bary.X()<0)
		bary.X()=0;
	return true;
}

template <class MeshType>
void AssingFather(typename MeshType::VertexType &v,
									typename MeshType::FaceType *father,
									typename MeshType::CoordType &bary,
									MeshType &domain)
{
#ifdef _DEBUG
	const typename MeshType::ScalarType eps=(typename MeshType::ScalarType)0.00001;
	assert((father-&(*domain.face.begin()))<(int)domain.face.size());
	assert(!(father->IsD()));
	assert(!(father==NULL));
	assert((bary.X()>=0)&&(bary.X()<=1)&&
				 (bary.Y()>=0)&&(bary.Y()<=1)&&
				 (bary.Z()>=0)&&(bary.Z()<=1)&&
				 ((bary.X()+bary.Y()+bary.Z())<=1+eps)&&
				 ((bary.X()+bary.Y()+bary.Z())>=1-eps));
#endif
	v.father=father;
	v.Bary=bary;
}

template <class MeshType>
bool testParametrization(MeshType &domain,
						 MeshType &Hlev)
{
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::VertexType VertexType;
	bool is_good=true;
	int num_del=0;
	int num_null=0;
	int fath_son=0;
	int wrong_address=0;
	for (unsigned int i=0;i<Hlev.vert.size();i++)
	{
		VertexType *v=&Hlev.vert[i];
		bool isGoodAddr=true;
		if ((v->father-&(*domain.face.begin()))>=(int)domain.face.size())
		{
			printf("\n ADDRESS EXCEEDS OF %d \n",v->father-&(*domain.face.begin()));
			wrong_address++;
			is_good=false;
			isGoodAddr=false;
		}
		if ((isGoodAddr)&&(v->father==NULL))
		{
			//printf("\n PAR ERROR : father NULL\n");
			num_null++;
			is_good=false;
		}
		if ((isGoodAddr)&&(v->father->IsD()))
		{
			//printf("\n PAR ERROR : father DELETED \n");
			num_del++;
			is_good=false;
		}
		if ((isGoodAddr)&&(!(((v->Bary.X()>=0)&&(v->Bary.X()<=1))&&
		((v->Bary.Y()>=0)&&(v->Bary.Y()<=1))&&
		((v->Bary.Z()>=0)&&(v->Bary.Z()<=1)))))
		{
			printf("\n PAR ERROR : bary coords exceeds: %f,%f,%f \n",v->Bary.X(),v->Bary.Y(),v->Bary.Z());
			is_good=false;
		}
	}
	for (unsigned int i=0;i<domain.face.size();i++)
	{
		FaceType *face=&domain.face[i];
		if (!face->IsD())
		{
			for (unsigned int j=0;j<face->vertices_bary.size();j++)
			{
				VertexType *v=face->vertices_bary[j].first;
				if (v->father!=face)
				{
					//printf("\n PAR ERROR : Father<->son \n");
					fath_son++;
					v->father=face;
					is_good=false;
				}
			}
		}
	}
	if (num_del>0)
		printf("\n PAR ERROR %d Father isDel  \n",num_del);
	if (num_null>0)
		printf("\n PAR ERROR %d Father isNull \n",num_null);
	if (fath_son>0)
		printf("\n PAR ERROR %d Father<->son  \n",fath_son);
	if (wrong_address>0)
	{
		printf("\n PAR ERROR %d Wrong Address Num Faces %d\n",wrong_address,domain.fn);
		/*system("pause");*/
	}
	return (is_good);
}

template <class MeshType>
bool NonFolded(MeshType &parametrized)
{
	//const ScalarType epsilon=0.00001;
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::VertexType VertexType;
	///assert parametrization
	for (unsigned int i=0;i<parametrized.face.size();i++)
	{
		FaceType *f=&parametrized.face[i];
		if (!((f->V(0)->IsB())&&(f->V(1)->IsB())&&(f->V(2)->IsB())))
		{

			vcg::Point2<ScalarType> tex0=vcg::Point2<ScalarType>(f->V(0)->T().U(),f->V(0)->T().V());
			vcg::Point2<ScalarType> tex1=vcg::Point2<ScalarType>(f->V(1)->T().U(),f->V(1)->T().V());
			vcg::Point2<ScalarType> tex2=vcg::Point2<ScalarType>(f->V(2)->T().U(),f->V(2)->T().V());
			vcg::Triangle2<ScalarType> t2d=vcg::Triangle2<ScalarType>(tex0,tex1,tex2);
			ScalarType area=(tex1-tex0)^(tex2-tex0);
			if (area<=0)
				return false;
		}
	}
	return true;
}

template <class MeshType>
bool NonFolded(MeshType &parametrized,std::vector<typename MeshType::FaceType*> &folded)
{
	
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::VertexType VertexType;

	const ScalarType epsilon=(ScalarType)0.00001;
	folded.resize(0);
	///assert parametrization
	for (unsigned int i=0;i<parametrized.face.size();i++)
	{
		FaceType *f=&parametrized.face[i];
		if (!((f->V(0)->IsB())&&(f->V(1)->IsB())&&(f->V(2)->IsB())))
		{

			vcg::Point2<ScalarType> tex0=vcg::Point2<ScalarType>(f->V(0)->T().U(),f->V(0)->T().V());
			vcg::Point2<ScalarType> tex1=vcg::Point2<ScalarType>(f->V(1)->T().U(),f->V(1)->T().V());
			vcg::Point2<ScalarType> tex2=vcg::Point2<ScalarType>(f->V(2)->T().U(),f->V(2)->T().V());
			vcg::Triangle2<ScalarType> t2d=vcg::Triangle2<ScalarType>(tex0,tex1,tex2);
			ScalarType area=(tex1-tex0)^(tex2-tex0);
			if (area<=epsilon)
				folded.push_back(f);
		}
	}
	return (folded.size()==0);
}
//getFoldedFaces(std::vector)
///parametrize a submesh from trinagles that are incident on vertices with equi-area subdivision
template <class MeshType>
void ParametrizeStarEquilateral(MeshType &parametrized,
								const typename MeshType::ScalarType &radius=1)
{
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;
	typedef typename MeshType::VertexType VertexType;
	
	UpdateTopologies(&parametrized);

	//set borders

	///find first border & non border vertex
	std::vector<VertexType*> non_border;
	VertexType* Start=NULL;
	for (unsigned int i=0;i<parametrized.vert.size();i++)
	{
		VertexType* vert=&parametrized.vert[i];
		if ((Start==NULL)&&(vert->IsB()))
			Start=vert;

		if (!vert->IsB())
			non_border.push_back(vert);
	}
	assert(non_border.size()!=0);
	
	///get sorted border vertices
  std::vector<VertexType*> vertices;
	FindSortedBorderVertices<MeshType>(parametrized,Start,vertices);
	
	///set border vertices
	int num=vertices.size();
        typename std::vector<VertexType*>::iterator iteV;
	ScalarType curr_angle=0;
	vertices[0]->T().U()=cos(curr_angle)*radius;
	vertices[0]->T().V()=sin(curr_angle)*radius;
	ScalarType division=(2*M_PI)/(ScalarType)num;
	///set border
	for (unsigned int i=1;i<vertices.size();i++)
	{
		curr_angle+=division;
		vertices[i]->T().U()=radius*cos(curr_angle);
		vertices[i]->T().V()=radius*sin(curr_angle);
	}

	if (non_border.size()==1)
	{
		///if non-border vertex is one then set it to zero otherwise 
		///set it to the average of neighbors
		non_border[0]->T().P()=vcg::Point2<ScalarType>(0,0);
	}
	else
	{
		///set media of star vertices
		assert(non_border.size()==2);
		for (unsigned int i=0;i<non_border.size();i++)
		{
			VertexType *v=non_border[i];
			v->T().P()=vcg::Point2<ScalarType>(0,0);
			int ariety_vert=0;
			std::vector<VertexType*> star;
			getVertexStar<MeshType>(v,star);
			for (unsigned int k=0;k<star.size();k++)
			{
				if ((!star[k]->IsD())&&(star[k]->IsB()))
				{
					v->T().P()+=star[k]->T().P();
					ariety_vert++;
				}
			}
			v->T().P()/=(ScalarType)ariety_vert;
		}
		///test particular cases
		if (!NonFolded(parametrized))
		{
			std::vector<VertexType*> shared;
			getSharedVertexStar<MeshType>(non_border[0],non_border[1],shared);
		
			assert(shared.size()==2);
			assert(shared[0]->IsB());
			assert(shared[1]->IsB());
			assert(shared[0]!=shared[1]);
		
			//ScalarType epsilon=(ScalarType)0.001;
			///then get the media of two shared vertices
			vcg::Point2<ScalarType> uvAve=shared[0]->T().P()+shared[1]->T().P();
			assert(uvAve.Norm()>(ScalarType)0.001);
			uvAve.Normalize();
			vcg::Point2<ScalarType> p0=uvAve*(ScalarType)0.3;
			vcg::Point2<ScalarType> p1=uvAve*(ScalarType)(-0.3);
			///then test and set right assignement
			non_border[0]->T().P()=p0;
			non_border[1]->T().P()=p1;
			if (!NonFolded(parametrized)){
				non_border[0]->T().P()=p1;
				non_border[1]->T().P()=p0;
			}
		}
	}

	///final assert parametrization
	assert(NonFolded(parametrized));

}

///given the mesh and the two edges (same) seen from face[0] and face[1] of the mesh construct
///a diamond parametrization using equilateral triangles of edge edge_len
template <class MeshType>
void ParametrizeDiamondEquilateral(MeshType &parametrized,
								   const int &edge0,const int &edge1,
								   const typename MeshType::ScalarType &edge_len=1)
{

	typedef typename MeshType::FaceType FaceType;
	typedef typename FaceType::CoordType CoordType;
	typedef typename FaceType::ScalarType ScalarType;
	typedef typename FaceType::VertexType VertexType;
	
	ScalarType h=(sqrt(3.0)/2.0)*edge_len;

	FaceType *fd0=&parametrized.face[0];
#ifndef NDEBUG
	FaceType *fd1=&parametrized.face[1];
#endif
	assert(fd0->FFp(edge0)==fd1);
	assert(fd1->FFp(edge1)==fd0);

	///get 2 vertex on the edge
	VertexType *v0=fd0->V(edge0);
	VertexType *v1=fd0->V((edge0+1)%3);

#ifndef NDEBUG
	VertexType *vtest0=fd1->V(edge1);
	VertexType *vtest1=fd1->V((edge1+1)%3);

	assert(v0!=v1);
	assert(vtest0!=vtest1);
	assert(((v0==vtest0)&&(v1==vtest1))||((v1==vtest0)&&(v0==vtest1)));
#endif

	///other 2 vertex
	VertexType *v2=parametrized.face[0].V((edge0+2)%3);
	VertexType *v3=parametrized.face[1].V((edge1+2)%3);
	assert((v2!=v3)&&(v0!=v2)&&(v0!=v3)&&(v1!=v2)&&(v1!=v3));

	///assing texcoords
	v0->T().P()=vcg::Point2<ScalarType>(0,-edge_len/2.0);
	v1->T().P()=vcg::Point2<ScalarType>(0,edge_len/2.0);
	v2->T().P()=vcg::Point2<ScalarType>(-h,0);
	v3->T().P()=vcg::Point2<ScalarType>(h,0);

	///test
	assert(NonFolded(parametrized));
}

///given the mesh and the two edges (same) seen from face[0] and face[1] of the mesh construct
///a diamond parametrization using equilateral triangles of edge edge_len
template <class MeshType>
void ParametrizeFaceEquilateral(MeshType &parametrized,
								   const typename MeshType::ScalarType &edge_len=1)
{
	typedef typename MeshType::FaceType FaceType;
	typedef typename FaceType::CoordType CoordType;
	typedef typename FaceType::ScalarType ScalarType;
	typedef typename FaceType::VertexType VertexType;
	
	ScalarType h=(sqrt(3.0)/2.0)*edge_len;

	FaceType *f_param=&(parametrized.face[0]);
				
	f_param->V(0)->T().P()=vcg::Point2<ScalarType>(edge_len/2.0,0);
	f_param->V(1)->T().P()=vcg::Point2<ScalarType>(0,h);
	f_param->V(2)->T().P()=vcg::Point2<ScalarType>(-edge_len/2.0,0);
}


///parametrize and create a submesh from trinagles that are incident on
/// vertices .... seturn a vetor of original faces
template <class MeshType>
void ParametrizeLocally(MeshType &parametrized,
						const std::vector<typename MeshType::VertexType*> &subset,
						std::vector<typename MeshType::FaceType*> &orderedFaces,
						std::vector<typename MeshType::VertexType*> &orderedVertex)
{
	typedef typename MeshType::FaceType FaceType;
	typedef typename FaceType::CoordType CoordType;
	typedef typename FaceType::ScalarType ScalarType;
	typedef typename FaceType::VertexType VertexType;

	orderedFaces.clear();
        std::vector<VertexType*> vertices;
	
	
	///get faces referenced by vertices
	getSharedFace<MeshType>(subset,orderedFaces);
	
	///do a first copy of the mesh
	///and parametrize it
	///NB: order of faces is mantained
	CopyMeshFromFaces<MeshType>(orderedFaces,orderedVertex,parametrized);
	
	//CreateMeshVertexStar(subset,orderedFaces,parametrized);
	ParametrizeLocally(parametrized);	
	
}



template <class MeshType>
void GetUV(const typename MeshType::FaceType* f,
		   const typename MeshType::CoordType &bary,
		   typename MeshType::ScalarType &U,
		   typename MeshType::ScalarType &V)
{
	U=bary.X()*f->V(0)->T().U()+bary.Y()*f->V(1)->T().U()+bary.Z()*f->V(2)->T().U();
	V=bary.X()*f->V(0)->T().V()+bary.Y()*f->V(1)->T().V()+bary.Z()*f->V(2)->T().V();
	/*if ((!((U>=-1)&&(U<=1)))||(!((V>=-1)&&(V<=1))))
	{
		printf("Bary:%f,%f,%f \n",bary.X(),bary.Y(),bary.Z());
		printf("texCoord:%f,%f \n",U,V);
		assert(0);
	}*/
	//assert ((U>=-1)&&(U<=1));
	//assert ((V>=-1)&&(V<=1));
}

template <class MeshType>
bool GetBaryFaceFromUV(const MeshType &m,
				       const typename MeshType::ScalarType &U,
					   const typename MeshType::ScalarType &V,
				       typename MeshType::CoordType &bary,
				       int &index)
{
	typedef typename MeshType::ScalarType ScalarType;
	const ScalarType _EPSILON = ScalarType(0.0000001);
	/*assert ((U>=-1)&&(U<=1));
	assert ((V>=-1)&&(V<=1));*/
	for (unsigned int i=0;i<m.face.size();i++)
	{
                const typename  MeshType::FaceType *f=&m.face[i];
		vcg::Point2<ScalarType> tex0=vcg::Point2<ScalarType>(f->V(0)->T().U(),f->V(0)->T().V());
		vcg::Point2<ScalarType> tex1=vcg::Point2<ScalarType>(f->V(1)->T().U(),f->V(1)->T().V());
		vcg::Point2<ScalarType> tex2=vcg::Point2<ScalarType>(f->V(2)->T().U(),f->V(2)->T().V());

		vcg::Point2<ScalarType> test=vcg::Point2<ScalarType>(U,V);
		vcg::Triangle2<ScalarType> t2d=vcg::Triangle2<ScalarType>(tex0,tex1,tex2);
		ScalarType area=(tex1-tex0)^(tex2-tex0);
		//assert(area>-_EPSILON);
		///then find if the point 2d falls inside
		if ((area>_EPSILON)&&(t2d.InterpolationParameters(test,bary.X(),bary.Y(),bary.Z())))
		{
			index=i;
			///approximation errors
			ScalarType sum=0;
			for (int x=0;x<3;x++)
			{
				if (((bary[x])<=0)&&(bary[x]>=-_EPSILON))
					bary[x]=0;
				else
				if (((bary[x])>=1)&&(bary[x]<=1+_EPSILON))
					bary[x]=1;
				sum+=bary[x];
			}
			if (sum==0)
				printf("error SUM %f \n",sum);

			bary/=sum;
			return true;
		}
	}

	return (false);
}

template <class FaceType>
bool GetBaryFaceFromUV(std::vector<FaceType*> faces,
				       const typename FaceType::ScalarType &U,
					   const typename FaceType::ScalarType &V,
				       typename FaceType::CoordType &bary,
				       int &index)
{
	typedef typename FaceType::CoordType CoordType;
	typedef typename FaceType::ScalarType ScalarType;
	typedef typename FaceType::VertexType VertexType;
	typedef typename FaceType::ScalarType ScalarType;
	const ScalarType _EPSILON = ScalarType(0.0000001);
	/*assert ((U>=-1)&&(U<=1));
	assert ((V>=-1)&&(V<=1));*/
	for (unsigned int i=0;i<faces.size();i++)
	{
		FaceType *f=faces[i];
		vcg::Point2<ScalarType> tex0=vcg::Point2<ScalarType>(f->V(0)->T().U(),f->V(0)->T().V());
		vcg::Point2<ScalarType> tex1=vcg::Point2<ScalarType>(f->V(1)->T().U(),f->V(1)->T().V());
		vcg::Point2<ScalarType> tex2=vcg::Point2<ScalarType>(f->V(2)->T().U(),f->V(2)->T().V());

		vcg::Point2<ScalarType> test=vcg::Point2<ScalarType>(U,V);
		vcg::Triangle2<ScalarType> t2d=vcg::Triangle2<ScalarType>(tex0,tex1,tex2);
		ScalarType area=fabs((tex1-tex0)^(tex2-tex0));
		//assert(area>-_EPSILON);
		///then find if the point 2d falls inside
		if ((area>_EPSILON)&&(t2d.InterpolationParameters(test,bary.X(),bary.Y(),bary.Z())))
		{
			index=i;
			
			///approximation errors
			ScalarType sum=0;
			for (int x=0;x<3;x++)
			{
				if (((bary[x])<=0)&&(bary[x]>=-_EPSILON))
					bary[x]=0;
				else
				if (((bary[x])>=1)&&(bary[x]<=1+_EPSILON))
					bary[x]=1;
				sum+=fabs(bary[x]);
			}
			if (sum==0)
				printf("error SUM %f \n",sum);
			
			bary/=sum;
			/*if (!((bary.X()>=0)&& (bary.X()<=1)))
				printf("error %f \n",bary.X());*/
			/*ScalarType diff=(1.0-bary.X()-bary.Y()-bary.Z());
			bary.X()+=diff;*/
			return true;
		}
	}

	return (false);
}

template <class MeshType>
bool GetBaryFaceFromUV(const MeshType &m,
				       const typename MeshType::ScalarType &U,
					   const typename MeshType::ScalarType &V,
				       const std::vector<typename MeshType::FaceType*> &orderedFaces,
				       typename MeshType::CoordType &bary,
				       typename MeshType::FaceType* &chosen)
{
	int index;
	bool found=GetBaryFaceFromUV(m,U,V,bary,index);
	if(!found) {
			chosen=0;
			return false;
		}
	chosen=orderedFaces[index];
	return true;
}

template <class MeshType>
bool GetCoordFromUV(const MeshType &m,
				    const typename MeshType::ScalarType &U,
					const typename MeshType::ScalarType &V,
				    typename MeshType::CoordType &val,
					bool rpos=false)
{
	typedef typename MeshType::ScalarType ScalarType;
	const ScalarType _EPSILON = (ScalarType)0.00001;
	for (unsigned int i=0;i<m.face.size();i++)
	{
                const typename MeshType::FaceType *f=&m.face[i];
		vcg::Point2<ScalarType> tex0=vcg::Point2<ScalarType>(f->V(0)->T().U(),f->V(0)->T().V());
		vcg::Point2<ScalarType> tex1=vcg::Point2<ScalarType>(f->V(1)->T().U(),f->V(1)->T().V());
		vcg::Point2<ScalarType> tex2=vcg::Point2<ScalarType>(f->V(2)->T().U(),f->V(2)->T().V());

		vcg::Point2<ScalarType> test=vcg::Point2<ScalarType>(U,V);
		vcg::Triangle2<ScalarType> t2d=vcg::Triangle2<ScalarType>(tex0,tex1,tex2);
		ScalarType area=(tex1-tex0)^(tex2-tex0);
		///then find if the point 2d falls inside
                typename MeshType::CoordType bary;
		if ((area>_EPSILON)&&(t2d.InterpolationParameters(test,bary.X(),bary.Y(),bary.Z())))
		{
			///approximation errors
			for (int x=0;x<3;x++)
			{
				if (((bary[x])<=0)&&(bary[x]>=-_EPSILON))
					bary[x]=0;
				else
				if (((bary[x])>=1)&&(bary[x]<=1+_EPSILON))
					bary[x]=1;
			}	
			ScalarType diff=(1.0-bary.X()-bary.Y()-bary.Z());
			bary.X()+=diff;
			if (!rpos)
				val=f->P(0)*bary.X()+f->P(1)*bary.Y()+f->P(0)*bary.Z();
			else
				val=f->V(0)->RPos*bary.X()+f->V(1)->RPos*bary.Y()+f->V(2)->RPos*bary.Z();
			
			return true;
		}
	}
	return false;
}

template <class MeshType>
typename MeshType::ScalarType GetSmallestUVEdgeSize(const MeshType &m)
{
	typedef typename MeshType::ScalarType ScalarType;

	ScalarType smallest=100.f;
	assert(m.fn>0);
	for (int i=0;i<m.face.size();i++)
	{

		///approximation errors
			for (int j=0;j<3;j++)
			{

				vcg::Point2<ScalarType> uv0=m.face[i].V(j)->T().P();
				vcg::Point2<ScalarType> uv1=m.face[i].V((j+1)%3)->T().P();
				ScalarType test=(uv0-uv1).Norm();
				if (test<smallest)
					smallest=test;
			}
	}
	return smallest;
}

template <class MeshType>
typename MeshType::ScalarType GetSmallestUVHeight(const MeshType &m)
{
	typedef typename MeshType::ScalarType ScalarType;
	ScalarType smallest=(ScalarType)100.0;
	ScalarType eps=(ScalarType)0.0001;
	assert(m.fn>0);
	for (unsigned int i=0;i<m.face.size();i++)
	{
                const typename MeshType::FaceType *f=&m.face[i];
		///approximation errors
		for (int j=0;j<3;j++)
		{
			vcg::Point2<ScalarType> uv0=f->V(j)->cT().P();
			vcg::Point2<ScalarType> uv1=f->V1(j)->cT().P();
			vcg::Point2<ScalarType> uv2=f->V2(j)->cT().P();
			ScalarType area=fabs((uv1-uv0)^(uv2-uv0));
			ScalarType base=(uv1-uv2).Norm();
			ScalarType h_test=area/base;
			if (h_test<smallest)
					smallest=h_test;
			}
	}
	if (smallest<eps)
		smallest=(ScalarType)eps;
	if (smallest>(ScalarType)0.05)
		smallest=(ScalarType)0.05;
	return smallest;
}

template <class MeshType>
void ParametrizeStarEquilateral(typename MeshType::VertexType *center,
								bool /*subvertices=true*/)
{
	///initialize domain
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::CoordType CoordType;

	MeshType parametrized;
	std::vector<VertexType*> vertices,ordVert;
	std::vector<VertexType*> HresVert;
	std::vector<FaceType*> faces;
	vertices.push_back(center);
	getSharedFace<MeshType>(vertices,faces);
	CopyMeshFromFaces<MeshType>(faces,ordVert,parametrized);

	///parametrize and then copy back
	ParametrizeStarEquilateral<MeshType>(parametrized);
	for (unsigned int i=0;i<ordVert.size();i++)
		ordVert[i]->T().P()=parametrized.vert[i].T().P();

	///initialize sub-vertices
	getHresVertex<FaceType>(faces,HresVert);
	for (unsigned int i=0;i<HresVert.size();i++)
	{
		FaceType *father=HresVert[i]->father;
		CoordType Bary=HresVert[i]->Bary;
		GetUV<MeshType>(father,Bary,HresVert[i]->T().U(),HresVert[i]->T().V());
	}
}

#endif
