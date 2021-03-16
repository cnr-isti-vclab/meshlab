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

#include "io_u3d.h"

#include <wrap/io_trimesh/export.h>
#include <wrap/io_trimesh/export_idtf.h>
#include <wrap/io_trimesh/io_mask.h>

#include <QMessageBox>
#include <qapplication.h>
#include <QSettings>
#include "Converter.h"

using namespace std;
using namespace vcg;


U3DIOPlugin::U3DIOPlugin() :
	QObject(), IOMeshPlugin(), _param()
{
}

bool U3DIOPlugin::open(
		const QString &, 
		const QString &, 
		MeshModel &, 
		int&, 
		const RichParameterList &, 
		CallBackPos *, 
		QWidget *)
{
	return false;
}
bool U3DIOPlugin::save(
		const QString &formatName, 
		const QString &fileName, 
		MeshModel &m, 
		const int mask, 
		const RichParameterList & par, 
		vcg::CallBackPos *, 
		QWidget *)
{
	vcg::tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
	vcg::tri::Allocator<CMeshO>::CompactFaceVector(m.cm);

	QString errorMsgFormat = "Error encountered while exporting file %1:\n%2";
	string filename = QFile::encodeName(fileName).constData();
	std::string ex = formatName.toUtf8().data();

	
	QStringList textures_to_be_restored;
	QStringList lst = 
			vcg::tri::io::ExporterIDTF<CMeshO>::convertInTGATextures(
				m.cm, QDir::tempPath(), textures_to_be_restored);
	if(formatName.toUpper() == tr("U3D")) {
		qApp->restoreOverrideCursor();
		saveParameters(par);
		QSettings settings;
		
		//tmp idtf
		QString tmp(QDir::tempPath());
		QString curr = QDir::currentPath();
		QString out(fileName);
		QStringList out_trim;
		vcg::tri::io::QtUtilityFunctions::splitFilePath(fileName,out_trim);
		tmp = tmp + "/" +
				vcg::tri::io::QtUtilityFunctions::fileNameFromTrimmedPath(out_trim) + ".idtf";
		vcg::tri::io::ExporterIDTF<CMeshO>::Save(m.cm,qPrintable(tmp),mask);

		//conversion from idtf to u3d
		int resCode = 0;
		bool result = IDTFConverter::IDTFToU3d(tmp.toStdString(), filename, resCode, _param.positionQuality);

		if(result==false) {
			errorMessage = "Error saving " + QString::fromStdString(filename) + ": \n" + vcg::tri::io::ExporterU3D<CMeshO>::ErrorMsg(resCode) + " (" + QString::number(resCode) + ")";
			return false;
		}
		
		//saving latex:
		QDir::setCurrent(curr);
		QString lat (fileName);
		QStringList l = lat.split(".");
		saveLatex(l[0], _param);
		QDir dir(QDir::tempPath());
		dir.remove(tmp);

		vcg::tri::io::ExporterIDTF<CMeshO>::removeConvertedTGATextures(lst);
	}
	
	if(formatName.toUpper() == tr("IDTF")) 
		tri::io::ExporterIDTF<CMeshO>::Save(m.cm,filename.c_str(),mask);
	vcg::tri::io::ExporterIDTF<CMeshO>::restoreConvertedTextures(
				m.cm,
				textures_to_be_restored);
	return true;
}

/*
	returns the list of the file's type which can be imported
*/
QString U3DIOPlugin::pluginName() const
{
	return "IOU3D";
}

std::list<FileFormat> U3DIOPlugin::importFormats() const
{
	std::list<FileFormat> formatList;
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<FileFormat> U3DIOPlugin::exportFormats() const
{
	QList<FileFormat> formatList;
	formatList << FileFormat("U3D File Format"	,tr("U3D"));
	formatList << FileFormat("IDTF File Format"	,tr("IDTF"));
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void U3DIOPlugin::exportMaskCapability(
		const QString &format, 
		int &capability, 
		int &defaultBits) const
{
	if(format.toUpper() == tr("U3D")) {
		capability = defaultBits =
				vcg::tri::io::ExporterU3D<CMeshO>::GetExportMaskCapability();
		defaultBits &= (~vcg::tri::io::Mask::IOM_VERTNORMAL);
		defaultBits &= (~vcg::tri::io::Mask::IOM_VERTCOLOR);
		defaultBits &= (~vcg::tri::io::Mask::IOM_FACECOLOR);
		return;
	}
	if(format.toUpper() == tr("IDTF")) {
		capability = defaultBits =
				vcg::tri::io::ExporterIDTF<CMeshO>::GetExportMaskCapability();
		defaultBits &= (~vcg::tri::io::Mask::IOM_VERTNORMAL);
		defaultBits &= (~vcg::tri::io::Mask::IOM_VERTCOLOR);
		defaultBits &= (~vcg::tri::io::Mask::IOM_FACECOLOR);
		return;
	}

	assert(0);
}

void U3DIOPlugin::initSaveParameter(const QString &, MeshModel &m, RichParameterList &par) 
{
	_param._campar = 
			new vcg::tri::io::u3dparametersclasses::Movie15Parameters<CMeshO>::CameraParameters(
				m.cm.bbox.Center(),m.cm.bbox.Diag());
	Point3m pos = _param._campar->_obj_pos;
	Point3m dir(0.0f,0.0f,-1.0f * _param._campar->_obj_bbox_diag);
	par.addParam(RichPoint3f("position_val",dir, "Camera Position",
		"The position in which the camera is set. The default value is derived by the 3d mesh's bounding box."));
	par.addParam(RichPoint3f("target_val",pos, "Camera target point",
		"The point towards the camera is seeing. The default value is derived by the 3d mesh's bounding box."));
	par.addParam(RichFloat("fov_val",60.0f,
		"Camera's FOV Angle 0..180","Camera's FOV Angle. The values' range is between 0-180 degree. The default value is 60."));
	par.addParam(RichInt("compression_val",500,"U3D quality 0..1000",
		"U3D mesh's compression ratio. The values' range is between 0-1000 degree. The default value is 500."));
}

void U3DIOPlugin::saveParameters(const RichParameterList &par)
{
	Point3m from_target_to_camera = 
			Point3m(par.getPoint3m(QString("position_val")) - par.getPoint3m(QString("target_val")));
	vcg::tri::io::u3dparametersclasses::Movie15Parameters<CMeshO>::CameraParameters* sw = _param._campar;
	Point3m p = sw->_obj_pos;
	_param._campar = new vcg::tri::io::u3dparametersclasses::Movie15Parameters<CMeshO>::CameraParameters(
		par.getFloat(QString("fov_val")),0.0,from_target_to_camera,from_target_to_camera.Norm(),sw->_obj_bbox_diag,p);
	_param.positionQuality = par.getInt(QString("compression_val"));

	delete sw;
}

void U3DIOPlugin::saveLatex(const QString& file,const vcg::tri::io::u3dparametersclasses::Movie15Parameters<CMeshO>& mov_par)
{
	Output_File latex(file.toStdString() + ".tex");
	QString u3df = file + ".u3d";
	QStringList file_trim;
	vcg::tri::io::QtUtilityFunctions::splitFilePath(u3df,file_trim);
	std::string u3d_final = 
			vcg::tri::io::QtUtilityFunctions::fileNameFromTrimmedPath(file_trim).toStdString();
	latex.write(0,"\\documentclass[a4paper]{article}");
	latex.write(0,"\\usepackage[3D]{movie15}");
	latex.write(0,"\\usepackage{hyperref}");
	latex.write(0,"\\usepackage[UKenglish]{babel}");
	latex.write(0,"\\begin{document}");
	latex.write(0,"\\includemovie[");
	latex.write(1,"poster,");
	latex.write(1,"toolbar, %same as `controls\'");


	QString u3d_text = QString::fromStdString(u3d_final);
	substituteChar(u3d_text,QChar('_'),QString(""));
	latex.write(1,"label=" + u3d_text.toStdString() + ",");
	latex.write(1,"text=(" + u3d_text.toStdString() + "),");
	std::string cam_string;
	typename vcg::tri::io::u3dparametersclasses::Movie15Parameters<CMeshO>::CameraParameters* cam = mov_par._campar;
	if (cam != NULL) {
		cam_string = cam_string + "3Daac=" + TextUtility::nmbToStr(cam->_cam_fov_angle) +
				", 3Droll=" + TextUtility::nmbToStr(cam->_cam_roll_angle) +
				", 3Dc2c=" + TextUtility::nmbToStr(cam->_obj_to_cam_dir.X()) + " " + TextUtility::nmbToStr(cam->_obj_to_cam_dir.Z()) + " " + TextUtility::nmbToStr(cam->_obj_to_cam_dir.Y()) +
				", 3Droo=" + TextUtility::nmbToStr(cam->_obj_to_cam_dist) +
				", 3Dcoo=" + TextUtility::nmbToStr(-cam->_obj_pos.X()) + " " + TextUtility::nmbToStr(cam->_obj_pos.Z()) + " " + TextUtility::nmbToStr(cam->_obj_pos.Y()) + ",";
		latex.write(1,cam_string);
	}
	latex.write(1,"3Dlights=CAD,");
	latex.write(0,"]{\\linewidth}{\\linewidth}{" + u3d_final + "}");
	latex.write(0,"\\end{document}");
}

void U3DIOPlugin::substituteChar(QString& st,const QChar& ch_remove,const QString& sub)
{
	int ii = 0;
	while ((ii = st.indexOf(ch_remove,ii)) != -1)
		st = st.replace(ii,1,sub);
}

MESHLAB_PLUGIN_NAME_EXPORTER(U3DIOPlugin)
