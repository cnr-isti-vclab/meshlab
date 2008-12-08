#include <vcg/space/intersection3.h>
#include <vcg/math/matrix44.h>
#include <meshlab/meshmodel.h>

using namespace vcg;

template <class MESH_TYPE>
class SlicedEdge
{
public:
	SlicedEdge(const Plane3f &_p)
	{
	  p=_p;
	}

	bool operator()(face::Pos<typename MESH_TYPE::FaceType> ep)
	{
    Point3f rotv1=rot*ep.f->V0(ep.z)->P();
    Point3f rotv2=rot*ep.f->V1(ep.z)->P();
	  Point3f pp;
	  Segment3f seg(ep.f->V(ep.z)->P(),ep.f->V1(ep.z)->P());
    return Intersection<Segment3f>(p,seg,pp);

  }
protected:
  Plane3f p;
  Matrix44f rot;
};


template<class MESH_TYPE>
struct SlicingFunction : public std::unary_function<face::Pos<typename MESH_TYPE::FaceType> , typename MESH_TYPE::CoordType >
{
public :
  SlicingFunction(const Plane3f& _p)
	{
	  p=_p;
	}

	void operator()(typename MESH_TYPE::VertexType &nv, face::Pos<typename MESH_TYPE::FaceType> ep)
	{
	  Segment3f seg(ep.f->V(ep.z)->P(),ep.f->V1(ep.z)->P());
	  Point3f pp;
    Intersection<Segment3f>(p,seg,pp);
    nv.P()=pp;
    nv.Q()=3;
	}

	// raw calculation for wedgeinterp
	Color4<typename MESH_TYPE::ScalarType> WedgeInterp(Color4<typename MESH_TYPE::ScalarType> &c0, Color4<typename MESH_TYPE::ScalarType> &c1)
	{
		Color4<CMeshO::ScalarType> cc;
		return cc.lerp(c0,c1,0.5f);
	}

	template<class FL_TYPE>
	TexCoord2<FL_TYPE,1> WedgeInterp(TexCoord2<FL_TYPE,1> &t0, TexCoord2<FL_TYPE,1> &t1)
	{
		TexCoord2<FL_TYPE,1> tmp;
		assert(t0.n()== t1.n());
		tmp.n()=t0.n();
		tmp.t()=(t0.t()+t1.t())/2.0;
		return tmp;
	}
protected:
  Plane3f p;
};


