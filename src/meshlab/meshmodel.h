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
Revision 1.25  2006/09/22 06:28:02  granzuglia
abstract pointer to fileformat's dependent additional info added

Revision 1.24  2006/06/16 01:26:07  cignoni
Added Initial Filter Script Dialog

Revision 1.23  2006/06/07 08:49:25  cignoni
Disable rendering during processing and loading

Revision 1.22  2006/05/25 09:46:37  cignoni
missing std and and all the other gcc detected syntax errors

Revision 1.21  2006/05/25 04:57:45  cignoni
Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.

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
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/complex/trimesh/update/flag.h>

#include <wrap/gl/trimesh.h>
#include <wrap/callback.h>
#include <wrap/io_trimesh/io_mask.h>
#include <wrap/io_trimesh/additionalinfo.h>

using namespace vcg;
using namespace std;

class CEdge;    // dummy prototype never used
class CFaceO;
class CVertexO;

//Vert Mem Occupancy  --- 36 ---

class CVertexO  : public VertexSimp2< CVertexO, CEdge, CFaceO, 
  vert::Coord3f,     /* 12b */ 
  vert::BitFlags,    /*  4b */
  vert::Normal3f,    /* 12b */
  vert::Qualityf,    /*  4b */
  vert::Color4b      /*  4b */
  >{ 
};

//Face Mem Occupancy  --- 32 ---

class CFaceO    : public FaceSimp2<  CVertexO, CEdge, CFaceO,  
      face::InfoOcf,              /* 4b */
      face::VertexRef,            /*12b */
      face::BitFlags,             /* 4b */
      face::Normal3f,             /*12b */
      face::MarkOcf,              /* 0b */
      face::Color4bOcf,           /* 0b */
      face::FFAdjOcf,             /* 0b */
      face::WedgeTexturefOcf      /* 0b */
    > {};
class CMeshO    : public vcg::tri::TriMesh< vector<CVertexO>, face::vector_ocf<CFaceO> > {};

/*
  MeshModel Class

contiene i dati relativi ad un singolo oggetto.
Ogni oggetto si sa caricare e contiene una mesh
*/

class MeshModel : public tri::io::Mask
{
public:

 	enum FilterReq {  MM_NONE          = 0x0000,
                    MM_BORDERFLAG    = 0x0001,
                    MM_FACETOPO      = 0x0002,
                    MM_WEDGTEXCOORD  = 0x0004,
                    MM_FACECOLOR     = 0x0008,
                    MM_FACEMARK      = 0x0010,
                    MM_ALL           = 0xffff} ;


  CMeshO cm;
  GlTrimesh<CMeshO> glw;
  vector<Color4b> originalVertexColor;

  // Bitmask denoting what fields are currently kept updated in mesh
  int currentDataMask;
  // Bitmask denoting what fields are loaded/saved
  int mask;
  bool busy;

  //abstract pointer to fileformat's dependent additional info
  AdditionalInfo* addinfo;

  MeshModel() {
//    size_t faceSize=sizeof(CFaceO);
//    size_t vertSize=sizeof(CVertexO);
    
    glw.m=&cm; 
    currentDataMask=MM_NONE;
    mask= IOM_VERTCOORD | IOM_FACEINDEX | IOM_FLAGS;
    busy=true;
  }
  bool Render(GLW::DrawMode dm, GLW::ColorMode cm, GLW::TextureMode tm);
  bool RenderSelectedFaces();

  inline void storeVertexColor()
  {
    originalVertexColor.resize(cm.vert.size());
    vector<Color4b>::iterator ci;
	  CMeshO::VertexIterator vi;
	  for(vi=cm.vert.begin(),ci=originalVertexColor.begin();vi!=cm.vert.end();++vi,++ci) 
      (*ci)=(*vi).C();
  }
  inline void restoreVertexColor()
  {
    if(originalVertexColor.empty()) return;
    vector<Color4b>::iterator ci;
	  CMeshO::VertexIterator vi;
	  for(vi=cm.vert.begin(),ci=originalVertexColor.begin();vi!=cm.vert.end();++vi,++ci) 
      (*vi).C()=(*ci);
  }

// FUNZIONE equivalente alla updatedatamask ma solo che prende in ingresso mask da filetype.
  void Enable(int openingFileMask)
  {
   if( openingFileMask & IOM_WEDGTEXCOORD ) updateDataMask(MM_WEDGTEXCOORD);
   if( openingFileMask & IOM_FACECOLOR    ) updateDataMask(MM_FACECOLOR);
  }

  // Ogni filtro dichiara
  // 1) di che cosa ha bisogno
  // 2) che cosa sa aggiornare (di solito quello di cui ha bisogno)
  // 3) quello che ha cambiato (vertici topologia colore)

  // il framework si preoccupa
  // 1) prima di invocare il filtro di preparare quel che serve
  // 2) dopo il filtro di aggiornare quello che non ha aggiornato il filtro

  // Enable optional fields that could be needed
  void updateDataMask(int neededDataMask)
  {
   if( ( (neededDataMask & MM_FACETOPO)!=0) && (currentDataMask& MM_FACETOPO)==0)			
   {
    cm.face.EnableFFAdjacency();
    currentDataMask |= MM_FACETOPO;
	  tri::UpdateTopology<CMeshO>::FaceFace(cm);
   }
   if( ( (neededDataMask & MM_BORDERFLAG)!=0) && (currentDataMask& MM_BORDERFLAG)==0)			
   {
     if(currentDataMask& MM_FACETOPO) tri::UpdateFlags<CMeshO>::FaceBorderFromFF(cm);
     else tri::UpdateFlags<CMeshO>::FaceBorderFromNone(cm);
     currentDataMask |= MM_BORDERFLAG;
   }
   if( ( (neededDataMask & MM_WEDGTEXCOORD)!=0) && (currentDataMask& MM_WEDGTEXCOORD)==0)			
   {
    cm.face.EnableWedgeTex();
    currentDataMask |= MM_WEDGTEXCOORD;
   }
   if( ( (neededDataMask & MM_FACECOLOR)!=0) && (currentDataMask& MM_FACECOLOR)==0)			
   {
    cm.face.EnableColor();
    currentDataMask |= MM_FACECOLOR;
   } 
   if( ( (neededDataMask & MM_FACEMARK)!=0) && (currentDataMask& MM_FACEMARK)==0)			
   {
    cm.face.EnableMark();
    currentDataMask |= MM_FACEMARK;
   }
  }
};

class RenderMode
{
public:
  	vcg::GLW::DrawMode	drawMode;
  	vcg::GLW::ColorMode	colorMode;
  	vcg::GLW::TextureMode	textureMode;

    bool lighting;
    bool backFaceCull;
    bool doubleSideLighting;  
    bool fancyLighting;
    bool castShadow;
    bool selectedFaces;
    vcg::Point3f lightDir;
		
		
		RenderMode()
		{
      drawMode	= GLW::DMFlat;
			colorMode = GLW::CMNone;
			textureMode = GLW::TMNone;

			lighting = true;
			backFaceCull = false;
			doubleSideLighting = false;
			fancyLighting = false;
			castShadow = false;
      selectedFaces=false;
    }
};

#endif
