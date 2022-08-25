#ifndef METADATAFILE_GENERATOR_H
#define METADATAFILE_GENERATOR_H

#include <QString>
#include <QList>

class MetaDataFileGenerator
{
public:
	class Organization
	{
	public:
		QString name;
		QString website;
		QString city;
		QString country;
		QString geoRef;

		Organization(const QString& nm,const QString& web,const QString& ct,const QString& cty,const QString& geo = QString());
	};

	class Person
	{
	public:
		QString uuid;
		QString name;
		QString surname;
		Organization* org;

		Person(const QString& uuidPer,const QString& nm,const QString& srnm,Organization* orgz = NULL);
	};

	class MeshInfo 
	{
	public:
		QString label;
		QString uuid;
		int nfc;
		int nvt;
		int megabyte;

		MeshInfo(const QString& lbl,const QString& uuidMesh,const int fcs,const int vert,const float mb);
	};

	class Process
	{
	public:
		QString comment;
		QString uuidProc;
		QList<MeshInfo> input;
		MeshInfo output; 
		QString meshlabVer;
		QString filtHistory;

		Process(const QString& comm,const QString& uuidPr,const QList<MeshInfo>& inp,const MeshInfo& out,const QString& mlver,const QString& filts);
	};

	static QString generateRDF(const Process& proc,const Person& pers);
};

#endif