#include "io_3dcoform.h"

bool CoformIOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &par, vcg::CallBackPos *cb, QWidget *parent)
{
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
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> CoformIOPlugin::exportFormats() const
{
	QList<Format> formatList;
	formatList << Format("3DRI File Format"	,tr("3DRI"));
	return formatList;
}