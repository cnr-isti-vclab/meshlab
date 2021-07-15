/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005 - 2020                                          \/)\/   *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#ifndef CMESH_H
#define CMESH_H

#include "base_types.h"

namespace vcg
{
namespace vertex
{
template <class T> class Coord3m: public Coord<vcg::Point3<Scalarm>, T> {
public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("Coord3m"));T::Name(name);}
};

template <class T> class Normal3m: public Normal<vcg::Point3<Scalarm>, T> {
public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("Normal3m"));T::Name(name);}
};

template <class T> class Qualitym: public Quality<Scalarm, T> {
public: static void Name(std::vector<std::string> & name){name.push_back(std::string("Qualitym"));T::Name(name);}
};

template <class T> class CurvaturemOcf: public CurvatureOcf<Scalarm, T> {
public: static void Name(std::vector<std::string> & name){name.push_back(std::string("CurvaturemOcf"));T::Name(name);}
};


template <class T> class CurvatureDirmOcf: public CurvatureDirOcf<CurvatureDirTypeOcf<Scalarm>, T> {
public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("CurvatureDirmOcf"));T::Name(name);}
};

template <class T> class RadiusmOcf: public RadiusOcf<Scalarm, T> {
public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("RadiusmOcf"));T::Name(name);}
};

}//end namespace vertex

namespace face
{
template <class T> class Normal3m: public NormalAbs<vcg::Point3<Scalarm>, T> {
public:  static void Name(std::vector<std::string> & name){name.push_back(std::string("Normal3m"));T::Name(name);}
};

template <class T> class QualitymOcf: public QualityOcf<Scalarm, T> {
public:  static void Name(std::vector<std::string> & name){name.push_back(std::string("QualitymOcf"));T::Name(name);}
};

template <class T> class CurvatureDirmOcf: public CurvatureDirOcf<CurvatureDirOcfBaseType<Scalarm>, T> {
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
		vcg::vertex::Qualitym,          /*  4b */
		vcg::vertex::Color4b,           /*  4b */
		vcg::vertex::VFAdjOcf,          /*  0b */
		vcg::vertex::MarkOcf,           /*  0b */
		vcg::vertex::TexCoordfOcf,      /*  0b */
		vcg::vertex::CurvaturemOcf,     /*  0b */
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
		vcg::face::QualitymOcf,          /* 0b */
		vcg::face::MarkOcf,              /* 0b */
		vcg::face::Color4bOcf,           /* 0b */
		vcg::face::FFAdjOcf,             /* 0b */
		vcg::face::VFAdjOcf,             /* 0b */
		vcg::face::CurvatureDirmOcf,     /* 0b */
		vcg::face::WedgeTexCoordfOcf     /* 0b */
		> {};

typedef vcg::tri::TriMesh< vcg::vertex::vector_ocf<CVertexO>, vcg::face::vector_ocf<CFaceO> > vcgTriMesh;

class CMeshO    : public vcgTriMesh
{
public :
	CMeshO();
	
	CMeshO(const CMeshO& oth);
	
	CMeshO(CMeshO&& oth);

	virtual ~CMeshO();
	
	CMeshO& operator=(CMeshO oth);

	friend void swap(CMeshO& m1, CMeshO& m2);
	
	Box3m trBB() const;
	
	int sfn;    //The number of selected faces.
	int svn;    //The number of selected vertices.
	
	int pvn; //the number of the polygonal vertices
	int pfn; //the number of the polygonal faces 
	
	Matrix44m Tr; // Usually it is the identity. It is applied in rendering and filters can or cannot use it. (most of the filter will ignore this)

private:
	void enableComponentsFromOtherMesh(const CMeshO& oth);
};

//must be inlined
inline void swap(CMeshO& m1, CMeshO& m2)
{
	using std::swap;
	swap(m1.vn, m2.vn);
	swap(m1.vert, m2.vert);
	m1.vert._updateOVP(m1.vert.begin(), m1.vert.end());
	m2.vert._updateOVP(m2.vert.begin(), m2.vert.end());
	swap(m1.en, m2.en);
	swap(m1.edge, m2.edge);
	swap(m1.fn, m2.fn);
	swap(m1.face, m2.face);
	m1.face._updateOVP(m1.face.begin(), m1.face.end());
	m2.face._updateOVP(m2.face.begin(), m2.face.end());
	swap(m1.hn, m2.hn);
	swap(m1.hedge, m2.hedge);
	swap(m1.tn, m2.tn);
	swap(m1.tetra, m2.tetra);
	swap(m1.bbox, m2.bbox);
	swap(m1.textures, m2.textures);
	swap(m1.normalmaps, m2.normalmaps);
	swap(m1.attrn, m2.attrn);
	swap(m1.vert_attr, m2.vert_attr);
	swap(m1.edge_attr, m2.edge_attr);
	swap(m1.face_attr, m2.face_attr);
	swap(m1.mesh_attr, m2.mesh_attr);
	swap(m1.tetra_attr, m2.tetra_attr);
	swap(m1.shot, m2.shot);
	swap(m1.imark, m2.imark);
}


#endif //CMESH_H
