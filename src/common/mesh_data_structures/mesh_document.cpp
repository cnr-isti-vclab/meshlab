/****************************************************************************
* MeshLab                                                           o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2020                                           \/)\/    *
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

#include "mesh_document.h"

template <class LayerElement>
QString NameDisambiguator(QList<LayerElement*> &elemList, QString meshLabel )
{
    QString newName=std::move(meshLabel);
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

MeshDocument::MeshDocument() 
{
	meshIdCounter=0;
	rasterIdCounter=0;
	currentMesh = nullptr;
	currentRaster = nullptr;
	busy=false;
}

//deletes each meshModel
MeshDocument::~MeshDocument()
{
	foreach(MeshModel *mmp, meshList)
		delete mmp;
	foreach(RasterModel* rmp,rasterList)
		delete rmp;
}

void MeshDocument::clear()
{
	for(MeshModel *mmp : meshList)
		delete mmp;
	meshList.clear();
	
	for(RasterModel* rmp :rasterList)
		delete rmp;
	rasterList.clear();
	
	meshIdCounter=0;
	rasterIdCounter=0;
	currentMesh = nullptr;
	currentRaster = nullptr;
	busy=false;
	filterHistory.clear();
	fullPathFilename = "";
	documentLabel = "";
	meshDocStateData().clear();
}

const MeshModel* MeshDocument::getMesh(int id) const
{
	for (const MeshModel* m : meshList)
		if (m->id() == id)
			return m;
	return nullptr;
}

//returns the mesh ata given position in the list
MeshModel* MeshDocument::getMesh(int id)
{
	for (MeshModel* m : meshList)
		if (m->id() == id)
			return m;
	return nullptr;
}

const MeshModel* MeshDocument::getMesh(const QString& name) const
{
	for (const MeshModel* m : meshList)
		if (m->shortName() == name)
			return m;
	return nullptr;
}

MeshModel* MeshDocument::getMesh(const QString& name)
{
	for (MeshModel* m : meshList)
		if (m->shortName() == name)
			return m;
	return nullptr;
}

const MeshModel* MeshDocument::getMeshByFullName(const QString& pathName) const
{
	for (const MeshModel* m : meshList)
		if (m->fullName() == pathName)
			return m;
	return nullptr;
}

MeshModel* MeshDocument::getMeshByFullName(const QString& pathName)
{
	for (MeshModel* m : meshList)
		if (m->fullName() == pathName)
			return m;
	return nullptr;
}


void MeshDocument::setCurrentMesh(int new_curr_id)
{
    if(new_curr_id<0)
    {
        currentMesh=0;
        return;
    }
    currentMesh = getMesh(new_curr_id);
    emit currentMeshChanged(new_curr_id);
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
void MeshDocument::setCurrentRaster( int new_curr_id)
{
    if(new_curr_id<0)
    {
        currentRaster=0;
        return;
    }

    foreach(RasterModel *rmp, rasterList)
    {
        if(rmp->id() == new_curr_id)
        {
            currentRaster = rmp;
            return;
        }
    }
    assert(0);
}

void MeshDocument::requestUpdatingPerMeshDecorators(int mesh_id)
{	
	emit updateDecorators(mesh_id);
}

MeshModel * MeshDocument::addNewMesh(QString fullPath, QString label, bool setAsCurrent)
{
    QString newlabel = NameDisambiguator(this->meshList,std::move(label));

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

MeshModel * MeshDocument::addOrGetMesh(QString fullPath, const QString& label, bool setAsCurrent)
{
  MeshModel *newMesh = getMesh(label);
  if(newMesh) {
    if(setAsCurrent)
        this->setCurrentMesh(newMesh->id());
    return newMesh;
  }
  return addNewMesh(std::move(fullPath),label,setAsCurrent);
}

bool MeshDocument::delMesh(MeshModel *mmToDel)
{
    if(!meshList.removeOne(mmToDel))
        return false;
    if((currentMesh == mmToDel) && (!meshList.empty()))
        setCurrentMesh(this->meshList.at(0)->id());
    else if (meshList.empty())
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
        if (!rasterList.empty())
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

