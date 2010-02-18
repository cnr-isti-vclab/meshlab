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
	setupModel();
	cofDiag = new CoformImportDialog(model);
	cofDiag->hide();
	initImportingFiltersExt();
}

CoformIOPlugin::~CoformIOPlugin()
{
	delete cofDiag;
	delete model;
}

void CoformIOPlugin::initImportingFiltersExt()
{
	QStringList stl;
	stl.push_back(tr("ZIP files (.zip)"));
	stl.push_back(tr("COLLADA files (.dae)"));
	cofDiag->addExtensionFilters(stl);
}

void CoformIOPlugin::setupModel()
{
	model = new QStandardItemModel(0,2,this);
	model->setHeaderData(0, Qt::Horizontal, tr("UUID"));
	model->setHeaderData(1, Qt::Horizontal, tr("File Name"));
}

QString CoformIOPlugin::computePluginsPath() const
{
	QDir pluginsDir(PluginManager::getPluginDirPath());
	pluginsDir.cd("3D-COFORM");
	qDebug("U3D plugins dir %s", qPrintable(pluginsDir.absolutePath()));
	return pluginsDir.absolutePath();
}

bool CoformIOPlugin::getUuidAndFileList(const QString hostURL,const int port,QList< UuidFileNamePair >& res) const
{
	return true;
}	

QString CoformIOPlugin::query() const
{
	return QString();
}

Q_EXPORT_PLUGIN(CoformIOPlugin)