#ifndef FILTER_HQRENDER_H
#define FILTER_HQRENDER_H

#include <QObject>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QFileInfo>
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
  virtual bool applyFilter(QAction *filter, MeshDocument &m, RichParameterSet & param, vcg::CallBackPos * cb) ;
  virtual FilterClass getClass(QAction *a);
  void initGlobalParameterSet(QAction *, RichParameterSet &/*globalparam*/);	
	
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
	
  inline QString AqsisBinPathParam() const { return  "MeshLab::Filter::AqsisBinPath" ; }

  enum aqsisFile { AQSIS, AQSL, TEQSER, PIQSL };
  QStringList aqsisFileName;
  
  const void setAqsisFileName() {
    #if defined(Q_OS_WIN)
    aqsisFileName = QStringList() << "aqsis.exe" << "aqsl.exe" << "teqser.exe" << "piqsl.exe";
    #elif defined(Q_OS_MAC)
	  aqsisFileName = QStringList() << "aqsis" << "aqsl" << "teqser" << "piqsl";
    #endif
  }

  inline const QString defaultAqsisBinPath() {
    #if defined(Q_OS_WIN)
    return "c:/Program Files (x86)/Aqsis/bin";
    //return "c:/Program Files/Aqsis/bin";
    #elif defined(Q_OS_MAC)
    return "/Applications/Aqsis.app/Contents/Resources/bin";
    #endif
  }

  inline const QString mainFileName() { return QString("scene.rib"); }
  	
  //parser_rib.cpp
  int numberOfDummies, numOfObject;
  //the state graphics at procedure call time
  struct Procedure {
    QString name;
    vcg::Matrix44f matrix;
    float bound[6];
    QString surfaceShader;
  };
  //Graphics state
  QStack<vcg::Matrix44f> transfMatrixStack;
  QStack<QString> surfaceShaderStack;
  float objectBound[6]; // xmin, xmax, ymin, ymax, zmin, zmax

  bool makeScene(MeshModel* m, QStringList* textureList, RichParameterSet &par, QFileInfo* templateFile, QString destDirString, QStringList* shaderDirs, QStringList* textureDirs, QStringList* proceduralDirs, QStringList* imagesRendered);
  QString convertObject(int currentFrame, QString destDir, MeshModel* m, RichParameterSet &par, QStringList* textureList);
  bool resetBound();
  bool resetGraphicsState();
  int writeMatrix(FILE* fout, const vcg::Matrix44f* matrix, bool transposed = true);
  vcg::Matrix44f getMatrix(const QString* matrixString)  const;
  enum searchType{ ERR, ARCHIVE, SHADER, TEXTURE, PROCEDURAL };  
  QStringList readSearchPath(const QStringList* token, int* type);
};

#endif
