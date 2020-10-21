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

#ifndef IORenderman_H
#define IORenderman_H

#include <QObject>
#include <common/interfaces.h>
#include "utilities_hqrender.h"
#include "RibFileStack.h"
#include "export_rib.h"

class IORenderman : public QObject, public MeshIOInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshIOInterface)

  
public:
	QList<Format> importFormats() const;
	QList<Format> exportFormats() const;

	virtual void GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const;
    void initSaveParameter(const QString &/*format*/, MeshModel &/*m*/, RichParameterSet & /*par*/);
    bool open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask,const RichParameterSet & par, vcg::CallBackPos *cb=0, QWidget *parent=0);
    bool save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask,const RichParameterSet & par,  vcg::CallBackPos *cb, QWidget *parent);
    IORenderman();
private:
  vcg::CallBackPos * cb;
  QDir templatesDir; //directory of templates ("render_template")
  QStringList templates; //list of templates found
  
  //parser_rib.cpp
  enum alignValue { CENTER, TOP, BOTTOM };
  inline const QString mainFileName() { return QString("scene.rib"); }
  bool convertedGeometry;	
  int worldBeginRendered, numOfWorldBegin, lastCb; //for progress bar update
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

  bool makeScene(MeshModel* m, QStringList* textureList,const RichParameterSet &par, QFileInfo* templateFile, QString destDirString, QStringList* shaderDirs, QStringList* textureDirs, QStringList* proceduralDirs, QStringList* imagesRendered);
  QString convertObject(int currentFrame, QString destDir, MeshModel* m,const RichParameterSet &par, QStringList* textureList);
  bool resetBound();
  bool resetGraphicsState();
  int writeMatrix(FILE* fout, const vcg::Matrix44f* matrix, bool transposed = true);
  vcg::Matrix44f getMatrix(const QString* matrixString)  const;
  enum searchType{ ERR, ARCHIVE, SHADER, TEXTURE, PROCEDURAL };  
  QStringList readSearchPath(const QStringList* token, int* type);
};

#endif
