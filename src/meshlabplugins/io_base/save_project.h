#ifndef SAVE_PROJECT_H
#define SAVE_PROJECT_H

#include <common/ml_document/mesh_document.h>

void saveMLP(
		const QString& filename,
		const MeshDocument& md,
		bool onlyVisibleLayers,
		bool binary,
		const std::vector<MLRenderingData>& rendOpt,
		vcg::CallBackPos* cb);

#endif // SAVE_PROJECT_H
