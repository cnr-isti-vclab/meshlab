#include "coform_interface.h"
#include "ui_coform_interface.h"
#include <QFileDialog>
#include <QSettings>
#include <QUuid>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include "metadatafile_generator.h"
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/import_ply.h>
#include <vcg/complex/algorithms/create/platonic.h>
#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/normal.h>

CoformGUI::CoformGUI(MeshDocument* mdoc)
:QDialog(),authorUuid(),defaultMeshPath(),md(mdoc)
{
	if ((md != NULL) && (md->mm() != NULL))
		defaultMeshPath = md->mm()->fullName();
	dialog = new Ui::CoformDialog();
	dialog->setupUi(this);
	dialog->filterBox->addItem(QString());
	dialog->filterBox->addItem(QString("*.ply"));
	dialog->filterBox->addItem(QString("*.zip"));
	connect(dialog->riPropFileButton,SIGNAL(clicked()),this,SLOT(openPropertiesFile()));
	connect(dialog->javaDirButton,SIGNAL(clicked()),this,SLOT(javaDir()));
	connect(dialog->saveButton,SIGNAL(clicked()),this,SLOT(saveSettings()));
	//connect(dialog->uploadButton,SIGNAL(clicked()),this,SLOT(uploadFile()));
	connect(dialog->downloadDirButton,SIGNAL(clicked()),this,SLOT(downloadDirSelection()));
	connect(dialog->searchButton,SIGNAL(clicked()),this,SLOT(searchAuthor()));
	connect(dialog->ingestButton,SIGNAL(clicked()),this,SLOT(ingestFile()));
	connect(dialog->authorTable,SIGNAL(itemSelectionChanged()),this,SLOT(selectedAuthor()));
	connect(dialog->retrieveButton,SIGNAL(clicked()),this,SLOT(retrieveFile()));
	connect(dialog->filterBox,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(getFileList(const QString&)));
	connect(this,SIGNAL(accepted()),this,SLOT(close()));
	
	QSettings set;
	int index = set.allKeys().indexOf(QRegExp(settingsPrefix() + "\\w*"));
	if (index >= 0)
		loadSettings();
	
	dialog->uploadFileLine->setText(defaultMeshPath);
}

CoformGUI::~CoformGUI()
{
	delete dialog;
}

void CoformGUI::openPropertiesFile()
{
	dialog->riPropFileLine->setText(QFileDialog::getOpenFileName(this,tr("3D-COFORM Properties File"),QDir::current().absolutePath(),tr("*.properties")));
}

void CoformGUI::saveSettings()
{
	QSettings set;
	set.setValue(settingsPrefix() + tr("User"),dialog->userLine->text());
	set.setValue(settingsPrefix() + tr("Password"),dialog->passwordLine->text());
	set.setValue(settingsPrefix() + tr("Location"),dialog->locationLine->text());
	set.setValue(settingsPrefix() + tr("IP"),dialog->ipLine->text());
	set.setValue(settingsPrefix() + tr("RIPropFilePath"),dialog->riPropFileLine->text());
	set.setValue(settingsPrefix() + tr("JavaBinPath"),dialog->javaDirLine->text());

}

void CoformGUI::loadSettings()
{
	QSettings set;
	dialog->userLine->setText(set.value(settingsPrefix() + tr("User")).toString());
	dialog->passwordLine->setText(set.value(settingsPrefix() + tr("Password")).toString());
	dialog->locationLine->setText(set.value(settingsPrefix() + tr("Location")).toString());
	dialog->ipLine->setText(set.value(settingsPrefix() + tr("IP")).toString());
	dialog->riPropFileLine->setText(set.value(settingsPrefix() + tr("RIPropFilePath")).toString());
	dialog->javaDirLine->setText(set.value(settingsPrefix() + tr("JavaBinPath")).toString());
}

void CoformGUI::uploadFile()
{
	dialog->uploadFileLine->setText(QFileDialog::getOpenFileName(this,tr("3D-COFORM Properties File"),QDir::current().absolutePath(),tr("*.ply")));
}

void CoformGUI::generateNewUuid()
{

}

void CoformGUI::downloadDirSelection()
{
	QString res = QFileDialog::getExistingDirectory(this,"Download Directory");
	dialog->downloadDir->setText(res);
}

void CoformGUI::updateTable(QTableWidget* wid,const QList< QStringList* >& tb)
{
	if (tb.size() > 0)
	{
		wid->setColumnCount(tb[0]->size());
		wid->setRowCount(tb.size() - 1);
		for(int ii = 1; ii < tb.size();++ii)
			for(int jj = 0;jj < tb[ii]->size();++jj)
			{
				QTableWidgetItem* it = new QTableWidgetItem(tb[ii]->at(jj));
				wid->setItem(ii - 1,jj,it);
			}
			QHeaderView* view = new QHeaderView(Qt::Horizontal,wid);
			view->setResizeMode(QHeaderView::ResizeToContents);
			//for(int hh = 0;hh < tb[0]->size();++hh)
			wid->setSelectionBehavior(QAbstractItemView::SelectRows);
			wid->setHorizontalHeader(view);
			wid->setHorizontalHeaderLabels(*tb[0]);
	}
}

void CoformGUI::searchAuthor()
{
	QDir jav(dialog->javaDirLine->text());
	RIBridgeInterface ri(RIBridgeInterface::coformClientPath(),dialog->ipLine->text(),dialog->riPropFileLine->text(),jav,dialog->userLine->text(),dialog->passwordLine->text(),dialog->locationLine->text());
	
	QString queryText = QString("\"" + ri.prefix() + "SELECT DISTINCT ?uuid ?member WHERE{?uuid rdf:type crm:E21.Person. ?node crm:P131B.identifies ?uuid. ?node crmdig:L51F.has_first_name '" + dialog->authNameLine->text() +"'@en. ?node crmdig:L52F.has_last_name '" +  dialog->authSurLine->text() + "'@en. ?uuid crm:P107B.is_current_or_former_member_of ?member}\"");
	//QString queryText = QString("\"SELECT DISTINCT ?uuid ?member WHERE{ graph ?MR{?uuid rdf:type crm:E21.Person. ?node crm:P131B.identifies ?uuid. ?node crmdig:L51F.has_first_name '" + dialog->authNameLine->text() +"'@en. ?node crmdig:L52F.has_last_name '" +  dialog->authSurLine->text() + "'@en. ?uuid crm:P107B.is_current_or_former_member_of ?member.}}\"");
	//QString queryText = QString("\"SELECT DISTINCT * WHERE{ graph <uuid:aeac5200-0138-11e0-a976-0800200c9a66>}\"");

	QByteArray result;
	bool rr = ri.execQueryBridge(queryText,result);
	if (rr)
	{
		QList< QStringList* > tb;
		bool done = RIBridgeInterface::parseQueryResult(result,&tb);
		if (done)
			updateTable(dialog->authorTable,tb);

/*		foreach(QString resString,ll)
			dialog->uuidList->addItem(resString);*/		
	}
	dialog->logViewer->append(QString(result));
}



void CoformGUI::ingestFile()
{
	QDir javaD(dialog->javaDirLine->text());
	RIBridgeInterface bridge(PluginManager::getPluginDirPath() + "/CoformIO.java",dialog->ipLine->text(),dialog->riPropFileLine->text(),javaD,dialog->userLine->text(),dialog->passwordLine->text(),dialog->locationLine->text());
	if (md->mm() == NULL)
		return;
	int firstId = md->mm()->id();
	int ii = 0; 
	QList<MetaDataFileGenerator::MeshInfo> mil;
	while(ii < md->size())
	{
		md->advanceCurrentMesh(1);
		if (md->mm() != NULL)
		{
			QString uuidNum(RIBridgeInterface::uuidFromFileName(md->mm()->fullName()));
			if (!uuidNum.isEmpty())
			{
				uuidNum.push_front("uuid:");
				QFileInfo info(md->mm()->fullName());
				MetaDataFileGenerator::MeshInfo inf(info.fileName(),uuidNum,md->mm()->cm.fn,md->mm()->cm.vn,(int) info.size());
				mil.append(inf);
			}
			//if (!uuidName.isEmpty());
		}
		++ii;
	}
	md->setCurrentMesh(firstId);
	//created layer not saved
	bool tmpsaved = false;
	if (md->mm()->fullName().isEmpty())
	{
		QString tmp = QDir::tempPath() + "/" + md->mm()->label();
		tmpsaved= vcg::tri::io::ExporterPLY<CMeshO>::Save(md->mm()->cm,qPrintable(tmp),true);
	}
	QFileInfo in(md->mm()->fullName());
	QString outFileUuid = RIBridgeInterface::uuidString(QUuid::createUuid());
	MetaDataFileGenerator::MeshInfo outInfo(in.fileName(),RIBridgeInterface::uuidString(outFileUuid),md->mm()->cm.fn,md->mm()->cm.vn,(int) in.size());
	if (dialog->authorTable->selectedItems().size() > 0)
	{
		QTableWidgetItem* pitem = dialog->authorTable->item(dialog->authorTable->currentRow(),0);
		MetaDataFileGenerator::Person pers(pitem->text(),dialog->authNameLine->text(),dialog->authSurLine->text());
		QString procUuid = RIBridgeInterface::uuidString(QUuid::createUuid());
		MetaDataFileGenerator::Process proc(dialog->commentLine->text(),procUuid,mil,outInfo,"MeshLab_130a.exe",md->filterHistory.xmlDoc().toString());
		QString rdf = MetaDataFileGenerator::generateRDF(proc,pers);
		QString rdfPath(in.absolutePath() + "/" + in.baseName() + ".rdf");
		QFile file(rdfPath);
		file.open(QIODevice::WriteOnly);
		QTextStream stream(&file);
		stream << rdf << "\n";
		file.close();
		QByteArray result;
		bridge.execIngestionBridge("\"" + dialog->uploadFileLine->text() + "\"",outFileUuid,"\"" + rdfPath + "\"",procUuid,result);
		dialog->logViewer->append(QString(result));
	}
	MetaDataFileGenerator::Person pers("uuid:5","Guido","Ranzuglia");
	QString procUuid = RIBridgeInterface::uuidString(QUuid::createUuid());
	MetaDataFileGenerator::Process proc(dialog->commentLine->text(),procUuid,mil,outInfo,"MeshLab_130a.exe",md->filterHistory.xmlDoc().toString());
	QString rdf = MetaDataFileGenerator::generateRDF(proc,pers);
	QString rdfPath(in.absolutePath() + "/" + in.baseName() + ".rdf");
	QFile file(rdfPath);
	file.open(QIODevice::WriteOnly);
	QTextStream stream(&file);
	stream << rdf << "\n";
	file.close();
	QByteArray result;
	bridge.execIngestionBridge("\"" + dialog->uploadFileLine->text() + "\"",outFileUuid,"\"" + rdfPath + "\"",procUuid,result);
	dialog->logViewer->append(QString(result));
}

void CoformGUI::retrieveFile()
{
	QDir javaD(dialog->javaDirLine->text());
	RIBridgeInterface bridge(PluginManager::getPluginDirPath() + "/CoformIO.java",dialog->ipLine->text(),dialog->riPropFileLine->text(),javaD,dialog->userLine->text(),dialog->passwordLine->text(),dialog->locationLine->text());
	QByteArray result;
	if (dialog->filesTable->rowCount() > 0)
	{
		bool rr = bridge.execRetrieveBridge(dialog->filesTable->item(dialog->filesTable->currentRow(),0)->text(),dialog->downloadDir->text(),result);
		dialog->logViewer->append(result);
		QString fileName = dialog->downloadDir->text() + "/" + dialog->filesTable->item(dialog->filesTable->currentRow(),1)->text();
		QFileInfo ff(fileName);
		MeshModel* mm = md->addNewMesh(ff.absoluteFilePath(),ff.baseName());
		mm->clearDataMask(0);
		int mask = 0;
		vcg::tri::io::ImporterPLY<CMeshO>::LoadMask(qPrintable(ff.absoluteFilePath()), mask); 
		mm->Enable(mask);
		mm->updateDataMask(mask);
		vcg::tri::io::ImporterPLY<CMeshO>::Open(mm->cm,qPrintable(ff.absoluteFilePath()),mask);
		vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
		if( mask & vcg::tri::io::Mask::IOM_VERTNORMAL)
			vcg::tri::UpdateNormals<CMeshO>::PerFace(mm->cm);
		else
			vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(mm->cm);
	}

	
//vcg::tri::Tetrahedron<CMeshO>(mm->cm);
	emit accepted();
}

void CoformGUI::collectConnectionInfo()
{
	user = dialog->userLine->text();
	pass = dialog->passwordLine->text();
	location = dialog->locationLine->text();
	ip = dialog->ipLine->text();
	propFilePath = dialog->riPropFileLine->text();
}

void CoformGUI::javaDir()
{
	dialog->javaDirLine->setText(QFileDialog::getExistingDirectory(this,tr("Java Bin Directory")));
}

void CoformGUI::selectedAuthor()
{
	authorUuid = dialog->authorTable->currentItem()->text();
}

void CoformGUI::updateFiles(const QStringList& ls)
{

}

void CoformGUI::getFileList( const QString& ext )
{
	if (ext.isEmpty())
		return;
	QString ex= ext;
	ex.remove(QRegExp("\\*\\."));
	QString put = dialog->javaDirLine->text();
	QDir jav(dialog->javaDirLine->text());
	RIBridgeInterface ri(RIBridgeInterface::coformClientPath(),dialog->ipLine->text(),dialog->riPropFileLine->text(),jav,dialog->userLine->text(),dialog->passwordLine->text(),dialog->locationLine->text());
	QString queryText("\"" + ri.prefix() + "SELECT DISTINCT ?uuid ?name WHERE{?uuid crm:P2F.has_type <http://www.3d-coform.eu/#mimetype/" + ex +">. ?uuid rdfs:label ?name}\"");
	//queryText= QString("\"BASE    <http://localhost:8080/openrdf-sesame/repositories/COFORM/>\nPREFIX 	rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#>\nPREFIX	rdfs: <http://www.w3.org/2000/01/rdf-schema#>\nPREFIX  xsd: <http://www.w3.org/2001/XMLSchema#>\nPREFIX  owl: <http://www.w3.org/2002/07/owl#>\nPREFIX  crm: <http://www.ics.forth.gr/isl/rdfs/3D-COFORM_CIDOC-CRM.rdfs#>\nPREFIX  crmdig:	<http://www.ics.forth.gr/isl/rdfs/3D-COFORM_CRMdig.rdfs#>\nPREFIX  protons: <http://proton.semanticweb.org/2005/04/protons#>\nPREFIX  protont: <http://proton.semanticweb.org/2005/04/protont#>\nSELECT DISTINCT ?name WHERE{ graph ?MR{<uuid:4ff524a4-038e-11e0-a976-0800200c9a66> rdfs:label ?name}}\"");
	//QString queryText("\"SELECT DISTINCT ?uuid ?name WHERE{ graph ?MR{?uuid rdfs:label ?name}}\"");

	//QString queryText = QString("\"SELECT DISTINCT ?uuid ?name WHERE{ graph ?MR{?uuid rdf:type crm:E21.Person. ?node crm:P131B.identifies ?uuid. ?node crmdig:L51F.has_first_name '" + dialog->authNameLine->text() +"'@en. ?node crmdig:L52F.has_last_name '" +  dialog->authSurLine->text() + "'@en.}}\"");
	QByteArray result;
	bool rr = ri.execQueryBridge(queryText,result);
	if (rr)
	{
		QList<QStringList*> tb;
		bool done = RIBridgeInterface::parseQueryResult(result,&tb);
		if (done)
			updateTable(dialog->filesTable,tb);

		//dialog->filesTable->clear();
		//foreach(QString resString,ll)
		//{
		//	//QString fileNameQuery("\"SELECT DISTINCT ?name WHERE{<" + resString + "> rdfs:label ?name}\"");
		//	//rr = ri.execQueryBridge(fileNameQuery,result);
		//	//if (rr)
		//	dialog->filesTable->addItem(resString);
		//}
	}
	dialog->logViewer->append(QString(result));
}


RIBridgeInterface::RIBridgeInterface(const QString& coformJavaClient,const QString& ip,const QString& propFile,const QDir& jvDir,const QString& us,const QString& passw,const QString& loc)
:ipHost(ip),javaDir(jvDir),user(us),pass(passw),location(loc)
{
	QFileInfo javafi(coformJavaClient);
	coformJavaClientDir = javafi.absoluteDir();
	coformJavaClientName = javafi.absoluteFilePath();
	QFileInfo propfi(propFile);
	propFileDir = propfi.absoluteDir();
}

bool RIBridgeInterface::execJavaClient(const QString& procString,const QProcessEnvironment& env,QByteArray& stdOutResult) const
{
	QProcess proc;
	proc.setProcessChannelMode(QProcess::MergedChannels);
	//proc.setStandardOutputFile("ingestionResult.txt");
	proc.setProcessEnvironment(env);
	proc.start(procString);
	//QString tmp("\"" + javaDir.absolutePath() + "/" + javaExec() + "\"");
	//proc.start(tmp);
	bool correctlyTerminated = proc.waitForFinished(-1);
	if (!correctlyTerminated)
	{
		QProcess::ProcessError err = proc.error();
		return false;
	}
	stdOutResult = proc.readAllStandardOutput();
	//int rr = proc.exitCode();
	proc.close();
	return true;
}

bool RIBridgeInterface::execQueryBridge( const QString& queryText,QByteArray& stdOutResult) const
{
	QProcessEnvironment env = setEnv();
	QString text(createExecBasicString());
	text += QString(" -query " + queryText + "\n");
	return execJavaClient(text,env,stdOutResult);
}

bool RIBridgeInterface::execRetrieveBridge( const QString& uuidFile,const QDir& retrieveDir, QByteArray& stdOutResult ) const
{
	QProcessEnvironment env = setEnv();
	QString authQuery(createExecBasicString());
	authQuery += QString(" -retrieve " + uuidFile + " " + retrieveDir.absolutePath() + "\n");
	return execJavaClient(authQuery,env,stdOutResult);
}

bool RIBridgeInterface::execIngestionBridge( const QString& filePath,const QString& uuidFile,const QString& metaFilePath,const QString& uuidMeta, QByteArray& stdOutResult ) const
{
	QProcessEnvironment env = setEnv();
	QString procString = createExecBasicString();
	procString += QString(" -ingest " + filePath + " " + uuidFile + " " + metaFilePath + " " + uuidMeta + "\n");
	return execJavaClient(procString,env,stdOutResult);
}

//QStringList RIBridgeInterface::parseQueryResult(const QString& res) 
//{
//	//QString elemString("\\S+");
//	QRegExp elem("\\S+");
//	//QString listString(elemString + "(, " + elemString + ")*");
//	QRegExp list(elem.pattern() + "(, " + elem.pattern() + ")*");
//	//QString resExpString("\\[" + listString + "\\]");
//	QRegExp resultExp("\\[" + list.pattern() + "\\]");
//	res.indexOf(resultExp);
//	return resultExp.cap().remove(QRegExp("(\\[|\\])")).split(QRegExp(",\\s*"),QString::SkipEmptyParts);
//}

bool RIBridgeInterface::parseQueryResult( const QDomDocument& doc,QList< QStringList* >* table)
{
	QDomNodeList varTag = doc.elementsByTagName("variable");
	int col = varTag.size();
	QStringList* title = new QStringList();
	for(int ii = 0;ii < col;++ii)
		title->append(varTag.at(ii).toElement().attribute("name"));
	table->append(title);
	QDomNodeList bindTag = doc.elementsByTagName("binding");
	if ((bindTag.size() == 0) || (bindTag.size()  % col != 0))
		return false;
	QStringList* res;
	for(int jj = 0;jj < bindTag.size();++jj)
	{
		QDomNode nd = bindTag.at(jj);
		while(nd.isElement())
			nd = nd.firstChild();
		if (jj % col == 0)
			res = new QStringList();
		res->append(nd.nodeValue());
		if (jj % col == 0)
			table->append(res);
	}
	return true;
}

bool RIBridgeInterface::parseQueryResult( const QByteArray& res,QList<QStringList*>* table )
{
	QRegExp xmlExp = RIBridgeInterface::sparqlExp();
	QString re(res);
	re.indexOf(xmlExp);
	QDomDocument doc;
	doc.setContent(xmlExp.cap());
	return RIBridgeInterface::parseQueryResult(doc,table);
}

//bool RIBridgeInterface::compile() const
//{
//	QProcessEnvironment env = setEnv();
//	
//	QStringList params;
//	params << coformClientPath() << QString("-d") << QDir::tempPath();
//	QProcess javac;
//	javac.setProcessEnvironment(env);
//	QStringList rs = env.toStringList();
//	javac.setStandardOutputFile(QDir::tempPath() + "/porcaccia.txt");
//	javac.start(QString(javaDir.absolutePath() + "/javac"),params);
//	bool res = javac.waitForFinished();
//	return res;
//}

QProcessEnvironment RIBridgeInterface::setEnv() const
{
	QProcessEnvironment env;
	QProcessEnvironment sysEnv = QProcessEnvironment::systemEnvironment();
	//QString st = javaDir.absolutePath();
	env.insert(QString("JAVAPATH"),QString(javaDir.absolutePath()));

	QString absPath = propFileDir.absolutePath();
	env.insert(QString("COFORM_CONFIG_DIR"),absPath);
	env.insert(QString("VM_URL"),ipHost);
	QString libspath(absPath + "/COFORM_lib/antlr-2.7.6.jar;" + absPath + "/COFORM_LIB/commons-codec-1.3.jar;" + absPath + "../COFORM_lib/OR_libs/webserviceclienttest.jar;" + absPath + "COFORM_LIB/commons-collections-3.1.jar;" + absPath + "/COFORM_LIB/commons-dbcp-1.2.2.jar;" + absPath + "/COFORM_LIB/commons-httpclient-3.1.jar;" + absPath + "/COFORM_LIB/commons-logging-1.1.1.jar;" + absPath + "/COFORM_LIB/commons-pool-1.3.jar;" + absPath + "/COFORM_LIB/dom4j-1.6.1.jar;" + absPath + "/COFORM_LIB/hibernate3.jar;" + absPath + "/COFORM_LIB/javassist-3.9.0.GA.jar;" + absPath + "/COFORM_LIB/jta-1.1.jar;" + absPath + "/COFORM_LIB/log4j-1.2.15.jar;" + absPath + "/COFORM_LIB/log4j-over-slf4j-1.5.0.jar;" + absPath + "/COFORM_LIB/mysql-connector-java-5.1.10-bin.jar;" + absPath + "/COFORM_LIB/openrdf-sesame-2.3.2-onejar.jar;" + absPath + "/COFORM_LIB/owlim-3.3.jar;" + absPath + "/COFORM_LIB/simple-xml-2.3.3(12-05-2010 1816).jar;" + absPath + "/COFORM_LIB/jcl104-over-slf4j-1.5.0.jar;" + absPath + "/COFORM_LIB/slf4j-api-1.5.0.jar;" + absPath + "/COFORM_LIB/slf4j-api-1.5.6.jar;" + absPath + "/COFORM_LIB/slf4j-api-1.5.8.jar;" + absPath + "/COFORM_LIB/slf4j-jdk14-1.5.0.jar;" + absPath + "/COFORM_LIB/slf4j-nop-1.5.2.jar;" + absPath + "/COFORM_LIB/slf4j-simple-1.5.0.jar;" + absPath + "/COFORM_LIB/slf4j-log4j12-1.5.6.jar;" + absPath + "/COFORM_LIB/trree-3.3.jar;" + absPath + "/COFORM_lib/OR_libs/SOAPServerSVNstub.jar;" + absPath + "/COFORM_LIB/OR_libs/originalwebserviceclienttest.jar;" + absPath + "/COFORM_LIB/OR_libs/upto2.2.5awebserviceclienttest.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/activation-1.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/annogen-0.1.0.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axiom-api-1.2.7.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axiom-dom-1.2.7.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axiom-impl-1.2.7.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-adb-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-adb-codegen-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-ant-plugin-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-clustering-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-jibx-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-codegen-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-corba-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-fastinfoset-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-java2wsdl-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-jaxbri-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-jaxws-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-jaxws-api-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-json-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-jws-api-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-kernel-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-metadata-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-mtompolicy-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-saaj-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-saaj-api-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/commons-io-1.4.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-spring-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-xmlbeans-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/backport-util-concurrent-3.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/commons-codec-1.3.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/commons-fileupload-1.2.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/commons-httpclient-3.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/commons-logging-1.1.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/geronimo-annotation_1.0_spec-1.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/geronimo-stax-api_1.0_spec-1.0.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/httpcore-4.0-beta1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/httpcore-nio-4.0-beta1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jalopy-1.5rc3.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jaxb-api-2.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jaxb-impl-2.1.6.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jaxb-xjc-2.1.6.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jaxen-1.1.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jettison-1.0-RC2.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jibx-bind-1.1.5.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jibx-run-1.1.5.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/log4j-1.2.15.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/mail-1.4.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/mex-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/neethi-2.0.4.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/soapmonitor-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/woden-api-1.0M8.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/woden-impl-dom-1.0M8.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/wsdl4j-1.6.2.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/wstx-asl-3.2.4.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/xalan-2.7.0.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/xercesImpl-2.8.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/xml-apis-1.3.04.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/xml-resolver-1.2.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/xmlbeans-2.3.0.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/XmlSchema-1.4.2.jar");
	env.insert(QString("LIBS_PATH"),libspath);
	//absPath + "/COFORM_lib/antlr-2.7.6.jar;" + absPath + "/COFORM_LIB/commons-codec-1.3.jar;" + absPath + "COFORM_LIB/commons-collections-3.1.jar;" + absPath + "/COFORM_LIB/commons-dbcp-1.2.2.jar;" + absPath + "/COFORM_LIB/commons-httpclient-3.1.jar;" + absPath + "/COFORM_LIB/commons-logging-1.1.1.jar;" + absPath + "/COFORM_LIB/commons-pool-1.3.jar;" + absPath + "/COFORM_LIB/dom4j-1.6.1.jar;" + absPath + "/COFORM_LIB/hibernate3.jar;" + absPath + "/COFORM_LIB/javassist-3.9.0.GA.jar;" + absPath + "/COFORM_LIB/jta-1.1.jar;" + absPath + "/COFORM_LIB/log4j-1.2.15.jar;" + absPath + "/COFORM_LIB/log4j-over-slf4j-1.5.0.jar;" + absPath + "/COFORM_LIB/mysql-connector-java-5.1.10-bin.jar;" + absPath + "/COFORM_LIB/openrdf-sesame-2.3.2-onejar.jar;" + absPath + "/COFORM_LIB/owlim-3.3.jar;" + absPath + "/COFORM_LIB/simple-xml-2.3.3(12-05-2010 1816).jar;" + absPath + "/COFORM_LIB/jcl104-over-slf4j-1.5.0.jar;" + absPath + "/COFORM_LIB/slf4j-api-1.5.0.jar;" + absPath + "/COFORM_LIB/slf4j-api-1.5.6.jar;" + absPath + "/COFORM_LIB/slf4j-api-1.5.8.jar;" + absPath + "/COFORM_LIB/slf4j-jdk14-1.5.0.jar;" + absPath + "/COFORM_LIB/slf4j-nop-1.5.2.jar;" + absPath + "/COFORM_LIB/slf4j-simple-1.5.0.jar;" + absPath + "/COFORM_LIB/slf4j-log4j12-1.5.6.jar;" + absPath + "/COFORM_LIB/trree-3.3.jar;" + absPath + "/COFORM_LIB/OR_libs/webserviceclienttest.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/activation-1.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/annogen-0.1.0.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axiom-api-1.2.7.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axiom-dom-1.2.7.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axiom-impl-1.2.7.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-adb-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-adb-codegen-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-ant-plugin-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-clustering-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-jibx-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-codegen-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-corba-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-fastinfoset-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-java2wsdl-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-jaxbri-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-jaxws-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-jaxws-api-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-json-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-jws-api-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-kernel-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-metadata-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-mtompolicy-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-saaj-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-saaj-api-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/commons-io-1.4.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-spring-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-xmlbeans-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/backport-util-concurrent-3.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/commons-codec-1.3.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/commons-fileupload-1.2.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/commons-httpclient-3.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/commons-logging-1.1.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/geronimo-annotation_1.0_spec-1.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/geronimo-stax-api_1.0_spec-1.0.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/httpcore-4.0-beta1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/httpcore-nio-4.0-beta1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jalopy-1.5rc3.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jaxb-api-2.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jaxb-impl-2.1.6.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jaxb-xjc-2.1.6.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jaxen-1.1.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jettison-1.0-RC2.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jibx-bind-1.1.5.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jibx-run-1.1.5.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/log4j-1.2.15.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/mail-1.4.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/mex-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/neethi-2.0.4.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/soapmonitor-1.4.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/woden-api-1.0M8.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/woden-impl-dom-1.0M8.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/wsdl4j-1.6.2.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/wstx-asl-3.2.4.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/xalan-2.7.0.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/xercesImpl-2.8.1.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/xml-apis-1.3.04.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/xml-resolver-1.2.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/xmlbeans-2.3.0.jar;" + absPath + "/COFORM_LIB/OR_libs/axis2-1.4.1-libs/XmlSchema-1.4.2.jar";*/
	QString path("libs;" + env.value("JAVAPATH") + ";" + sysEnv.value("PATH"));
	env.insert(QString("PATH"),path);
	QString classpath(PluginManager::getPluginDirPath() + ";" + absPath + "/COFORM-2.2.5.jar;" + env.value("LIBS_PATH"));
	env.insert(QString("CLASSPATH"),classpath);
	return env;


	/*env.insert(QString("PATH"),QString("libs;" + env.value(QString("JAVAPATH")) + env.value(QString("PATH"))));
	env.insert(QString("CLASSPATH"),QString(".;" + env.value("COFORM_CONFIG_DIR") + "/COFORM-2.2.2.jar;" + env.value(QString("LIBS_PATH"))));
	QString text;
	text += QString("set JAVAPATH=\"" + javaDir.absolutePath() + "\"\n");
	text += QString("set COFORM_CONFIG_DIR=\"" + propFileDir.absolutePath() + "\"\n");
	text += QString("set VM_URL=" + ipHost + "\n");
	text += QString("set LIBS_PATH=%COFORM_CONFIG_DIR%/COFORM_lib/antlr-2.7.6.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/commons-codec-1.3.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/commons-collections-3.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/commons-dbcp-1.2.2.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/commons-httpclient-3.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/commons-logging-1.1.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/commons-pool-1.3.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/dom4j-1.6.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/hibernate3.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/javassist-3.9.0.GA.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/jta-1.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/log4j-1.2.15.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/log4j-over-slf4j-1.5.0.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/mysql-connector-java-5.1.10-bin.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/openrdf-sesame-2.3.2-onejar.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/owlim-3.3.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/simple-xml-2.3.3(12-05-2010 1816).jar;%COFORM_CONFIG_DIR%/COFORM_LIB/jcl104-over-slf4j-1.5.0.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/slf4j-api-1.5.0.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/slf4j-api-1.5.6.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/slf4j-api-1.5.8.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/slf4j-jdk14-1.5.0.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/slf4j-nop-1.5.2.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/slf4j-simple-1.5.0.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/slf4j-log4j12-1.5.6.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/trree-3.3.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/../COFORM_lib/OR_libs/SOAPServerSVNstub.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/activation-1.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/annogen-0.1.0.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axiom-api-1.2.7.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axiom-dom-1.2.7.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axiom-impl-1.2.7.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-adb-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-adb-codegen-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-ant-plugin-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-clustering-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-jibx-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-codegen-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-corba-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-fastinfoset-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-java2wsdl-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-jaxbri-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-jaxws-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-jaxws-api-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-json-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-jws-api-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-kernel-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-metadata-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-mtompolicy-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-saaj-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-saaj-api-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/commons-io-1.4.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-spring-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/axis2-xmlbeans-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/backport-util-concurrent-3.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/commons-codec-1.3.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/commons-fileupload-1.2.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/commons-httpclient-3.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/commons-logging-1.1.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/geronimo-annotation_1.0_spec-1.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/geronimo-stax-api_1.0_spec-1.0.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/httpcore-4.0-beta1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/httpcore-nio-4.0-beta1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jalopy-1.5rc3.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jaxb-api-2.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jaxb-impl-2.1.6.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jaxb-xjc-2.1.6.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jaxen-1.1.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jettison-1.0-RC2.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jibx-bind-1.1.5.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/jibx-run-1.1.5.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/log4j-1.2.15.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/mail-1.4.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/mex-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/neethi-2.0.4.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/soapmonitor-1.4.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/woden-api-1.0M8.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/woden-impl-dom-1.0M8.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/wsdl4j-1.6.2.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/wstx-asl-3.2.4.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/xalan-2.7.0.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/xercesImpl-2.8.1.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/xml-apis-1.3.04.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/xml-resolver-1.2.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/xmlbeans-2.3.0.jar;%COFORM_CONFIG_DIR%/COFORM_LIB/OR_libs/axis2-1.4.1-libs/XmlSchema-1.4.2.jar");
	text += QString("set PATH=libs;%JAVAPATH%;%PATH%\n");
	text += QString("set CLASSPATH=.;%COFORM_CONFIG_DIR%/COFORM-2.2.2.jar;%LIBS_PATH%\n");
	text += QString("javac \"" + coformJavaClientName + "\" -d \"" + QDir::tempPath() + "\"\n");*/
	//return text;
}

QString RIBridgeInterface::uuidFromFileName( const QString& name )
{
	QRegExp uuidExp("_\\w{8}-\\w{4}-\\w{4}-\\w{4}-\\w{12}\\.");
	name.indexOf(uuidExp);
	return uuidExp.cap().remove(QRegExp("(_|\\.)"));
}

QString RIBridgeInterface::uuidString(const QUuid& uuid)
{
	QString clean = uuid.toString().remove(QRegExp("(\\{|\\})"));
	clean.push_front("uuid:");
	return clean;
}