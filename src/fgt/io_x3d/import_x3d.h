/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
/****************************************************************************
  History

 $Log$
 Revision 1.2  2008/02/04 13:24:38  gianpaolopalma
 Added support to follow elements: IndexedTriangleSet, IndexedTriangleFanSet, IndexedTriangleStripSet, IndexedQuadSet.
 Improved support to QuadSet element.
 Bug fixed in methods getColor, getNormal, getTextureCoord.

 Revision 1.1  2008/02/02 13:51:40  gianpaolopalma
 Defined an X3D importer that parse and load mesh data from following element:
 Shape, Appearance, Transform, TriangleSet, TriangleStripSet, TriangleFanSet, QuadSet, Inline, ProtoInstance, ProtoDecalre, ExternProtoDecalre, ImageTexture

 
 *****************************************************************************/
#ifndef IMPORTERX3D
#define IMPORTERX3D

#include<QtXml>
#include <vcg/complex/trimesh/allocate.h>
#include <wrap/gl/glu_tesselator.h>

#include <util_x3d.h>

namespace vcg {
namespace tri {
namespace io {

	template<typename OpenMeshType>
	class ImporterX3D : public UtilX3D
	{
	private:

		
		static void ManageSwitchNode(QDomDocument* doc)
		{
			QDomNodeList switchNodes = doc->elementsByTagName("Switch");
			for(int sn = 0; sn < switchNodes.size(); sn++)
			{
				QDomElement swt = switchNodes.at(sn).toElement();
				QDomElement parent = swt.parentNode().toElement();
				int whichChoice = swt.attribute("whichChoice", "-1").toInt();
				if(whichChoice != -1)
				{
					QDomElement child = swt.firstChildElement();
					int i = 0;
					while (i < whichChoice && !child.isNull())
					{
						child = child.nextSiblingElement();
						i++;
					}
					if (!child.isNull())
					{
						ManageDefUse(swt, whichChoice, child);
						parent.replaceChild(child, swt);
					}
					else
						parent.removeChild(swt);
				}
				else
					parent.removeChild(swt);

			}
		}
		
		static void ManageLODNode(QDomDocument* doc)
		{
			QDomNodeList lodNodes = doc->elementsByTagName("LOD");
			for(int ln = 0; ln < lodNodes.size(); ln++)
			{
				QDomElement lod = lodNodes.at(ln).toElement();
				QDomNode parent = lod.parentNode();
				QString center = lod.attribute("center");
				QDomElement transform = doc->createElement("Transform");
				transform.setAttribute("traslation", center);
				QDomElement firstChild = lod.firstChildElement();
				if (!firstChild.isNull())
				{
					ManageDefUse(lod, 0, QDomElement());
					if (center != "")
					{
						parent.replaceChild(transform, lod);
						transform.appendChild(firstChild);
					}
					else
						parent.replaceChild(firstChild, lod);
				}
			}
		}

		
		static int ManageInlineNode(QDomDocument* doc, AdditionalInfoX3D* info)
		{
			QDomNodeList inlineNodes = doc->elementsByTagName("Inline");
			for(int in = 0; in < inlineNodes.size(); in++)
			{
				QDomElement inl = inlineNodes.at(in).toElement();
				if(inl.attribute("load", "true") == "true")
				{
					QString url = inl.attribute("url");
					QStringList paths = url.split(" ", QString::SkipEmptyParts);
					int i = 0;
					bool found = false;
					while (i < paths.size() && !found)
					{
						QString path = paths.at(i).trimmed().remove(QChar('"'));
						QFileInfo fi(path);
						if(fi.exists())
						{
							std::map<QString, QDomNode*>::const_iterator iter = info->inlineNodeMap.begin();
							bool load = true;
							QFileInfo tmpFi(info->filename);
							if (tmpFi.fileName() == fi.fileName())
							{
								info->lineNumberError = inl.lineNumber();
								return E_LOOPDEPENDENCE;
							}
							while(iter != info->inlineNodeMap.end() && load){
								tmpFi = QFileInfo(iter->first);
								if (tmpFi.fileName() == fi.fileName())
									load = false;
								iter++;
							}
							if(load && fi.suffix().toLower()=="x3d")
							{
								QDomDocument* docChild = new QDomDocument(path);
								QFile file(path);
								if (file.open(QIODevice::ReadOnly))
								{
									info->filenameStack.push_back(path);
									if (!docChild->setContent(&file)) 
										return E_INVALIDXML;
									info->inlineNodeMap[path] = docChild;
									file.close();
									int result = LoadMaskByDom(docChild, info, fi.fileName());
									if (result != E_NOERROR) return result;
									info->filenameStack.pop_back();
									found = true;
								}
							}
						}
						i++;
					}
				}
			}
			return E_NOERROR;
		}
		
		static int ManageExternProtoDeclare(QDomDocument* doc, AdditionalInfoX3D* info, const QString filename)
		{
			QDomNodeList exProtoDeclNodes = doc->elementsByTagName("ExternProtoDeclare");
			for(int en = 0; en < exProtoDeclNodes.size(); en++)
			{
				QDomElement exProtoDecl = exProtoDeclNodes.at(en).toElement();
				QString url = exProtoDecl.attribute("url");
				QStringList paths = url.split(" ", QString::SkipEmptyParts);
				int i = 0;
				bool found = false;
				while (i < paths.size() && !found)
				{
					QString str = paths.at(i).trimmed().remove(QChar('"'));
					QStringList list = str.split("#");
					QString path = list.at(0).trimmed();
					QString namePrototype = QString();
					if (list.size() == 1)
						namePrototype = "";
					if (list.size() == 2)
						namePrototype = list.at(1);
					if (list.size() < 3)
					{
						QFileInfo fi(path);
						if(fi.exists())
						{
							std::map<QString, QDomNode*>::const_iterator iter = info->protoDeclareNodeMap.begin();
							bool load = true;
							QFileInfo tmpFi(filename);
							if(tmpFi.fileName() == fi.fileName())
								load = false;
							while(iter != info->protoDeclareNodeMap.end() && load){
								QStringList pair = iter->first.split("#"); 
								tmpFi = QFileInfo(pair.at(0));
								if (tmpFi.fileName() == fi.fileName() && namePrototype == pair.at(1))
									load = false;
								iter++;
							}
							if (load && fi.suffix().toLower() == "x3d")
							{
								QDomDocument docChild(path);
								QFile file(path);
								if (file.open(QIODevice::ReadOnly))
								{
									if (!docChild.setContent(&file)) 
									{
										info->filenameStack.push_back(path);
										return E_INVALIDXML;
									}
									QDomNodeList prototypes = docChild.elementsByTagName("ProtoDeclare");
									int j = 0;
									while (j < prototypes.size() && !found)
									{
										QDomElement elem = prototypes.at(j).toElement();
										if (elem.attribute("name") == namePrototype)
										{
											found = true;
											QDomDocument* x = new QDomDocument(namePrototype);
											x->appendChild(elem);
											info->filenameStack.push_back(fi.fileName() + "#" + namePrototype);
											info->protoDeclareNodeMap[fi.fileName() + "#" + namePrototype] = x;
											int result = LoadMaskByDom(x, info, fi.fileName());
											if (result != E_NOERROR) return result;
											info->filenameStack.pop_back();
										}
										j++;
									}
									file.close();
								}
							}
						}
					}
					i++;
				}
			}
			return E_NOERROR;
		}

		
		static void ManageDefUse(const QDomElement& swt, int whichChoice, QDomElement& res)
		{
			std::map<QString, QDomElement> def;
			QDomNodeList ndl = swt.childNodes();
			for(int i = 0; i < whichChoice; i++)
				if (ndl.at(i).isElement())
					FindDEF(ndl.at(i).toElement(), def);
			if(whichChoice > 0)
				FindAndReplaceUSE(res, def);
			for(int i = whichChoice + 1; i < ndl.size(); i++)
				if (ndl.at(i).isElement())
					FindDEF(ndl.at(i).toElement(), def);
			QDomElement parent = swt.parentNode().toElement();
			bool flag = false;
			while (!parent.isNull() && parent.tagName() != "X3D")
			{
				QDomNodeList x = parent.childNodes();
				for (int j = 0; j < x.size(); j++)
				{
					if (x.at(j).isElement())
					{
						if (!flag)
						{
							if (x.at(j) == swt)
								flag = true;
						}
						else
							FindAndReplaceUSE(x.at(j).toElement(), def);
					}
				}
				parent = parent.parentNode().toElement();
			}
		}

		
		static void FindDEF(QDomElement& elem, std::map<QString, QDomElement>& map)
		{
			if (elem.isNull()) return;
			QString attrValue = elem.attribute("DEF");
			if(attrValue != QString())
				map[attrValue] = elem;
			QDomElement child = elem.firstChildElement();
			while(!child.isNull())
			{
				FindDEF(child, map);
				child = child.nextSiblingElement();
			}
		}
		
		static void FindAndReplaceUSE (QDomElement& elem, const std::map<QString, QDomElement>& map)
		{
			if (elem.isNull()) return;
			QString attrValue = elem.attribute("USE");
			if (attrValue != QString())
			{
				std::map<QString, QDomElement>::const_iterator iter = map.find(attrValue);
				if (iter != map.end())
				{
					QDomNode parent = elem.parentNode();
					parent.replaceChild(iter->second.cloneNode(true), elem);
					return;
				}
			}
			QDomNodeList children = elem.childNodes();
			for(int i = 0; i < children.size(); i++)
			{
				if (children.at(i).isElement())
					FindAndReplaceUSE(children.at(i).toElement(), map);
			}
		}

	
		inline static QDomElement findNode(QString tagName[], int n, const QDomElement root)
		{
			int i = 0;
			QDomElement elem;
			while (i < n)
			{
				elem = root.firstChildElement(tagName[i]);
				if (!elem.isNull())
					return elem;
				i++;
			}
			return QDomElement();
		}

	
		inline static bool isTextCoorGenSupported(const QDomElement& elem)
		{
			if (elem.isNull()) return false;
			if (elem.tagName() != "TextureCoordinateGenerator") return true;
			QString mode = elem.attribute("mode", "SPHERE");
			if (mode == "COORD")
				return true;
			return false;
		}
	
		inline static void findAndParseAttribute(QStringList& list, const QDomElement& elem, QString attribute, QString defValue)
		{
			if (elem.isNull())
			{
				list = QStringList();
				return;
			}
			QString value = elem.attribute(attribute, defValue);
			value.remove(",");
			list = value.split(" ", QString::SkipEmptyParts);
		}

		
		static int InitializeProtoDeclare(QDomElement& root, const std::map<QString, QString>& fields, const std::map<QString, QDomElement>& defMap, AdditionalInfoX3D* info)
		{
			QDomElement protoInterface = root.firstChildElement("ProtoInterface");
			QDomElement protoBody = root.firstChildElement("ProtoBody");
			if (protoInterface.isNull() || protoBody.isNull())
			{
				info->lineNumberError = root.lineNumber();
				return E_INVALIDPROTODECL;
			}
			std::map<QString, QString> defField;
			std::map<QString, QString> defNodeField;
			QDomElement child = protoInterface.firstChildElement("field");
			while (!child.isNull())
			{
				QString fName = child.attribute("name");
				QString fType = child.attribute("type");
				QString fAccType = child.attribute("accessType");
				QString fValue = child.attribute("value");
				if (fName =="" || fType == "" || fAccType == "")
				{
					info->lineNumberError = child.lineNumber();
					return E_INVALIDPROTODECLFIELD;
				}
				if (fAccType != "outputOnly")
				{
					if (fType == "SFNode" || fType == "MFNode")
						defNodeField[fName] = fValue;
					else
						defField[fName] = fValue;
				}
				child = child.nextSiblingElement("field");
			}
			QDomNodeList isList = protoBody.elementsByTagName("IS");
			std::map<QString, QString>::const_iterator iterDefField;
			std::map<QString, QString>::const_iterator iterValue;
			for (int i = 0; i < isList.size(); i++)
			{
				QDomElement is = isList.at(i).toElement();
				QDomNodeList connectList = is.elementsByTagName("connect");
				QDomElement parent = is.parentNode().toElement();
				for (int con = 0; con < connectList.size(); con++)
				{
					QDomElement connect = connectList.at(con).toElement();
					QString nodeField = connect.attribute("nodeField");
					QString protoField = connect.attribute("protoField");
					if (nodeField == "" || protoField == "")
					{
						info->lineNumberError = connect.lineNumber();
						return E_INVALIDISCONNECTOR;
					}
					iterDefField = defNodeField.find(protoField);
					iterValue = fields.find(protoField);
					if (iterDefField == defNodeField.end())
					{
						iterDefField = defField.find(protoField);
						if (iterDefField != defField.end())
						{
							if (iterValue != fields.end())
								parent.setAttribute(nodeField, iterValue->second);
							else if (iterDefField->second != "")
								parent.setAttribute(nodeField, iterDefField->second);
						}
					}
					else
					{
						if (iterValue != fields.end())
						{
							QStringList defList = iterValue->second.split(" ", QString::SkipEmptyParts);
							std::map<QString, QDomElement>::const_iterator iterDefMap;
							for (int j = 0; j < defList.size(); j++)
							{
								QString def = defList.at(j);
								def.remove('"');
								iterDefMap = defMap.find(def);
								if (iterDefMap == defMap.end())
								{
									info->lineNumberError = connect.lineNumber();
									return E_INVALIDDEFINFIELD;
								}
								QDomElement cloneDef = iterDefMap->second.cloneNode(true).toElement();
								parent.insertAfter(cloneDef, is);
							}
						}
					}
				}
			}
			for (int i = isList.size() -1; i > -1; i--)
			{
				QDomElement is = isList.at(i).toElement();
				QDomElement parent = is.parentNode().toElement();
				parent.removeChild(is);
			}
			return E_NOERROR;
		}

		
		inline static QDomElement solveDefUse(QDomElement root, std::map<QString, QDomElement>& defMap)
		{
			if (root.isNull()) return root;
			QString use = root.attribute("USE");
			if (use != "")
			{
				//DOTO: se esiste un parent con lo stesso def ritorno root
				std::map<QString, QDomElement>::const_iterator iter = defMap.find(use);
				if (iter != defMap.end() && root.tagName() == iter->second.tagName())
					return iter->second;
			}
			QString def = root.attribute("DEF");
			if (def != "" && defMap.find(def) == defMap.end())
				defMap[def] = root;
			return root;
		}


		
		
		static int LoadMaskByDom(QDomDocument* doc, AdditionalInfoX3D*& info, const QString filename)
		{
			bool bHasPerWedgeTexCoord = false;
			bool bHasPerWedgeNormal = false;
			bool bHasPerWedgeColor = false;
			bool bHasPerVertexColor	= false;
			bool bHasPerVertexNormal = false;
			bool bHasPerVertexText = false;
			bool bHasPerFaceColor = false;
			bool bHasPerFaceNormal = false;
			
			ManageSwitchNode(doc);
			ManageLODNode(doc);
			int result = ManageInlineNode(doc, info);
			if (result != E_NOERROR) return result;
			result = ManageExternProtoDeclare(doc, info, filename);
			if (result != E_NOERROR) return result;
			
			QDomNodeList shapeNodes = doc->elementsByTagName("Shape");
			for (int s = 0; s < shapeNodes.size(); s++)
			{
				QDomElement shape = shapeNodes.at(s).toElement();
				QDomElement appearance = shape.firstChildElement("Appearance");
				if (!appearance.isNull())
				{
					QDomNodeList texture;
					QDomElement multiTexture = appearance.firstChildElement("MultiTexture");
					if (!multiTexture.isNull())
						texture = multiTexture.elementsByTagName("ImageTexture");
					else
						texture = appearance.elementsByTagName("ImageTexture");
					for(int i = 0; i < texture.size(); i++)
					{
						QDomElement imageTexture = texture.at(i).toElement();
						QString url = imageTexture.attribute("url");
						if (url != "")
						{
							QStringList paths = url.split(" ", QString::SkipEmptyParts);
							int j = 0;
							bool load = false;
							while (j < paths.size() && !load)
							{
								QString path = paths.at(j).trimmed().remove('"');
								if(QFile::exists(path))
								{
									info->textuxeFile.push_back(path);
									load = true;
								}
								j++;
							}
							if (!load)
								info->textuxeFile.push_back(url);
						}
					}
				}
				QDomElement geometry = shape.firstChildElement();
				while(!geometry.isNull())
				{
					QString tagName = geometry.tagName();
					QString coorTag[] = {"Coordinate", "CoordinateDouble"};
					QDomElement coordinate = findNode(coorTag, 2, geometry);
					if (!coordinate.isNull() && (coordinate.attribute("point") != ""))
					{
						QString colorTag[] = {"Color", "ColorRGBA"};
						QDomElement color = findNode(colorTag, 2, geometry);
						QDomElement normal = geometry.firstChildElement("Normal");
						QString textCoorTag[] = {"TextureCoordinate", "MultiTextureCoordinate", "TextureCoordinateGenerator"};
						QDomElement textureCoor = findNode(textCoorTag, 3, geometry);
						QString colorPerVertex = geometry.attribute("colorPerVertex", "true");
						QString normalPerVertex = geometry.attribute("normalPerVertex", "true");
 						bool textureSup = isTextCoorGenSupported(textureCoor);
						if (tagName == "IndexedTriangleFanSet" || tagName == "IndexedTriangleSet" || tagName == "IndexedTriangleStripSet" || tagName == "IndexedQuadSet")
						{
							if (!color.isNull()) bHasPerVertexColor = true;
							if (textureSup) bHasPerVertexText = true;
							if (!normal.isNull())
							{
								if (normalPerVertex == "true")
									bHasPerVertexNormal = true;
								else
									bHasPerFaceNormal = true;
							}
						}
						else if (tagName == "TriangleFanSet" || tagName == "TriangleSet" || tagName == "TriangleStripSet" || tagName == "QuadSet")
						{
							if (!color.isNull()) bHasPerWedgeColor = true;
							if (textureSup) bHasPerWedgeTexCoord = true;
							if (!normal.isNull())
							{
								if (normalPerVertex == "true")
									bHasPerWedgeNormal = true;
								else
									bHasPerFaceNormal = true;
							}
						}
						else if (tagName == "IndexedFaceSet")
						{
							if (textureSup) bHasPerWedgeTexCoord = true;
							if (!color.isNull())
							{
								if (colorPerVertex == "true")
									bHasPerWedgeColor = true;
								else
									bHasPerFaceColor = true;
							}
							if (!normal.isNull())
							{
								if (normalPerVertex == "true")
									bHasPerWedgeTexCoord = true;
								else
									bHasPerFaceNormal = true;
							}
						}
						else if (tagName == "ElevationGrid")
						{
							if (textureSup) bHasPerVertexText = true;
							if (!color.isNull())
							{
								if (colorPerVertex == "true")
									bHasPerVertexColor = true;
								else
									bHasPerFaceColor = true;
							}
							if (!normal.isNull())
							{
								if (normalPerVertex == "true")
									bHasPerVertexNormal = true;
								else
									bHasPerFaceNormal = true;
							}

						}
					}
					geometry = geometry.nextSiblingElement();
				}
			}

			if (bHasPerWedgeTexCoord) 
				info->mask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
			if (bHasPerWedgeNormal) 
				info->mask |= vcg::tri::io::Mask::IOM_WEDGNORMAL;
			if (bHasPerWedgeColor)
				info->mask |= vcg::tri::io::Mask::IOM_WEDGCOLOR;
			if (bHasPerVertexColor)	
				info->mask |= vcg::tri::io::Mask::IOM_VERTCOLOR;
			if (bHasPerVertexNormal) 
				info->mask |= vcg::tri::io::Mask::IOM_VERTNORMAL;
			if (bHasPerVertexText) 
				info->mask |= vcg::tri::io::Mask::IOM_VERTTEXCOORD;
			if (bHasPerFaceColor) 
				info->mask |= vcg::tri::io::Mask::IOM_FACECOLOR;
			if (bHasPerFaceNormal) 
				info->mask |= vcg::tri::io::Mask::IOM_FACENORMAL;
			return E_NOERROR;
		}

		
		
		static int LoadSet(QDomElement geometry,
									OpenMeshType& m,
									const vcg::Matrix44f tMatrix,
									const TextureInfo& texture,
									const QStringList& coordList,
									const QStringList& colorList,
									const QStringList& normalList,
									int colorComponent,
									AdditionalInfoX3D* info,
									CallBackPos *cb=0)
		{
			QString normalPerVertex = geometry.attribute("normalPerVertex", "true");
			std::vector<vcg::Point4f> vertexSet;
			int index = 0;
			std::vector<int> vertexFaceIndex;
			while (index + 2 < coordList.size())
			{
				vcg::Point4f vertex(coordList.at(index).toFloat(), coordList.at(index + 1).toFloat(), coordList.at(index + 2).toFloat(), 1.0);
				size_t vi = 0;
				bool found = false;
				while (vi < vertexSet.size() && !found)
				{
					if (vertexSet.at(vi) == vertex)
						found = true;
					else
						vi++;
				}
				if (!found)
				{
					vertexSet.push_back(vertex);
					vertexFaceIndex.push_back(vertexSet.size() - 1);
				}
				else
					vertexFaceIndex.push_back(vi);
				index += 3;
			}

			int offset = m.vert.size();
			vcg::tri::Allocator<OpenMeshType>::AddVertices(m, vertexSet.size());
			for (size_t vv = 0; vv < vertexSet.size(); vv++)
			{
				vcg::Point4f tmp = tMatrix * vertexSet.at(vv);
				m.vert[offset + vv].P() = vcg::Point3f(tmp.X(),tmp.Y(),tmp.Z());
			}
			
			int offsetFace = m.face.size();
			int nFace = 0;
			if (geometry.tagName() == "TriangleSet")
			{
				nFace = vertexFaceIndex.size()/3;
				vcg::tri::Allocator<OpenMeshType>::AddFaces(m, nFace);
				for (int ff = 0; ff < nFace; ff++)
				{
					int faceIndex = ff + offsetFace;
					for (int tt = 0; tt < 3; tt++)
					{
						m.face[faceIndex].V(tt) = &(m.vert[vertexFaceIndex.at(tt + ff*3) + offset]);
						if (m.HasPerWedgeNormal() && normalPerVertex == "true" && !normalList.isEmpty())
							getNormal(normalList, (tt + ff*3)*3, m.face[faceIndex].WN(tt));
						if (m.HasPerWedgeColor() && !colorList.isEmpty())
							getColor(colorList, colorComponent, (tt + ff*3)*colorComponent, m.face[faceIndex].WC(tt));
						if (m.HasPerWedgeTexCoord() && (info->mask & MeshModel::IOM_WEDGTEXCOORD))
							getTextureCoord(texture, (tt + ff*3)*2, m.vert[vertexFaceIndex.at(tt + ff*3) + offset].cP(), m.face[faceIndex].WT(tt));
					}
					if (m.HasPerFaceNormal() && normalPerVertex == "false" && !normalList.isEmpty())
						getNormal(normalList, ff*3, m.face[faceIndex].N());
				}
			}
			else if (geometry.tagName() == "TriangleFanSet" || geometry.tagName() == "TriangleStripSet")
			{
				QStringList countList;
				if (geometry.tagName() == "TriangleFanSet")
					findAndParseAttribute(countList, geometry, "fanCount", "");
				else
					findAndParseAttribute(countList, geometry, "stripCount", "");
				std::vector<int> count;
				if (countList.isEmpty())
				{
					count.push_back(vertexFaceIndex.size());
					nFace = vertexFaceIndex.size() > 2 ? vertexFaceIndex.size() - 2: 0;
				}
				else
				{
					for (int i = 0; i < countList.size(); i++)
					{
						if (countList.at(i).toInt() < 3)
						{
							info->lineNumberError = geometry.lineNumber();
							return E_INVALIDFANSTRIP;
						}
						else
						{
							count.push_back(countList.at(i).toInt());
							nFace += countList.at(i).toInt() - 2;
						}
					}
				}
				vcg::tri::Allocator<OpenMeshType>::AddFaces(m, nFace);									
				int index = 0;
				int ff = 0;
				for (size_t ns = 0; ns < count.size() && ff < nFace; ns++)
				{
					int numVertex = count.at(ns);
					int firstVertexIndex = vertexFaceIndex.at(index) + offset;
					int secondVertexIndex = vertexFaceIndex.at(index + 1) + offset;
					vcg::Point3f firstNormal, secondNormal;
					if (normalPerVertex == "true" && !normalList.isEmpty())
					{
						getNormal(normalList, index*3, firstNormal);
						getNormal(normalList, index*3 + 3, secondNormal);
					}
					vcg::Color4b firstColor, secondColor;
					if (!colorList.isEmpty())
					{
						getColor(colorList, colorComponent, index*colorComponent, firstColor);
						getColor(colorList, colorComponent, index*colorComponent + colorComponent, secondColor);
					}
					vcg::TexCoord2<float> firstTextCoord, secondTextCoord;
					bool validFirst = getTextureCoord(texture, index*2, m.vert[vertexFaceIndex.at(index) + offset].cP(), firstTextCoord);
					bool validSecond =  getTextureCoord(texture, index*2, m.vert[vertexFaceIndex.at(index + 1) + offset].cP(), secondTextCoord);
					for(int vi = 2; vi < numVertex; vi++)
					{
						int faceIndex = ff + offsetFace;
						m.face[faceIndex].V(0) = &(m.vert[firstVertexIndex]);
						m.face[faceIndex].V(1) = &(m.vert[secondVertexIndex]);
						if (m.HasPerWedgeNormal() && normalPerVertex == "true" && !normalList.isEmpty())
						{
							m.face[faceIndex].WN(0) = firstNormal;
							m.face[faceIndex].WN(1) = secondNormal;
						}
						if (m.HasPerWedgeColor() && !colorList.isEmpty())
						{
							m.face[faceIndex].WC(0) = firstColor;
							m.face[faceIndex].WC(1) = secondColor;
						}
						if (m.HasPerWedgeTexCoord() && (info->mask & MeshModel::IOM_WEDGTEXCOORD))
						{
							if (validFirst)
								m.face[faceIndex].WT(0) = firstTextCoord;
							if (validSecond)
								m.face[faceIndex].WT(1) = secondTextCoord;
						}
						
						m.face[faceIndex].V(2) = &(m.vert[vertexFaceIndex.at(index + vi) + offset]);
						if (!normalList.isEmpty())
						{
							if (normalPerVertex == "true" && m.HasPerWedgeNormal())
								getNormal(normalList, (index + vi)*3, m.face[faceIndex].WN(2));
							if (normalPerVertex == "false" && m.HasPerFaceNormal())
								getNormal(normalList, ff*3, m.face[faceIndex].N());
						}
						if (!colorList.isEmpty() && m.HasPerWedgeColor())
							getColor(colorList, colorComponent, (index + vi)*colorComponent, m.face[faceIndex].WC(2));
						bool valid;
						if (m.HasPerWedgeTexCoord() && (info->mask & MeshModel::IOM_WEDGTEXCOORD))
							valid = getTextureCoord(texture, (index + vi)*2, m.vert[vertexFaceIndex.at(index + vi) + offset].cP(), m.face[faceIndex].WT(2)); 
						
						if (geometry.tagName() == "TriangleStripSet")
						{
							firstVertexIndex = secondVertexIndex;
							firstColor = secondColor;
							firstNormal = secondNormal;
							firstTextCoord = secondTextCoord;
							validFirst = validSecond;
						}
						secondVertexIndex = vertexFaceIndex.at(index + vi) + offset;
						if (m.HasPerWedgeColor())
							secondColor = m.face[faceIndex].WC(2);
						if (m.HasPerWedgeNormal())
							secondNormal = m.face[faceIndex].WN(2);
						if (m.HasPerWedgeTexCoord() && (info->mask & MeshModel::IOM_WEDGTEXCOORD))
						{
							if (valid)
								secondTextCoord = m.face[faceIndex].WT(2);
							validSecond = valid;
						}
						ff++;
					}
					index += numVertex;
				}
			}
			else if (geometry.tagName() == "QuadSet")
			{
				nFace = vertexFaceIndex.size()/4;
				vcg::tri::Allocator<OpenMeshType>::AddFaces(m, nFace * 2);
				for (int ff = 0; ff < nFace; ff++)
				{
					std::vector<std::vector<vcg::Point3f>> polygonVect;
					std::vector<Point3f> polygon;
					for (int tt = 0; tt < 4; tt++)
						polygon.push_back(m.vert[vertexFaceIndex.at(tt + ff*4) + offset].cP());
					polygonVect.push_back(polygon);
					std::vector<int> indexVect;
					vcg::glu_tesselator::tesselate<vcg::Point3f>(polygonVect, indexVect);
					int faceIndex = ff*2 + offsetFace;
					size_t iv = 0;
					while (iv + 2 < indexVect.size())
					{
						for (int tt = 0; tt < 3; tt++)
						{
							int indexVertex = indexVect.at(iv) + ff*4;
							m.face[faceIndex].V(tt) = &(m.vert[vertexFaceIndex.at(indexVertex) + offset]);
							if (m.HasPerWedgeNormal() && normalPerVertex == "true" && !normalList.isEmpty())
								getNormal(normalList, indexVertex*3, m.face[faceIndex].WN(tt));
							if (m.HasPerWedgeColor() && !colorList.isEmpty())
								getColor(colorList, colorComponent, indexVertex*colorComponent, m.face[faceIndex].WC(tt));
							if (m.HasPerWedgeTexCoord() && (info->mask & MeshModel::IOM_WEDGTEXCOORD))
								getTextureCoord(texture, indexVertex*2, m.vert[vertexFaceIndex.at(indexVertex) + offset].cP(), m.face[faceIndex].WT(tt)); 
							iv++;
						}
						if (m.HasPerFaceNormal() && normalPerVertex == "false" && !normalList.isEmpty())
							getNormal(normalList, ff*3, m.face[faceIndex].N());
						faceIndex++;
					}
				}
			}
			return E_NOERROR;
		}

		

		static int LoadIndexedSet(QDomElement geometry,
									OpenMeshType& m,
									const vcg::Matrix44f tMatrix,
									const TextureInfo& texture,
									const QStringList& coordList,
									const QStringList& colorList,
									const QStringList& normalList,
									int colorComponent,
									AdditionalInfoX3D* info,
									CallBackPos *cb=0)
		{
			QStringList indexList;
			findAndParseAttribute(indexList, geometry, "index", "");
			if (!indexList.isEmpty())
			{
				QString normalPerVertex = geometry.attribute("normalPerVertex", "true");
				int offset = m.vert.size();
				int nVertex = coordList.size()/3;
				vcg::tri::Allocator<OpenMeshType>::AddVertices(m, nVertex);
				for (int vv = 0; vv < nVertex; vv++)
				{
					vcg::Point4f tmp = tMatrix * vcg::Point4f(coordList.at(vv*3).toFloat(), coordList.at(vv*3 + 1).toFloat(), coordList.at(vv*3 + 2).toFloat(), 1.0);
					m.vert[offset + vv].P() = vcg::Point3f(tmp.X(),tmp.Y(),tmp.Z());
					if (m.HasPerVertexNormal() && !normalList.isEmpty() && normalPerVertex == "true")
						getNormal(normalList, vv*3, m.vert[offset + vv].N());
					if (m.HasPerVertexColor() && !colorList.isEmpty())
						getColor(colorList, colorComponent, vv*3, m.vert[offset + vv].C());
					if (m.HasPerVertexTexCoord())
						getTextureCoord(texture, vv*2, m.vert[offset + vv].cP(), m.vert[offset + vv].T());
				}
				int offsetFace = m.face.size();
				int nFace = 0;
				if (geometry.tagName() == "IndexedTriangleSet")
				{
					nFace = indexList.size()/3;
					vcg::tri::Allocator<OpenMeshType>::AddFaces(m, nFace);
					for (int ff = 0; ff < nFace; ff++)
					{
						int faceIndex = ff + offsetFace;
						for (int tt = 0; tt < 3; tt++)
						{
							int vertIndex = indexList.at(tt + ff*3).toInt() + offset;
							if (vertIndex >= m.vert.size())
							{
								info->lineNumberError = geometry.lineNumber();
								return E_INVALIDINDEXED;
							}
							m.face[faceIndex].V(tt) = &(m.vert[vertIndex]);
							if(!m.HasPerVertexTexCoord() && m.HasPerWedgeTexCoord() && (info->mask & MeshModel::IOM_WEDGTEXCOORD))
								getTextureCoord(texture, indexList.at(tt + ff*3).toInt()*2, m.vert[vertIndex].cP(), m.face[faceIndex].WT(tt));
 						}
						if (m.HasPerFaceNormal() && normalPerVertex == "false" && !normalList.isEmpty())
							getNormal(normalList, ff*3, m.face[faceIndex].N());
					}
				}								
				else if (geometry.tagName() == "IndexedTriangleFanSet" || geometry.tagName() == "IndexedTriangleStripSet")
				{									
					int count = 0;
					int pos = indexList.indexOf("-1");
					while(pos != -1)
					{
						count ++;
						int nextPos = indexList.indexOf("-1", pos+1);
						int tmp = (nextPos == -1)? indexList.size(): nextPos;
						if ((tmp - pos -1) < 3)
						{
							info->lineNumberError = geometry.lineNumber();
							return E_INVALIDINDEXEDFANSTRIP;
						}
						pos = (nextPos == indexList.size() -1)? -1: nextPos;
					}
					int sub = count;
					count++;
					if (indexList.at(indexList.size()-1) == "-1")
						sub++;
					nFace = indexList.size() - 2*count - sub;
					vcg::tri::Allocator<OpenMeshType>::AddFaces(m, nFace);
					int ff = 0;
					int firstVertexIndex;
					int secondVertexIndex;
					int vertIndex;
					for (int ls = 0; ls < indexList.size() && ff < nFace; ls++)
					{
						if (indexList.at(ls) == "-1" || ls == 0)
						{
							if (ls == 0) ls = -1;
							vertIndex = indexList.at(ls + 1).toInt() + offset;
							if (vertIndex >= m.vert.size())
							{
								info->lineNumberError = geometry.lineNumber();
								return E_INVALIDINDEXED;
							}
							firstVertexIndex = vertIndex;
							vertIndex = indexList.at(ls + 2).toInt() + offset;
							if (vertIndex >= m.vert.size())
							{
								info->lineNumberError = geometry.lineNumber();
								return E_INVALIDINDEXED;
							}
							secondVertexIndex = vertIndex;
							ls = ls + 3;
						}
						int faceIndex = ff + offsetFace;
						m.face[faceIndex].V(0) = &(m.vert[firstVertexIndex]);
						m.face[faceIndex].V(1) = &(m.vert[secondVertexIndex]);
						if(!m.HasPerVertexTexCoord() && m.HasPerWedgeTexCoord() && (info->mask & MeshModel::IOM_WEDGTEXCOORD))
						{
								getTextureCoord(texture, (firstVertexIndex - offset)*2, m.vert[firstVertexIndex].cP(), m.face[faceIndex].WT(0));
								getTextureCoord(texture, (secondVertexIndex - offset)*2, m.vert[firstVertexIndex].cP(), m.face[faceIndex].WT(1));
						}

 						vertIndex = indexList.at(ls).toInt() + offset;
						if (vertIndex >= m.vert.size())
						{
							info->lineNumberError = geometry.lineNumber();
							return E_INVALIDINDEXED;
						}
						m.face[faceIndex].V(2) = &(m.vert[vertIndex]);
						if (!normalList.isEmpty() && normalPerVertex == "false")
							getNormal(normalList, ff*3, m.face[faceIndex].N());
						if(!m.HasPerVertexTexCoord() && m.HasPerWedgeTexCoord() && (info->mask & MeshModel::IOM_WEDGTEXCOORD))
								getTextureCoord(texture, (vertIndex - offset)*2, m.vert[vertIndex].cP(), m.face[faceIndex].WT(2));
						if (geometry.tagName() == "IndexedTriangleStripSet")
							firstVertexIndex = secondVertexIndex;
						secondVertexIndex = vertIndex;
						ff++;
					}
				}
				else if (geometry.tagName() == "IndexedQuadSet")
				{
					nFace = indexList.size()/4;
					vcg::tri::Allocator<OpenMeshType>::AddFaces(m, nFace * 2);
					for (int ff = 0; ff < nFace; ff++)
					{
						std::vector<std::vector<vcg::Point3f>> polygonVect;
						std::vector<Point3f> polygon;
						for (int tt = 0; tt < 4; tt++)
							polygon.push_back(m.vert[indexList.at(tt + ff*4).toInt() + offset].cP());
						polygonVect.push_back(polygon);
						std::vector<int> indexVect;
						vcg::glu_tesselator::tesselate<vcg::Point3f>(polygonVect, indexVect);
						int faceIndex = ff*2 + offsetFace;
						size_t iv = 0;
						while (iv + 2 < indexVect.size())
						{
							for (int tt = 0; tt < 3; tt++)
							{
								int indexVertex = indexVect.at(iv) + ff*4;
								m.face[faceIndex].V(tt) = &(m.vert[indexList.at(indexVertex).toInt() + offset]);
								if(!m.HasPerVertexTexCoord() && m.HasPerWedgeTexCoord() && (info->mask & MeshModel::IOM_WEDGTEXCOORD))
									getTextureCoord(texture, indexList.at(indexVertex).toInt()*2, m.vert[indexList.at(indexVertex).toInt() + offset].cP(), m.face[faceIndex].WT(tt));
								iv++;
							}
							if (m.HasPerFaceNormal() && normalPerVertex == "false" && !normalList.isEmpty())
								getNormal(normalList, ff*3, m.face[faceIndex].N());
							faceIndex++;
						}
					}
				}
			}
			return E_NOERROR;
		}



		
		
		inline static vcg::Matrix33f createTextureTrasformMatrix(QDomElement elem)
		{
			vcg::Matrix33f matrix, tmp;
			matrix.SetIdentity();
			QStringList coordList, center;
			findAndParseAttribute(center, elem, "center", "0 0");
			if (center.size() == 2)
			{			
				matrix[0][2] = -center.at(0).toFloat();
				matrix[1][2] = -center.at(1).toFloat();
			}
			findAndParseAttribute(coordList, elem, "scale", "1 1");
			if(coordList.size() == 2)
			{
				tmp.SetIdentity();
				tmp[0][0] = coordList.at(0).toFloat();
				tmp[1][1] = coordList.at(1).toFloat();
				matrix *= tmp;
			}
			findAndParseAttribute(coordList, elem, "rotation", "0");
			if(coordList.size() == 1)
			{
				tmp.Rotate(math::ToDeg(coordList.at(0).toFloat()), vcg::Point3f(0, 0, 1));
				matrix *= tmp;
			}
			if (center.size() == 2)
			{
				tmp.SetIdentity();
				tmp[0][2] = center.at(0).toFloat();
				tmp[1][2] = center.at(1).toFloat();
				matrix *= tmp;
			}
			findAndParseAttribute(coordList, elem, "traslation", "0 0");
			if(coordList.size() == 2)
			{
				tmp.SetIdentity();
				tmp[0][2] = coordList.at(0).toFloat();
				tmp[1][2] = coordList.at(1).toFloat();
				matrix *= tmp;
			}
			return matrix;
		}
		
		
		inline static vcg::Matrix44f createTransformMatrix(QDomElement root, vcg::Matrix44f tMatrix)
		{
			vcg::Matrix44f t, tmp;
			t.SetIdentity();
			QStringList coordList, center, scale;
			findAndParseAttribute(coordList, root, "translation", "0 0 0");
			if(coordList.size() == 3)
				t.SetTranslate(coordList.at(0).toFloat(), coordList.at(1).toFloat(), coordList.at(2).toFloat()); 
			findAndParseAttribute(center, root, "center", "0 0 0");
			if(center.size() == 3)
			{
				tmp.SetTranslate(center.at(0).toFloat(), center.at(1).toFloat(), center.at(2).toFloat());
				t *= tmp;
			}
			findAndParseAttribute(coordList, root, "rotation", "0 0 1 0");
			if(coordList.size() == 4)
			{
				tmp.SetRotate(coordList.at(3).toFloat(), vcg::Point3f(coordList.at(0).toFloat(), coordList.at(1).toFloat(), coordList.at(2).toFloat()));
				t *= tmp;
			}
			findAndParseAttribute(scale, root, "scaleOrientation", "0 0 1 0");
			if(scale.size() == 4)
			{
				tmp.SetRotate(scale.at(3).toFloat(), vcg::Point3f(scale.at(0).toFloat(), scale.at(1).toFloat(), scale.at(2).toFloat()));
				t *= tmp;
			}
			findAndParseAttribute(coordList, root, "scale", "1 1 1");
			if(coordList.size() == 3)
			{
				tmp.SetScale(coordList.at(0).toFloat(), coordList.at(1).toFloat(), coordList.at(2).toFloat());
				t *= tmp;
			}
			if(scale.size() == 4)
			{
				tmp.SetRotate(-scale.at(3).toFloat(), vcg::Point3f(scale.at(0).toFloat(), scale.at(1).toFloat(), scale.at(2).toFloat()));
				t *= tmp;
			}
			if(center.size() == 3)
			{
				tmp.SetTranslate(-center.at(0).toFloat(), -center.at(1).toFloat(), -center.at(2).toFloat());
				t *= tmp;
			}
			t = tMatrix * t;
			return t;
		}

		
		
		static int NavigateInline(OpenMeshType& m,
									QDomElement root,
									const vcg::Matrix44f tMatrix,
									AdditionalInfoX3D* info,
									CallBackPos *cb=0)
		{
			QString load = root.attribute("load", "true");
			if (load != "true") return E_NOERROR;
			QString url = root.attribute("url");
			if (url == "")
			{
				info->lineNumberError = root.lineNumber();
				return E_INVALIDINLINEURL;
			}
			QStringList paths = url.split(" ", QString::SkipEmptyParts);
			int i = 0;
			bool found = false;
			std::map<QString, QDomNode*>::const_iterator iter;
			while (i < paths.size() && !found)
			{
				QString path = paths.at(i).trimmed().remove(QChar('"'));
				iter = info->inlineNodeMap.find(path);
				if (iter != info->inlineNodeMap.end()){
					for (size_t j = 0; j < info->filenameStack.size(); j++)
					{
						if ( info->filenameStack[j] == path ) 
						{
							info->lineNumberError = root.lineNumber();
							return E_LOOPDEPENDENCE;
						}
					}
					info->filenameStack.push_back(path);
					QDomElement first = iter->second->firstChildElement("X3D");
					std::map<QString, QDomElement> newDefMap;
					std::map<QString, QDomElement> newProtoDeclMap;
					int result = NavigateScene(m, first, tMatrix, newDefMap, newProtoDeclMap, info, cb);
					if (result != E_NOERROR)
						return result;
					info->filenameStack.pop_back();
					found = true;
				}
				i++;
			}
			if (!found)
			{
				info->lineNumberError = root.lineNumber();
				return E_INVALIDINLINE;
			}
			return E_NOERROR;
		}
		
		
		static int NavigateExternProtoDeclare(QDomElement root,
									const vcg::Matrix44f tMatrix,
									std::map<QString, QDomElement>& protoDeclareMap,
									AdditionalInfoX3D* info,
									CallBackPos *cb=0)
		{
			QString name = root.attribute("name");
			QString url = root.attribute("url");
			if (url == "")
			{
				info->lineNumberError = root.lineNumber();
				return E_INVALIDPROTOURL;
			}
			if (protoDeclareMap.find(name) != protoDeclareMap.end())
			{
				info->lineNumberError = root.lineNumber();
				return E_MULTINAMEPROTODECL;
			}
			QStringList paths = url.split(" ", QString::SkipEmptyParts);
			int i = 0;
			bool found = false;
			std::map<QString, QDomNode*>::const_iterator iter;
			while (i < paths.size() && !found)
			{
				QString path = paths.at(i).trimmed().remove(QChar('"'));
				QStringList list = path.split("#");
				QFileInfo fi(list.at(0));
				QString filename = fi.fileName();
				QString namePrototype;
				if (list.size() == 1)
					namePrototype = "";
				if (list.size() == 2)
					namePrototype = list.at(1);
				iter = info->protoDeclareNodeMap.find(filename + "#" + namePrototype);
				if (iter != info->protoDeclareNodeMap.end())
				{
					QDomElement first = iter->second->firstChildElement();
					protoDeclareMap[name] = first;
					found = true;
				}
				i++;
			}
			if (!found)
			{
				info->lineNumberError = root.lineNumber();
				return E_INVALIDPROTO;
			}
			return E_NOERROR;
		}
		
		
		static int NavigateProtoInstance(OpenMeshType& m,
									QDomElement root,
									const vcg::Matrix44f tMatrix,
									std::map<QString, QDomElement>& defMap,
									std::map<QString, QDomElement>& protoDeclareMap,
									AdditionalInfoX3D* info,
									CallBackPos *cb=0)
		{
			QString name = root.attribute("name");
			std::map<QString, QString> fields;
			QDomElement child = root.firstChildElement("fieldValue");
			while (!child.isNull())
			{
				QString fName = child.attribute("name");
				QString fValue = child.attribute("value");
				if (fName =="" || fValue == "")
				{
					info->lineNumberError = root.lineNumber();
					return E_INVALIDINSTFIELD;
				}
				fields[fName] = fValue;
				child = child.nextSiblingElement("fieldValue");
			}
			std::map<QString, QDomElement>::const_iterator iter = protoDeclareMap.find(name);
			if (iter == protoDeclareMap.end())
			{
				info->lineNumberError = root.lineNumber();
				return E_INVALIDPROTOINST;
			}
			QDomElement protoInstance = iter->second.cloneNode(true).toElement();
			//recuperare il nome del file della protoDeclare
			QString filename = "";
			for (std::map<QString, QDomNode*>::const_iterator i = info->protoDeclareNodeMap.begin(); i != info->protoDeclareNodeMap.end(); i++)
			{
				if (i->second->firstChildElement() == iter->second)
				{
					filename = i->first;
					break;
				}
			}
			for (size_t j = 0; j < info->filenameStack.size(); j++)
			{
				if ( info->filenameStack[j] == filename ) 
				{
					info->lineNumberError = root.lineNumber();
					return E_LOOPDEPENDENCE;
				}
			}
			if (filename != "")
				info->filenameStack.push_back(filename);
			int result = InitializeProtoDeclare(protoInstance, fields, defMap, info);
			if (result != E_NOERROR) return result;
			QDomElement body = protoInstance.firstChildElement("ProtoBody");
			std::map<QString, QDomElement> newDefMap;
			std::map<QString, QDomElement> newProtoDeclMap;
			result = NavigateScene(m, body, tMatrix, newDefMap, newProtoDeclMap, info, cb);
			if (result != E_NOERROR)
				return result;
			//eliminare il nome del file dal vettore
			if (filename != "")
				info->filenameStack.pop_back();
			return E_NOERROR;
		}		
		
		
		inline static void getNormal(const QStringList& list, int index, vcg::Point3f& dest)
		{
			if(!list.isEmpty() && (index + 2) < list.size())
			{
				vcg::Point3f normal(list.at(index).toFloat(), list.at(index + 1).toFloat(), list.at( index+ 2).toFloat());
				//trasformare la normale?
				dest = normal;
			}
		}

		
		inline static void getColor(const QStringList& list, int component, int index, vcg::Color4b& dest)
		{
			if(!list.isEmpty() && (index + component - 1) < list.size())
			{
				vcg::Color4f color;
				if (component == 3)
					color = vcg::Color4f(list.at(index).toFloat(), list.at(index + 1).toFloat(), list.at(index + 2).toFloat(), 0); 
				else
					color = vcg::Color4f(list.at(index).toFloat(), list.at(index + 1).toFloat(), list.at(index + 2).toFloat(), list.at(index + 3).toFloat());
				vcg::Color4b colorB;
				colorB.Import(color);
				dest = colorB;
			}
		}

		
		inline static bool getTextureCoord(const TextureInfo& textInfo, int index, const vcg::Point3f& vertex, vcg::TexCoord2<float>& dest)
		{
			vcg::Point3f point;
			if (textInfo.isCoordGenerator)
			{
				if (textInfo.mode == "COORD")
					point = vcg::Point3f(vertex.X(), vertex.Y(), 1.0);
				else
					return false;
			}
			else if (!textInfo.textureCoordList.isEmpty() && (index + 2) < textInfo.textureCoordList.size())
				point = vcg::Point3f(textInfo.textureCoordList.at(index).toFloat(), textInfo.textureCoordList.at(index + 1).toFloat(), 1.0);
			else
				return false;
			point = textInfo.textureTransform * point;
			if (!textInfo.repeatS)
			{
				point.X() = point.X() < 0? 0: point.X();
				point.X() = point.X() > 1? 1: point.X();
			}
			else
				point.X() = point.X() - floorf(point.X());
			if (!textInfo.repeatT)
			{
				point.Y() = point.Y() < 0? 0: point.Y();
				point.Y() = point.Y() > 1? 1: point.Y();
			}
			else
				point.Y() = point.Y() - floorf(point.Y());
			vcg::TexCoord2<float> textCoord(point.X(), point.Y());
			dest = textCoord;
			return true;
		}


		
		static int NavigateScene(OpenMeshType& m,
									QDomElement root,
									const vcg::Matrix44f tMatrix,
									std::map<QString, QDomElement>& defMap,
									std::map<QString, QDomElement>& protoDeclareMap,
									AdditionalInfoX3D* info,
									CallBackPos *cb=0)
		{
			if (root.isNull()) return E_NOERROR;
			QString use = root.attribute("USE");
			if (use != "")
			{
				std::map<QString, QDomElement>::const_iterator iter = defMap.find(use);
				if (iter != defMap.end())
				{
					if (iter->second.tagName() == root.tagName())
						return NavigateScene(m, iter->second, tMatrix, defMap, protoDeclareMap, info, cb);
					info->lineNumberError = root.lineNumber();
					return E_MISMATCHDEFUSETYPE;
				}
				info->lineNumberError = root.lineNumber();
				return E_NODEFFORUSE;
			}

			QString def = root.attribute("DEF");
			if (def != "" && defMap.find(def) == defMap.end())
				defMap[def] = root;
			
			if (root.tagName() == "Transform")
			{
				vcg::Matrix44f t = createTransformMatrix(root, tMatrix);
				QDomElement child = root.firstChildElement();
				while(!child.isNull())
				{
					int result = NavigateScene(m, child, t, defMap, protoDeclareMap, info, cb);
					if (result != E_NOERROR)
						return result;
					child = child.nextSiblingElement();
				}
				return E_NOERROR;
			}
			
			if (root.tagName() == "Inline")
				return NavigateInline(m, root, tMatrix, info, cb);
			
			if (root.tagName() == "ProtoDeclare")
			{
				QString name = root.attribute("name");
				if (protoDeclareMap.find(name) != protoDeclareMap.end())
				{
					info->lineNumberError = root.lineNumber();
					return E_MULTINAMEPROTODECL;
				}
				protoDeclareMap[name] = root;
				return E_NOERROR;
			}
			
			if (root.tagName() == "ExternProtoDeclare")
				return NavigateExternProtoDeclare(root, tMatrix, protoDeclareMap, info, cb);
			
			if (root.tagName() == "ProtoInstance")
				return NavigateProtoInstance(m, root, tMatrix, defMap, protoDeclareMap, info, cb);
			
			if (root.tagName() == "Shape")
			{
				QDomElement appearance = root.firstChildElement("Appearance");
				std::vector<bool> validTexture;
				std::vector<TextureInfo> textureInfo;
				QDomNodeList textureTransformList;
				if (!appearance.isNull())
				{
					appearance = solveDefUse(appearance, defMap);
					QDomNodeList texture;
					QDomElement multiTexture = appearance.firstChildElement("MultiTexture");
					if (!multiTexture.isNull())
					{
						multiTexture = solveDefUse(multiTexture, defMap);
						QDomElement child = multiTexture.firstChildElement();
						while (!child.isNull())
						{
							validTexture.push_back((child.tagName() == "ImageTexture"));
							child = child.nextSiblingElement();
						}
						texture = multiTexture.elementsByTagName("ImageTexture");
					}
					else
					{
						texture = appearance.elementsByTagName("ImageTexture");
						if (texture.size() > 1)
						{
							info->lineNumberError = appearance.lineNumber();
							return E_MULTITEXT;
						}
						validTexture.push_back(!texture.isEmpty());
					}
					for (int i = 0; i < texture.size(); i++)
					{
						QDomElement imageTexture = texture.at(i).toElement();
						imageTexture = solveDefUse(imageTexture, defMap);
						QString url = imageTexture.attribute("url");
						QStringList paths = url.split(" ", QString::SkipEmptyParts);
						int j = 0;
						bool found = false;
						while (j < paths.size() && !found)
						{
							QString path = paths.at(j).trimmed().remove('"');
							size_t z = 0;
							while (z < info->textuxeFile.size() && !found)
							{
								if (info->textuxeFile.at(z) == path || info->textuxeFile.at(z) == url)
								{
									TextureInfo tInfo = TextureInfo();
									tInfo.textureIndex = z;
									tInfo.repeatS = (imageTexture.attribute("repeatS", "true") == "true");
									tInfo.repeatT = (imageTexture.attribute("repeatT", "true") == "true");
									textureInfo.push_back(tInfo);
									found = true;
								}
								z++;
							}
							j++;
						}
						if (!found){
							TextureInfo tInfo = TextureInfo();
							tInfo.isValid = false;
							textureInfo.push_back(tInfo);
						}
					}
					textureTransformList = appearance.elementsByTagName("TextureTransform");
				}

				QDomElement geometryNode = root.firstChildElement();
				while (!geometryNode.isNull())
				{
					QDomElement geometry = solveDefUse(geometryNode, defMap);
					QString coordTag[] = {"Coordinate", "CoordinateDouble"};
					QDomElement coordinate = findNode(coordTag, 2, geometry);
					if (!coordinate.isNull()&& (coordinate.attribute("point") != ""))
					{
						coordinate = solveDefUse(coordinate, defMap);
						QStringList coordList;
						findAndParseAttribute(coordList, coordinate, "point", "");

						QString colorTag[] = {"Color", "ColorRGBA"};
						QDomElement color = findNode(colorTag, 2, geometry);
						color = solveDefUse(color, defMap);
						QStringList colorList;
						findAndParseAttribute(colorList, color, "color", "");

						QDomElement normal = geometry.firstChildElement("Normal");
						normal = solveDefUse(normal, defMap);
						QStringList normalList;
						findAndParseAttribute(normalList, normal, "vector", "");
						
						QString textCoorTag[] = {"TextureCoordinate", "MultiTextureCoordinate", "TextureCoordinateGenerator"};
						QDomElement textureCoord = findNode(textCoorTag, 3, geometry);
						textureCoord = solveDefUse(textureCoord, defMap);
						if (textureCoord.tagName() == "MultiTextureCoordinate")
						{
							QDomElement child = textureCoord.firstChildElement();
							size_t i= 0;
							int j = 0;
							while (!child.isNull())
							{
								if (i >= validTexture.size()) break;
								QDomElement solveChild = solveDefUse(child, defMap);
								if (validTexture.at(i))
								{
									if (solveChild.tagName() == "TextureCoordinate")
									{
										findAndParseAttribute(textureInfo[j].textureCoordList, solveChild, "point", "");
										if (textureInfo[j].textureCoordList.isEmpty())
											textureInfo[j].isValid = false;
									}
									else if (solveChild.tagName() == "TextureCoordinateGenerator")
									{
										QString mode = solveChild.attribute("mode", "SPHERE");
										textureInfo[j].mode = mode;
										textureInfo[j].parameter = solveChild.attribute("parameter");
										textureInfo[j].isValid = (mode == "COORD");
										textureInfo[j].isCoordGenerator = true;
									}
									if ( i < textureTransformList.size())										
										textureInfo[j].textureTransform = createTextureTrasformMatrix(textureTransformList.at(i).toElement());
									j++;
								}
								i++;
								child = child.nextSiblingElement();
							}
						}
						else if (textureCoord.tagName() == "TextureCoordinate")
						{
							if (validTexture.size() == 1 && validTexture.at(0))
							{
								findAndParseAttribute(textureInfo[0].textureCoordList, textureCoord, "point", "");
								if (textureInfo[0].textureCoordList.isEmpty())
									textureInfo[0].isValid = false;
								if (textureTransformList.size() > 0)
									textureInfo[0].textureTransform = createTextureTrasformMatrix(textureTransformList.at(0).toElement());
							}
						}
						else if (textureCoord.tagName() == "TextureCoordinateGenerator")
						{
							if (validTexture.size() == 1 && validTexture.at(0))
							{
								QString mode = textureCoord.attribute("mode", "SPHERE");
								textureInfo[0].mode = mode;
								textureInfo[0].parameter = textureCoord.attribute("parameter");
								textureInfo[0].isValid = (mode == "COORD");
								textureInfo[0].isCoordGenerator = true;
								if (textureTransformList.size() > 0)
									textureInfo[0].textureTransform = createTextureTrasformMatrix(textureTransformList.at(0).toElement());
							}
						}

						int colorComponent = (!color.isNull() && color.tagName() == "Color")? 3: 4;
						TextureInfo texture;
						for (size_t i = 0; i < textureInfo.size(); i++)
						{
							if (textureInfo.at(i).isValid)
							{
								texture = textureInfo.at(i);
								break;
							}
						}
						if (geometry.tagName() == "TriangleSet" || geometry.tagName() == "TriangleFanSet" || geometry.tagName() == "TriangleStripSet" || geometry.tagName() == "QuadSet")
							return LoadSet(geometry, m, tMatrix, texture, coordList, colorList, normalList, colorComponent, info, cb);
						else if (geometry.tagName() == "IndexedTriangleSet" || geometry.tagName() == "IndexedTriangleFanSet" || geometry.tagName() == "IndexedTriangleStripSet" || geometry.tagName() == "IndexedQuadSet")
							return LoadIndexedSet(geometry, m, tMatrix, texture, coordList, colorList, normalList, colorComponent, info, cb);
								
					}
					geometryNode = geometryNode.nextSiblingElement();
				}
				return E_NOERROR;

			}
			QDomElement child = root.firstChildElement();
			while(!child.isNull())
			{
				int result = NavigateScene(m, child, tMatrix, defMap, protoDeclareMap, info, cb);
				if (result != E_NOERROR)
					return result;
				child = child.nextSiblingElement();
			}
			return E_NOERROR;
		}



	public:	
	
		static int LoadMask(const char * filename, AdditionalInfoX3D*& addinfo)
		{
			AdditionalInfoX3D* info = new AdditionalInfoX3D();
			QDomDocument* doc = new QDomDocument(filename);
			info->filenameStack.push_back(QString(filename));
			addinfo = info;
			QFile file(filename);
			if (!file.open(QIODevice::ReadOnly))
				return E_CANTOPEN;
			if (!doc->setContent(&file)) 
			{
				file.close();
				return E_INVALIDXML;
			}
			file.close();
			info->doc = doc;
			info->mask = 0;
			info->filename = QString(filename);
			return LoadMaskByDom(doc, info, info->filename);
		}

		
		//merge all meshes in the x3d's file in the templeted mesh m
		static int Open(OpenMeshType& m, const char* filename, AdditionalInfoX3D*& info, CallBackPos *cb=0)
		{
			vcg::Matrix44f tMatrix;
			tMatrix.SetIdentity();
			std::vector<vcg::Matrix44f> matrixStack;
			matrixStack.push_back(tMatrix);
			std::map<QString, QDomElement> defMap;
			std::map<QString, QDomElement> protoDeclareMap;
			QDomNodeList scene = info->doc->elementsByTagName("Scene");
			info->filenameStack.clear();
			info->filenameStack.push_back(QString(filename));
			if (scene.size() == 0)
				return E_NO3DSCENE;
			if (scene.size() > 1)
				return E_MULTISCENE;
			return NavigateScene(m, scene.at(0).toElement(), tMatrix, defMap, protoDeclareMap, info, cb); 
		}

	
	};
}
}
}
#endif