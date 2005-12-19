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
Revision 1.14  2005/12/19 16:50:30  cignoni
Added Mark component (needed for self-intersection)

Revision 1.13  2005/12/07 00:56:40  fmazzant
added support for exporter generic obj file (level base)

Revision 1.12  2005/12/04 00:22:46  cignoni
Switched from progresBar widget to progressbar dialog

Revision 1.11  2005/12/02 00:54:13  cignoni
Added TextureMode in render

Revision 1.10  2005/11/30 16:26:56  cignoni
All the modification, restructuring seen during the 30/12 lesson...

Revision 1.9  2005/11/26 00:14:17  cignoni
Cleaned up mesh types

Revision 1.8  2005/11/25 16:25:35  ggangemi
Added vert::Color4b, vert::Qualityf components to CVertexO: required by MeshColorizeInterface plugins

Revision 1.7  2005/11/22 16:16:35  glvertex
Added preprocessor intructions to prevent multiple inclusion

Revision 1.6  2005/11/21 22:06:47  cignoni
Reinserted optional normals (now working)

Revision 1.5  2005/11/21 12:09:33  cignoni
Added copyright info
****************************************************************************/

#ifndef MESHMODEL_H
#define MESHMODEL_H

#include <stdio.h>
#include <time.h>

#include<vcg/simplex/vertexplus/base.h>
#include<vcg/simplex/faceplus/base.h>
#include<vcg/simplex/face/topology.h>

#include<vcg/simplex/vertexplus/component_ocf.h>
#include<vcg/simplex/faceplus/component_ocf.h>
#include<vcg/complex/trimesh/base.h>
#include<vcg/complex/trimesh/create/platonic.h>
#include<vcg/complex/trimesh/update/topology.h>
#include<vcg/complex/trimesh/update/flag.h>
#include<vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/refine.h>

#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/gl/trimesh.h>

using namespace vcg;
using namespace std;

class CEdge;    // dummy prototype never used
class CFaceO;
class CVertexO;

// Opt stuff

class CVertexO  : public VertexSimp2< CVertexO, CEdge, CFaceO, vert::Coord3f, vert::Color4b, vert::Normal3f, vert::Qualityf, vert::BitFlags  >{};
class CFaceO    : public FaceSimp2<  CVertexO, CEdge, CFaceO,  face::InfoOcf, face::Color4b, face::FFAdj, face::WedgeTexturefOcf, face::VertexRef, face::BitFlags, face::Normal3fOcf, face::Mark > {};
class CMeshO    : public vcg::tri::TriMesh< vector<CVertexO>, face::vector_ocf<CFaceO> > {};

/*
  MeshModel Class

contiene i dati relativi ad un singolo oggetto.
Ogni oggetto si sa caricare e contiene una mesh
*/

class MeshModel
{
public:

  CMeshO cm;
  GlTrimesh<CMeshO> glw;
  MeshModel() {glw.m=&cm; cm.face.EnableWedgeTex();}
  bool Open(const char* filename, CallBackPos *cb=0);
  bool Save(const char* filename, CallBackPos *cb=0);
  bool Render(GLW::DrawMode dm, GLW::ColorMode cm, GLW::TextureMode tm);
};

#endif