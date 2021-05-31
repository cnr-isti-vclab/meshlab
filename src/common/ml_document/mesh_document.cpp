/****************************************************************************
* MeshLab                                                           o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2021                                           \/)\/    *
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
QString nameDisambiguator(std::list<LayerElement*> &elemList, QString meshLabel)
{
	QString newName=std::move(meshLabel);
	typename std::list<LayerElement*>::iterator mmi;

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
				numDisamb = baseName.midRef((startDisamb+1),(endDisamb-startDisamb-1)).toInt(&ok);
			else
				numDisamb = 0;

			if(startDisamb!=-1)
				newName = baseName.left(startDisamb)+ "(" + QString::number(numDisamb+1) + ")";
			else
				newName = baseName + "(" + QString::number(numDisamb+1) + ")";

			if (suffix != QString(""))
				newName = newName + "." + suffix;

			// now recurse to see if the new name is free
			newName = nameDisambiguator(elemList, newName);
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
	for(MeshModel *mmp : meshList)
		delete mmp;
	for(RasterModel* rmp : rasterList)
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

const MeshModel* MeshDocument::getMesh(unsigned int id) const
{
	for (const MeshModel* m : meshList)
		if (m->id() == id)
			return m;
	return nullptr;
}

//returns the mesh ata given position in the list
MeshModel* MeshDocument::getMesh(unsigned int id)
{
	for (MeshModel* m : meshList)
		if (m->id() == id)
			return m;
	return nullptr;
}

void MeshDocument::setCurrentMesh(int new_curr_id)
{
	if(new_curr_id<0) {
		currentMesh=0;
		return;
	}
	currentMesh = getMesh(new_curr_id);
	emit currentMeshChanged(new_curr_id);
	assert(currentMesh);
}

void MeshDocument::setVisible(int meshId, bool val)
{
	getMesh(meshId)->visible=val;
	emit meshSetChanged();
}

//returns the raster at a given position in the list
RasterModel *MeshDocument::getRaster(int i)
{
	for(RasterModel *rmp : rasterList) {
		if(rmp->id() == i) return rmp;
	}
	return 0;
}

//if i is <0 it means that no currentRaster is set
void MeshDocument::setCurrentRaster(int new_curr_id)
{
	if(new_curr_id<0) {
		currentRaster=0;
		return;
	}

	for(RasterModel *rmp : rasterList) {
		if(rmp->id() == new_curr_id) {
			currentRaster = rmp;
			return;
		}
	}
	assert(0);
}

void MeshDocument::setCurrent(MeshModel* newCur)
{
	setCurrentMesh(newCur->id());
}

void MeshDocument::setCurrent(RasterModel* newCur)
{
	setCurrentRaster(newCur->id());
}

MeshModel* MeshDocument::nextVisibleMesh(MeshModel* _m)
{
	MeshModel *newM = nextMesh(_m);
	if(newM==0)
		return newM;

	if(newM->isVisible())
		return newM;
	else
		return nextVisibleMesh(newM);
}

MeshModel* MeshDocument::nextMesh(MeshModel* _m)
{
	if(_m==0 && meshList.size()>0)
		return meshList.front();
	for (auto it = meshList.begin(); it != meshList.end(); ++it) {
		if (*it == _m) {
			auto next = it;
			next++;
			if(next != meshList.end())
				return *next;
		}
	}
	return nullptr;
}

RasterModel* MeshDocument::nextRaster(RasterModel* _rm)
{
	if(_rm==0 && rasterList.size()>0)
		return rasterList.front();
	for (auto it = rasterList.begin(); it != rasterList.end(); ++it) {
		if (*it == _rm) {
			auto next = it;
			next++;
			if(next != rasterList.end())
				return *next;
		}
	}
	return nullptr;
}

MeshModel* MeshDocument::mm()
{
	return currentMesh;
}

const MeshModel* MeshDocument::mm() const
{
	return currentMesh;
}

RasterModel* MeshDocument::rm()
{
	return currentRaster;
}

const RasterModel* MeshDocument::rm() const
{
	return currentRaster;
}

void MeshDocument::requestUpdatingPerMeshDecorators(int mesh_id)
{
	emit updateDecorators(mesh_id);
}

MeshDocumentStateData& MeshDocument::meshDocStateData()
{
	return mdstate;
}

void MeshDocument::setDocLabel(const QString& docLb)
{
	documentLabel = docLb;
}

QString MeshDocument::docLabel() const
{
	return documentLabel;
}

QString MeshDocument::pathName() const
{
	QFileInfo fi(fullPathFilename);
	return fi.absolutePath();
}

void MeshDocument::setFileName(const QString& newFileName)
{
	fullPathFilename = newFileName;
}

unsigned int MeshDocument::meshNumber() const
{
	return meshList.size();
}

unsigned int MeshDocument::rasterNumber() const
{
	return rasterList.size();
}

bool MeshDocument::isBusy()
{
	return busy;
}

void MeshDocument::setBusy(bool _busy)
{
	busy=_busy;
}

/**
 * @brief Adds a new mesh to the MeshDocument. The added mesh is a COPY of the mesh
 * passed as parameter.
 */
MeshModel* MeshDocument::addNewMesh(
		const CMeshO& mesh,
		const QString& label,
		bool setAsCurrent)
{
	MeshModel* m = addNewMesh("", label, setAsCurrent);
	m->cm = mesh;
	m->updateBoxAndNormals();
	m->updateDataMask();
	return m;
}

MeshModel * MeshDocument::addNewMesh(
		QString fullPath,
		const QString& label,
		bool setAsCurrent)
{
	QString newlabel = nameDisambiguator(this->meshList, label);

	if(!fullPath.isEmpty())
	{
		QFileInfo fi(fullPath);
		fullPath = fi.absoluteFilePath();
	}

	MeshModel *newMesh = new MeshModel(newMeshId(), fullPath,newlabel);
	meshList.push_back(newMesh);

	if(setAsCurrent)
		this->setCurrentMesh(newMesh->id());

	emit meshSetChanged();
	emit meshAdded(newMesh->id());
	return newMesh;
}

MeshModel * MeshDocument::addOrGetMesh(
		const QString& fullPath,
		const QString& label,
		bool setAsCurrent)
{
	MeshModel *newMesh = nullptr;
	for (MeshModel* m : meshList)
		if (m->shortName() == label)
			newMesh = m;
	if(newMesh) {
		if(setAsCurrent)
			this->setCurrentMesh(newMesh->id());
		return newMesh;
	}
	return addNewMesh(fullPath,label,setAsCurrent);
}

/**
 * Returns the ordered list of the meshes loaded from the same
 * file in which has been loaded the model mm.
 */
std::list<MeshModel*> MeshDocument::getMeshesLoadedFromSameFile(MeshModel* mm)
{
	std::list<MeshModel*> ml;
	if (mm->idInFile() == -1){
		ml.push_back(mm);
	}
	else {
		if (!mm->fullName().isEmpty()){
			for (MeshModel* m : meshList){
				if (m->fullName() == mm->fullName())
					ml.push_back(m);
			}
		}
	}
	return ml;
}

bool MeshDocument::delMesh(MeshModel *mmToDel)
{
	auto pos = std::find(meshList.begin(), meshList.end(), mmToDel);
	if (pos == meshList.end())
		return false;
	meshList.erase(pos);

	if((currentMesh == mmToDel) && (!meshList.empty()))
		setCurrentMesh(this->meshList.front()->id());
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

	RasterModel *newRaster=new RasterModel(this, newRasterId(), newLabel);
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
	auto pos = std::find(rasterList.begin(), rasterList.end(), rasterToDel);
	if (pos == rasterList.end())
		return false;
	rasterList.erase(pos);

	if((currentRaster == rasterToDel) && (!rasterList.empty()))
		setCurrentRaster(this->rasterList.front()->id());
	else if (rasterList.empty())
		setCurrentRaster(-1);

	delete rasterToDel;
	emit rasterSetChanged();

	return true;
}

int MeshDocument::vn()
{
	int tot=0;
	for(MeshModel* mmp : meshList)
		tot+= mmp->cm.vn;
	return tot;
}

int MeshDocument::fn()
{
	int tot=0;
	for(MeshModel *mmp : meshList)
		tot+= mmp->cm.fn;
	return tot;
}

Box3m MeshDocument::bbox() const
{
	Box3m FullBBox;
	for(MeshModel * mp : meshList)
		FullBBox.Add(mp->cm.Tr,mp->cm.bbox);
	return FullBBox;
}

bool MeshDocument::hasBeenModified()
{
	for(MeshModel *m : meshList)
		if(m->meshModified()) return true;
	return false;
}

MeshDocument::MeshRangeIterator MeshDocument::meshIterator()
{
	return MeshRangeIterator(this);
}

MeshDocument::ConstMeshRangeIterator MeshDocument::meshIterator() const
{
	return ConstMeshRangeIterator(this);
}

MeshDocument::RasterRangeIterator MeshDocument::rasterIterator()
{
	return RasterRangeIterator(this);
}

MeshDocument::ConstRasterRangeIterator MeshDocument::rasterIterator() const
{
	return ConstRasterRangeIterator(this);
}

unsigned int MeshDocument::newMeshId()
{
	return meshIdCounter++;
}

unsigned int MeshDocument::newRasterId()
{
	return rasterIdCounter++;
}
