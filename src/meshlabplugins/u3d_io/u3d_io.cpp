#include <algorithm>

#include <Qt>
#include <QtGui>

#include "u3d_io.h"

//#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>

#include <wrap/io_trimesh/export.h>
#include <wrap/io_trimesh/io_mask.h>
#include <wrap/io_trimesh/export_u3d.h>
#include "u3d_gui.h"


#include <QMessageBox>
using namespace vcg;

bool U3DIOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, CallBackPos *cb, QWidget *parent)
{
	return false;
}

bool U3DIOPlugin::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, vcg::CallBackPos *cb, QWidget *parent)
{
	QString errorMsgFormat = "Error encountered while exportering file %1:\n%2";
	string filename = QFile::encodeName(fileName).constData ();
  //std::string filename = fileName.toUtf8().data();
	std::string ex = formatName.toUtf8().data();

	vcg::tri::io::u3dparametersclasses::Movie15Parameters mp;
	U3D_GUI pw(mp,parent);
	pw.exec();
	

	QString conv_loc;
	QSettings settings("VCG","U3D_TEST");
	QString conv_loc_std("..\\..\\..\\..\\code\\lib\\U3D\\Bin\\Win32\\Release\\IDTFConverter.exe");
	QString conv_key = settings.value("U3D/converter").toString();
	if (conv_key.isNull())
		settings.setValue("U3D/converter",conv_loc_std);
	else 
		conv_loc = conv_loc_std; 
	

	int result;
	
	result = vcg::tri::io::ExporterU3D<CMeshO>::Save(m.cm,filename.c_str(),qPrintable(conv_loc),mp,mask);

	if(result!=0)
	{
		QMessageBox::warning(parent, tr("Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ExporterU3D<CMeshO>::ErrorMsg(result)));
		return false;
	}
	return true;
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
		capability=defaultBits = vcg::tri::io::ExporterU3D<CMeshO>::GetExportMaskCapability();
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