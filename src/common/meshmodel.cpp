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
#include "mlexception.h"

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

/*
When you create a new mesh it can be either a newly created one or an opened one.
If it is an opened one the fullpathname is meaningful and the label, by default is just the short name.
If it is a newly created one the fullpath is an empty string and the user has to provide a label.
*/

MeshModel * MeshDocument::addOrGetMesh(QString fullPath, QString label, bool setAsCurrent,const RenderMode& rm)
{
    MeshModel*newMM = this->getMesh(label);
    if(newMM==0)  newMM=this->addNewMesh(fullPath,label,setAsCurrent,rm);
    return newMM;
}

MeshModel * MeshDocument::addNewMesh(QString fullPath, QString label, bool setAsCurrent,const RenderMode& rm)
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
    qRegisterMetaType<RenderMode>("RenderMode");
    emit meshAdded(newMesh->id(),rm);
    if(setAsCurrent)
        this->setCurrentMesh(newMesh->id());

    return newMesh;
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

void MeshModel::UpdateBoxAndNormals()
{
    tri::UpdateBounding<CMeshO>::Box(cm);
    if(cm.fn>0) {
        tri::UpdateNormal<CMeshO>::PerFaceNormalized(cm);
        tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(cm);
    }
}

MeshModel::MeshModel(MeshDocument *_parent, QString fullFileName, QString labelName)
    :bor(),glw()
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

    if((neededDataMask & MM_WEDGTEXCOORD)!=0)  cm.face.EnableWedgeTexCoord();
    if((neededDataMask & MM_FACECOLOR)!=0)     cm.face.EnableColor();
    if((neededDataMask & MM_FACEQUALITY)!=0)   cm.face.EnableQuality();
    if((neededDataMask & MM_FACECURVDIR)!=0)   cm.face.EnableCurvatureDir();
    if((neededDataMask & MM_FACEMARK)!=0)	     cm.face.EnableMark();
    if((neededDataMask & MM_VERTMARK)!=0)      cm.vert.EnableMark();
    if((neededDataMask & MM_VERTCURV)!=0)      cm.vert.EnableCurvature();
    if((neededDataMask & MM_VERTCURVDIR)!=0)   cm.vert.EnableCurvatureDir();
    if((neededDataMask & MM_VERTRADIUS)!=0)    cm.vert.EnableRadius();
    if((neededDataMask & MM_VERTTEXCOORD)!=0)  cm.vert.EnableTexCoord();

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

BufferObjectsRendering::BufferObjectsRendering()
    :vcg::GLW(),_lock(QReadWriteLock::Recursive)
{
    this->HighPrecisionMode =false;
}



BufferObjectsRendering::~BufferObjectsRendering()
{
    clearState();
}

void BufferObjectsRendering::DrawEdges(vcg::GLW::ColorMode colm, vcg::GLW::NormalMode nolm)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBO);
    glVertexPointer(3, GL_FLOAT, 0, 0);               // last param is offset, not ptr
    glEnableClientState(GL_VERTEX_ARRAY);             // activate vertex coords array

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexEdgeBufferObject);
    glDrawElements( GL_LINES, en*2, GL_UNSIGNED_INT,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
}


void BufferObjectsRendering::DrawPoints(vcg::GLW::ColorMode colm, vcg::GLW::NormalMode nolm, TextureMode tm)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBO);
    glVertexPointer(3, GL_FLOAT, 0, 0);               // last param is offset, not ptr
    glEnableClientState(GL_VERTEX_ARRAY);             // activate vertex coords array

    if(nolm==GLW::NMPerVert) {
        glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBO);
        glNormalPointer(GL_FLOAT, 0, 0);               // last param is offset, not ptr
        glEnableClientState(GL_NORMAL_ARRAY);             // activate vertex coords array
    }

    if(colm == GLW::CMPerVert){
        glBindBuffer(GL_ARRAY_BUFFER, vertexColorBO);
        glColorPointer(4,GL_UNSIGNED_BYTE, 0, 0);               // last param is offset, not ptr
        glEnableClientState(GL_COLOR_ARRAY);             // activate vertex coords array
    }

    glDrawArrays(GL_POINTS, 0, vn);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

/// Two main path of rendering:
/// Plain vertex attribute (normal, color, and texture must be absent or per vertex)
/// Duplicated Vertex rendering (all the other cases)

void BufferObjectsRendering::DrawTriangles(vcg::GLW::ColorMode cm, vcg::GLW::NormalMode nm, TextureMode tm)
{
    if(nm!=GLW::NMPerFace && cm != GLW::CMPerFace && tm != GLW::TMPerWedge)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBO);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        if(nm == GLW::NMPerVert){
            glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBO);
            glNormalPointer(GL_FLOAT, 0, 0);
            glEnableClientState(GL_NORMAL_ARRAY);
        }

        if(cm == GLW::CMPerVert){
            glBindBuffer(GL_ARRAY_BUFFER, vertexColorBO);
            glColorPointer(4,GL_UNSIGNED_BYTE, 0, 0);
            glEnableClientState(GL_COLOR_ARRAY);
        }

        if(tm == GLW::TMPerVert){
            glBindBuffer(GL_ARRAY_BUFFER, vertexTextureBO);
            glTexCoordPointer(2,GL_FLOAT, 0, 0);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        for(size_t i=0;i<indexTriBufferObject.size();++i)
        {
            if(tm==GLW::TMPerVert)
            {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D,TMId[i]);
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexTriBufferObject[i]);
            glDrawElements( GL_TRIANGLES, indexTriBufferObjectSz[i], GL_UNSIGNED_INT,0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glDisable(GL_TEXTURE_2D);
        }
    }

    else /// Duplicated Vertex Pipeline
    {   
        glBindBuffer(GL_ARRAY_BUFFER, positionDupBufferObject);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        if(nm == GLW::NMPerVert){
            glBindBuffer(GL_ARRAY_BUFFER, normalDupBufferObject);
            glNormalPointer(GL_FLOAT, 0, 0);
            glEnableClientState(GL_NORMAL_ARRAY);
        }
        if(nm == GLW::NMPerFace){
            glBindBuffer(GL_ARRAY_BUFFER, normalDupBufferObject);
            //glBindBuffer(GL_ARRAY_BUFFER, normalFaceBufferObject);
            glNormalPointer(GL_FLOAT, 0, 0);
            glEnableClientState(GL_NORMAL_ARRAY);
        }

        if(cm == GLW::CMPerVert){
            glBindBuffer(GL_ARRAY_BUFFER, colorDupBufferObject);
            glColorPointer(4,GL_UNSIGNED_BYTE, 0, 0);
            glEnableClientState(GL_COLOR_ARRAY);
        }
        if(cm == GLW::CMPerFace){
            glBindBuffer(GL_ARRAY_BUFFER, colorFaceBufferObject);
            glColorPointer(4,GL_UNSIGNED_BYTE, 0, 0);
            glEnableClientState(GL_COLOR_ARRAY);
        }

        if (tm != GLW::TMPerWedgeMulti)
            glDrawArrays(GL_TRIANGLES, 0, tn*3);
        else
        {
            glEnable(GL_TEXTURE_2D);
            
            glBindBuffer(GL_ARRAY_BUFFER, wedgeTextBufferObject);
            glTexCoordPointer(2,GL_FLOAT, 0, 0);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);

            int i = 0;
            
            int firsttriangleoffset = 0;
            for(std::map< int,std::vector<size_t> >::const_iterator it = chunkMap.cbegin();it != chunkMap.cend();++it)
            {
                
                if (it->first != -1)
                    glBindTexture(GL_TEXTURE_2D,TMId[it->first]);
                else
                    glBindTexture(GL_TEXTURE_2D,0);

                glDrawArrays(GL_TRIANGLES,firsttriangleoffset,it->second.size() * 3);
                firsttriangleoffset += it->second.size() * 3; 
                ++i;
            }
            glDisable(GL_TEXTURE_2D);
        }
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BufferObjectsRendering::render(const Box3m &bbDoc, vcg::GLW::DrawMode dm,vcg::GLW::ColorMode colm,vcg::GLW::TextureMode tm )
{
    QReadLocker locker(&_lock);
    glPushMatrix();
    if(this->HighPrecisionMode)
    {

    }
    else
    {
        glTranslate(-bbDoc.Center());
        glMultMatrix(Tr);
    }

    if(dm== GLW::DMPoints) DrawPoints(colm,GLW::NMPerVert,tm);
    else
        if(dm==GLW::DMWire) DrawEdges(GLW::CMNone,GLW::NMNone);
        else
        {
            if( dm==GLW::DMSmooth)
                DrawTriangles(colm,GLW::NMPerVert,tm);
            else
                DrawTriangles(colm,GLW::NMPerFace,tm);
        }
        glPopMatrix();
}

bool BufferObjectsRendering::update(CMeshO& mm, const int updateattributesmask)
{
    if (updateattributesmask == MeshModel::MM_NONE)
        return false;

    QWriteLocker locker(&_lock);
    tri::Allocator<CMeshO>::CompactEveryVector(mm);

    QTime aa; aa.start();
    std::vector<Point3f> pv(mm.vn);
    std::vector<Point3f> nv(mm.vn);
    std::vector<Color4b> cv(mm.vn); // Per vertex Colors

    vn = mm.vn;

    // In HighPrecisionMode each vertex is pretransformed in double in its final position.
    // but we save the coord of the centered bbox after the transformation.

    if(this->HighPrecisionMode)
    {
        Box3m localBB;
        localBB.Add(mm.Tr,mm.bbox);
        Point3m localBBC =localBB.Center();

        Tr.SetTranslate(localBBC);
        bbCenter.Import(localBBC);

        Matrix33m  mat33(mm.Tr,3);
        for(size_t i=0;i<vn;++i)
        {
            pv[i].Import(mm.Tr*mm.vert[i].P() - localBBC);
            nv[i].Import(mat33*mm.vert[i].N());
            nv[i].Normalize();
            cv[i]=mm.vert[i].C();
        }
    } else {
        qDebug("Low Precision buffers");
        Tr = mm.Tr;
        bbCenter = mm.bbox.Center();
        for(size_t i=0;i<vn;++i)
        {
            pv[i].Import(mm.vert[i].P());
            nv[i].Import(mm.vert[i].N());
            nv[i].Normalize();
            cv[i]=mm.vert[i].C();
        }
    }

    tn = mm.fn;
    std::vector < std::vector<unsigned int> > ti;
    // Build the vector with all the unique edge pairs
    std::vector<std::pair<unsigned int, unsigned int> > ev;

    std::vector<Point3f> rpv(mm.fn*3);
    std::vector<Point3f> rnv(mm.fn*3);
    std::vector<Color4b> rcv(mm.fn*3);
    std::vector<Point3f> rnt(mm.fn*3);
    std::vector<Color4b> rct(mm.fn*3);

    if (tri::HasPerWedgeTexCoord(mm))
    {
        //std::vector<vcg::TexCoord2f> wtv(mm.fn*3);
        std::vector<float> wtv(mm.fn*3*2);
        chunkMap.clear();
        //AggregatedTriangleChunkMap
        for(size_t i=0;i<tn;++i) // replicated coords
        {
            int texId = mm.face[i].WT(0).N();
            chunkMap[texId].push_back(i);
        }
        
        int k = 0;
        int t = 0;
        for(std::map< int,std::vector<size_t> >::iterator it = chunkMap.begin();it != chunkMap.end();++it)
        {
            for(size_t j = 0;j<it->second.size();++j)
            {
                size_t indf(it->second[j]);

                rpv[k*3+0].Import(mm.face[indf].V(0)->P());
                rpv[k*3+1].Import(mm.face[indf].V(1)->P());
                rpv[k*3+2].Import(mm.face[indf].V(2)->P());

                rnv[k*3+0].Import(mm.face[indf].V(0)->N().Normalize());
                rnv[k*3+1].Import(mm.face[indf].V(1)->N().Normalize());
                rnv[k*3+2].Import(mm.face[indf].V(2)->N().Normalize());

                rcv[k*3+0] = mm.face[indf].V(0)->C();
                rcv[k*3+1] = mm.face[indf].V(1)->C();
                rcv[k*3+2] = mm.face[indf].V(2)->C();

                wtv[k*6+0]=mm.face[indf].WT(0).U();
                wtv[k*6+1]=mm.face[indf].WT(0).V();
                wtv[k*6+2]=mm.face[indf].WT(1).U();
                wtv[k*6+3]=mm.face[indf].WT(1).V();
                wtv[k*6+4]=mm.face[indf].WT(2).U();
                wtv[k*6+5]=mm.face[indf].WT(2).V();
                

                ++k;  
            }

            ++t;
        }


        if (k != tn)
            throw MeshLabException("Mesh has not been properly partitioned");

        glGenBuffers(1, &wedgeTextBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, wedgeTextBufferObject);
        glBufferData(GL_ARRAY_BUFFER, wtv.size() * sizeof(GLfloat), &wtv[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else
    {
        if(tri::HasPerVertexTexCoord(mm))
        {
            std::vector<float> tv(mm.vn*2); // Per vertex Textures
            for(size_t i=0;i<vn;++i)
            {
                tv[i*2+0]= mm.vert[i].T().U();
                tv[i*2+1]= mm.vert[i].T().V();
            }
            glGenBuffers(1, &vertexTextureBO);
            glBindBuffer(GL_ARRAY_BUFFER, vertexTextureBO);
            glBufferData(GL_ARRAY_BUFFER, vn *2 *sizeof(GLfloat), &tv[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        if(mm.textures.size()>1 && tri::HasPerVertexTexCoord(mm))
        {
            ti.resize(mm.textures.size());
            for(size_t i=0;i<tn;++i)
            {
                int tid= mm.face[i].V(0)->T().n();
                ti[tid].push_back(tri::Index(mm,mm.face[i].V(0)));
                ti[tid].push_back(tri::Index(mm,mm.face[i].V(1)));
                ti[tid].push_back(tri::Index(mm,mm.face[i].V(2)));
            }
        }
        else
        {
            ti.resize(1);
            ti[0].resize(tn*3);
            for(size_t i=0;i<tn;++i)
            {
                ti[0][i*3+0] = tri::Index(mm,mm.face[i].V(0));
                ti[0][i*3+1] = tri::Index(mm,mm.face[i].V(1));
                ti[0][i*3+2] = tri::Index(mm,mm.face[i].V(2));
            }
        }

        bool perfcol = tri::HasPerFaceColor(mm);
        // Now doing the replicated stuff
        for(size_t i=0;i<tn;++i) // replicated coords
        {
            unsigned int i0 = tri::Index(mm,mm.face[i].V(0));
            unsigned int i1 = tri::Index(mm,mm.face[i].V(1));
            unsigned int i2 = tri::Index(mm,mm.face[i].V(2));
            if(i0<i1)  ev.push_back(std::make_pair(i0,i1));
            else       ev.push_back(std::make_pair(i1,i0));
            if(i1<i2)  ev.push_back(std::make_pair(i1,i2));
            else       ev.push_back(std::make_pair(i2,i1));
            if(i2<i0)  ev.push_back(std::make_pair(i2,i0));
            else       ev.push_back(std::make_pair(i0,i2));
            

            rpv[i*3+0] = pv[tri::Index(mm,mm.face[i].V(0))];
            rpv[i*3+1] = pv[tri::Index(mm,mm.face[i].V(1))];
            rpv[i*3+2] = pv[tri::Index(mm,mm.face[i].V(2))];


            rcv[i*3+0] = cv[tri::Index(mm,mm.face[i].V(0))];
            rcv[i*3+1] = cv[tri::Index(mm,mm.face[i].V(1))];
            rcv[i*3+2] = cv[tri::Index(mm,mm.face[i].V(2))];
        

            rnv[i*3+0] = nv[tri::Index(mm,mm.face[i].V(0))];
            rnv[i*3+1] = nv[tri::Index(mm,mm.face[i].V(1))];
            rnv[i*3+2] = nv[tri::Index(mm,mm.face[i].V(2))];
               
            if (perfcol)
            {
                rct[i*3+0]=mm.face[i].C();
                rct[i*3+1]=mm.face[i].C();
                rct[i*3+2]=mm.face[i].C();
            }
        
            rnt[i*3+0].Import(mm.face[i].N().Normalize());
            rnt[i*3+1].Import(mm.face[i].N().Normalize());
            rnt[i*3+2].Import(mm.face[i].N().Normalize());
        }

        std::sort(ev.begin(),ev.end());
        std::vector<std::pair<unsigned int, unsigned int> >::iterator newEnd =
            std::unique(ev.begin(), ev.end());
        ev.resize(newEnd-ev.begin());
        en=ev.size();
    }
    qDebug("Buffer prepared in %i",aa.elapsed());
    glGenBuffers(1, &vertexPositionBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBO);
    glBufferData(GL_ARRAY_BUFFER, vn *3 *sizeof(GLfloat), &pv[0], GL_STATIC_DRAW);

    glGenBuffers(1, &positionDupBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, positionDupBufferObject);
    glBufferData(GL_ARRAY_BUFFER, tn * 9 *sizeof(GLfloat), &rpv[0], GL_STATIC_DRAW);

    glGenBuffers(1, &vertexNormalBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBO);
    glBufferData(GL_ARRAY_BUFFER, vn *3 *sizeof(GLfloat), &nv[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &normalDupBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, normalDupBufferObject);
    glBufferData(GL_ARRAY_BUFFER, tn * 9 *sizeof(GLfloat), &rnv[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &normalFaceBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, normalFaceBufferObject);
    glBufferData(GL_ARRAY_BUFFER, tn * 9 *sizeof(GLfloat), &rnt[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vertexColorBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexColorBO);
    glBufferData(GL_ARRAY_BUFFER, vn*4*sizeof(GLbyte), &cv[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &colorDupBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, colorDupBufferObject);
    glBufferData(GL_ARRAY_BUFFER, tn*3*4*sizeof(GLbyte), &rcv[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &colorFaceBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, colorFaceBufferObject);
    glBufferData(GL_ARRAY_BUFFER, tn*3*4*sizeof(GLbyte), &rct[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);



    /*glGenBuffers(1, &indexEdgeBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexEdgeBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, en *2 *sizeof(GLuint), &ev[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/


    indexTriBufferObject.resize(ti.size());
    indexTriBufferObjectSz.resize(ti.size());
    for(size_t i=0;i<ti.size();++i)
    {
        indexTriBufferObjectSz[i]=ti[i].size();
        glGenBuffers(1, &indexTriBufferObject[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexTriBufferObject[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ti[i].size() *sizeof(GLuint), &ti[i][0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    qDebug("Buffer feed in %i",aa.elapsed());
    return true;
}

void BufferObjectsRendering::clearState()
{
    QWriteLocker locker(&_lock);

    //TODO: delete frame objects
}

GLWRendering::GLWRendering()
    :vcg::GlTrimesh<CMeshO>()
{
    m = NULL;
}

GLWRendering::GLWRendering( CMeshO& mm )
    :vcg::GlTrimesh<CMeshO>()
{
    m = &mm;
}


GLWRendering::~GLWRendering()
{
    m = NULL;
}

void GLWRendering::render(vcg::GLW::DrawMode dm,vcg::GLW::ColorMode colm,vcg::GLW::TextureMode tm )
{
    if (m != NULL)
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glPushMatrix();
        glMultMatrix(m->Tr);
        if( (colm == vcg::GLW::CMPerFace)  && (!vcg::tri::HasPerFaceColor(*m)) )
            colm=vcg::GLW::CMNone;
        if( (tm == vcg::GLW::TMPerWedge )&& (!vcg::tri::HasPerWedgeTexCoord(*m)) )
            tm=vcg::GLW::TMNone;
        if( (tm == vcg::GLW::TMPerWedgeMulti )&& (!vcg::tri::HasPerWedgeTexCoord(*m)))
            tm=vcg::GLW::TMNone;
        if( (tm == vcg::GLW::TMPerVert )&& (!vcg::tri::HasPerVertexTexCoord(*m)))
            tm=vcg::GLW::TMNone;
        Draw(dm,colm,tm);
        glPopMatrix();
        glPopAttrib();
        return;
    }
}
