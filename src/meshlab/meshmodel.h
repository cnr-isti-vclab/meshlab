/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
/****************************************************************************
  History
$Log$
Revision 1.20  2006/02/13 14:20:13  cignoni
Removed glew.h

Revision 1.19  2006/01/30 05:27:25  cignoni
Switched the order of component ocf and trimesh_base includes

Revision 1.18  2006/01/19 11:58:28  cignoni
Added mask field

Revision 1.17  2006/01/17 23:39:52  cignoni
removed useless includes

Revision 1.16  2006/01/04 16:22:20  cignoni
Made FFAdj optional and added store and restore color functions
****************************************************************************/

#ifndef MESHMODEL_H
#define MESHMODEL_H
#include <GL/glew.h>

#include <stdio.h>
#include <time.h>

#include<vcg/simplex/vertexplus/base.h>
#include<vcg/simplex/faceplus/base.h>
#include<vcg/simplex/face/topology.h>

#include<vcg/complex/trimesh/base.h>
#include<vcg/simplex/vertexplus/component_ocf.h>
#include<vcg/simplex/faceplus/component_ocf.h>

#include <wrap/gl/trimesh.h>
#include <wrap/callback.h>
#include <wrap/io_trimesh/io_mask.h>

using namespace vcg;
using namespace std;

class CEdge;    // dummy prototype never used
class CFaceO;
class CVertexO;

// Opt stuff

class CVertexO  : public VertexSimp2< CVertexO, CEdge, CFaceO, vert::Coord3f, vert::Color4b, vert::Normal3f, vert::Qualityf, vert::BitFlags  >{ 
public:
	Color4b origC;
};
class CFaceO    : public FaceSimp2<  CVertexO, CEdge, CFaceO,  face::InfoOcf, face::Color4b, face::FFAdjOcf, face::WedgeTexturefOcf, face::VertexRef, face::BitFlags, face::Normal3f, face::Mark > {};
class CMeshO    : public vcg::tri::TriMesh< vector<CVertexO>, face::vector_ocf<CFaceO> > {};

/*
  MeshModel Class

contiene i dati relativi ad un singolo oggetto.
Ogni oggetto si sa caricare e contiene una mesh
*/

class MeshModel : public tri::io::Mask
{
public:

  CMeshO cm;
  GlTrimesh<CMeshO> glw;
  MeshModel() {
    glw.m=&cm; 
    cm.face.EnableWedgeTex();
    cm.face.EnableFFAdjacency();
    mask= IOM_VERTCOORD | IOM_FACEINDEX | IOM_FLAGS;
  }
  bool Render(GLW::DrawMode dm, GLW::ColorMode cm, GLW::TextureMode tm);
  inline void storeVertexColor()
  {
	  CMeshO::VertexIterator vi;
	  for(vi=cm.vert.begin();vi!=cm.vert.end();++vi) (*vi).origC=(*vi).C();
  }
  inline void restoreVertexColor()
  {
	  CMeshO::VertexIterator vi;
	  for(vi=cm.vert.begin();vi!=cm.vert.end();++vi) (*vi).C()=(*vi).origC;
  }
  int mask;

};

#endif