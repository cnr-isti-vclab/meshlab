#include "load_project.h"

#include <QDir>

#include <wrap/io_trimesh/alnParser.h>
#include <wrap/io_trimesh/import_out.h>
#include <wrap/io_trimesh/import_nvm.h>

#include <common/ml_document/mesh_document.h>
#include <common/utilities/load_save.h>
#include <common/meshlabdocumentbundler.h>

std::list<MeshModel*> loadALN(
		const QString& filename,
		MeshDocument& md,
		vcg::CallBackPos* cb)
{
	std::list<MeshModel*> meshList;
	std::vector<RangeMap> rmv;
	int retVal = ALNParser::ParseALN(rmv, qUtf8Printable(filename));
	if(retVal != ALNParser::NoError) {
		throw MLException("Unable to open ALN file");
	}
	QFileInfo fi(filename);

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
	return meshList;
}

std::list<MeshModel*> loadOUT(
		const QString& filename,
		const QString& imageListFile,
		MeshDocument& md,
		std::vector<std::string>& unloadedImgList,
		vcg::CallBackPos*)
{
	std::list<MeshModel*> meshList;
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
	QDir::setCurrent(imi.absoluteDir().absolutePath());

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

std::list<MeshModel*> loadNVM(
		const QString& filename,
		MeshDocument& md,
		std::vector<std::string>& unloadedImgList,
		vcg::CallBackPos*)
{
	std::list<MeshModel*> meshList;
	unloadedImgList.clear();

	QFileInfo fi(filename);

	MeshModel* newMesh = md.addNewMesh(fi.baseName(),QString("model"));
	std::vector<Shotm> shots;

	std::vector<std::string> image_filenames;
	vcg::tri::io::ImporterNVM<CMeshO>::Open(md.mm()->cm,shots,image_filenames, qUtf8Printable(filename));
	md.mm()->updateDataMask(MeshModel::MM_VERTCOLOR);

	QString curr_path = QDir::currentPath();
	//QFileInfo imi(image_list_filename);

	//QDir::setCurrent(imi.absoluteDir().absolutePath());
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

//	//todo: move here this function...
//	if(!MeshDocumentFromNvm(md, filename, fi.baseName())){
//		throw MLException("Unable to open NVMs file");
//	}

	return meshList;
}
