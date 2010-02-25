#ifndef FILTER_HQRENDER_H
#define FILTER_HQRENDER_H

#include <QObject>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QProcess>
#include "RibFileStack.h"
#include "export_rib.h"
#include "utilities_hqrender.h"
#include <common/meshmodel.h>
#include <common/interfaces.h>



class FilterHighQualityRender : public QObject, public MeshFilterInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshFilterInterface)

public:

  enum { FP_HIGHQUALITY_RENDER } ;

  FilterHighQualityRender();

  //filter_hqrender.cpp
  virtual QString filterName(FilterIDType filter) const;
  virtual QString filterInfo(FilterIDType filter) const;
  virtual bool autoDialog(QAction *) {return true;}
  virtual void initParameterSet(QAction *,MeshModel &m, RichParameterSet & param);
  //virtual bool applyFilter(QAction *filter, MeshModel &m, RichParameterSet & param, vcg::CallBackPos * cb) ;
  virtual bool applyFilter(QAction *filter, MeshDocument &m, RichParameterSet & param, vcg::CallBackPos * cb) ;
  virtual FilterClass getClass(QAction *a);

private slots:
  void updateOutputProcess();
  void errSgn();  
private:
  vcg::CallBackPos * cb;
  int worldBeginRendered, numOfWorldBegin, lastCb; //for progress bar update
  QProcess renderProcess;
  QDir templatesDir; //directory of templates ("render_template")
  QStringList templates; //list of templates found
  QStringList imageFormatsSupported; //list of image formats supported by qt for conversion of final image
  
  enum alignValue { CENTER, TOP, BOTTOM };
  bool convertedGeometry;
	
  inline const QString aqsisName() 
  { 
  #if defined(Q_OS_WIN)
	return QString("aqsis.exe");
  #elif defined(Q_OS_MAC)
	return QString("aqsis");
  #endif
  }
  inline const QString aqslName() 
  { 
  #if defined(Q_OS_WIN)
	return QString("aqsl.exe");
  #elif defined(Q_OS_MAC)
	return QString("aqsl");
  #endif
  }
  inline const QString teqserName() 
  { 
  #if defined(Q_OS_WIN)
	return QString("teqser.exe");
  #elif defined(Q_OS_MAC)
	return QString("teqser");
  #endif
  }
  inline const QString piqslName() 
  { 
  #if defined(Q_OS_WIN)
	return QString("piqsl.exe");
  #elif defined(Q_OS_MAC)
	return QString("piqsl");
  #endif
  }
  inline const QString aqsisBinPath() 
  { 
  #if defined(Q_OS_WIN)
	return QString("bin/");
  #elif defined(Q_OS_MAC)
	return QString("/Contents/Resources/bin/");
  #endif
  }

  inline const QString mainFileName() { return QString("scene.rib"); }
  	
  //parser_rib.cpp
  int numberOfDummies, numOfObject;
  /*struct ObjValues {
    vcg::Matrix44f objectMatrix;
	  float objectBound[6]; // xmin, xmax, ymin, ymax, zmin, zmax
	  QStringList objectShader;
	  QString objectId;
	  QString objectDisplacementbound;
  };*/
  QStack<vcg::Matrix44f> transfMatrix;
  QStack<QString> surfaceShader;
  float objectBound[6]; // xmin, xmax, ymin, ymax, zmin, zmax

  bool makeScene(MeshModel* m, QStringList* textureList, RichParameterSet &par, QString templatePath, QString destDirString, QStringList* shaderDirs, QStringList* textureDirs, QStringList* proceduralDirs, QStringList* imagesRendered);
  //QString parseObject(RibFileStack* files, QString destDir, int currentFrame, MeshModel* m, RichParameterSet &par, QStringList* textureList);
  QString convertObject(int currentFrame, QString destDir, MeshModel* m, RichParameterSet &par, QStringList* textureList);
  bool resetBound();
  bool resetGraphicsState();
  int writeMatrix(FILE* fout, const vcg::Matrix44f* matrix, bool transposed = true);
  vcg::Matrix44f getMatrix(const QString* matrixString)  const;
  enum searchType{ ERR, ARCHIVE, SHADER, TEXTURE, PROCEDURAL };  
  QStringList readSearchPath(const QStringList* token, int* type);
};

#endif
