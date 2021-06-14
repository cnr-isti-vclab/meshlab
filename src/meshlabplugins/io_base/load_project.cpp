#include "load_project.h"

#include <QDir>

#include <wrap/io_trimesh/alnParser.h>
#include <wrap/io_trimesh/import_out.h>
#include <wrap/io_trimesh/import_nvm.h>
#include <wrap/qt/shot_qt.h>

#include <common/ml_document/mesh_document.h>
#include <common/utilities/load_save.h>

std::vector<MeshModel*> loadALN(
		const QString& filename,
		MeshDocument& md,
		vcg::CallBackPos* cb)
{
	std::vector<MeshModel*> meshList;
	std::vector<RangeMap> rmv;
	int retVal = ALNParser::ParseALN(rmv, qUtf8Printable(filename));
	if(retVal != ALNParser::NoError) {
		throw MLException("Unable to open ALN file");
	}
	QFileInfo fi(filename);
	QString curr_path = QDir::currentPath();
	QDir::setCurrent(fi.absolutePath());

	for(const RangeMap& rm : rmv) {
		QString relativeToProj = fi.absoluteDir().absolutePath() + "/" + rm.filename.c_str();
		try {
			std::list<MeshModel*> tmp =
					meshlab::loadMeshWithStandardParameters(relativeToProj, md, cb);
			md.mm()->cm.Tr.Import(rm.transformation);
			meshList.insert(meshList.end(), tmp.begin(), tmp.end());
		}
		catch (const MLException& e){
			for (MeshModel* m : meshList)
				md.delMesh(m);
			throw e;
		}
	}
	QDir::setCurrent(curr_path);
	return meshList;
}

std::vector<MeshModel*> loadOUT(
		const QString& filename,
		const QString& imageListFile,
		MeshDocument& md,
		std::vector<std::string>& unloadedImgList,
		vcg::CallBackPos*)
{
	std::vector<MeshModel*> meshList;
	unloadedImgList.clear();
	QFileInfo fi(filename);

	MeshModel* newMesh = md.addNewMesh("", fi.baseName());
	std::vector<Shotm> shots;
	const QString path_im = QFileInfo(imageListFile).absolutePath()+QString("/");

	std::vector<std::string> image_filenames;
	vcg::tri::io::ImporterOUT<CMeshO>::Open(newMesh->cm, shots, image_filenames, qUtf8Printable(filename), qUtf8Printable(imageListFile));
	newMesh->updateDataMask(MeshModel::MM_VERTCOLOR);

	QString curr_path = QDir::currentPath();
	QFileInfo imi(imageListFile);
	QDir::setCurrent(imi.absoluteDir().absolutePath());
	//
	QStringList image_filenames_q;
	for(unsigned int i  = 0; i < image_filenames.size(); ++i)
	{
		QImageReader sizeImg(QString::fromStdString(image_filenames[i]));
		if(sizeImg.size()==QSize(-1,-1))
			image_filenames_q.push_back(path_im+QString::fromStdString(image_filenames[i]));
		else
			image_filenames_q.push_back(QString::fromStdString(image_filenames[i]));
	}


	for(size_t i=0 ; i<shots.size() ; i++)
	{
		md.addNewRaster();
		const QString fullpath_image_filename = image_filenames_q[int(i)];

		QImage img(":/img/dummy.png");
		try {
			img = meshlab::loadImage(fullpath_image_filename);
		}
		catch(const MLException& e){
			unloadedImgList.push_back(fullpath_image_filename.toStdString());
		}

		md.rm()->addPlane(new RasterPlane(img, fullpath_image_filename, RasterPlane::RGBA));
		int count=fullpath_image_filename.count('\\');
		if (count==0)
		{
			count=fullpath_image_filename.count('/');
			md.rm()->setLabel(fullpath_image_filename.section('/',count,1));
		}
		else
			md.rm()->setLabel(fullpath_image_filename.section('\\',count,1));
		md.rm()->shot = shots[i];
	}
	QDir::setCurrent(curr_path);

	meshList.push_back(newMesh);

	return meshList;
}

std::vector<MeshModel*> loadNVM(
		const QString& filename,
		MeshDocument& md,
		std::vector<std::string>& unloadedImgList,
		vcg::CallBackPos*)
{
	std::vector<MeshModel*> meshList;
	unloadedImgList.clear();

	QFileInfo fi(filename);

	MeshModel* newMesh = md.addNewMesh(fi.baseName(),QString("model"));
	std::vector<Shotm> shots;

	std::vector<std::string> image_filenames;
	vcg::tri::io::ImporterNVM<CMeshO>::Open(md.mm()->cm,shots,image_filenames, qUtf8Printable(filename));
	md.mm()->updateDataMask(MeshModel::MM_VERTCOLOR);

	QString curr_path = QDir::currentPath();

	QDir::setCurrent(fi.absolutePath());
	QStringList image_filenames_q;
	for(size_t i  = 0; i < image_filenames.size(); ++i)
		image_filenames_q.push_back(QString::fromStdString(image_filenames[int(i)]));

	for(size_t i=0 ; i<shots.size() ; i++){
		md.addNewRaster();
		const QString fullpath_image_filename = image_filenames_q[int(i)];
		QImage img(":/img/dummy.png");
		try {
			img = meshlab::loadImage(fullpath_image_filename);
		}
		catch(const MLException& e){
			unloadedImgList.push_back(fullpath_image_filename.toStdString());
		}

		md.rm()->addPlane(new RasterPlane(fullpath_image_filename,RasterPlane::RGBA));
		md.rm()->setLabel(image_filenames_q[int(i)].section('/',1,2));
		md.rm()->shot = shots[int(i)];
	}
	QDir::setCurrent(curr_path);

	meshList.push_back(newMesh);

	return meshList;
}

std::vector<MeshModel*> loadMLP(
		const QString& filename,
		MeshDocument& md,
		std::vector<MLRenderingData>& rendOpt,
		std::vector<std::string>& unloadedImgList,
		vcg::CallBackPos*)
{
	std::vector<MeshModel*> meshList;
	unloadedImgList.clear();

	QFile qf(filename);
	QFileInfo qfInfo(filename);
	bool binary = (QString(qfInfo.suffix()).toLower() == "mlb");

	if (!qf.open(QIODevice::ReadOnly))
		throw MLException("File not found.");

	QDomDocument doc("MeshLabDocument");    //It represents the XML document

	if (!doc.setContent(&qf))
		throw MLException(filename + " is not a MeshLab project.");

	QDomElement root = doc.documentElement();

	QDomNode node;

	node = root.firstChild();

	QDir tmpDir = QDir::current();
	QDir::setCurrent(qfInfo.absoluteDir().absolutePath());
	//Devices
	while (!node.isNull()) {
		if (QString::compare(node.nodeName(), "MeshGroup") == 0) {
			QDomNode mesh;
			QString filen, label;
			mesh = node.firstChild();
			while (!mesh.isNull()) {
				//return true;
				filen = mesh.attributes().namedItem("filename").nodeValue();
				label = mesh.attributes().namedItem("label").nodeValue();
				bool visible = true;
				if (mesh.attributes().contains("visible"))
					visible = (mesh.attributes().namedItem("visible").nodeValue().toInt() == 1);
				//MeshModel* mm = md.addNewMesh(filen, label);

				//mm->visible = visible;

				int idInFile = -1;
				if (mesh.attributes().contains("idInFile")){
					idInFile = mesh.attributes().namedItem("idInFile").nodeValue().toInt();
				}
				if (idInFile <= 0){
					//load the file just if it is the first layer contained
					//in the file (or it is the only one)
					try {
						auto tmp = meshlab::loadMeshWithStandardParameters(filen, md);
						for (auto m : tmp){
							m->setVisible(visible);
							m->setLabel(label);
						}
						meshList.insert(meshList.end(), tmp.begin(), tmp.end());
					}
					catch(const MLException& e) {
						for (MeshModel* mm : meshList)
							md.delMesh(mm);
						throw MLException(filen + " mesh file not found.");
					}
				}

				QDomNode tr = mesh.firstChildElement("MLMatrix44");

				if (!tr.isNull()) {
					vcg::Matrix44f trm;
					if (tr.childNodes().size() == 1) {
						if (!binary) {
							QStringList rows = tr.firstChild().nodeValue().split("\n", QString::SkipEmptyParts);
							int i = 0;
							for (const QString& row : qAsConst(rows)){
								if (rows.size() > 0) {
									QStringList values = row.split(" ", QString::SkipEmptyParts);
									int j = 0;
									for (const QString& value : qAsConst(values)) {
										if (i < 4 && j < 4) {
											md.mm()->cm.Tr[i][j] = value.toFloat();
											j++;
										}
									}
									i++;
								}
							}
						}
						else {
							QString str = tr.firstChild().nodeValue();
							QByteArray value = QByteArray::fromBase64(str.toLocal8Bit());
							memcpy(md.mm()->cm.Tr.V(), value.data(), sizeof(Matrix44m::ScalarType) * 16);
						}
					}
				}

				QDomNode renderingOpt = mesh.firstChildElement("RenderingOption");
				if (!renderingOpt.isNull())
				{
					QString value = renderingOpt.firstChild().nodeValue();
					MLRenderingData::GLOptionsType opt;
					if (renderingOpt.attributes().contains("pointSize"))
						opt._perpoint_pointsize = renderingOpt.attributes().namedItem("pointSize").nodeValue().toFloat();
					if (renderingOpt.attributes().contains("wireWidth"))
						opt._perwire_wirewidth = renderingOpt.attributes().namedItem("wireWidth").nodeValue().toFloat();
					if (renderingOpt.attributes().contains("boxColor"))
					{
						QStringList values = renderingOpt.attributes().namedItem("boxColor").nodeValue().split(" ", QString::SkipEmptyParts);
						opt._perbbox_fixed_color = vcg::Color4b(values[0].toInt(), values[1].toInt(), values[2].toInt(), values[3].toInt());
					}
					if (renderingOpt.attributes().contains("pointColor"))
					{
						QStringList values = renderingOpt.attributes().namedItem("pointColor").nodeValue().split(" ", QString::SkipEmptyParts);
						opt._perpoint_fixed_color = vcg::Color4b(values[0].toInt(), values[1].toInt(), values[2].toInt(), values[3].toInt());
					}
					if (renderingOpt.attributes().contains("wireColor"))
					{
						QStringList values = renderingOpt.attributes().namedItem("wireColor").nodeValue().split(" ", QString::SkipEmptyParts);
						opt._perwire_fixed_color = vcg::Color4b(values[0].toInt(), values[1].toInt(), values[2].toInt(), values[3].toInt());
					}
					if (renderingOpt.attributes().contains("solidColor"))
					{
						QStringList values = renderingOpt.attributes().namedItem("solidColor").nodeValue().split(" ", QString::SkipEmptyParts);
						opt._persolid_fixed_color = vcg::Color4b(values[0].toInt(), values[1].toInt(), values[2].toInt(), values[3].toInt());
					}
					MLRenderingData data;
					data.set(opt);
					if (data.deserialize(value.toStdString()))
						rendOpt.push_back(data);
				}

				mesh = mesh.nextSibling();
			}
		}
		// READ IN POINT CORRESPONDECES INCOMPLETO!!
		else if (QString::compare(node.nodeName(), "RasterGroup") == 0)
		{
			QDomNode raster;
			raster = node.firstChild();
			while (!raster.isNull())
			{
				//return true;
				md.addNewRaster();
				QString labelRaster = raster.attributes().namedItem("label").nodeValue();
				md.rm()->setLabel(labelRaster);
				QDomNode sh = raster.firstChild();
				ReadShotFromQDomNode(md.rm()->shot, sh);

				QDomElement el = raster.firstChildElement("Plane");
				while (!el.isNull())
				{
					QString filen = el.attribute("fileName");
					QFileInfo fi(filen);
					QString nm = fi.absoluteFilePath();
					QImage img(":/img/dummy.png");
					try {
						img = meshlab::loadImage(nm);
					}
					catch(const MLException& e){
						unloadedImgList.push_back(nm.toStdString());
					}
					md.rm()->addPlane(new RasterPlane(img, nm, RasterPlane::RGBA));
					el = node.nextSiblingElement("Plane");
				}
				raster = raster.nextSibling();
			}
		}
		node = node.nextSibling();
	}

	QDir::setCurrent(tmpDir.absolutePath());
	qf.close();

	if (rendOpt.size() != meshList.size()){
		std::cerr << "cannot load rend options\n";
	}

	return meshList;
}
