#ifndef __MESHLABDOC_XML_H
#define __MESHLABDOC_XML_H

#include <QDomDocument>

#include "ml_shared_data_context/ml_shared_data_context.h"
#include "ml_document/mesh_document.h"
#include<map>


QDomDocument MeshDocumentToXML(const MeshDocument &md, bool onlyVisibleLayers, bool saveViewState, bool binary, const std::map<int, MLRenderingData>& rendOpt = std::map<int, MLRenderingData>());
bool MeshDocumentToXMLFile(const MeshDocument &md, QString filename, bool onlyVisibleLayers, bool saveViewState, bool binary, const std::map<int, MLRenderingData>& rendOpt = std::map<int, MLRenderingData>());
QDomElement RasterModelToXML(const RasterModel *mp,QDomDocument &doc, bool binary);
QDomElement PlaneToXML(const RasterPlane* pl,const QString& basePath,QDomDocument& doc);
#endif // __MESHLABDOC_XML_H
