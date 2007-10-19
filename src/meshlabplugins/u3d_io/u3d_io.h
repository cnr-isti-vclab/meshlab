#ifndef U3D_IO_H
#define U3D_IO_H

#include <QObject>
#include <QList>
#include <QString>

#include "../../meshlab/meshmodel.h"
#include "../../meshlab/interfaces.h"

class U3DIOPlugin : public QObject, public MeshIOInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshIOInterface)
  
 public:
	QList<Format> importFormats() const;
	QList<Format> exportFormats() const;

	const PluginInfo &Info();
	int GetExportMaskCapability(QString &format) const;

	bool open(const QString &formatName, QString &fileName, MeshModel &m, int& mask, vcg::CallBackPos *cb=0, QWidget *parent=0);
	bool save(const QString &formatName, QString &fileName, MeshModel &m, const int& mask, vcg::CallBackPos *cb=0, QWidget *parent= 0);
};

#endif