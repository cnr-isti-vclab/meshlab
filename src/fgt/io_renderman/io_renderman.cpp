/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

#include <Qt>
#include <QtGui>
#include <QMessageBox>
#include <QFileDialog>

#include "io_renderman.h"

#include <wrap/io_trimesh/export_smf.h>
#include <wrap/io_trimesh/import_smf.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/polygon_support.h>
#include <common/pluginmanager.h>

using namespace vcg;

IORenderman::IORenderman() 
{  
  templatesDir = PluginManager::getBaseDirPath();

  if(!templatesDir.cd("render_template")) {
	  qDebug("Error. I was expecting to find the render_template dir. Now i am in dir %s",qPrintable(templatesDir.absolutePath()));
    ;//this->errorMessage = "\"render_template\" folder not found";
  }
}


bool IORenderman::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask,const RichParameterSet & par,  CallBackPos *cb, QWidget *parent)
{
    assert(0);
	return true;
}

bool IORenderman::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask,const RichParameterSet & par,  vcg::CallBackPos *cb, QWidget *parent)
{
	QString errorMsgFormat = "Error encountered while exportering file %1:\n%2";
  
  //***get the filter parameters and create destination directory***
	//MeshModel* m = md.mm();
	this->cb = cb;
	QTime tt; tt.start(); //time for debuging
	qDebug("Starting apply filter");

  if(templates.isEmpty()) //there aren't any template
	{
	  this->errorMessage = "No template scene has been found in \"render_template\" directory";
		return false;
	}
	QString templateName = templates.at(par.getEnum("scene")); //name of selected template
	QFileInfo templateFile = QFileInfo(templatesDir.absolutePath() + QDir::separator() + templateName + QDir::separator() + templateName + ".rib");
  
	//directory of current mesh
	//QString meshDirString = m->pathName();
  ////creating of destination directory
	//bool delRibFiles = !par.getBool("SaveScene");
	////if SaveScene is true create in same mesh directory, otherwise in system temporary directry
	//QDir destDir = QDir::temp(); //system temporary directry
	//if(!delRibFiles) {
	//	//destDir = QDir(par.getSaveFileName("SceneName"));
	//	destDir = QDir(meshDirString);
	//}
	////create scene directory if don't exists
	//if(!destDir.cd("scene")) {
 //   if(!destDir.mkdir("scene") || !destDir.cd("scene")) {
	//		this->errorMessage = "Creating scene directory at " + destDir.absolutePath();
	//		return false;
	//	}		
	//}
	//create a new directory with template name
	QDir destDir = QFileInfo(fileName).dir();
  QString destDirString = templateName + "-" + QFileInfo(m.fullName()).completeBaseName();
  QString newDir = destDirString;
	int k = 0;
	while(destDir.cd(newDir)) {
		destDir.cdUp();
		newDir = destDirString + "-" + QString::number(++k); //dir templateName+k
	}
	if(!destDir.mkdir(newDir) || !destDir.cd(newDir)) {
		this->errorMessage = "Creating scene directory at " + destDir.absolutePath();
		return false;
	}

	//destination diretory
	destDirString = destDir.absolutePath();

	//***Texture: take the list of texture mesh
	QStringList textureListPath = QStringList();
  for(size_t i=0; i<m.cm.textures.size(); i++) {
    QString path = QString(m.cm.textures[i].c_str());
		textureListPath << path;
	}
	
	//***read the template files and create the new scenes files
	QStringList shaderDirs, textureDirs, proceduralDirs, imagesRendered;
	qDebug("Starting reading cycle %i",tt.elapsed());
  if(!makeScene(&m, &textureListPath, par, &templateFile, destDirString, &shaderDirs, &textureDirs, &proceduralDirs, &imagesRendered))
		return false; //message already set
	qDebug("Cycle ending at %i",tt.elapsed());
	Log(GLLogStream::FILTER,"Successfully created scene");

	//check if the final rib file will render any image
	/*if(imagesRendered.size() == 0) {
		this->errorMessage = "The template description hasn't a statement to render any image";
		return false;
	}*/

	//***copy the rest of template files (shaders, textures, procedural..)
  UtilitiesHQR::copyFiles(templateFile.dir(), destDir, textureDirs);
  UtilitiesHQR::copyFiles(templateFile.dir(), destDir, shaderDirs);
  UtilitiesHQR::copyFiles(templateFile.dir(), destDir, proceduralDirs);
	qDebug("Copied needed file at %i",tt.elapsed());
	
  //***convert the texture mesh to tiff format 
  if(!textureListPath.empty() && (m.cm.HasPerWedgeTexCoord() || m.cm.HasPerVertexTexCoord())) {
    QString textureName = QFileInfo(textureListPath.first()).completeBaseName(); //just texture name
    QFile srcFile(m.pathName() + QDir::separator() + textureListPath.first());

		//destination directory it's the first readable/writable between textures directories
		QString newImageDir = ".";
		foreach(QString dir, textureDirs) {
			if(dir!="." && destDir.cd(dir)) {
				newImageDir = dir;
				destDir.cdUp();
				break;
			}
		}
		qDebug("source texture directory: %s", qPrintable(srcFile.fileName()));
		QString newTex = destDirString + QDir::separator() + newImageDir + QDir::separator() + textureName;
		qDebug("destination texture directory: %s", qPrintable(newTex + ".tiff"));
		if(srcFile.exists()) {
			//convert image to tiff format (the one readable in aqsis)
			QImage image;
			image.load(srcFile.fileName());
			image.save(newTex + ".tiff", "Tiff");

		}
		else {
			this->errorMessage = "Not founded the texture file: " + srcFile.fileName();
			return false; //the mesh has a texture not existing
		}
  }
  /*int result = vcg::tri::io::ExporterRIB<CMeshO>::Save(m.cm,qPrintable(fileName),mask,false,cb); 

	if(result!=0)
	{
    QMessageBox::warning(parent, tr("Saving Error"), errorMsgFormat.arg(qPrintable(fileName), vcg::tri::io::ExporterRIB<CMeshO>::ErrorMsg(result)));
		return false;
	}*/
	return true;
}

/*
	returns the list of the file's type which can be imported
*/
QList<MeshIOInterface::Format> IORenderman::importFormats() const
{
	QList<Format> formatList;
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> IORenderman::exportFormats() const
{
	QList<Format> formatList;
    formatList << Format("RenderMan Interface Bytestream Protocol"	,tr("rib"));
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void IORenderman::GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const
{
  capability=defaultBits=vcg::tri::io::ExporterRIB<CMeshO>::GetExportMaskCapability();
	return;
}
void IORenderman::initSaveParameter(const QString &/*format*/, MeshModel &/*m*/, RichParameterSet & parlst)
{
  //update the template list
	templates = QStringList();
	foreach(QString subDir, templatesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
    //foreach directory, search a file with the same name
		QString temp(templatesDir.absolutePath() + QDir::separator() + subDir + QDir::separator() + subDir + ".rib");
		if(QFile::exists(temp))
			templates << subDir;
	}
	if(templates.isEmpty())
	{
		this->errorMessage = "No template scene has been found in \"render_template\" directory";
    qDebug("%s",qPrintable(this->errorMessage));
	}
	parlst.addParam(new RichEnum("scene",0,templates,"Select scene",
    "Select the scene where the loaded mesh will be drawed in."));			
	parlst.addParam(new RichInt("FormatX", 640, "Format X", "Final image/s lenght size."));
	parlst.addParam(new RichInt("FormatY", 480, "Format Y", "Final image/s width size."));
	parlst.addParam(new RichFloat("PixelAspectRatio", 1.0, "Pixel aspect ratio", "Final image/s pixel aspect ratio."));
	parlst.addParam(new RichBool("Autoscale",true,"Auto-scale mesh","Check if the mesh will be scaled on render scene"));			
	QStringList alignValueList = (QStringList() << "Center" << "Top" << "Bottom");
	parlst.addParam(new RichEnum("AlignX",0,alignValueList,"Align X",
    "If it's checked the mesh will be aligned with scene x axis"));
	parlst.addParam(new RichEnum("AlignY",0,alignValueList,"Align Y",
	  "If it's checked the mesh will be aligned with scene y axis"));
  parlst.addParam(new RichEnum("AlignZ",0,alignValueList,"Align Z",			
	  "If it's checked the mesh will be aligned with scene z axis"));  
}
Q_EXPORT_PLUGIN(IORenderman)
