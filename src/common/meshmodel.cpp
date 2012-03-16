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


#include <QString>
#include <QtGlobal>
#include <QFileInfo>
#include "meshmodel.h"
#include <wrap/gl/math.h>
#include "scriptinterface.h"
#include <vcg/complex/append.h>


using namespace vcg;

TagBase::TagBase(MeshDocument *parent)
{
 _id=parent->newTagId();
}


//deletes each meshModel
MeshDocument::~MeshDocument()
{
  foreach(MeshModel *mmp, meshList)
    delete mmp;
}

//returns the mesh ata given position in the list
MeshModel *MeshDocument::getMesh(int i)
{
  foreach(MeshModel *mmp, meshList)
  {
    if(mmp->id() == i) return mmp;
  }
  //assert(0);
  return 0;
}

MeshModel *MeshDocument::getMesh(QString name)
{
	foreach(MeshModel *mmp, meshList)
			{
                if(mmp->shortName() == name) return mmp;
			}
	//assert(0);
	return 0;
}

MeshModel *MeshDocument::getMeshByFullName(QString pathName)
{
	foreach(MeshModel *mmp, meshList)
	{
		if(mmp->fullName() == pathName) return mmp;
	}
	//assert(0);
	return 0;
}

QList<TagBase *> MeshDocument::getMeshTags(int meshId)
{
	QList<TagBase *> meshTags;
	foreach(TagBase *tag, tagList)
		foreach(int id, tag->referringMeshes)
		{
			if(id==meshId)
				meshTags.append(tag);
		}
	
	return meshTags;
}

void MeshDocument::setCurrentMesh( int i)
{
  foreach(MeshModel *mmp, meshList)
  {
    if(mmp->id() == i)
    {
      currentMesh = mmp;
      emit currentMeshChanged(i);
      return;
    }
  }
  assert(0);
  return;
}

//returns the raster at a given position in the list
RasterModel *MeshDocument::getRaster(int i)
{
  foreach(RasterModel *rmp, rasterList)
  {
    if(rmp->id() == i) return rmp;
  }
  //assert(0);
  return 0;
}

//if i is <0 it means that no currentRaster is set
void MeshDocument::setCurrentRaster( int i)
{
  if(i<0)
  {
	  currentRaster=0;
	  return;
  }

  foreach(RasterModel *rmp, rasterList)
  {
    if(rmp->id() == i)
    {
      currentRaster = rmp;
      return;
    }
  }
  assert(0);
  return;
}

template <class LayerElement>
QString NameDisambiguator(QList<LayerElement*> &elemList, QString meshLabel )
{
  QFileInfo info(meshLabel);
  QString newName=info.fileName();
  typename QList<LayerElement*>::iterator mmi;
  for(mmi=elemList.begin(); mmi!=elemList.end(); ++mmi)
  {
    if((*mmi)->label() == newName)
    {
      QFileInfo fi((*mmi)->label());
      QString baseName = fi.baseName(); //  all characters in the file up to the first '.' Eg "/tmp/archive.tar.gz" -> "archive"
      int lastNum = baseName.right(1).toInt();
      if( baseName.right(2).toInt() >= 10)
        lastNum = baseName.right(2).toInt();
      if(lastNum)
        newName = baseName.left(baseName.length()-(lastNum<10?1:2))+QString::number(lastNum+1);
      else
        newName = baseName+"_1";
      if (info.suffix() != QString(""))
      newName = newName + "." + info.suffix();
    }
  }
  return newName;
}

/*
 When you create a new mesh it can be either a newly created one or an opened one.
 If it is an opened one the fullpathname is meaningful and the label, by default is just the short name.
 If it is a newly created one the fullpath is an empty string and the user has to provide a label.
 */

MeshModel * MeshDocument::addNewMesh(QString fullPath, QString label, bool setAsCurrent)
{	
  QString newlabel = NameDisambiguator(this->meshList,label);

  if(!fullPath.isEmpty())
  {
	  QFileInfo fi(fullPath);
	  fullPath = fi.absoluteFilePath(); 
  }

  MeshModel *newMesh = new MeshModel(this,qPrintable(fullPath),newlabel);
  meshList.push_back(newMesh);
  emit meshSetChanged();

  if(setAsCurrent)
    this->setCurrentMesh(newMesh->id());
  return newMesh;
}

bool MeshDocument::delMesh(MeshModel *mmToDel)
{
	if(meshList.size()==1) return false;
  if(!meshList.removeOne(mmToDel)) return false;
  if(currentMesh == mmToDel)
        setCurrentMesh(this->meshList.at(0)->id());

  delete mmToDel;

  emit meshSetChanged();
	return true;
}

RasterModel * MeshDocument::addNewRaster(/*QString fullPathFilename*/)
{
  QFileInfo info(fullPathFilename);
  QString newLabel=info.fileName();
  QString newName = NameDisambiguator(this->rasterList, newLabel);

  RasterModel *newRaster=new RasterModel(this, newLabel);
	rasterList.push_back(newRaster);

	//Add new plane
  //Plane *plane = new Plane(newRaster, fullPathFilename, QString());
  //newRaster->addPlane(plane);

	this->setCurrentRaster(newRaster->id());

  emit rasterSetChanged();
	return newRaster;
}

bool MeshDocument::delRaster(RasterModel *rasterToDel)
{
	QMutableListIterator<RasterModel *> i(rasterList);

	while (i.hasNext())
	{
		RasterModel *r = i.next();

		if (r==rasterToDel)
		{
			i.remove();
			delete rasterToDel;
		}
	}

	if(currentRaster == rasterToDel)
		setCurrentRaster(-1);

	emit rasterSetChanged();

	return true;
}

void MeshDocument::addNewTag(TagBase *newTag)
{
	tagList.append(newTag);
}

void MeshDocument::removeTag(int id){
	for(int i = 0; i<tagList.count(); i++){
		TagBase *tag = (TagBase *)tagList.at(i);
		if(tag->id() ==id){
			tagList.removeAt(i);
			delete tag;
		}
	}
}

bool MeshDocument::hasBeenModified()
{
	int ii = 0;
	while(ii < meshList.size())
	{
		if (meshList[ii]->meshModified())
			return true;
		++ii;
	}
	return false;
}

void MeshDocument::updateRenderMesh( MeshModel& mm )
{
  static QTime currTime;
  if(currTime.elapsed()< 100) return;
	renderState().updateMesh(mm.id(),mm.cm);
	emit meshUpdated();
	currTime.start();
}

void MeshModel::Clear()
{
  meshModified() = false;
  glw.m=&cm;
  // These data are always active on the mesh
  currentDataMask = MM_NONE;
  currentDataMask |= MM_VERTCOORD | MM_VERTNORMAL | MM_VERTFLAG ;
  currentDataMask |= MM_FACEVERT  | MM_FACENORMAL | MM_FACEFLAG ;

  visible=true;
  cm.Tr.SetIdentity();
  cm.sfn=0;
  cm.svn=0;
}

MeshModel::MeshModel(MeshDocument *_parent, QString fullFileName, QString labelName)
:MeshLabRenderMesh()
{

  Clear();
  parent=_parent;
  _id=parent->newMeshId();
  if(!fullFileName.isEmpty())   this->fullPathFileName=fullFileName;
  if(!labelName.isEmpty())     this->_label=labelName;
}

QString MeshModel::relativePathName() const
{
  QDir documentDir (documentPathName());
  QString relPath=documentDir.relativeFilePath(this->fullPathFileName);

  if(relPath.size()>1 && relPath[0]=='.' &&  relPath[1]=='.')
      qDebug("Error we have a mesh that is not in the same folder of the project: %s ",qPrintable(relPath));

  return relPath;
}

QString MeshModel::documentPathName() const
{
  return parent->pathName();
}

int MeshModel::io2mm(int single_iobit)
{
	switch(single_iobit) 
	{
		case tri::io::Mask::IOM_NONE					: return  MM_NONE;
		case tri::io::Mask::IOM_VERTCOORD		: return  MM_VERTCOORD;
		case tri::io::Mask::IOM_VERTCOLOR		: return  MM_VERTCOLOR;
		case tri::io::Mask::IOM_VERTFLAGS		: return  MM_VERTFLAG;
		case tri::io::Mask::IOM_VERTQUALITY	: return  MM_VERTQUALITY;
		case tri::io::Mask::IOM_VERTNORMAL		: return  MM_VERTNORMAL;
		case tri::io::Mask::IOM_VERTTEXCOORD : return  MM_VERTTEXCOORD;
		case tri::io::Mask::IOM_VERTRADIUS		: return  MM_VERTRADIUS;
		
		case tri::io::Mask::IOM_FACEINDEX   		: return  MM_FACEVERT  ;
		case tri::io::Mask::IOM_FACEFLAGS   		: return  MM_FACEFLAG  ;
		case tri::io::Mask::IOM_FACECOLOR   		: return  MM_FACECOLOR  ;
		case tri::io::Mask::IOM_FACEQUALITY 		: return  MM_FACEQUALITY;
		case tri::io::Mask::IOM_FACENORMAL  		: return  MM_FACENORMAL ;
		
		case tri::io::Mask::IOM_WEDGTEXCOORD 		: return  MM_WEDGTEXCOORD;
		case tri::io::Mask::IOM_WEDGCOLOR				: return  MM_WEDGCOLOR;
		case tri::io::Mask::IOM_WEDGNORMAL   		: return  MM_WEDGNORMAL  ;

		case tri::io::Mask::IOM_BITPOLYGONAL   	: return  MM_POLYGONAL  ;

		default:
			assert(0);
			return MM_NONE;  // FIXME: Returning this is not the best solution (!)
			break;
	} ;
}

Plane::Plane(RasterModel *_parent, const QString pathName, const QString _semantic){
	parent = _parent;
	semantic =_semantic;
	fullPathFileName = pathName;

	image = QImage(pathName);
}

RasterModel::RasterModel(MeshDocument *parent, QString _rasterName) {
  _id=parent->newRasterId(); 
  par = parent;
  this->_label= _rasterName;
  visible=true;
}

void RasterModel::addPlane(Plane *plane)
{
	planeList.append(plane);
	currentPlane = plane;
}

void MeshModelState::create(int _mask, MeshModel* _m)
{
    m=_m;
    changeMask=_mask;
    if(changeMask & MeshModel::MM_VERTCOLOR)
    {
        vertColor.resize(m->cm.vert.size());
        std::vector<Color4b>::iterator ci;
        CMeshO::VertexIterator vi;
        for(vi = m->cm.vert.begin(), ci = vertColor.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
            if(!(*vi).IsD()) (*ci)=(*vi).C();
    }

    if(changeMask & MeshModel::MM_VERTQUALITY)
    {
        vertQuality.resize(m->cm.vert.size());
        std::vector<float>::iterator qi;
        CMeshO::VertexIterator vi;
        for(vi = m->cm.vert.begin(), qi = vertQuality.begin(); vi != m->cm.vert.end(); ++vi, ++qi)
            if(!(*vi).IsD()) (*qi)=(*vi).Q();
    }

    if(changeMask & MeshModel::MM_VERTCOORD)
    {
        vertCoord.resize(m->cm.vert.size());
        std::vector<Point3f>::iterator ci;
        CMeshO::VertexIterator vi;
        for(vi = m->cm.vert.begin(), ci = vertCoord.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
             if(!(*vi).IsD()) (*ci)=(*vi).P();
    }

    if(changeMask & MeshModel::MM_VERTNORMAL)
    {
        vertNormal.resize(m->cm.vert.size());
        std::vector<Point3f>::iterator ci;
        CMeshO::VertexIterator vi;
        for(vi = m->cm.vert.begin(), ci = vertNormal.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
             if(!(*vi).IsD()) (*ci)=(*vi).N();
    }

    if(changeMask & MeshModel::MM_FACEFLAGSELECT)
    {
        faceSelection.resize(m->cm.face.size());
        std::vector<bool>::iterator ci;
        CMeshO::FaceIterator fi;
        for(fi = m->cm.face.begin(), ci = faceSelection.begin(); fi != m->cm.face.end(); ++fi, ++ci)
         if(!(*fi).IsD()) (*ci) = (*fi).IsS();
    }

	if(changeMask & MeshModel::MM_VERTFLAGSELECT)
	{
		vertSelection.resize(m->cm.vert.size());
		std::vector<bool>::iterator ci;
		CMeshO::VertexIterator vi;
		for(vi = m->cm.vert.begin(), ci = vertSelection.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
			if(!(*vi).IsD()) (*ci) = (*vi).IsS();
	}

  if(changeMask & MeshModel::MM_TRANSFMATRIX)
      Tr = m->cm.Tr;
  if(changeMask & MeshModel::MM_CAMERA)
      this->shot = m->cm.shot;
}

bool MeshModelState::apply(MeshModel *_m)
{
  if(_m != m) return false;
    if(changeMask & MeshModel::MM_VERTCOLOR)
    {
        if(vertColor.size() != m->cm.vert.size()) return false;
        std::vector<Color4b>::iterator ci;
        CMeshO::VertexIterator vi;
        for(vi = m->cm.vert.begin(), ci = vertColor.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
            if(!(*vi).IsD()) (*vi).C()=(*ci);
    }
    if(changeMask & MeshModel::MM_VERTQUALITY)
    {
        if(vertQuality.size() != m->cm.vert.size()) return false;
        std::vector<float>::iterator qi;
        CMeshO::VertexIterator vi;
        for(vi = m->cm.vert.begin(), qi = vertQuality.begin(); vi != m->cm.vert.end(); ++vi, ++qi)
            if(!(*vi).IsD()) (*vi).Q()=(*qi);
    }

    if(changeMask & MeshModel::MM_VERTCOORD)
    {
        if(vertCoord.size() != m->cm.vert.size()) return false;
        std::vector<Point3f>::iterator ci;
        CMeshO::VertexIterator vi;
        for(vi = m->cm.vert.begin(), ci = vertCoord.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
            if(!(*vi).IsD()) (*vi).P()=(*ci);
    }

    if(changeMask & MeshModel::MM_VERTNORMAL)
    {
        if(vertNormal.size() != m->cm.vert.size()) return false;
        std::vector<Point3f>::iterator ci;
        CMeshO::VertexIterator vi;
        for(vi = m->cm.vert.begin(), ci=vertNormal.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
            if(!(*vi).IsD()) (*vi).N()=(*ci);

        //now reset the face normals
        tri::UpdateNormals<CMeshO>::PerFaceNormalized(m->cm);
    }

    if(changeMask & MeshModel::MM_FACEFLAGSELECT)
    {
        if(faceSelection.size() != m->cm.face.size()) return false;
        std::vector<bool>::iterator ci;
        CMeshO::FaceIterator fi;
        for(fi = m->cm.face.begin(), ci = faceSelection.begin(); fi != m->cm.face.end(); ++fi, ++ci)
        {
            if((*ci))
                (*fi).SetS();
            else
                (*fi).ClearS();
        }
    }

	if(changeMask & MeshModel::MM_VERTFLAGSELECT)
	{
		if(vertSelection.size() != m->cm.vert.size()) return false;
		std::vector<bool>::iterator ci;
		CMeshO::VertexIterator vi;
		for(vi = m->cm.vert.begin(), ci = vertSelection.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
		{
			if((*ci))
				(*vi).SetS();
			else
				(*vi).ClearS();
		}
	}

    if(changeMask & MeshModel::MM_TRANSFMATRIX)
        m->cm.Tr=Tr;
    if(changeMask & MeshModel::MM_CAMERA)
        m->cm.shot = this->shot;

    return true;
}

/**** DATAMASK STUFF ****/

bool MeshModel::hasDataMask(const int maskToBeTested) const
{
  return ((currentDataMask & maskToBeTested)!= 0);
}
  void MeshModel::updateDataMask(MeshModel *m)
{
  updateDataMask(m->currentDataMask);
}

void MeshModel::updateDataMask(int neededDataMask)
{
  if( ( (neededDataMask & MM_FACEFACETOPO)!=0) && !hasDataMask(MM_FACEFACETOPO) )
  {
    cm.face.EnableFFAdjacency();
    tri::UpdateTopology<CMeshO>::FaceFace(cm);
  }
  if( ( (neededDataMask & MM_VERTFACETOPO)!=0) && !hasDataMask(MM_VERTFACETOPO) )
  {
    cm.vert.EnableVFAdjacency();
    cm.face.EnableVFAdjacency();
    tri::UpdateTopology<CMeshO>::VertexFace(cm);
  }
  if( ( (neededDataMask & MM_WEDGTEXCOORD)!=0)	&& !hasDataMask(MM_WEDGTEXCOORD)) 	cm.face.EnableWedgeTex();
  if( ( (neededDataMask & MM_FACECOLOR)!=0)			&& !hasDataMask(MM_FACECOLOR))			cm.face.EnableColor();
  if( ( (neededDataMask & MM_FACEQUALITY)!=0)		&& !hasDataMask(MM_FACEQUALITY))		cm.face.EnableQuality();
  if( ( (neededDataMask & MM_FACEMARK)!=0)			&& !hasDataMask(MM_FACEMARK))				cm.face.EnableMark();
  if( ( (neededDataMask & MM_VERTMARK)!=0)			&& !hasDataMask(MM_VERTMARK))				cm.vert.EnableMark();
  if( ( (neededDataMask & MM_VERTCURV)!=0)			&& !hasDataMask(MM_VERTCURV))				cm.vert.EnableCurvature();
  if( ( (neededDataMask & MM_VERTCURVDIR)!=0)		&& !hasDataMask(MM_VERTCURVDIR))		cm.vert.EnableCurvatureDir();
  if( ( (neededDataMask & MM_VERTRADIUS)!=0)		&& !hasDataMask(MM_VERTRADIUS))			cm.vert.EnableRadius();
  if( ( (neededDataMask & MM_VERTTEXCOORD)!=0)  && !hasDataMask(MM_VERTTEXCOORD))		cm.vert.EnableTexCoord();

  if(  ( (neededDataMask & MM_FACEFLAGBORDER) && !hasDataMask(MM_FACEFLAGBORDER) ) ||
       ( (neededDataMask & MM_VERTFLAGBORDER) && !hasDataMask(MM_VERTFLAGBORDER) )    )
  {
    if( (currentDataMask & MM_FACEFACETOPO) || (neededDataMask & MM_FACEFACETOPO))
         tri::UpdateFlags<CMeshO>::FaceBorderFromFF(cm);
    else tri::UpdateFlags<CMeshO>::FaceBorderFromNone(cm);
    tri::UpdateFlags<CMeshO>::VertexBorderFromFace(cm);
  }

  currentDataMask |= neededDataMask;
 }

void MeshModel::clearDataMask(int unneededDataMask)
{
  if( ( (unneededDataMask & MM_VERTFACETOPO)!=0)	&& hasDataMask(MM_VERTFACETOPO)) {cm.face.DisableVFAdjacency();
                                                                                    cm.vert.DisableVFAdjacency(); }
  if( ( (unneededDataMask & MM_FACEFACETOPO)!=0)	&& hasDataMask(MM_FACEFACETOPO))	cm.face.DisableFFAdjacency();

  if( ( (unneededDataMask & MM_WEDGTEXCOORD)!=0)	&& hasDataMask(MM_WEDGTEXCOORD)) 	cm.face.DisableWedgeTex();
  if( ( (unneededDataMask & MM_FACECOLOR)!=0)			&& hasDataMask(MM_FACECOLOR))			cm.face.DisableColor();
  if( ( (unneededDataMask & MM_FACEQUALITY)!=0)		&& hasDataMask(MM_FACEQUALITY))		cm.face.DisableQuality();
  if( ( (unneededDataMask & MM_FACEMARK)!=0)			&& hasDataMask(MM_FACEMARK))			cm.face.DisableMark();
  if( ( (unneededDataMask & MM_VERTMARK)!=0)			&& hasDataMask(MM_VERTMARK))			cm.vert.DisableMark();
  if( ( (unneededDataMask & MM_VERTCURV)!=0)			&& hasDataMask(MM_VERTCURV))			cm.vert.DisableCurvature();
  if( ( (unneededDataMask & MM_VERTCURVDIR)!=0)		&& hasDataMask(MM_VERTCURVDIR))		cm.vert.DisableCurvatureDir();
  if( ( (unneededDataMask & MM_VERTRADIUS)!=0)		&& hasDataMask(MM_VERTRADIUS))		cm.vert.DisableRadius();
  if( ( (unneededDataMask & MM_VERTTEXCOORD)!=0)	&& hasDataMask(MM_VERTTEXCOORD))	cm.vert.DisableTexCoord();

  currentDataMask = currentDataMask & (~unneededDataMask);
}

void MeshModel::Enable(int openingFileMask)
{
  if( openingFileMask & tri::io::Mask::IOM_VERTTEXCOORD ) 
	  updateDataMask(MM_VERTTEXCOORD);
  if( openingFileMask & tri::io::Mask::IOM_WEDGTEXCOORD ) 
	  updateDataMask(MM_WEDGTEXCOORD);
  if( openingFileMask & tri::io::Mask::IOM_VERTCOLOR    ) 
	  updateDataMask(MM_VERTCOLOR);
  if( openingFileMask & tri::io::Mask::IOM_FACECOLOR    ) 
	  updateDataMask(MM_FACECOLOR);
  if( openingFileMask & tri::io::Mask::IOM_VERTRADIUS   ) updateDataMask(MM_VERTRADIUS);
  if( openingFileMask & tri::io::Mask::IOM_CAMERA       ) updateDataMask(MM_CAMERA);
  if( openingFileMask & tri::io::Mask::IOM_VERTQUALITY  ) updateDataMask(MM_VERTQUALITY);
  if( openingFileMask & tri::io::Mask::IOM_FACEQUALITY  ) updateDataMask(MM_FACEQUALITY);
  if( openingFileMask & tri::io::Mask::IOM_BITPOLYGONAL ) updateDataMask(MM_POLYGONAL);
}

bool& MeshModel::meshModified()
{
	return this->modified;
}

MeshLabRenderMesh::MeshLabRenderMesh()
:glw(),cm()
{
}

MeshLabRenderMesh::MeshLabRenderMesh(CMeshO& mesh )
:glw(),cm()
{
	vcg::tri::Append<CMeshO,CMeshO>::MeshCopy(cm,mesh);
	//cm.Tr = mesh.Tr;
	cm.Tr.SetIdentity();
	cm.sfn = mesh.sfn;
	cm.svn = mesh.svn;
	glw.m = &cm; 
}

bool MeshLabRenderMesh::render(vcg::GLW::DrawMode dm,vcg::GLW::ColorMode colm,vcg::GLW::TextureMode tm )
{
	if (glw.m != NULL)
	{
		glPushMatrix();
		glMultMatrix(glw.m->Tr);
		if( (colm == vcg::GLW::CMPerFace)  && (!vcg::tri::HasPerFaceColor(*glw.m)) ) 
			colm=vcg::GLW::CMNone;
		if( (tm == vcg::GLW::TMPerWedge )&& (!vcg::tri::HasPerWedgeTexCoord(*glw.m)) ) 
			tm=vcg::GLW::TMNone;
		if( (tm == vcg::GLW::TMPerWedgeMulti )&& (!vcg::tri::HasPerWedgeTexCoord(*glw.m)) ) 
			tm=vcg::GLW::TMNone;
		glw.Draw(dm,colm,tm);
		glPopMatrix();
		return true;
	}
	return false;
}

bool MeshLabRenderMesh::renderSelectedFace()
{
	if (glw.m != NULL)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
		glColor4f(1.0f,0.0,0.0,.3f);
		glPolygonOffset(-1.0, -1);
		CMeshO::FaceIterator fi;
		glPushMatrix();
		glMultMatrix(glw.m->Tr);
		glBegin(GL_TRIANGLES);
		glw.m->sfn=0;
		for(fi=glw.m->face.begin();fi!=glw.m->face.end();++fi)
		{
			if(!(*fi).IsD() && (*fi).IsS())
			{
				glVertex((*fi).cP(0));
				glVertex((*fi).cP(1));
				glVertex((*fi).cP(2));
				++glw.m->sfn;
			}
		}
		glEnd();
		glPopMatrix();
		glPopAttrib();
		return true;
	}
	return false;
}

bool MeshLabRenderMesh::renderSelectedVert()
{
	if (glw.m != NULL)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
		glColor4f(1.0f,0.0,0.0,.3f);
		glDepthRange(0.00,0.999);
		glPointSize(3.0);
		glPushMatrix();
		glMultMatrix(glw.m->Tr);
		glBegin(GL_POINTS);
		glw.m->svn=0;
		CMeshO::VertexIterator vi;
		for(vi=glw.m->vert.begin();vi!=glw.m->vert.end();++vi)
		{
			if(!(*vi).IsD() && (*vi).IsS())
			{
				glVertex((*vi).cP());
				++glw.m->svn;
			}
		}
		glEnd();
		glPopMatrix();
		glPopAttrib();
		return true;
	}
	return false;
}

MeshLabRenderMesh::~MeshLabRenderMesh()
{
	glw.m = NULL;
	cm.Clear();
	CMeshO::VertContainer tempVert;
	CMeshO::FaceContainer tempFace;
	cm.vert.swap(tempVert);
	cm.face.swap(tempFace);
}

MeshLabRenderState::MeshLabRenderState()
:_rendermap(),_mutdoc(QReadWriteLock::Recursive)
{

}

MeshLabRenderState::~MeshLabRenderState()
{
	clearState();
}

bool MeshLabRenderState::updateMesh(const int id, CMeshO& mm )
{
	acquireRenderDocumentWrite();
	QMap<int,MeshLabRenderMesh*>::iterator it = _rendermap.find(id);
	if (it != _rendermap.end())
	{
		removeMesh(it);
		_rendermap[id] = new MeshLabRenderMesh(mm);
		releaseRenderDocument();
		return true;
	}
	releaseRenderDocument();
	return false;
}

void MeshLabRenderState::addMesh(const int id,CMeshO& mm )
{
	acquireRenderDocumentWrite();
	if (!_rendermap.contains(id))
	{
		_rendermap[id] = new MeshLabRenderMesh(mm);
	}
	releaseRenderDocument();
}

QMap<int,MeshLabRenderMesh*>::iterator MeshLabRenderState::removeMesh(QMap<int,MeshLabRenderMesh*>::iterator it )
{	
	acquireRenderDocumentWrite();
	if (it != _rendermap.end())
	{
		MeshLabRenderMesh* tmp = it.value();
		delete tmp;
		QMap<int,MeshLabRenderMesh*>::iterator tmpit = _rendermap.erase(it);
		releaseRenderDocument();
		return tmpit;
	}
	releaseRenderDocument();
	return _rendermap.end();
}

void MeshLabRenderState::clearState()
{
	acquireRenderDocumentWrite();
	QMap<int,MeshLabRenderMesh*>::iterator it = _rendermap.begin();
	while(it != _rendermap.end())
		it = removeMesh(it);
	releaseRenderDocument();
}

void MeshLabRenderState::copyBack( const int id,CMeshO& mm ) const
{
//	mm.Clear();
//	mm.vert.swap(CMeshO::VertContainer());
//	mm.face.swap(CMeshO::FaceContainer());
//	vcg::tri::Append<CMeshO,CMeshO>::MeshCopy(mm,_rendermap[id]->cm);
}

void MeshLabRenderState::render( const int id,vcg::GLW::DrawMode dm,vcg::GLW::ColorMode cm,vcg::GLW::TextureMode tm  )
{
	acquireRenderDocumentRead();
	_rendermap[id]->render(dm,cm,tm);
	releaseRenderDocument();
}

void MeshLabRenderState::render(vcg::GLW::DrawMode dm,vcg::GLW::ColorMode cm,vcg::GLW::TextureMode tm  )
{
	acquireRenderDocumentRead();
	for(QMap<int,MeshLabRenderMesh*>::iterator it = _rendermap.begin();it != _rendermap.end();++it)
		(*it)->render(dm,cm,tm);
	releaseRenderDocument();
}

bool MeshLabRenderState::isMeshInRenderingState( const int id )
{
	bool found = false;
	acquireRenderDocumentRead();
	found = _rendermap.contains(id);
	releaseRenderDocument();
	return found;
}
