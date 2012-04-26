#ifndef __MESHLABDOC_XML_H
#define __MESHLABDOC_XML_H

QDomDocument MeshDocumentToXML(MeshDocument &md, bool onlyVisibleLayers);
bool MeshDocumentToXMLFile(MeshDocument &md, QString filename, bool onlyVisibleLayers);
bool MeshDocumentFromXML(MeshDocument &md, QString filename);
QDomElement RasterModelToXML(RasterModel *mp,QDomDocument &doc);
QDomElement PlaneToXML(Plane* pl,const QString& basePath,QDomDocument& doc);
#endif // __MESHLABDOC_XML_H
