#ifndef U3D_IO_H
#define U3D_IO_H

#include <QObject>
#include <QList>
#include <QString>

#include "../../meshlab/meshmodel.h"
#include "../../meshlab/interfaces.h"
#include "../../meshlab/filterparameter.h"
#include <wrap/io_trimesh/export_u3d.h>
#include <wrap/io_trimesh/export_idtf.h>

class U3DIOPlugin : public QObject, public MeshIOInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshIOInterface)
  
 public:
	QList<Format> importFormats() const;
	QList<Format> exportFormats() const;

	virtual void GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const;

	bool open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const FilterParameterSet &, vcg::CallBackPos *cb=0, QWidget *parent=0);
	bool save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const FilterParameterSet &, vcg::CallBackPos *cb=0, QWidget *parent= 0);

	void initSaveParameter(const QString &format, MeshModel &/*m*/, FilterParameterSet &par);
	void applyOpenParameter(const QString &format, MeshModel &m, const FilterParameterSet &par);

	private:
	QString computePluginsPath();


	void saveParameters();
	void defaultParameters();

	vcg::tri::io::u3dparametersclasses::Movie15Parameters _param;
	
};

#endif
