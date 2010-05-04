/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#include <algorithm>


#include <Qt>
#include <QtGui>

#include "io_u3d.h"
#include <common/pluginmanager.h>
//#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>

#include <wrap/io_trimesh/export.h>
#include <wrap/io_trimesh/io_mask.h>

#include <QMessageBox>
using namespace std;
using namespace vcg;


U3DIOPlugin::U3DIOPlugin()
:QObject(),MeshIOInterface(),_param()
{

}

bool U3DIOPlugin::open(const QString & /*formatName*/, const QString &/*fileName*/, MeshModel &/*m*/, int& /*mask*/, const RichParameterSet &, CallBackPos */*cb*/, QWidget */*parent*/)
{
	return false;
}

QString U3DIOPlugin::computePluginsPath()
{
    QDir pluginsDir(PluginManager::getPluginDirPath());
		#if defined(Q_OS_WIN)
			pluginsDir.cd("U3D_W32");
		#elif defined(Q_OS_MAC)
				pluginsDir.cd("U3D_OSX");
		#elif defined(Q_OS_LINUX)
				pluginsDir.cd("U3D_LINUX");
		#endif
		qDebug("U3D plugins dir %s", qPrintable(pluginsDir.absolutePath()));
		return pluginsDir.absolutePath();
}


bool U3DIOPlugin::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet & par, vcg::CallBackPos */*cb*/, QWidget *parent)
{
  vcg::tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
	vcg::tri::Allocator<CMeshO>::CompactFaceVector(m.cm);

	QString errorMsgFormat = "Error encountered while exporting file %1:\n%2";
	string filename = QFile::encodeName(fileName).constData ();
  //std::string filename = fileName.toUtf8().data();
	std::string ex = formatName.toUtf8().data();
	//QString curr = QDir::currentPath();	
	//QString tmp(QDir::tempPath());
	//
	//if there are textures file that aren't in tga format I have to convert them	
	//
	//I maintain the converted file name (i.e. file_path + originalname without extension + tga) in mesh.textures but I have to revert to the original ones	////before the function return. 
	//QStringList oldtextname;	
	//for(unsigned int ii = 0; ii < m.cm.textures.size();++ii)	
	//	oldtextname.push_back(m.cm.textures[ii].c_str());
	//
	//tmp vector to save the tga created files that should be deleted.	
	//QStringList convfile;	
	//vcg::tri::io::TGA_Exporter::convertTexturesFiles(m.cm,curr,convfile);	
	
	QStringList textures_to_be_restored;	
	QStringList lst = vcg::tri::io::ExporterIDTF<CMeshO>::convertInTGATextures(m.cm,QDir::tempPath(),textures_to_be_restored);
	if(formatName.toUpper() == tr("U3D"))
	{
		qApp->restoreOverrideCursor();	
		/*vcg::tri::io::u3dparametersclasses::Movie15Parameters mp;
		mp._campar = new vcg::tri::io::u3dparametersclasses::Movie15Parameters::CameraParameters(m.cm.bbox.Center(),m.cm.bbox.Diag());
		U3D_GUI pw(mp,parent);
		pw.exec();
		qApp->setOverrideCursor(QCursor(Qt::WaitCursor));	  
		qDebug("Quality parameter %i",mp.positionQuality);*/
		saveParameters(par);
		QSettings settings;
		
		QString converterPath = computePluginsPath();
		QString converterCommandLine;		
	#if defined(Q_OS_WIN)
		converterPath += "/IDTFConverter.exe";
		converterCommandLine = converterPath;
	#elif defined(Q_OS_MAC)
		converterPath.replace(QString(" "),QString("\\ "));
		converterCommandLine = "\""+converterPath +"/IDTFConverter.sh"+ "\" \"" + converterPath+"\"";
		converterPath = converterPath +"/IDTFConverter.sh";
	#endif
		if (settings.contains("U3D/converter"))
			converterPath=settings.value("U3D/converter").toString();
		//else 
		//	settings.setValue("U3D/converter",converterPath);
	  QFileInfo converterFI(converterPath);
		if(!converterFI.exists())
		{
			QMessageBox::warning(parent, tr("Saving Error"), QString("Missing converter executable '%1'").arg(converterPath));
			return false;
		}
		
		int result = tri::io::ExporterU3D<CMeshO>::Save(m.cm,filename.c_str(),qPrintable(converterCommandLine),_param,mask);
		vcg::tri::io::ExporterIDTF<CMeshO>::removeConvertedTGATextures(lst);
		if(result!=0)
		{
			QMessageBox::warning(parent, tr("Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ExporterU3D<CMeshO>::ErrorMsg(result)));
			return false;
		}
	}
	
	if(formatName.toUpper() == tr("IDTF"))		int result = tri::io::ExporterIDTF<CMeshO>::Save(m.cm,filename.c_str(),mask);		vcg::tri::io::ExporterIDTF<CMeshO>::restoreConvertedTextures(m.cm,textures_to_be_restored);	return true;
}

/*
	returns the list of the file's type which can be imported
*/
QList<MeshIOInterface::Format> U3DIOPlugin::importFormats() const
{
	QList<Format> formatList;
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> U3DIOPlugin::exportFormats() const
{
	QList<Format> formatList;
	formatList << Format("U3D File Format"	,tr("U3D"));
	formatList << Format("IDTF File Format"	,tr("IDTF"));
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void U3DIOPlugin::GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const
{
	if(format.toUpper() == tr("U3D"))
	{
		capability = defaultBits = vcg::tri::io::ExporterU3D<CMeshO>::GetExportMaskCapability();
		defaultBits &= vcg::tri::io::Mask::IOM_VERTNORMAL;
		defaultBits &= vcg::tri::io::Mask::IOM_VERTCOLOR;
		return;
	}
	if(format.toUpper() == tr("IDTF"))
	{
		capability=defaultBits = vcg::tri::io::ExporterIDTF<CMeshO>::GetExportMaskCapability();
		return;
	}

	assert(0);
}

//static float avoidExponentialNotation(const float n,const float bboxdiag)
//{
//	float val_min = std::min(1000.0f,floorf(bboxdiag * 1000.0f));
//	return floorf(val_min * n ) / val_min;
//}
//
//static vcg::Point3f avoidExponentialNotation(const vcg::Point3f& p,const float bboxdiag)
//{
//	return vcg::Point3f(avoidExponentialNotation(p.X(),bboxdiag),
//		avoidExponentialNotation(p.Y(),bboxdiag),
//		avoidExponentialNotation(p.Z(),bboxdiag));
//}

void U3DIOPlugin::initSaveParameter(const QString &format, MeshModel &m, RichParameterSet &par) 
{
	_param._campar = new vcg::tri::io::u3dparametersclasses::Movie15Parameters::CameraParameters(m.cm.bbox.Center(),m.cm.bbox.Diag());
	//vcg::Point3f pos = avoidExponentialNotation(_param._campar->_obj_pos,_param._campar->_obj_bbox_diag);
	vcg::Point3f pos = _param._campar->_obj_pos;
	vcg::Point3f dir(0.0f,0.0f,-1.0f * _param._campar->_obj_bbox_diag);
	par.addParam(new RichPoint3f("position_val",dir, "Camera Position",
		"The position in which the camera is set. The default value is derived by the 3d mesh's bounding box."));		
	//vcg::Point3f dir(0.0f,0.0f,avoidExponentialNotation(-1.0f * _param._campar->_obj_bbox_diag,_param._campar->_obj_bbox_diag));
	par.addParam(new RichPoint3f("target_val",pos, "Camera target point",
		"The point towards the camera is seeing. The default value is derived by the 3d mesh's bounding box."));
	par.addParam(new RichFloat("fov_val",60.0f,"Camera's FOV Angle 0..180","Camera's FOV Angle. The values' range is between 0-180 degree. The default value is 60."));
	par.addParam(new RichInt("compression_val",500,"U3D quality 0..1000","U3D mesh's compression ratio. The values' range is between 0-1000 degree. The default value is 500."));
}

void U3DIOPlugin::saveParameters(const RichParameterSet &par)
{
        vcg::Point3f from_target_to_camera = vcg::Point3f(par.getPoint3f(QString("position_val")) - par.getPoint3f(QString("target_val")));
	vcg::tri::io::u3dparametersclasses::Movie15Parameters::CameraParameters* sw = _param._campar;
	//vcg::Point3f p = avoidExponentialNotation(sw->_obj_pos,_param._campar->_obj_bbox_diag);
	vcg::Point3f p = sw->_obj_pos;
	_param._campar = new vcg::tri::io::u3dparametersclasses::Movie15Parameters::CameraParameters(
		par.getFloat(QString("fov_val")),0.0f,from_target_to_camera,from_target_to_camera.Norm(),sw->_obj_bbox_diag,p);
	_param.positionQuality = par.getInt(QString("compression_val"));

	delete sw;
}

Q_EXPORT_PLUGIN(U3DIOPlugin)
