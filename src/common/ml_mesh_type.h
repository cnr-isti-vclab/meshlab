#ifndef __ML_MESH_TYPE_H
#define __ML_MESH_TYPE_H

#include <vcg/complex/complex.h>

#ifndef MESHLAB_SCALAR
#error "Fatal compilation error: MESHLAB_SCALAR must be defined"
#endif

typedef MESHLAB_SCALAR Scalarm;
typedef vcg::Point2<MESHLAB_SCALAR>   Point2m;
typedef vcg::Point3<MESHLAB_SCALAR>   Point3m;
typedef vcg::Point4<MESHLAB_SCALAR>   Point4m;
typedef vcg::Plane3<MESHLAB_SCALAR>   Plane3m;
typedef vcg::Segment2<MESHLAB_SCALAR> Segment2m;
typedef vcg::Segment3<MESHLAB_SCALAR> Segment3m;
typedef vcg::Box3<MESHLAB_SCALAR>     Box3m;
typedef vcg::Matrix44<MESHLAB_SCALAR> Matrix44m;
typedef vcg::Matrix33<MESHLAB_SCALAR> Matrix33m;
typedef vcg::Shot<MESHLAB_SCALAR>     Shotm;
typedef vcg::Similarity<MESHLAB_SCALAR> Similaritym;

template<typename T>
struct MeshLabScalarTest
{

};

template<>
struct MeshLabScalarTest<float>
{
	static const char* floatingPointPrecision() { return "fp"; }
	static const char* floatingPointPrecisionIOToken() { return "%f"; }
	static bool doublePrecision() { return false; }
};

template<>
struct MeshLabScalarTest<double>
{
	static const char* floatingPointPrecision() { return "dp"; }
	static const char* floatingPointPrecisionIOToken() { return "%lf"; }
	static bool doublePrecision() { return true; }
};

namespace vcg
{
    namespace vertex
    {
        template <class T> class Coord3m: public Coord<vcg::Point3<MESHLAB_SCALAR>, T> {
        public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("Coord3m"));T::Name(name);}
        };

        template <class T> class Normal3m: public Normal<vcg::Point3<MESHLAB_SCALAR>, T> {
        public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("Normal3m"));T::Name(name);}
        };

        template <class T> class CurvatureDirmOcf: public CurvatureDirOcf<CurvatureDirTypeOcf<MESHLAB_SCALAR>, T> {
        public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("CurvatureDirmOcf"));T::Name(name);}
        };

        template <class T> class RadiusmOcf: public RadiusOcf<MESHLAB_SCALAR, T> {
        public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("RadiusmOcf"));T::Name(name);}
        };

    }//end namespace vertex
    namespace face
    {
        template <class T> class Normal3m: public NormalAbs<vcg::Point3<MESHLAB_SCALAR>, T> {
        public:  static void Name(std::vector<std::string> & name){name.push_back(std::string("Normal3m"));T::Name(name);}
        };

        template <class T> class CurvatureDirmOcf: public CurvatureDirOcf<CurvatureDirOcfBaseType<MESHLAB_SCALAR>, T> {
        public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("CurvatureDirdOcf"));T::Name(name);}
        };

    }//end namespace face
}//end namespace vcg

// Forward declarations needed for creating the used types
class CVertexO;
class CEdgeO;
class CFaceO;

// Declaration of the semantic of the used types
class CUsedTypesO: public vcg::UsedTypes < vcg::Use<CVertexO>::AsVertexType,
    vcg::Use<CEdgeO   >::AsEdgeType,
    vcg::Use<CFaceO  >::AsFaceType >{};


// The Main Vertex Class
// Most of the attributes are optional and must be enabled before use.
// Each vertex needs 40 byte, on 32bit arch. and 44 byte on 64bit arch.

class CVertexO  : public vcg::Vertex< CUsedTypesO,
    vcg::vertex::InfoOcf,           /*  4b */
    vcg::vertex::Coord3m,           /* 12b */
    vcg::vertex::BitFlags,          /*  4b */
    vcg::vertex::Normal3m,          /* 12b */
    vcg::vertex::Qualityf,          /*  4b */
    vcg::vertex::Color4b,           /*  4b */
    vcg::vertex::VFAdjOcf,          /*  0b */
    vcg::vertex::MarkOcf,           /*  0b */
    vcg::vertex::TexCoordfOcf,      /*  0b */
    vcg::vertex::CurvaturefOcf,     /*  0b */
    vcg::vertex::CurvatureDirmOcf,  /*  0b */
    vcg::vertex::RadiusmOcf         /*  0b */
>{
};


// The Main Edge Class
class CEdgeO : public vcg::Edge<CUsedTypesO,
    vcg::edge::BitFlags,          /*  4b */
    vcg::edge::EVAdj,
    vcg::edge::EEAdj
>{
};

// Each face needs 32 byte, on 32bit arch. and 48 byte on 64bit arch.
class CFaceO    : public vcg::Face<  CUsedTypesO,
    vcg::face::InfoOcf,              /* 4b */
    vcg::face::VertexRef,            /*12b */
    vcg::face::BitFlags,             /* 4b */
    vcg::face::Normal3m,             /*12b */
    vcg::face::QualityfOcf,          /* 0b */
    vcg::face::MarkOcf,              /* 0b */
    vcg::face::Color4bOcf,           /* 0b */
    vcg::face::FFAdjOcf,             /* 0b */
    vcg::face::VFAdjOcf,             /* 0b */
    vcg::face::CurvatureDirmOcf,     /* 0b */
    vcg::face::WedgeTexCoordfOcf     /* 0b */
> {};


class CMeshO    : public vcg::tri::TriMesh< vcg::vertex::vector_ocf<CVertexO>, vcg::face::vector_ocf<CFaceO> >
{
public :
    int sfn;    //The number of selected faces.
    int svn;    //The number of selected vertices.

	int pvn; //the number of the polygonal vertices
	int pfn; //the number of the polygonal faces 

	Matrix44m Tr; // Usually it is the identity. It is applied in rendering and filters can or cannot use it. (most of the filter will ignore this)

	const Box3m &trBB() const
    {
        static Box3m bb;
        bb.SetNull();
        bb.Add(Tr,bbox);
        return bb;
    }
};

#endif
