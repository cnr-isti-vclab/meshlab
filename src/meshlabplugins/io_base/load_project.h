#ifndef LOAD_PROJECT_H
#define LOAD_PROJECT_H

#include <common/ml_document/mesh_model.h>

std::list<MeshModel*> loadALN(
		const QString& filename,
		MeshDocument& md,
		vcg::CallBackPos* cb);

std::list<MeshModel*> loadOUT(
		const QString& filename,
		const QString& imageListFile,
		MeshDocument& md,
		vcg::CallBackPos* cb);

std::list<MeshModel*> loadNVM(
		const QString& filename,
		MeshDocument& md,
		vcg::CallBackPos* cb);

#endif // LOAD_PROJECT_H
