#include "save_project.h"

#include <common/meshlabdocumentxml.h>

void saveMLP(
		const QString& filename,
		const MeshDocument& md,
		bool onlyVisibleLayers,
		bool binary,
		const std::vector<MLRenderingData>& rendOpt,
		vcg::CallBackPos* cb)
{
	MeshDocumentToXMLFile(md, filename, onlyVisibleLayers, false, binary);
}
