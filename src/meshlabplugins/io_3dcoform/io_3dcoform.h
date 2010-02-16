#ifndef COFORMIOPLUGIN_H
#define COFORMIOPLUGIN_H

#include <QList>
#include <common/interfaces.h>

class CoformIOPlugin : public QObject, public MeshIOInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshIOInterface)
public:
	CoformIOPlugin() {}

	QList<Format> importFormats() const;
	QList<Format> exportFormats() const;


	bool open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &par, vcg::CallBackPos *cb=0, QWidget *parent=0);
	bool save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet &, vcg::CallBackPos *cb=0, QWidget *parent= 0);
};

#endif