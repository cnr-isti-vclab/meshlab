#include <algorithm>


#include <Qt>
#include <QtGui>

#include "u3d_io.h"

//#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>

#include <wrap/io_trimesh/export.h>
#include <wrap/io_trimesh/io_mask.h>
#include <wrap/io_trimesh/export_u3d.h>
#include <wrap/io_trimesh/export_idtf.h>
#include "u3d_gui.h"


#include <QMessageBox>
using namespace vcg;

bool U3DIOPlugin::open(const QString & /*formatName*/, const QString &/*fileName*/, MeshModel &/*m*/, int& /*mask*/, CallBackPos */*cb*/, QWidget */*parent*/)
{
	return false;
}

QString U3DIOPlugin::computePluginsPath()
{
		QDir pluginsDir(MainWindowInterface::getPluginDirPath());
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


bool U3DIOPlugin::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, vcg::CallBackPos */*cb*/, QWidget *parent)
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
		vcg::tri::io::u3dparametersclasses::Movie15Parameters mp;
		mp._campar = new vcg::tri::io::u3dparametersclasses::Movie15Parameters::CameraParameters(m.cm.bbox.Center(),m.cm.bbox.Diag());
		U3D_GUI pw(mp,parent);
		pw.exec();
		qApp->setOverrideCursor(QCursor(Qt::WaitCursor));	  
		qDebug("Quality parameter %i",mp.positionQuality);

		QSettings settings;
		
		QString converterPath = computePluginsPath();
		QString converterCommandLine;		
	#if defined(Q_OS_WIN)
		converterPath += "/IDTFConverter.exe";
		converterCommandLine = converterPath;
	#elif defined(Q_OS_MAC)
		converterCommandLine = converterPath +"/IDTFConverter.sh "+converterPath;
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
		
		int result = tri::io::ExporterU3D<CMeshO>::Save(m.cm,filename.c_str(),qPrintable(converterCommandLine),mp,mask);
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
	formatList << Format("U3D File Format"	,tr("IDTF"));
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

const PluginInfo &U3DIOPlugin::Info()
{	
	static PluginInfo ai;
  ai.Date=tr("October 2007");
	ai.Version = tr("0.1");
	ai.Author = ("Guido Ranzuglia");
	return ai;
 }





Q_EXPORT_PLUGIN(U3DIOPlugin)