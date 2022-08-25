#ifndef COFORM_INTERFACE_H
#define COFORM_INTERFACE_H

#include <QDockWidget>
#include <QDir>
#include <QString>
 #include <QAbstractTableModel>
#include <QTableWidget>
#include "../../common/pluginmanager.h"

namespace Ui
{
	class CoformDialog;
}

class RIBridgeInterface
{
private:
	QString generateBat() const;
	QProcessEnvironment setEnv() const;
	
	QDir coformJavaClientDir;
	QString coformJavaClientName;
	const QString& user;
	const QString& pass;
	const QString& location;

	const QString& ipHost;
	QDir propFileDir;
	const QDir& javaDir;

	inline QString javaExec() const
	{
		#if defined(Q_OS_WIN)
			return QString("java.exe");
		#else
			return QString("java");
		#endif
	}

	inline QString createExecBasicString() const 
	{
		QFileInfo fi(coformJavaClientName);
		return QString("\"" + javaDir.absolutePath() + "/" + javaExec() + "\" " + fi.baseName() + " -host " + ipHost + " -user " + user + " -passw " + pass + " -location " + location + " -repository COFORM");
	}

	bool execJavaClient(const QString& procString,const QProcessEnvironment& env,QByteArray& stdOutResult) const;
public:
	RIBridgeInterface(const QString& coformJavaClient,const QString& ip,const QString& propFile,const QDir& jvDir,const QString& us,const QString& passw,const QString& loc);

	inline static QString coformClientPath() {return QString(PluginManager::getPluginDirPath() + "/CoformIO.java");}
	inline static QString resultPath() {return QString(QDir::tempPath() + "/result.txt");};
	bool execIngestionBridge(const QString& filePath,const QString& uuidFile,const QString& metaFilePath,const QString& uuidMeta,QByteArray& stdOutResult) const;
	bool execRetrieveBridge(const QString& uuidFile,const QDir& retrieveDir,QByteArray& stdOutResult) const;
	bool execQueryBridge( const QString& queryText,QByteArray& stdOutResult) const;
	static bool parseQueryResult( const QDomDocument& res,QList<QStringList*>* tb);
	static bool parseQueryResult(const QByteArray& res,QList<QStringList*>* tb);
	inline QString prefix(){return QString("BASE <http://" + ipHost + "/openrdf-sesame/repositories/COFORM/>\nPREFIX rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#>\nPREFIX rdfs: <http://www.w3.org/2000/01/rdf-schema#>\nPREFIX  xsd: <http://www.w3.org/2001/XMLSchema#>\nPREFIX  owl: <http://www.w3.org/2002/07/owl#>\nPREFIX  crm: <http://www.ics.forth.gr/isl/rdfs/3D-COFORM_CIDOC-CRM.rdfs#>\nPREFIX  crmdig:	<http://www.ics.forth.gr/isl/rdfs/3D-COFORM_CRMdig.rdfs#>\nPREFIX  protons: <http://proton.semanticweb.org/2005/04/protons#>\nPREFIX  protont: <http://proton.semanticweb.org/2005/04/protont#>\n");}
	static inline QRegExp sparqlExp() {return QRegExp("<\\?xml version='1\\.0'\\?>.+</sparql>");}
	static QString uuidFromFileName(const QString& name);
	static QString uuidString(const QUuid& uuid);
};

class CoformGUI : public QDialog
{
	Q_OBJECT
public:
	CoformGUI(MeshDocument* mdoc);
	~CoformGUI();

	Ui::CoformDialog* dialog;

	
	QString user;
	QString pass;
	QString location;

	QString ip;
	QString propFilePath;
	QDir downloadDir;

	QString ingFile;
	QString uuid;

	QString authName;
	QString authSur;
	QString authUuid;

	QString authorUuid;

	QList<QPair<QString,QString>> authors;
	MeshDocument* md;

public slots:
	void openPropertiesFile();
	void saveSettings();
	void loadSettings();
	void uploadFile();
	void generateNewUuid();
	void downloadDirSelection();
	void retrieveFile();
	void searchAuthor();
	void ingestFile();
	void selectedAuthor();
	void getFileList(const QString& ext);
	void javaDir();

private:
	void collectConnectionInfo();
	inline QString settingsPrefix() {return tr("MeshLab::3DCoformPlugin::");}
	void updateFiles(const QStringList& ls);
	void updateTable(QTableWidget* wid,const QList< QStringList* >& tb);
	QString log;
	QString authInfo;
	QString defaultMeshPath;
};

#endif