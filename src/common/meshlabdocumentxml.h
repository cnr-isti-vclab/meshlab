#ifndef __MESHLABDOC_XML_H
#define __MESHLABDOC_XML_H

#include <QDomDocument>

#include "ml_shared_data_context.h"
#include "mesh_data_structures/mesh_model.h"
#include<map>


QDomDocument MeshDocumentToXML(MeshDocument &md, bool onlyVisibleLayers, bool saveViewState, bool binary, const std::map<int, MLRenderingData>& rendOpt = std::map<int, MLRenderingData>());
bool MeshDocumentToXMLFile(MeshDocument &md, QString filename, bool onlyVisibleLayers, bool saveViewState, bool binary, const std::map<int, MLRenderingData>& rendOpt = std::map<int, MLRenderingData>());
bool MeshDocumentFromXML(MeshDocument &md, QString filename, bool binary, std::map<int, MLRenderingData>& rendOpt);
QDomElement RasterModelToXML(RasterModel *mp,QDomDocument &doc, bool binary);
QDomElement PlaneToXML(Plane* pl,const QString& basePath,QDomDocument& doc);
#endif // __MESHLABDOC_XML_H
