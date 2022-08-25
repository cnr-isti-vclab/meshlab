#ifndef COFORMIOPLUGIN_H
#define COFORMIOPLUGIN_H

#include <common/interfaces.h>
#include "metadatafile_generator.h"

class CoformIOPlugin : public QObject, public MeshIOInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshIOInterface)
public:
	CoformIOPlugin();
	~CoformIOPlugin();

	QList<Format> importFormats() const;
	QList<Format> exportFormats() const;

	//inline QString CoformQueryFilePathParam() const { return  "MeshLab::3D-COFORM::QueryFilePath" ; }

	void GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const;

	bool open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &par, vcg::CallBackPos *cb=0, QWidget *parent=0);
	bool save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet &, vcg::CallBackPos *cb=0, QWidget *parent= 0);	
	void initSaveParameter(const QString &format, MeshModel &/*m*/, RichParameterSet &par);
	//void initGlobalParameterSet(QAction * /*format*/, RichParameterSet & /*globalparam*/);
private:
	inline static const QString uuidPerMeshAttributeName() {return QString("UUID");}
	inline static const QString coformRIUserNameParam() { return  QString("MeshLab::3D-COFORM::RIUserName"); }
	inline static const QString coformRIUserSurNameParam() { return  QString("MeshLab::3D-COFORM::RIUserSurName"); }
	inline static const QString coformRIUserUUIDParam() { return  QString("MeshLab::3D-COFORM::RIUserUUID"); }
	static void genMeshInfo(MeshModel& m,const bool isoutputfile, MetaDataFileGenerator::MeshInfo* minfo);
	
};

#endif