#include "io_3dcoform.h"
#include <QtXml/QDomDocument>
#include <QString>
#include <common/pluginmanager.h>

bool CoformIOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &par, vcg::CallBackPos *cb, QWidget *parent)
{
	// initializing mask
	mask = 0;
	// initializing progress bar status
	if (cb != NULL)		(*cb)(0, "Loading...");

	QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";

	if (formatName.toUpper() == tr("3DRI"))
	{
		QDomDocument doc("3D-COFORM");
		QFile file(fileName);
		if (!file.open(QIODevice::ReadOnly))
			return false;
		cofDiag->exec(); 
	}
	else 
	{
		assert(0); // Unknown File type
		return false;
	}

	if (cb != NULL)	(*cb)(99, "Done");
	return true;
}

bool CoformIOPlugin::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet &, vcg::CallBackPos *cb, QWidget *parent)
{
	return false;
}

/*
	returns the list of the file's type which can be imported
*/
QList<MeshIOInterface::Format> CoformIOPlugin::importFormats() const
{
	QList<Format> formatList;
	formatList << Format("3DRI File Format"	,tr("3DRI"));
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> CoformIOPlugin::exportFormats() const
{
	QList<Format> formatList;
	formatList << Format("3DC File Format",tr("3DC"));
	return formatList;
}

void CoformIOPlugin::GetExportMaskCapability( QString &format, int &capability, int &defaultBits ) const
{
	capability = 0;
}

CoformIOPlugin::CoformIOPlugin()
{
	cofDiag = new CoformImportDialog();
	cofDiag->hide();
	initImportingFiltersExt();
}

CoformIOPlugin::~CoformIOPlugin()
{
	delete cofDiag;
}

void CoformIOPlugin::initImportingFiltersExt()
{
	QStringList stl;
	stl.push_back(tr("ZIP files (.zip)"));
	stl.push_back(tr("COLLADA files (.dae)"));
	cofDiag->addExtensionFilters(stl);
}

Q_EXPORT_PLUGIN(CoformIOPlugin)