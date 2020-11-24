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

#ifndef MESH_DOCUMENT_H
#define MESH_DOCUMENT_H

#include "mesh_model.h"

class MeshDocument : public QObject
{
	Q_OBJECT

public:

	MeshDocument();

	//deletes each meshModel
	~MeshDocument();
	
	void clear();

	///returns the mesh with the given unique id
	const MeshModel* getMesh(int id) const;
	MeshModel *getMesh(int id);
	const MeshModel *getMesh(const QString& name) const;
	MeshModel *getMesh(const QString& name);
	const MeshModel *getMeshByFullName(const QString& pathName) const;
	MeshModel *getMeshByFullName(const QString& pathName);

    //set the current mesh to be the one with the given ID
    void setCurrentMesh( int new_curr_id );

    void setVisible(int meshId, bool val);

    /// returns the raster with the given unique id
    RasterModel *getRaster(int i);

    //set the current raster to be the one with the given ID
    void setCurrentRaster( int new_curr_id );
    void setCurrent(MeshModel   *newCur)  { setCurrentMesh(newCur->id());}
    void setCurrent(RasterModel *newCur)  { setCurrentRaster(newCur->id());}

    /// methods to access the set of Meshes in a ordered fashion.
    MeshModel   *nextVisibleMesh(MeshModel *_m = NULL)
    {
      MeshModel *newM = nextMesh(_m);
      if(newM==0)
        return newM;

      if(newM->isVisible())
        return newM;
      else
        return nextVisibleMesh(newM);
    }

    MeshModel   *nextMesh(MeshModel *_m = NULL)
    {
      if(_m==0 && meshList.size()>0)
        return meshList.at(0);
      for (int i = 0; i < meshList.size(); ++i) {
          if (meshList.at(i) == _m)
          {
            if(i+1 < meshList.size())
              return meshList.at(i+1);
          }
      }
      return 0;
    }
    /// methods to access the set of Meshes in a ordered fashion.
    RasterModel   *nextRaster(RasterModel *_rm = NULL) {
      for (int i = 0; i < rasterList.size(); ++i) {
          if (rasterList.at(i) == _rm)
          {
            if(i+1 < rasterList.size())
              return rasterList.at(i+1);
          }
      }
      return 0;
    }

	MeshModel* mm() {
		return currentMesh;
	}

	const MeshModel* mm() const {
		return currentMesh;
	}

    //Could return 0 if no raster has been selected
    RasterModel *rm(){
        return currentRaster;
    }

    /// The very important member:
    /// The list of MeshModels.
    QList<MeshModel *> meshList;
    /// The list of the raster models of the project
    QList<RasterModel *> rasterList;
    int newMeshId() {return meshIdCounter++;}
    int newRasterId() {return rasterIdCounter++;}

    //functions to update the document entities (meshes and/or rasters) during the filters execution
    //WARNING! please note that if you have to update both meshes and rasters calling updateRenderState function it's far more efficient
    //than calling in sequence updateRenderRasterStateMeshes and updateRenderStateRasters. Use them only if you have to update only rasters or only meshes.
    /*void updateRenderState(const QList<int>& mm,const int meshupdatemask,const QList<int>& rm,const int rasterupdatemask);

    void updateRenderStateMeshes(const QList<int>& mm,const int meshupdatemask);
    void updateRenderStateRasters(const QList<int>& rm,const int rasterupdatemask);*/
	void requestUpdatingPerMeshDecorators(int mesh_id);

private:
    int meshIdCounter;
    int rasterIdCounter;

    /**
    All the files referred in a document are relative to the folder containing the project file.
    this is the full path to the document.
    */
    QString fullPathFilename;

    //it is the label of the document. it should only be something like Project_n (a temporary name for a new empty document) or the fullPathFilename.
    QString documentLabel;

	MeshDocumentStateData mdstate;
public:

	inline MeshDocumentStateData& meshDocStateData() { return mdstate; }
    void setDocLabel(const QString& docLb) {documentLabel = docLb;}
    QString docLabel() const {return documentLabel;}
    QString pathName() const {QFileInfo fi(fullPathFilename); return fi.absolutePath();}
    void setFileName(const QString& newFileName) {fullPathFilename = newFileName;}
    GLLogStream Log;
    FilterScript filterHistory;
    QStringList xmlhistory;

    int size() const {return meshList.size();}
    bool isBusy() { return busy;}    // used in processing. To disable access to the mesh by the rendering thread
    void setBusy(bool _busy)
    {
        /*if(busy && _busy==false)
        {
            emit meshDocumentModified();
        }*/
        busy=_busy;
    }

private:
    bool  busy;

public:
    ///add a new mesh with the given name
    MeshModel *addNewMesh(QString fullPath, QString Label, bool setAsCurrent=true);
    MeshModel *addOrGetMesh(QString fullPath, const QString& Label, bool setAsCurrent=true);
    

    ///remove the mesh from the list and delete it from memory
    bool delMesh(MeshModel *mmToDel);

    ///add a new raster model
    RasterModel *addNewRaster(/*QString rasterName*/);

    ///remove the raster from the list and delete it from memory
    bool delRaster(RasterModel *rasterToDel);

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

    Box3m bbox()
    {
		Box3m FullBBox;
		foreach(MeshModel * mp, meshList)
			FullBBox.Add(mp->cm.Tr,mp->cm.bbox);
		return FullBBox;
    }

    bool hasBeenModified();

private:
    MeshModel *currentMesh;
    //the current raster model
	RasterModel* currentRaster;

signals:
    ///whenever the current mesh is changed (e.g. the user click on a different mesh)
    // this signal will send out with the index of the newest mesh
    void currentMeshChanged(int index);

    /// whenever the document (or even a single mesh) is modified by a filter
    void meshDocumentModified();

    ///whenever the meshList is changed
    void meshSetChanged();

	void meshAdded(int index);
    void meshRemoved(int index);

    ///whenever the rasterList is changed
    void rasterSetChanged();

    //this signal is emitted when a filter request to update the mesh in the renderingState
    void documentUpdated();
	void updateDecorators(int mesh_id);

};// end class MeshDocument


#endif // MESH_DOCUMENT_H
