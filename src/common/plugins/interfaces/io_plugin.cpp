#include "io_plugin.h"
#include "../../ml_document/mesh_document.h"

void IOPlugin::open(
		const QString& format,
		const QString& fileName,
		MeshDocument& md,
		std::list<MeshModel*>& meshModelList,
		std::list<int>& maskList,
		const RichParameterList& par,
		vcg::CallBackPos* cb)
{
	QFileInfo info(fileName);
	MeshModel *mm = md.addNewMesh(fileName, info.fileName());
	meshModelList.push_back(mm);
	int mask;
	try {
		open(format, fileName, *mm, mask, par, cb);
		maskList.push_back(mask);
	}
	catch(const MLException& exc){
		md.delMesh(mm);
		throw exc;
	}
}

void IOPlugin::reportWarning(const QString& warningMessage) const
{
	MeshLabPluginLogger::log(GLLogStream::WARNING, warningMessage.toStdString());
	warnString += "\n" + warningMessage;
}

void IOPlugin::wrongOpenFormat(const QString& format)
{
	throw MLException("Internal error: unknown open format " + format + " to " + pluginName() + " plugin.");
}

void IOPlugin::wrongSaveFormat(const QString& format)
{
	throw MLException("Internal error: unknown save format " + format + " to " + pluginName() + " plugin.");
}

QString IOPlugin::warningMessageString() const
{
	QString tmp = warnString;
	warnString.clear();
	return tmp;
}
