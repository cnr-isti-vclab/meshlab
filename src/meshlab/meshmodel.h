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

#ifndef MESHMODEL_H
#define MESHMODEL_H
#include <GL/glew.h>

#include <stdio.h>
#include <time.h>

#include <vcg/simplex/vertexplus/base.h>
#include <vcg/simplex/vertexplus/component_ocf.h>
#include <vcg/simplex/edge/edge.h>
#include <vcg/simplex/faceplus/base.h>
#include <vcg/simplex/faceplus/component_ocf.h>
#include <vcg/simplex/face/topology.h>

#include <vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/complex/trimesh/update/flag.h>

#include <wrap/gl/trimesh.h>
#include <wrap/callback.h>
#include <wrap/io_trimesh/io_mask.h>
#include <wrap/io_trimesh/additionalinfo.h>
#include <QList>
#include <QString>

class CEdge;   
class CFaceO;
class CVertexO;

//Vert Mem Occupancy  --- 40b ---

class CVertexO  : public vcg::VertexSimp2< CVertexO, CEdge, CFaceO, 
	vcg::vert::InfoOcf,      /* 4b */
  vcg::vert::Coord3f,     /* 12b */ 
  vcg::vert::BitFlags,    /*  4b */
  vcg::vert::Normal3f,    /* 12b */
  vcg::vert::Qualityf,    /*  4b */
  vcg::vert::Color4b,      /*  4b */
  vcg::vert::VFAdjOcf,    /*  0b */
  vcg::vert::MarkOcf,     /*  0b */
  vcg::vert::CurvaturefOcf, /*  0b */
  vcg::vert::CurvatureDirfOcf  /*  0b */
  >{ 
};

  
class CEdge : public vcg::Edge<CEdge,CVertexO> {
public:
  inline CEdge() {};
  inline CEdge( CVertexO * v0, CVertexO * v1):vcg::Edge<CEdge,CVertexO>(v0,v1){};
  static inline CEdge OrderedEdge(VertexType* v0,VertexType* v1){
   if(v0<v1) return CEdge(v0,v1);
   else return CEdge(v1,v0);
  }

  //inline CEdge( Edge<CEdge,CVertexO> &e):Edge<CEdge,CVertexO>(e){};
};

//Face Mem Occupancy  --- 32b ---

class CFaceO    : public vcg::FaceSimp2<  CVertexO, CEdge, CFaceO,  
      vcg::face::InfoOcf,              /* 4b */
      vcg::face::VertexRef,            /*12b */
      vcg::face::BitFlags,             /* 4b */
      vcg::face::Normal3f,             /*12b */
      vcg::face::MarkOcf,              /* 0b */
      vcg::face::Color4bOcf,           /* 0b */
      vcg::face::FFAdjOcf,             /* 0b */
      vcg::face::VFAdjOcf,             /* 0b */
      vcg::face::WedgeTexCoordfOcf      /* 0b */
    > {};

class CMeshO    : public vcg::tri::TriMesh< vcg::vert::vector_ocf<CVertexO>, vcg::face::vector_ocf<CFaceO> > {
public :
	int sfn; //The number of selected faces.
  vcg::Matrix44f Tr; // Usually it is the identity. It is applied in rendering and filters can or cannot use it. (most of the filter will ignore this)
  const vcg::Box3f &trBB() { 
	static vcg::Box3f bb;
	bb.SetNull();
	bb.Add(Tr,bbox);
		return bb;
	}
};

/*
MeshModel Class
The base class for representing a single mesh.
It contains a single vcg mesh object with some additional information for keeping track of its origin and of what info it has. 
*/

class MeshModel : public vcg::tri::io::Mask
{
public:
/* This enum specify the various mesh elements.
	It is used for 
	*/
 	enum MeshElement {  
		MM_NONE						= 0x0000,
		MM_VERTCOORD			= 0x0001,
		MM_VERTNORMAL			= 0x0002,
		MM_VERTCOLOR			= 0x0004,
		MM_VERTQUALITY		= 0x0008,
		MM_VERTGEOM = MM_VERTCOORD | MM_VERTNORMAL,
		MM_VERTMARK				= 0x0010,
		MM_VERTFACETOPO		= 0x0020,
		MM_VERTCONTAINER	= 0x0040,
		MM_VERTFLAG				= 0x0080,
		MM_BORDERFLAG			= 0x0100,
		MM_FACECOLOR			= 0x0200,
		MM_FACEMARK				= 0x0400,
		MM_FACETOPO				= 0x0800,
		MM_WEDGTEXCOORD		= 0x1000,
		MM_CURV						= 0x2000,
		MM_CURVDIR				= 0x4000,
		MM_FACESELECTION			= 0x8000,
		MM_ALL						= 0xffff
	} ;


  CMeshO cm;

public:
  vcg::GlTrimesh<CMeshO> glw;
	std::string fileName;

  // Bitmask denoting what fields are currently kept updated in mesh
  // it is composed by OR-ing MM_XXXX enums (defined in the above FilterReq)
  int currentDataMask;

  // Bitmask denoting what fields are loaded/saved
  // it is composed by OR-ing IOM_XXXX enums (defined in tri::io::Mask)
  int ioMask;
	
	bool visible; // used in rendering; Needed for toggling on and off the meshes
		
  //abstract pointer to fileformat's dependent additional info
  AdditionalInfo* addinfo;

  MeshModel(const char *meshName=0) {    
    glw.m=&cm; 
    currentDataMask=MM_NONE;
    ioMask= IOM_VERTCOORD | IOM_FACEINDEX | IOM_FLAGS | IOM_VERTNORMAL;
    visible=true;
		cm.Tr.SetIdentity();
		cm.sfn=0;
		if(meshName) fileName=meshName;
  }
  bool Render(vcg::GLW::DrawMode dm, vcg::GLW::ColorMode cm, vcg::GLW::TextureMode tm);
  bool RenderSelectedFaces();

	
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
	  vcg::tri::UpdateTopology<CMeshO>::FaceFace(cm);
   }
   if( ( (neededDataMask & MM_VERTFACETOPO)!=0) && (currentDataMask& MM_VERTFACETOPO)==0)			
   {
		cm.vert.EnableVFAdjacency();
    cm.face.EnableVFAdjacency();
    currentDataMask |= MM_VERTFACETOPO;
	  vcg::tri::UpdateTopology<CMeshO>::VertexFace(cm);
   }
   if( ( (neededDataMask & MM_BORDERFLAG)!=0) && (currentDataMask& MM_BORDERFLAG)==0)			
   {
     if(currentDataMask& MM_FACETOPO) vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(cm);
     else vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromNone(cm);
		 vcg::tri::UpdateFlags<CMeshO>::VertexBorderFromFace(cm);

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
	 if( ( (neededDataMask & MM_VERTMARK)!=0) && (currentDataMask& MM_VERTMARK)==0)			
		{
			cm.vert.EnableMark();
			currentDataMask |= MM_VERTMARK;
		}
		if( ( (neededDataMask & MM_CURV)!=0) && (currentDataMask& MM_CURV)==0)			
		{
			cm.vert.EnableCurvature();
			currentDataMask |= MM_CURV;
		}
		if( ( (neededDataMask & MM_CURVDIR)!=0) && (currentDataMask& MM_CURVDIR)==0)			
		{
			cm.vert.EnableCurvatureDir();
			currentDataMask |= MM_CURVDIR;
		}
  }

   void clearDataMask(int neededDataMask)
   {
     currentDataMask = currentDataMask & (~neededDataMask);
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
      drawMode	= vcg::GLW::DMFlat;
			colorMode = vcg::GLW::CMNone;
			textureMode = vcg::GLW::TMNone;

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
	{
		currentMesh = NULL;
		busy=false;
	}
 
 ~MeshDocument()
		{
			foreach(MeshModel *mmp, meshList)
					delete mmp;
   }
	MeshModel *getMesh(int i)
	{ 
		return meshList.at(i);
	}
	
	MeshModel *getMesh(const char *name);
	
	void setCurrentMesh(unsigned int i)
	{
	  assert(i < (unsigned int)meshList.size());
		currentMesh=meshList.at(i);
	}
	
	MeshModel *mm() {
		return currentMesh;
	}
/// The very important member:
/// The list of MeshModels. 
	
  QList<MeshModel *> meshList;	
	
	int size() const {return meshList.size();}
	bool busy;    // used in processing. To disable access to the mesh by the rendering thread

	MeshModel *addNewMesh(const char *meshName,MeshModel *newMesh=0)
	{
		QString newName=meshName;
		for(QList<MeshModel*>::iterator mmi=meshList.begin();mmi!=meshList.end();++mmi)
		{
			QString shortName( (*mmi)->fileName.c_str() );
			if(shortName == newName) 
				newName = newName+"_copy";
		}
		if(newMesh==0) 
			newMesh=new MeshModel(qPrintable(newName));
		else 
			newMesh->fileName = qPrintable(newName);
		meshList.push_back(newMesh);
		currentMesh=meshList.back();
		return newMesh;
	}
	

  bool delMesh(MeshModel *mmToDel)
	{
		if(meshList.size()==1) return false; 
		QMutableListIterator<MeshModel *> i(meshList);
		while (i.hasNext()) {
     MeshModel *md = i.next();
     if (md==mmToDel) 
		  {
         i.remove();
				 delete mmToDel;
			}
		}
		if(currentMesh == mmToDel) setCurrentMesh(0);
		return true;
 }
		
  int vn() /// Sum of all the vertices of all the meshes
	{ 
			int tot=0;
			foreach(MeshModel *mmp, meshList)
					tot+= mmp->cm.vn;
			return tot;
	}
	int fn() { 
			int tot=0;
			foreach(MeshModel *mmp, meshList)
					tot+= mmp->cm.fn;
			return tot;
	}		

 vcg::Box3f bbox()
 {
		vcg::Box3f FullBBox;
		foreach(MeshModel * mp, meshList) 
			FullBBox.Add(mp->cm.Tr,mp->cm.bbox);
		return FullBBox;
 }
	private:
		MeshModel *currentMesh;	
};

/* 

*/
class MeshModelState
{
	public:
	int changeMask; // a bit mask indicating What have been changed.
	MeshModel *m; // the mesh which the changes refers to.
	std::vector<vcg::Color4b> vertColor;
	std::vector<vcg::Point3f> vertCoord;
	std::vector<vcg::Point3f> vertNormal;
	std::vector<bool> faceSelection;	
	
	void create(int _mask, MeshModel* _m)
	{
		m=_m;
		changeMask=_mask;
		if(changeMask & MeshModel::MM_VERTCOLOR)
		 {
			 vertColor.resize(m->cm.vert.size());
			 std::vector<vcg::Color4b>::iterator ci;
			 CMeshO::VertexIterator vi;
			 for(vi=m->cm.vert.begin(),ci=vertColor.begin();vi!=m->cm.vert.end();++vi,++ci) 
				 if(!(*vi).IsD()) (*ci)=(*vi).C();
		} else if(changeMask & MeshModel::MM_VERTCOORD)
		{
			vertCoord.resize(m->cm.vert.size());
			std::vector<vcg::Point3f>::iterator ci;
			CMeshO::VertexIterator vi;
			 for(vi=m->cm.vert.begin(),ci=vertCoord.begin();vi!=m->cm.vert.end();++vi,++ci) 
				 if(!(*vi).IsD()) (*ci)=(*vi).P();
		} else if(changeMask & MeshModel::MM_FACESELECTION)
		{
			faceSelection.resize(m->cm.face.size());
			std::vector<bool>::iterator ci;
			CMeshO::FaceIterator fi;
			for(fi = m->cm.face.begin(), ci=faceSelection.begin(); fi != m->cm.face.end(); ++fi, ++ci)
			{
				if((*fi).IsS())
					(*ci) = true;
				else
					(*ci) = false;
			}
		}
	}
	
	bool apply(MeshModel *_m)
	{
	  if(_m != m) return false;	
		if(changeMask & MeshModel::MM_VERTCOLOR)
		{
			if(vertColor.size() != m->cm.vert.size()) return false;
			std::vector<vcg::Color4b>::iterator ci;
			CMeshO::VertexIterator vi;
			for(vi=m->cm.vert.begin(),ci=vertColor.begin();vi!=m->cm.vert.end();++vi,++ci) 
				if(!(*vi).IsD()) (*vi).C()=(*ci);
		} else if(changeMask & MeshModel::MM_VERTCOORD)
		{
			if(vertCoord.size() != m->cm.vert.size()) return false;
			std::vector<vcg::Point3f>::iterator ci;
			CMeshO::VertexIterator vi;
			for(vi=m->cm.vert.begin(),ci=vertCoord.begin();vi!=m->cm.vert.end();++vi,++ci) 
				if(!(*vi).IsD()) (*vi).P()=(*ci);
		} else if(changeMask & MeshModel::MM_FACESELECTION)
		{
			if(faceSelection.size() != m->cm.face.size()) return false;
			std::vector<bool>::iterator ci;
			CMeshO::FaceIterator fi;
			for(fi = m->cm.face.begin(), ci=faceSelection.begin(); fi != m->cm.face.end(); ++fi, ++ci)
			{
				if((*ci))
					(*fi).SetS();
				else
					(*fi).ClearS();
			}
		}
		return true;
  }
		
	bool isValid(MeshModel *m);
};

#endif
