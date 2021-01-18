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
	
	CMeshO& operator=(const CMeshO& oth);
	
	Box3m trBB() const;
	
	int sfn;    //The number of selected faces.
	int svn;    //The number of selected vertices.
	
	int pvn; //the number of the polygonal vertices
	int pfn; //the number of the polygonal faces 
	
	Matrix44m Tr; // Usually it is the identity. It is applied in rendering and filters can or cannot use it. (most of the filter will ignore this)
};

#endif //CMESH_H
