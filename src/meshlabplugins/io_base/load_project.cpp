#include "load_project.h"

#include <QDir>

#include <wrap/io_trimesh/alnParser.h>
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
		vcg::CallBackPos*)
{
	std::list<MeshModel*> meshList;

	QFileInfo fi(filename);

	//todo: move here this function...
	if(!MeshDocumentFromBundler(md, filename, imageListFile, fi.baseName())){
		throw MLException("Unable to open OUTs file");
	}

	return meshList;
}

std::list<MeshModel*> loadNVM(
		const QString& filename,
		MeshDocument& md,
		vcg::CallBackPos*)
{
	std::list<MeshModel*> meshList;

	QFileInfo fi(filename);

	//todo: move here this function...
	if(!MeshDocumentFromNvm(md, filename, fi.baseName())){
		throw MLException("Unable to open NVMs file");
	}

	return meshList;
}
