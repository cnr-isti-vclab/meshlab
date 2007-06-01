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
Revision 1.35  2007/06/01 09:19:24  cignoni
Added MeshDocument and computation of VertexBorderFlags when asking border flags

Revision 1.34  2007/04/16 09:24:37  cignoni
** big change **
Added Layers managemnt.
Interfaces are changing...

Revision 1.33  2007/03/20 16:22:34  cignoni
Big small change in accessing mesh interface. First step toward layers

Revision 1.32  2007/03/14 22:59:34  cignoni
Texture -> TexCoord name change

Revision 1.31  2007/03/03 02:03:26  cignoni
Reformatted lower bar, added number of selected faces. Updated about dialog

Revision 1.30  2007/01/11 10:40:48  cignoni
added a safety test in restorecolor

Revision 1.29  2006/11/29 01:03:38  cignoni
Edited some comments

Revision 1.28  2006/11/07 09:03:40  cignoni
Added clearDataMask

Revision 1.27  2006/10/15 23:45:51  cignoni
Added orderedEdge constructor for gcc compiling of quadric simplifciation stuff

Revision 1.26  2006/10/10 21:16:13  cignoni
Added VF optional component

Revision 1.25  2006/09/22 06:28:02  granzuglia
abstract pointer to fileformat's dependent additional info added

****************************************************************************/

#ifndef MESHMODEL_H
#define MESHMODEL_H
#include <GL/glew.h>

#include <stdio.h>
#include <time.h>

#include <vcg/simplex/vertexplus/base.h>
#include <vcg/simplex/edge/edge.h>
#include <vcg/simplex/faceplus/base.h>
#include <vcg/simplex/face/topology.h>

#include <vcg/complex/trimesh/base.h>
#include <vcg/simplex/vertexplus/component_ocf.h>
#include <vcg/simplex/faceplus/component_ocf.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/complex/trimesh/update/flag.h>

#include <wrap/gl/trimesh.h>
#include <wrap/callback.h>
#include <wrap/io_trimesh/io_mask.h>
#include <wrap/io_trimesh/additionalinfo.h>
#include <QList>
using namespace vcg;
using namespace std;

class CEdge;   
class CFaceO;
class CVertexO;

//Vert Mem Occupancy  --- 44b ---

class CVertexO  : public VertexSimp2< CVertexO, CEdge, CFaceO, 
  vert::Coord3f,     /* 12b */ 
  vert::BitFlags,    /*  4b */
  vert::Normal3f,    /* 12b */
  vert::Qualityf,    /*  4b */
  vert::VFAdj,       /*  4b */
  vert::Mark,        /*  4b */
  vert::Color4b      /*  4b */
  >{ 
};

  
class CEdge : public Edge<CEdge,CVertexO> {
public:
  inline CEdge() {};
  inline CEdge( CVertexO * v0, CVertexO * v1):Edge<CEdge,CVertexO>(v0,v1){};
  static inline CEdge OrderedEdge(VertexType* v0,VertexType* v1){
   if(v0<v1) return CEdge(v0,v1);
   else return CEdge(v1,v0);
  }

  //inline CEdge( Edge<CEdge,CVertexO> &e):Edge<CEdge,CVertexO>(e){};
};

//Face Mem Occupancy  --- 32b ---

class CFaceO    : public FaceSimp2<  CVertexO, CEdge, CFaceO,  
      face::InfoOcf,              /* 4b */
      face::VertexRef,            /*12b */
      face::BitFlags,             /* 4b */
      face::Normal3f,             /*12b */
      face::MarkOcf,              /* 0b */
      face::Color4bOcf,           /* 0b */
      face::FFAdjOcf,             /* 0b */
      face::VFAdjOcf,             /* 0b */
      face::WedgeTexCoordfOcf      /* 0b */
    > {};

class CMeshO    : public vcg::tri::TriMesh< vector<CVertexO>, face::vector_ocf<CFaceO> > {
public :
	int sfn; //The number of selected faces.
};

/*
MeshModel Class
The base class for representing a single mesh.
It contains a single vcg mesh object with some additional information for keeping track of its origin and of what info it has. 
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
                    MM_VERTFACETOPO  = 0x0020,
                    MM_ALL           = 0xffff} ;


  CMeshO cm;

public:
  GlTrimesh<CMeshO> glw;
  vector<Color4b> originalVertexColor;
	std::string fileName;

  // Bitmask denoting what fields are currently kept updated in mesh
  // it is composed by OR-ing MM_XXXX enums (defined in the above FilterReq)
  int currentDataMask;

  // Bitmask denoting what fields are loaded/saved
  // it is composed by OR-ing IOM_XXXX enums (defined in tri::io::Mask)
  int ioMask;
	
  bool busy;
	bool visible;

  //abstract pointer to fileformat's dependent additional info
  AdditionalInfo* addinfo;

  MeshModel() {    
    glw.m=&cm; 
    currentDataMask=MM_NONE;
    ioMask= IOM_VERTCOORD | IOM_FACEINDEX | IOM_FLAGS;
    busy=true;
		visible=true;
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
    if(originalVertexColor.size() != cm.vert.size()) return;
    vector<Color4b>::iterator ci;
	  CMeshO::VertexIterator vi;
	  for(vi=cm.vert.begin(),ci=originalVertexColor.begin();vi!=cm.vert.end();++vi,++ci) 
      (*vi).C()=(*ci);
  }
	
// This function is roughly equivalent to the updateDataMask,
// but it takes in input a mask coming from a filetype instead of a filter requirement (like topology etc)
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
   if( ( (neededDataMask & MM_VERTFACETOPO)!=0) && (currentDataMask& MM_VERTFACETOPO)==0)			
   {
    cm.face.EnableVFAdjacency();
    currentDataMask |= MM_VERTFACETOPO;
	  tri::UpdateTopology<CMeshO>::VertexFace(cm);
   }
   if( ( (neededDataMask & MM_BORDERFLAG)!=0) && (currentDataMask& MM_BORDERFLAG)==0)			
   {
     if(currentDataMask& MM_FACETOPO) tri::UpdateFlags<CMeshO>::FaceBorderFromFF(cm);
     else tri::UpdateFlags<CMeshO>::FaceBorderFromNone(cm);
		 tri::UpdateFlags<CMeshO>::VertexBorderFromFace(cm);

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

   void clearDataMask(int neededDataMask)
   {
     currentDataMask = currentDataMask & (!neededDataMask);
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

class MeshDocument
{
public :
MeshDocument()
		{currentMesh = NULL;
	}
 ~MeshDocument()
		{
			foreach(MeshModel *mmp, meshList)
					delete mmp;
   }
	 
	void setCurrentMesh(unsigned int i)
	{
	  assert(i>=0 && i < meshList.size());
		currentMesh=meshList.at(i);
	}
	
	MeshModel *mm() {
	return currentMesh;
	}
	
  QList<MeshModel *> meshList;	

void addMesh(MeshModel *mm)
{
 meshList.push_back(mm);
 currentMesh=meshList.back();
}

private:
  MeshModel *currentMesh;	
};

#endif
