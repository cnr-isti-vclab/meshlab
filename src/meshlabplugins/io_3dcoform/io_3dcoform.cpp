#include "io_3dcoform.h"

bool 3DCoformIOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &par, vcg::CallBackPos *cb=0, QWidget *parent=0)
{
	return true;
}

bool 3DCoformIOPlugin::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet &, vcg::CallBackPos *cb=0, QWidget *parent= 0)ù
{
	return false;
}

/*
	returns the list of the file's type which can be imported
*/
QList<MeshIOInterface::Format> 3DCoformIOPlugin::importFormats() const
{
	QList<Format> formatList;
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> 3DCoformIOPlugin::exportFormats() const
{
	QList<Format> formatList;
	formatList << Format("3DRI File Format"	,tr("3DRI"));
	return formatList;
}