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
#include <map>

#include "ml_mesh_type.h"

#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/color.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/update/position.h>
#include <vcg/complex/algorithms/update/quality.h>
#include <vcg/complex/algorithms/update/selection.h>
#include <vcg/complex/algorithms/update/topology.h>


//#include <wrap/gl/trimesh.h>



#include <wrap/callback.h>
#include <wrap/io_trimesh/io_mask.h>
#include <wrap/io_trimesh/additionalinfo.h>

#include <QList>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QReadWriteLock>
#include <QImage>
#include <QAction>
#include "GLLogStream.h"
#include "filterscript.h"
#include "ml_shared_data_context.h"


/*
MeshModel Class
The base class for representing a single mesh.
It contains a single vcg mesh object with some additional information for keeping track of its origin and of what info it has.
*/

class MeshDocument;

class MeshModel
{
public:
    /*
    This enum specify the various simplex components
    It is used in various parts of the framework:
    - to know what elements are currently active and therefore can be saved on a file
    - to know what elements are required by a filter and therefore should be made ready before starting the filter (e.g. if a
    - to know what elements are changed by a filter and therefore should be saved/restored in case of dynamic filters with a preview
    */
    enum MeshElement{
        MM_NONE             = 0x00000000,
        MM_VERTCOORD        = 0x00000001,
        MM_VERTNORMAL       = 0x00000002,
        MM_VERTFLAG         = 0x00000004,
        MM_VERTCOLOR        = 0x00000008,
        MM_VERTQUALITY      = 0x00000010,
        MM_VERTMARK	        = 0x00000020,
        MM_VERTFACETOPO     = 0x00000040,
        MM_VERTCURV	        = 0x00000080,
        MM_VERTCURVDIR      = 0x00000100,
        MM_VERTRADIUS       = 0x00000200,
        MM_VERTTEXCOORD     = 0x00000400,
        MM_VERTNUMBER       = 0x00000800,

        MM_FACEVERT         = 0x00001000,
        MM_FACENORMAL       = 0x00002000,
        MM_FACEFLAG	        = 0x00004000,
        MM_FACECOLOR        = 0x00008000,
        MM_FACEQUALITY      = 0x00010000,
        MM_FACEMARK	        = 0x00020000,
        MM_FACEFACETOPO     = 0x00040000,
        MM_FACENUMBER       = 0x00080000,
        MM_FACECURVDIR      = 0x00100000,

        MM_WEDGTEXCOORD     = 0x00200000,
        MM_WEDGNORMAL       = 0x00400000,
        MM_WEDGCOLOR        = 0x00800000,

        // 	Selection
        MM_VERTFLAGSELECT   = 0x01000000,
        MM_FACEFLAGSELECT   = 0x02000000,

        // Per Mesh Stuff....
        MM_CAMERA			= 0x08000000,
        MM_TRANSFMATRIX     = 0x10000000,
        MM_COLOR            = 0x20000000,
        MM_POLYGONAL        = 0x40000000,

		// unknown - will raise exceptions, to be avoided, here just for compatibility
        MM_UNKNOWN          = 0x80000000,

		// geometry change (for filters that remove stuff or modify geometry or topology, but not touch face/vertex color or face/vertex quality)
		MM_GEOMETRY_AND_TOPOLOGY_CHANGE  = 0x431e7be7,

		// everything - dangerous, will add unwanted data to layer (e.g. if you use MM_ALL it could means that it could add even color or quality)
        MM_ALL				= 0xffffffff
    };

    MeshModel(MeshDocument *parent, QString fullFileName, QString labelName);
	MeshModel(MeshModel* cp);
	
	MeshDocument *parent;

    CMeshO cm;




    /*vcg::GlTrimesh<CMeshO> glw;*/
    
    
    
    /*
    Bitmask denoting what fields are currently used in the mesh
    it is composed by MeshElement enums.
    it should be changed by only mean the following functions:

    updateDataMask(neededStuff)
    clearDataMask(no_needed_stuff)
    hasDataMask(stuff)

    Note that if an element is active means that is also allocated
    Some unactive elements (vertex color) are usually already allocated
    other elements (FFAdj or curvature data) not necessarily.

    */

private:
    int currentDataMask;
    QString fullPathFileName;
    QString _label;
    int _id;
    bool modified;

public:
    void Clear();
    void UpdateBoxAndNormals(); // This is the STANDARD method that you should call after changing coords.
    inline int id() const {return _id;}
    
    
    // Some notes about the files and naming.
    // Each mesh when shown in the layer dialog has a label.
    // By default the label is just the name of the file, but the

    // in a future the path should be moved outside the meshmodel into the meshdocument (and assume that all the meshes resides in a common subtree)
    // currently we just fix the interface and make the pathname private for avoiding future hassles.

    QString label() const { if(_label.isEmpty()) return shortName(); else return _label;}

    /// The whole full path name of the mesh
    QString fullName() const {return fullPathFileName;}

    /// just the name of the file
    QString shortName() const { return QFileInfo(fullPathFileName).fileName(); }

    /// the full path without the name of the file (e.g. the dir where the mesh and often its textures are)
    QString pathName() const {QFileInfo fi(fullName()); return fi.absolutePath();}

    /// just the extension.
    QString suffixName() const {QFileInfo fi(fullName()); return fi.suffix();}

    /// the relative path with respect to the current project
    QString relativePathName() const;

    /// the absolute path of the current project
    QString documentPathName() const;

    void setFileName(QString newFileName) {
        QFileInfo fi(newFileName);
        if(!fi.isAbsolute()) qWarning("Someone is trying to put a non relative filename");
        fullPathFileName = fi.absoluteFilePath();
    }
    void setLabel(QString newName) {_label=newName;}

    bool visible; // used in rendering; Needed for toggling on and off the meshes
    bool isVisible() { return visible; }

    // This function is roughly equivalent to the updateDataMask,
    // but it takes in input a mask coming from a filetype instead of a filter requirement (like topology etc)
    void Enable(int openingFileMask);

    bool hasDataMask(const int maskToBeTested) const;
    void updateDataMask(MeshModel *m);
    void updateDataMask(int neededDataMask);
    void clearDataMask(int unneededDataMask);
    int dataMask() const;


    bool& meshModified();
    static int io2mm(int single_iobit);
};// end class MeshModel


/*
Plane Class
the base class for a registered image that contains the path, the semantic and the data of the image
*/

class Plane
{
public:

    enum PlaneSemantic
    {
      NONE        = 0x0000,
      RGBA        = 0x0001,
      MASK_UB     = 0x0002,
      MASK_F      = 0x0004,
      DEPTH_F     = 0x0008,
      EXTRA00_F        = 0x0100,
      EXTRA01_F        = 0x0200,
      EXTRA02_F        = 0x0400,
      EXTRA03_F        = 0x0800,
      EXTRA00_RGBA     = 0x1000,
      EXTRA01_RGBA     = 0x2000,
      EXTRA02_RGBA     = 0x4000,
      EXTRA03_RGBA     = 0x8000
    };

    int semantic;
    QString fullPathFileName;
    QImage image;
    QImage thumb;
    float *buf;

    bool IsInCore() { return !image.isNull(); }
    void Load();
    void Discard(); //discard  the loaded image freeing the mem.

    /// The whole full path name of the mesh
    const QString fullName() const {return fullPathFileName;}
    /// just the name of the file
    const QString shortName() const { return QFileInfo(fullPathFileName).fileName(); }

    Plane(const Plane& pl);
    Plane(const QString pathName, const int _semantic);

}; //end class Plane


class MeshLabRenderRaster
{
public:
    MeshLabRenderRaster();
    MeshLabRenderRaster(const MeshLabRenderRaster& rm);
    ~MeshLabRenderRaster();

    Shotm shot;

    ///The list of the registered images
    QList<Plane *> planeList;
    Plane *currentPlane;

    void addPlane(Plane * plane);
};

/*
RasterModel Class
The base class for keeping a set of "registered" images (e.g. images that can be projected onto a 3D space).
Each Raster model is composed by a list of registered images, each image with its own "semantic" (color, depth, normals, quality, masks)
and with all the images sharing the same shot.
*/

class RasterModel : public MeshLabRenderRaster
{
public:
    RasterModel();
    MeshDocument* par;

private:
    int _id;
    QString _label;

public:
    bool visible;
    inline int id() const {return _id;}

    RasterModel(MeshDocument *parent, QString _rasterName=QString());

    void setLabel(QString newLabel) {_label = newLabel;}

    const QString label() const {
        if(!_label.isEmpty())  return _label;
        if(!planeList.empty()) return planeList.first()->shortName();
        return "Error!";
    }

    enum RasterElement
    {
        RM_NONE		= 0x00000000,
        RM_ALL		= 0xffffffff
    };
};// end class RasterModel

//class RenderMode
//{
//private:
//    QList<QAction*> declist;
//
//public:
//    vcg::GLW::DrawMode	drawMode;
//    vcg::GLW::ColorMode	colorMode;
//    vcg::GLW::TextureMode	textureMode;
//
//
//
//    MLRenderingData::PRIMITIVE_MODALITY_MASK pmmask;
//    MLRenderingData::RendAtts atts;
//
//    bool lighting;
//    bool backFaceCull;
//    bool doubleSideLighting;
//    bool fancyLighting;
//
//    RenderMode(vcg::GLW::DrawMode dm) // :declist()
//    {
//      Init();
//      drawMode=dm;
//    }
//
//    RenderMode() {  Init(); }
//
//    void Init()
//    {
//        drawMode	= vcg::GLW::DMSmooth;
//        colorMode = vcg::GLW::CMNone;
//        textureMode = vcg::GLW::TMNone;
//        
//        pmmask = MLRenderingData::PR_SOLID;
//        atts = MLRenderingData::RendAtts();
//        atts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
//        atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;
//
//        lighting = true;
//        backFaceCull = false;
//        doubleSideLighting = false;
//        fancyLighting = false;
//    }
//
//    inline void addPrimitiveModality(MLRenderingData::PRIMITIVE_MODALITY addedpm)
//    {
//        pmmask = pmmask | addedpm;
//    }
//
//    inline void removePrimitiveModality(MLRenderingData::PRIMITIVE_MODALITY removedpm)
//    {
//        pmmask = pmmask & (~removedpm);
//    }
//
//    inline void setDrawMode(const vcg::GLW::DrawMode dm)
//    {
//        drawMode = dm;
//    }
//
//    inline void setColorMode(const vcg::GLW::ColorMode cm)
//    {
//        colorMode = cm;
//    }
//
//    inline void setTextureMode(const vcg::GLW::TextureMode tm)
//    {
//        textureMode = tm;
//    }
//
//    inline void setLighting(const bool ison)
//    {
//        lighting = ison;
//    }
//
//    inline void setBackFaceCull(const bool ison)
//    {
//        backFaceCull = ison;
//    }
//
//    inline void setDoubleFaceLighting(const bool ison)
//    {
//        doubleSideLighting = ison;
//    }
//
//    inline void setFancyLighting(const bool ison)
//    {
//        fancyLighting = ison;
//    }
//
//    inline QList<QAction*>& decoratorList()
//    {
//        return declist;
//    }
//
//    static RenderMode defaultRenderingAtts();
//}; // end class RenderMode
//Q_DECLARE_METATYPE(RenderMode)

//class RasterModelState : public QObject
//{
//	Q_OBJECT
//public:
//	MeshLabRenderState();
//	~MeshLabRenderState();
//
//private:
//	//for quickness I added a RasterModel, but should be something less.
//	QMap<int,RasterModel*> _rendermap;
//	QReadWriteLock _mutdoc;
//};
//class FilterScript;


struct MeshModelStateData
{
	int _mask;
	size_t _nvert;
	size_t _nface;
	size_t _nedge;

	MeshModelStateData(int mask, size_t nvert, size_t nface, size_t nedge)
		:_mask(mask), _nvert(nvert), _nface(nface), _nedge(nedge)
	{}
};

class MeshDocumentStateData
{
public:
	MeshDocumentStateData();
	~MeshDocumentStateData();

	void create(MeshDocument& md);
	QMap<int, MeshModelStateData>::iterator insert(const int key, const MeshModelStateData & value);
	QMap<int, MeshModelStateData>::iterator find(const int key);
	QMap<int, MeshModelStateData>::iterator begin();
	QMap<int, MeshModelStateData>::iterator end();
	void clear();

private:
	mutable QReadWriteLock _lock;
	QMap<int, MeshModelStateData> _existingmeshesbeforeoperation;
};

class MeshDocument : public QObject
{
    Q_OBJECT

public:

    MeshDocument();

    //deletes each meshModel
    ~MeshDocument();

    /// returns the mesh with the given unique id
    MeshModel *getMesh(int id);
    MeshModel *getMesh(QString name);
    MeshModel *getMeshByFullName(QString pathName);


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

    MeshModel *mm() {
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
    FilterScript* filterHistory;
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
    MeshModel *addOrGetMesh(QString fullPath, QString Label, bool setAsCurrent=true);
    

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

/*
A class designed to save partial aspects of the state of a mesh, such as vertex colors, current selections, vertex positions
and then be able to restore them later.
This is a fundamental part for the dynamic filters framework.

Note: not all the MeshElements are supported!!
*/
class MeshModelState
{
private:
    int changeMask; // a bit mask indicating what have been changed. Composed of MeshModel::MeshElement (e.g. stuff like MeshModel::MM_VERTCOLOR)
    MeshModel *m; // the mesh which the changes refers to.
    std::vector<float> vertQuality;
    std::vector<vcg::Color4b> vertColor;
    std::vector<vcg::Color4b> faceColor;
    std::vector<Point3m> vertCoord;
    std::vector<Point3m> vertNormal;
    std::vector<Point3m> faceNormal;
    std::vector<bool> faceSelection;
    std::vector<bool> vertSelection;
    Matrix44m Tr;
    Shotm shot;
public:
    // This function save the <mask> portion of a mesh into the private members of the MeshModelState class;
    void create(int _mask, MeshModel* _m);
    bool apply(MeshModel *_m);
    bool isValid(MeshModel *m);
    int maskChangedAtts() const {return changeMask;}
};



#endif
