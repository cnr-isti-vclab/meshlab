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
#include "mlexception.h"
#include "ml_shared_data_context.h"


using namespace vcg;


//deletes each meshModel
MeshDocument::~MeshDocument()
{
    foreach(MeshModel *mmp, meshList)
        delete mmp;
    foreach(RasterModel* rmp,rasterList)
        delete rmp;
    delete filterHistory;
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


void MeshDocument::setCurrentMesh( int i)
{
    if(i<0)
    {
        currentMesh=0;
        return;
    }
    currentMesh = getMesh(i);
    emit currentMeshChanged(i);
    assert(currentMesh);
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
    QString newName=meshLabel;
    typename QList<LayerElement*>::iterator mmi;

    for(mmi=elemList.begin(); mmi!=elemList.end(); ++mmi)
    {
        if((*mmi)->label() == newName) // if duplicated name found
        {
            QFileInfo fi((*mmi)->label());
            QString baseName = fi.baseName(); //  all characters in the file up to the first '.' Eg "/tmp/archive.tar.gz" -> "archive"
            QString suffix = fi.suffix();
            bool ok;

            // if name ends with a number between parenthesis (XXX),
            // it was himself a duplicated name, and we need to
            // just increase the number between parenthesis
            int numDisamb;
            int startDisamb;
            int endDisamb;

            startDisamb = baseName.lastIndexOf("(");
            endDisamb   = baseName.lastIndexOf(")");
            if((startDisamb!=-1)&&(endDisamb!=-1))
                numDisamb = (baseName.mid((startDisamb+1),(endDisamb-startDisamb-1))).toInt(&ok);
            else
                numDisamb = 0;

            if(startDisamb!=-1)
                newName = baseName.left(startDisamb)+ "(" + QString::number(numDisamb+1) + ")";
            else
                newName = baseName + "(" + QString::number(numDisamb+1) + ")";

            if (suffix != QString(""))
                newName = newName + "." + suffix;

            // now recurse to see if the new name is free
            newName = NameDisambiguator(elemList, newName);
        }
    }
    return newName;
}


MeshModel * MeshDocument::addNewMesh(QString fullPath, QString label, bool setAsCurrent)
{
    QString newlabel = NameDisambiguator(this->meshList,label);

    if(!fullPath.isEmpty())
    {
        QFileInfo fi(fullPath);
        fullPath = fi.absoluteFilePath();
    }

    MeshModel *newMesh = new MeshModel(this,fullPath,newlabel);
    meshList.push_back(newMesh);
    
	if(setAsCurrent)
        this->setCurrentMesh(newMesh->id());

	emit meshSetChanged();
	emit meshAdded(newMesh->id());
    return newMesh;
}

MeshModel * MeshDocument::addOrGetMesh(QString fullPath, QString label, bool setAsCurrent)
{
  MeshModel *newMesh = getMesh(label);
  if(newMesh) {
    if(setAsCurrent)
        this->setCurrentMesh(newMesh->id());
    return newMesh;
  }
  return addNewMesh(fullPath,label,setAsCurrent);
}

bool MeshDocument::delMesh(MeshModel *mmToDel)
{
    if(!meshList.removeOne(mmToDel))
        return false;
    if((currentMesh == mmToDel) && (meshList.size() != 0))
        setCurrentMesh(this->meshList.at(0)->id());
    else if (meshList.size() == 0)
        setCurrentMesh(-1);

    int index = mmToDel->id();
    delete mmToDel;

    emit meshSetChanged();
    emit meshRemoved(index);
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
    {
        if (rasterList.size() > 0)
            setCurrentRaster(rasterList.at(0)->id());
        else
            setCurrentRaster(-1);
    }
    emit rasterSetChanged();

    return true;
}

bool MeshDocument::hasBeenModified()
{
    foreach(MeshModel *m, meshList)
        if(m->meshModified()) return true;
    return false;
}

//void MeshDocument::updateRenderStateMeshes(const QList<int>& mm,const int meshupdatemask)
//{
//    static QTime currTime  = QTime::currentTime();
//    if(currTime.elapsed()< 100)
//        return;
//    for (QList<int>::const_iterator mit = mm.begin();mit != mm.end();++mit)
//    {
//        MeshModel* mesh = getMesh(*mit);
//        if (mesh != NULL)
//            mesh->bor.update(mesh->cm,meshupdatemask);
//    }
//    if ((mm.size() > 0) && (meshupdatemask != MeshModel::MM_NONE))
//        emit documentUpdated();
//    currTime.start();
//}

//void MeshDocument::updateRenderStateRasters(const QList<int>& rm,const int rasterupdatemask)
//{
//    static QTime currTime = QTime::currentTime();
//    if(currTime.elapsed()< 100)
//        return;
//    for (QList<int>::const_iterator rit = rm.begin();rit != rm.end();++rit)
//    {
//        RasterModel* raster = getRaster(*rit);
//
//        /**********READD*****/
//        /*  if (raster != NULL)
//        renderState().update(raster->id(),*raster,rasterupdatemask);*/
//        /********************/
//    }
//    if ((rm.size() > 0) && (rasterupdatemask != RasterModel::RM_NONE))
//        emit documentUpdated();
//    currTime.start();
//}
//
//void MeshDocument::updateRenderState(const QList<int>& mm,const int meshupdatemask,const QList<int>& rm,const int rasterupdatemask)
//{
//    static QTime currTime = QTime::currentTime();
//    if(currTime.elapsed()< 100)
//        return;
//  /*  for (QList<int>::const_iterator mit = mm.begin();mit != mm.end();++mit)
//    {
//        MeshModel* mesh = getMesh(*mit);
//        if (mesh != NULL)
//            renderState().update(mesh->id(),mesh->cm,meshupdatemask);
//    }
//    for (QList<int>::const_iterator rit = rm.begin();rit != rm.end();++rit)
//    {
//        RasterModel* raster = getRaster(*rit);
//        if (raster != NULL)
//            renderState().update(raster->id(),*raster,rasterupdatemask);
//    }*/
//    if (((mm.size() > 0) && (meshupdatemask != MeshModel::MM_NONE)) || (rm.size() > 0 && (rasterupdatemask != RasterModel::RM_NONE)))
//        emit documentUpdated();
//    currTime.start();
//}

MeshDocument::MeshDocument() : QObject(),Log(),xmlhistory()
{
    meshIdCounter=0;
    rasterIdCounter=0;
    currentMesh = 0;
    currentRaster = 0;
    busy=false;
    filterHistory = new FilterScript();
}


void MeshModel::Clear()
{
    meshModified() = false;
    // These data are always active on the mesh
    currentDataMask = MM_NONE;
    currentDataMask |= MM_VERTCOORD | MM_VERTNORMAL | MM_VERTFLAG ;
    currentDataMask |= MM_FACEVERT  | MM_FACENORMAL | MM_FACEFLAG ;

    visible=true;
    cm.Tr.SetIdentity();
    cm.sfn=0;
    cm.svn=0;
}

void MeshModel::UpdateBoxAndNormals()
{
    tri::UpdateBounding<CMeshO>::Box(cm);
    if(cm.fn>0) {
        tri::UpdateNormal<CMeshO>::PerFaceNormalized(cm);
        tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(cm);
    }
}

MeshModel::MeshModel(MeshDocument *_parent, QString fullFileName, QString labelName)
{
    /*glw.m = &(cm);*/
    Clear();
    parent=_parent;
    _id=parent->newMeshId();
    if(!fullFileName.isEmpty())   this->fullPathFileName=fullFileName;
    if(!labelName.isEmpty())     this->_label=labelName;
}

MeshModel::MeshModel(MeshModel* cp)
{
	if (cp == NULL)
		return;
	parent = cp->parent;
	if (parent != NULL)
		_id = parent->newMeshId();
	cm.Tr = cp->cm.Tr;
	cm.sfn = cp->cm.sfn;
	cm.svn = cp->cm.svn;
	visible = cp->visible;
	updateDataMask(cp->currentDataMask);
	vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(cm, cp->cm);
}

QString MeshModel::relativePathName() const
{
    QDir documentDir (documentPathName());
    QString relPath=documentDir.relativeFilePath(this->fullPathFileName);

    if(relPath.size()>1 && relPath[0]=='.' &&  relPath[1]=='.')
        qDebug("Error we have a mesh that is not in the same folder of the project: %s ", qUtf8Printable(relPath));

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

Plane::Plane(const Plane& pl)
{
    semantic = pl.semantic;
    fullPathFileName = pl.fullPathFileName;
    image = QImage(pl.image);
}

Plane::Plane(const QString pathName, const int _semantic)
{
    semantic =_semantic;
    fullPathFileName = pathName;

    image = QImage(pathName);
}

RasterModel::RasterModel(MeshDocument *parent, QString _rasterName)
    : MeshLabRenderRaster()
{
    _id=parent->newRasterId();
    par = parent;
    this->_label= _rasterName;
    visible=true;
}

RasterModel::RasterModel()
    : MeshLabRenderRaster()
{

}


MeshLabRenderRaster::MeshLabRenderRaster()
{

}

MeshLabRenderRaster::MeshLabRenderRaster( const MeshLabRenderRaster& rm )
    :shot(rm.shot),planeList()
{
    for(QList<Plane*>::const_iterator it = rm.planeList.begin();it != rm.planeList.end();++it)
    {
        planeList.push_back(new Plane(**it));
        if (rm.currentPlane == *it)
            currentPlane = planeList[planeList.size() - 1];
    }
}

void MeshLabRenderRaster::addPlane(Plane *plane)
{
    planeList.append(plane);
    currentPlane = plane;
}

MeshLabRenderRaster::~MeshLabRenderRaster()
{
    currentPlane = NULL;
    for(int ii = 0;ii < planeList.size();++ii)
        delete planeList[ii];
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
        std::vector<Point3m>::iterator ci;
        CMeshO::VertexIterator vi;
        for(vi = m->cm.vert.begin(), ci = vertCoord.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
            if(!(*vi).IsD()) (*ci)=(*vi).P();
    }

    if(changeMask & MeshModel::MM_VERTNORMAL)
    {
        vertNormal.resize(m->cm.vert.size());
        std::vector<Point3m>::iterator ci;
        CMeshO::VertexIterator vi;
        for(vi = m->cm.vert.begin(), ci = vertNormal.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
            if(!(*vi).IsD()) (*ci)=(*vi).N();
    }

    if(changeMask & MeshModel::MM_FACENORMAL)
    {
        faceNormal.resize(m->cm.face.size());
        std::vector<Point3m>::iterator ci;
        CMeshO::FaceIterator fi;
        for(fi = m->cm.face.begin(), ci = faceNormal.begin(); fi != m->cm.face.end(); ++fi, ++ci)
            if(!(*fi).IsD()) (*ci) = (*fi).N();
    }

    if(changeMask & MeshModel::MM_FACECOLOR)
    {
        m->updateDataMask(MeshModel::MM_FACECOLOR);
        faceColor.resize(m->cm.face.size());
        std::vector<Color4b>::iterator ci;
        CMeshO::FaceIterator fi;
        for(fi = m->cm.face.begin(), ci = faceColor.begin(); fi != m->cm.face.end(); ++fi, ++ci)
            if(!(*fi).IsD()) (*ci) = (*fi).C();
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
    if(_m != m)
        return false;
    if(changeMask & MeshModel::MM_VERTCOLOR)
    {
        if(vertColor.size() != m->cm.vert.size()) return false;
        std::vector<Color4b>::iterator ci;
        CMeshO::VertexIterator vi;
        for(vi = m->cm.vert.begin(), ci = vertColor.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
            if(!(*vi).IsD()) (*vi).C()=(*ci);
    }
    if(changeMask & MeshModel::MM_FACECOLOR)
    {
        if(faceColor.size() != m->cm.face.size()) return false;
        std::vector<Color4b>::iterator ci;
        CMeshO::FaceIterator fi;
        for(fi = m->cm.face.begin(), ci = faceColor.begin(); fi != m->cm.face.end(); ++fi, ++ci)
            if(!(*fi).IsD()) (*fi).C()=(*ci);
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
        std::vector<Point3m>::iterator ci;
        CMeshO::VertexIterator vi;
        for(vi = m->cm.vert.begin(), ci = vertCoord.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
            if(!(*vi).IsD()) (*vi).P()=(*ci);
    }

    if(changeMask & MeshModel::MM_VERTNORMAL)
    {
        if(vertNormal.size() != m->cm.vert.size()) return false;
        std::vector<Point3m>::iterator ci;
        CMeshO::VertexIterator vi;
        for(vi = m->cm.vert.begin(), ci=vertNormal.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
            if(!(*vi).IsD()) (*vi).N()=(*ci);
    }

    if(changeMask & MeshModel::MM_FACENORMAL)
    {
        if(faceNormal.size() != m->cm.face.size()) return false;
        std::vector<Point3m>::iterator ci;
        CMeshO::FaceIterator fi;
        for(fi = m->cm.face.begin(), ci=faceNormal.begin(); fi != m->cm.face.end(); ++fi, ++ci)
            if(!(*fi).IsD()) (*fi).N()=(*ci);
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

void MeshDocument::setVisible(int meshId, bool val)
{
    getMesh(meshId)->visible=val;
    emit meshSetChanged();
}

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
    if((neededDataMask & MM_FACEFACETOPO)!=0)
    {
        cm.face.EnableFFAdjacency();
        tri::UpdateTopology<CMeshO>::FaceFace(cm);
    }
    if((neededDataMask & MM_VERTFACETOPO)!=0)
    {
        cm.vert.EnableVFAdjacency();
        cm.face.EnableVFAdjacency();
        tri::UpdateTopology<CMeshO>::VertexFace(cm);
    }

    if((neededDataMask & MM_WEDGTEXCOORD)!=0)  
        cm.face.EnableWedgeTexCoord();
    if((neededDataMask & MM_FACECOLOR)!=0)     
        cm.face.EnableColor();
    if((neededDataMask & MM_FACEQUALITY)!=0)
        cm.face.EnableQuality();
    if((neededDataMask & MM_FACECURVDIR)!=0)   
        cm.face.EnableCurvatureDir();
    if((neededDataMask & MM_FACEMARK)!=0)	     
        cm.face.EnableMark();
    if((neededDataMask & MM_VERTMARK)!=0)      
        cm.vert.EnableMark();
    if((neededDataMask & MM_VERTCURV)!=0)      
        cm.vert.EnableCurvature();
    if((neededDataMask & MM_VERTCURVDIR)!=0)   
        cm.vert.EnableCurvatureDir();
    if((neededDataMask & MM_VERTRADIUS)!=0)    
        cm.vert.EnableRadius();
    if((neededDataMask & MM_VERTTEXCOORD)!=0)  
        cm.vert.EnableTexCoord();

    currentDataMask |= neededDataMask;
}

void MeshModel::clearDataMask(int unneededDataMask)
{
    if( ( (unneededDataMask & MM_VERTFACETOPO)!=0)	&& hasDataMask(MM_VERTFACETOPO)) {cm.face.DisableVFAdjacency();
    cm.vert.DisableVFAdjacency(); }
    if( ( (unneededDataMask & MM_FACEFACETOPO)!=0)	&& hasDataMask(MM_FACEFACETOPO))	cm.face.DisableFFAdjacency();

    if( ( (unneededDataMask & MM_WEDGTEXCOORD)!=0)	&& hasDataMask(MM_WEDGTEXCOORD)) 	cm.face.DisableWedgeTexCoord();
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

int MeshModel::dataMask() const
{
    return currentDataMask;
}

MeshDocumentStateData::MeshDocumentStateData()
	:_lock(QReadWriteLock::Recursive),_existingmeshesbeforeoperation()
{

}

MeshDocumentStateData::~MeshDocumentStateData()
{
	QWriteLocker locker(&_lock);
	_existingmeshesbeforeoperation.clear();
}

void MeshDocumentStateData::create(MeshDocument& md)
{
	QWriteLocker locker(&_lock);
	for (int ii = 0; ii < md.meshList.size(); ++ii)
	{
		MeshModel* mm = md.meshList[ii];
		if (mm != NULL)
			insert(mm->id(), MeshModelStateData(mm->dataMask(), mm->cm.VN(), mm->cm.FN(), mm->cm.EN()));
	}
}

QMap<int, MeshModelStateData>::iterator MeshDocumentStateData::insert(const int key, const MeshModelStateData & value)
{
	QWriteLocker locker(&_lock);
	return _existingmeshesbeforeoperation.insert(key,value);
}

QMap<int, MeshModelStateData>::iterator MeshDocumentStateData::find(const int key)
{
	QReadLocker locker(&_lock);
	return _existingmeshesbeforeoperation.find(key);
}

QMap<int, MeshModelStateData>::iterator MeshDocumentStateData::begin()
{
	QReadLocker locker(&_lock);
	return _existingmeshesbeforeoperation.begin();
}

QMap<int, MeshModelStateData>::iterator MeshDocumentStateData::end()
{
	QReadLocker locker(&_lock);
	return _existingmeshesbeforeoperation.end();
}

void MeshDocumentStateData::clear()
{
	QWriteLocker locker(&_lock);
	_existingmeshesbeforeoperation.clear();
}
