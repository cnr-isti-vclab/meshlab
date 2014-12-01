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

#include <vcg/complex/complex.h>

#include <vcg/simplex/face/topology.h>

#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/color.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/update/position.h>
#include <vcg/complex/algorithms/update/quality.h>
#include <vcg/complex/algorithms/update/selection.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/create/mc_trivial_walker.h>

#include <wrap/gl/trimesh.h>
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

#ifndef MESHLAB_SCALAR
#error "Fatal compilation error: MESHLAB_SCALAR must be defined"
#endif

typedef MESHLAB_SCALAR Scalarm;
typedef vcg::Point2<MESHLAB_SCALAR>   Point2m;
typedef vcg::Point3<MESHLAB_SCALAR>   Point3m;
typedef vcg::Point4<MESHLAB_SCALAR>   Point4m;
typedef vcg::Plane3<MESHLAB_SCALAR>   Plane3m;
typedef vcg::Segment2<MESHLAB_SCALAR> Segment2m;
typedef vcg::Segment3<MESHLAB_SCALAR> Segment3m;
typedef vcg::Box3<MESHLAB_SCALAR>     Box3m;
typedef vcg::Matrix44<MESHLAB_SCALAR> Matrix44m;
typedef vcg::Matrix33<MESHLAB_SCALAR> Matrix33m;
typedef vcg::Shot<MESHLAB_SCALAR>     Shotm;
typedef vcg::SimpleVoxel<MESHLAB_SCALAR> SimpleVoxelm;
typedef vcg::Similarity<MESHLAB_SCALAR> Similaritym;

namespace vcg
{
  namespace vertex
  {
    template <class T> class Coord3m: public Coord<vcg::Point3<MESHLAB_SCALAR>, T> {
    public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("Coord3m"));T::Name(name);}
    };

    template <class T> class Normal3m: public Normal<vcg::Point3<MESHLAB_SCALAR>, T> {
    public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("Normal3m"));T::Name(name);}
    };

    template <class T> class CurvatureDirmOcf: public CurvatureDirOcf<CurvatureDirTypeOcf<MESHLAB_SCALAR>, T> {
    public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("CurvatureDirmOcf"));T::Name(name);}
    };

    template <class T> class RadiusmOcf: public RadiusOcf<MESHLAB_SCALAR, T> {
    public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("RadiusmOcf"));T::Name(name);}
  };

  }//end namespace vertex
  namespace face
  {
    template <class T> class Normal3m: public NormalAbs<vcg::Point3<MESHLAB_SCALAR>, T> {
    public:  static void Name(std::vector<std::string> & name){name.push_back(std::string("Normal3m"));T::Name(name);}
    };

    template <class T> class CurvatureDirmOcf: public CurvatureDirOcf<CurvatureDirOcfBaseType<MESHLAB_SCALAR>, T> {
    public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("CurvatureDirdOcf"));T::Name(name);}
    };

  }//end namespace face
}//end namespace vcg



// Forward declarations needed for creating the used types
class CVertexO;
class CEdgeO;
class CFaceO;

// Declaration of the semantic of the used types
class CUsedTypesO: public vcg::UsedTypes < vcg::Use<CVertexO>::AsVertexType,
    vcg::Use<CEdgeO   >::AsEdgeType,
    vcg::Use<CFaceO  >::AsFaceType >{};


// The Main Vertex Class
// Most of the attributes are optional and must be enabled before use.
// Each vertex needs 40 byte, on 32bit arch. and 44 byte on 64bit arch.

class CVertexO  : public vcg::Vertex< CUsedTypesO,
    vcg::vertex::InfoOcf,           /*  4b */
    vcg::vertex::Coord3m,           /* 12b */
    vcg::vertex::BitFlags,          /*  4b */
    vcg::vertex::Normal3m,          /* 12b */
    vcg::vertex::Qualityf,          /*  4b */
    vcg::vertex::Color4b,           /*  4b */
    vcg::vertex::VFAdjOcf,          /*  0b */
    vcg::vertex::MarkOcf,           /*  0b */
    vcg::vertex::TexCoordfOcf,      /*  0b */
    vcg::vertex::CurvaturefOcf,     /*  0b */
    vcg::vertex::CurvatureDirmOcf,  /*  0b */
    vcg::vertex::RadiusmOcf         /*  0b */
>{
};


// The Main Edge Class
class CEdgeO : public vcg::Edge<CUsedTypesO,
    vcg::edge::BitFlags,          /*  4b */
    vcg::edge::EVAdj,
    vcg::edge::EEAdj
>{
};

// Each face needs 32 byte, on 32bit arch. and 48 byte on 64bit arch.
class CFaceO    : public vcg::Face<  CUsedTypesO,
    vcg::face::InfoOcf,              /* 4b */
    vcg::face::VertexRef,            /*12b */
    vcg::face::BitFlags,             /* 4b */
    vcg::face::Normal3m,             /*12b */
    vcg::face::QualityfOcf,          /* 0b */
    vcg::face::MarkOcf,              /* 0b */
    vcg::face::Color4bOcf,           /* 0b */
    vcg::face::FFAdjOcf,             /* 0b */
    vcg::face::VFAdjOcf,             /* 0b */
    vcg::face::CurvatureDirmOcf,     /* 0b */
    vcg::face::WedgeTexCoordfOcf     /* 0b */
> {};


class CMeshO    : public vcg::tri::TriMesh< vcg::vertex::vector_ocf<CVertexO>, vcg::face::vector_ocf<CFaceO> >
{
public :
    int sfn;    //The number of selected faces.
    int svn;    //The number of selected vertices.
    Matrix44m Tr; // Usually it is the identity. It is applied in rendering and filters can or cannot use it. (most of the filter will ignore this)

    const Box3m &trBB()
    {
        static Box3m bb;
        bb.SetNull();
        bb.Add(Tr,bbox);
        return bb;
    }
};

/** This class provide vbo buffered rendering
 *
 * It can generate/update all the needed VBO buffers usually needed for efficient rendering.
 *
 */

class BufferObjectsRendering : public QObject
{
    Q_OBJECT
public:
    BufferObjectsRendering(bool highprecmode);
    ~BufferObjectsRendering();
    void DrawPoints(vcg::GLW::ColorMode colm);
    void DrawWire(vcg::GLW::ColorMode colm,vcg::GLW::NormalMode norm);
    void DrawFlatWire(vcg::GLW::ColorMode colm,vcg::GLW::TextureMode textm);
    void DrawTriangles(vcg::GLW::ColorMode cm, vcg::GLW::NormalMode nm, vcg::GLW::TextureMode tm );
   

//    bool requestUpdate(CMeshO& mm, Box3m bb, const int updateattributesmask);

    //render function for invoking buffer objects based rendering
    void render(const Box3m &bbDoc, vcg::GLW::DrawMode drawm, vcg::GLW::NormalMode norm, vcg::GLW::ColorMode colm, vcg::GLW::TextureMode textm );

    //function to clear/deallocate the buffer objects memory space
    void clearState();

    void clearState(int updateattributesmask,vcg::GLW::DrawMode drawm, vcg::GLW::NormalMode norm, vcg::GLW::ColorMode colm, vcg::GLW::TextureMode textm);
    std::vector<unsigned int> TMId;

public slots:
        //buffer objects update function. Info are collected from the mm and inserted inside the correspondent buffer objects
    bool update(CMeshO& mm, int updateattributesmask,vcg::GLW::DrawMode drawm, vcg::GLW::NormalMode nolm,vcg::GLW::ColorMode colm, vcg::GLW::TextureMode tm );
private:
    bool updateIndexedAttributesPipeline(CMeshO& mm, int updateattributesmask,vcg::GLW::ColorMode colm, vcg::GLW::NormalMode nolm, vcg::GLW::TextureMode tm );
    bool updateReplicatedAttributesPipeline(CMeshO& mm, int updateattributesmask,vcg::GLW::ColorMode colm, vcg::GLW::NormalMode nolm, vcg::GLW::TextureMode tm );

    void importPerVertexAttributes( const CMeshO& mm,std::vector<vcg::Point3f>& pv,std::vector<vcg::Point3f>& nv);
    void importPerVertexAttributes(const CMeshO& mm,std::vector<vcg::Point3f>& pv,std::vector<vcg::Point3f>& nv,std::vector<vcg::Color4b>& cv,std::vector<float>& tv);

    //enum BufferObjectType	{
    //  OTVertexPosition,
    //  OTVertexNormal,
    //  OTVertexColor,
    //  OTVertexTexture,
    //  OTTriangleIndex,
    //  OTEdgeIndex,
    //  OTFauxEdgeIndex,
    //  OTVertexReplicatedPosition,
    //  OTVertexReplicatedNormal,
    //  OTVertexReplicatedColor,
    //  OTFaceReplicatedNormal,
    //  OTFaceReplicatedColor,
    //  OTWedgeReplicatedTexture,
    //  OTLast
    //  } ;

   //GLuint bid[OTLast];

   GLuint positionBO;
   GLuint normalBO;
   GLuint textureBO;
   GLuint colorBO;
   std::vector<GLuint> indexTriBO;
   std::vector<GLuint> indexTriBOSz;

   std::vector< std::pair<short,GLuint> > texIndNumTrianglesV;

   bool HighPrecisionMode;

   // For sake of precision the buffers are created so that their bb is centered at the origin.
   // And in rendering we render them there (not in their original position).
   // Problem 1 :  multiple meshes
   // Solution 1: we hope that the meshes are more or less all in the same zone and we compute
   // the transformation that maps all of them in a common position.
   // P_1 = T_1 * P'_1
   // P_2 = T_2 * P'_2
   // Let C be a common translation
   // When we render we render them as centered in C.

   // To guarantee precision we have to pre-apply the matrix position
   // P = M * p
   // P = M * T * p'
   //
   //

   Point3m bbCenter;
   Matrix44m Tr;

   int vn;
   int tn;

   QReadWriteLock _lock;
};

class GLWRendering : public vcg::GlTrimesh<CMeshO>
{
public:
    GLWRendering();
    GLWRendering(CMeshO& mm);
    ~GLWRendering();

    void render(vcg::GLW::DrawMode dm,vcg::GLW::NormalMode nm,vcg::GLW::ColorMode cm,vcg::GLW::TextureMode tm );
};

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
        MM_UNKNOWN          = 0x80000000,

        MM_ALL				= 0xffffffff
    };

    MeshDocument *parent;

    CMeshO cm;
    BufferObjectsRendering bor;
    GLWRendering glw;

public:
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
    MeshModel(MeshDocument *parent, QString fullFileName, QString labelName);

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

class RenderMode
{
private:
    QList<QAction*> declist;

public:
    vcg::GLW::DrawMode	drawMode;
    vcg::GLW::ColorMode	colorMode;
    vcg::GLW::TextureMode	textureMode;

    bool lighting;
    bool backFaceCull;
    bool doubleSideLighting;
    bool fancyLighting;

    RenderMode(vcg::GLW::DrawMode dm) // :declist()
    {
      Init();
      drawMode=dm;
    }

    RenderMode() {  Init(); }

    void Init()
    {
        drawMode	= vcg::GLW::DMSmooth;
        colorMode = vcg::GLW::CMNone;
        textureMode = vcg::GLW::TMNone;

        lighting = true;
        backFaceCull = false;
        doubleSideLighting = false;
        fancyLighting = false;
    }

    inline void setDrawMode(const vcg::GLW::DrawMode dm)
    {
        drawMode = dm;
    }

    inline void setColorMode(const vcg::GLW::ColorMode cm)
    {
        colorMode = cm;
    }

    inline void setTextureMode(const vcg::GLW::TextureMode tm)
    {
        textureMode = tm;
    }

    inline void setLighting(const bool ison)
    {
        lighting = ison;
    }

    inline void setBackFaceCull(const bool ison)
    {
        backFaceCull = ison;
    }

    inline void setDoubleFaceLighting(const bool ison)
    {
        doubleSideLighting = ison;
    }

    inline void setFancyLighting(const bool ison)
    {
        fancyLighting = ison;
    }

    inline QList<QAction*>& decoratorList()
    {
        return declist;
    }

}; // end class RenderMode
Q_DECLARE_METATYPE(RenderMode)



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
public:

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
        if(busy && _busy==false)
        {
            emit meshDocumentModified();
        }
        busy=_busy;
    }

private:
    bool  busy;

public:
    ///add a new mesh with the given name
    MeshModel *addNewMesh(QString fullPath, QString Label, bool setAsCurrent=true,const RenderMode& rm = RenderMode());

    MeshModel *addOrGetMesh(QString fullPath, QString Label, bool setAsCurrent=true,const RenderMode& rm = RenderMode());

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
    void meshAdded(int index,RenderMode rm);
    void meshRemoved(int index);

    ///whenever the rasterList is changed
    void rasterSetChanged();

    //this signal is emitted when a filter request to update the mesh in the renderingState
    void documentUpdated();

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
};


#endif
