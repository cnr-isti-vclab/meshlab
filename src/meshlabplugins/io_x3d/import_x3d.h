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
 Revision 1.12  2008/02/20 21:58:11  gianpaolopalma
 Added support to file .x3dv and .wrl
 Bug fixed in the management of ProtoInstance node

 Revision 1.11  2008/02/15 08:27:44  cignoni
 - '>> 'changed into '> >'
 - Used HasPerFaceSomething(M) instead of M.HasPerFaceSomething() that is deprecated.
 - many unsigned warning removed
 - added const to some functions parameters (FindDEF, FindAndReplaceUSE ...)

 Revision 1.10  2008/02/15 01:07:36  gianpaolopalma
 Bug fixed in the texture index assignment

 Revision 1.9  2008/02/14 13:05:21  gianpaolopalma
 Changed updating progress bar in the method LoadIndexedFaceSet
 Bug fixed in the method LoadMaskByDom

 Revision 1.8  2008/02/13 15:11:03  gianpaolopalma
 Bug fixed in the attribute parsing
 Bug fixed in the assignment of texture index
 Added support to attribute ccw

 Revision 1.7  2008/02/11 09:31:13  gianpaolopalma
 Big change to reduce the loading time of X3D file

 Revision 1.6  2008/02/08 17:04:32  gianpaolopalma
 Added methods to reduce loading time of X3D scene from the file

 Revision 1.5  2008/02/06 16:52:58  gianpaolopalma
 Added comments

 Revision 1.4  2008/02/06 13:12:59  gianpaolopalma
 Added support to TriangleSet2D, PointSet, Polypoint2D.
 Added inline function to set default value (color and texture coordinate) per vertex and face

 Revision 1.3  2008/02/05 16:47:49  gianpaolopalma
 Added support to follow ElevationGrid and IndexedFaceSet
 Bug fixed in the method solveDefUse

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
#include <vcg/complex/trimesh/create/platonic.h>
#include <wrap/gl/glu_tesselator.h>

#include "util_x3d.h"
#include "vrml/Parser.h"

#include <set>

namespace vcg {
namespace tri {
namespace io {

	template<typename OpenMeshType>
	class ImporterX3D : public UtilX3D
	{
	private:

		
		//search all Switch nodes and replace everyone with the child marked in the whichChoice attribute
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
						//resolve attributes USE and DEF
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
		
		
		//search all LOD nodes and replace everyone with the first child
		static void ManageLODNode(QDomDocument* doc)
		{
			QDomNodeList lodNodes = doc->elementsByTagName("LOD");
			for(int ln = 0; ln < lodNodes.size(); ln++)
			{
				QDomElement lod = lodNodes.at(ln).toElement();
				QDomNode parent = lod.parentNode();
				//Create a traslation Trasform node from attribute 'center'
				QString center = lod.attribute("center");
				QDomElement transform = doc->createElement("Transform");
				transform.setAttribute("traslation", center);
				QDomElement firstChild = lod.firstChildElement();
				if (!firstChild.isNull())
				{
					//resolve attributes USE and DEF
					QDomElement tmpQDom;
					ManageDefUse(lod, 0, tmpQDom);
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

		
		
		//search all Inline nodes and try to open the linked files
		static int ManageInlineNode(QDomDocument* doc, AdditionalInfoX3D*& info)
		{
			QDomNodeList inlineNodes = doc->elementsByTagName("Inline");
			for(int in = 0; in < inlineNodes.size(); in++)
			{
				QDomElement inl = inlineNodes.at(in).toElement();
				//Is the Inline node loadable?
				if(inl.attribute("load", "true") == "true")
				{
					QString url = inl.attribute("url");
					QStringList paths = url.split(" ", QString::SkipEmptyParts);
					int i = 0;
					bool found = false;
					QStringList vrmlPaths = QStringList();
					while (i < paths.size() && !found)
					{
						QString path = paths.at(i).trimmed().remove(QChar('"'));
						QFileInfo fi(path);
						if(fi.exists())
						{
							std::map<QString, QDomNode*>::const_iterator iter = info->inlineNodeMap.begin();
							bool load = true;
							QFileInfo tmpFi(info->filename);
							//check if exist a loop dependence
							if (tmpFi.fileName() == fi.fileName())
							{
								info->lineNumberError = inl.lineNumber();
								return E_LOOPDEPENDENCE;
							}
							while(iter != info->inlineNodeMap.end() && load)
							{
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
									//load components mesh info from file .x3d linked in Inline node
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
							else if (load && (fi.suffix().toLower() == "x3dv" || fi.suffix().toLower() == "wrl"))
								vrmlPaths.push_back(path);
						}
						i++;
					}
					int j = 0;
					//load components mesh info from file .x3dv e .wrl
					while(j < vrmlPaths.size() && !found)
					{
						QDomDocument* document = new QDomDocument();
						int result = ParseVrml(vrmlPaths.at(j).toStdString().c_str(), document);
						if (result == E_NOERROR)
						{
							info->filenameStack.push_back(vrmlPaths.at(j));
							info->inlineNodeMap[vrmlPaths.at(j)] = document;
							int result = LoadMaskByDom(document, info, vrmlPaths.at(j));
							if (result != E_NOERROR) return result;
							info->filenameStack.pop_back();
							found = true;
						}
						else
							delete document;
						j++;
					}
				}
			}
			return E_NOERROR;
		}
		

		//search all ExternProtoDeclare nodes and try to open the linked files 
		static int ManageExternProtoDeclare(QDomDocument* doc, AdditionalInfoX3D*& info, const QString filename)
		{
			QDomNodeList exProtoDeclNodes = doc->elementsByTagName("ExternProtoDeclare");
			for(int en = 0; en < exProtoDeclNodes.size(); en++)
			{
				QDomElement exProtoDecl = exProtoDeclNodes.at(en).toElement();
				QString url = exProtoDecl.attribute("url");
				QStringList paths = url.split(" ", QString::SkipEmptyParts);
				int i = 0;
				bool found = false;
				QStringList vrmlPaths = QStringList();
				while (i < paths.size() && !found)
				{
					//get file path and prototype name 
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
								//open the file linked in ExternProtoDeclare node
								if (file.open(QIODevice::ReadOnly))
								{
									if (!docChild.setContent(&file)) 
									{
										info->filenameStack.push_back(path);
										return E_INVALIDXML;
									}
									file.close();
									//search the ProtoDeclare node of the prototype
									QDomNodeList prototypes = docChild.elementsByTagName("ProtoDeclare");
									int j = 0;
									while (j < prototypes.size() && !found)
									{
										QDomElement elem = prototypes.at(j).toElement();
										if (elem.attribute("name") == namePrototype)
										{
											//load components mesh info from prototype
											found = true;
											QDomDocument* x = new QDomDocument(namePrototype);
											x->appendChild(elem.cloneNode());
											info->filenameStack.push_back(fi.fileName() + "#" + namePrototype);
											info->protoDeclareNodeMap[fi.fileName() + "#" + namePrototype] = x;
											int result = LoadMaskByDom(&docChild, info, fi.fileName());
											if (result != E_NOERROR) return result;
											info->filenameStack.pop_back();
										}
										j++;
									}
								}
							}
							else if (load && (fi.suffix().toLower() == "x3dv" || fi.suffix().toLower() == "wrl"))
								vrmlPaths.push_back(str);
						}
					}
					i++;
				}
				int t = 0;
				while (t < vrmlPaths.size() && !found)
				{
					QDomDocument* document = new QDomDocument();
					QStringList list = vrmlPaths.at(i).split("#", QString::SkipEmptyParts);
					QString path = list.at(0);
					QString protoName = "";
					if (list.size()>1)
						protoName = list.at(1);
					int result = ParseVrml(path.toStdString().c_str(), document);
					if (result == E_NOERROR)
					{					
						//search the ProtoDeclare node of the prototype
						QDomNodeList prototypes = document->elementsByTagName("ProtoDeclare");
						int j = 0;
						while (j < prototypes.size() && !found)
						{
							QDomElement elem = prototypes.at(j).toElement();
							if (elem.attribute("name") == protoName)
							{
								//load components mesh info from prototype
								found = true;
								QDomDocument* x = new QDomDocument(protoName);
								x->appendChild(elem.cloneNode());
								info->filenameStack.push_back(QFileInfo(path).fileName() + "#" + protoName);
								info->protoDeclareNodeMap[QFileInfo(path).fileName() + "#" + protoName] = x;
								int result = LoadMaskByDom(document, info, QFileInfo(path).fileName());
								if (result != E_NOERROR) return result;
								info->filenameStack.pop_back();
							}
							j++;
						}
					}
					else
						delete document;
				}
			}
			return E_NOERROR;
		}

		

		//resolve DEF and USE node in Switch and LOD node
		static void ManageDefUse(const QDomElement& swt, int whichChoice, QDomElement& res)
		{
			std::map<QString, QDomElement> def;
			QDomNodeList ndl = swt.childNodes();
			//find all node, with the attribute DEF, between the children that precede the child of index 'whichChoice'
			for(int i = 0; i < whichChoice; i++)
				if (ndl.at(i).isElement())
					FindDEF(ndl.at(i).toElement(), def);
			//resolve USE attribute in the child of index 'whichChoice'
			if(whichChoice > 0)
				FindAndReplaceUSE(res, def);
			//find all node, with the attribute DEF, between the children that follow the child of index 'whichChoice'
			for(int i = whichChoice + 1; i < ndl.size(); i++)
				if (ndl.at(i).isElement())
					FindDEF(ndl.at(i).toElement(), def);
			QDomElement parent = swt.parentNode().toElement();
			bool flag = false;
			//Find and resolve USE attribute in the node of DOM that link to DEF deleted with the Switch or LOD node
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

		
		
		//Find all node with the DEF attribute in 'elem' and its children
		static void FindDEF(const QDomElement& elem, std::map<QString, QDomElement>& map)
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
		

		//Find and replace all node with USE attribute in 'elem' and its children
		static void FindAndReplaceUSE (const QDomElement& elem, const std::map<QString, QDomElement>& map)
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

	
		
		//Find in the root's children the first node with a tagName in the array 'tagName'
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

	

		//Return true for the mode of TextureCoordinateGEnerator supported
		inline static bool isTextCoorGenSupported(const QDomElement& elem)
		{
			if (elem.isNull()) return false;
			QString mode = elem.attribute("mode", "SPHERE");
			if (mode == "COORD")
				return true;
			return false;
		}
	
		
		//Find and return the list of value of attribute in the node 'elem'
		inline static void findAndParseAttribute(QStringList& list, const QDomElement& elem, QString attribute, QString defValue)
		{
			if (elem.isNull())
			{
				list = QStringList();
				return;
			}
			QString value = elem.attribute(attribute, defValue);
			value.replace(",", " ");
			list = value.split(QRegExp("\\s+"), QString::SkipEmptyParts);
		}

		

		//Initialize a ProtoDeclare node with the fields received in input from a ProtoInstance node
		static int InitializeProtoDeclare(QDomElement& root, const std::map<QString, QString>& fields, const std::map<QString, QDomElement>& fieldsNode, const std::map<QString, QDomElement>& defMap, AdditionalInfoX3D* info)
		{
			QDomElement protoInterface = root.firstChildElement("ProtoInterface");
			QDomElement protoBody = root.firstChildElement("ProtoBody");
			if (protoInterface.isNull() || protoBody.isNull())
			{
				info->lineNumberError = root.lineNumber();
				return E_INVALIDPROTODECL;
			}
			std::map<QString, QString> defField;
			std::map<QString, QDomElement> defNodeField;
			//Get fields attribute(name, type, accessType, value) from the ProtoInterface node 
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
						defNodeField[fName] = child;
					else
						defField[fName] = fValue;
				}
				child = child.nextSiblingElement("field");
			}
			QDomNodeList isList = protoBody.elementsByTagName("IS");
			std::map<QString, QDomElement>::const_iterator iterDefField;
			std::map<QString, QString>::const_iterator iterValue;
			//Replace IS connectors with the values of fields
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
						std::map<QString, QString>::const_iterator iterDefFieldValue = defField.find(protoField);
						if (iterDefFieldValue != defField.end())
						{
							if (parent.tagName() != "ProtoInstance")
							{
								if (iterValue != fields.end())
									parent.setAttribute(nodeField, iterValue->second);
								else if (iterDefFieldValue->second != "")
									parent.setAttribute(nodeField, iterDefFieldValue->second);
							}
							else
							{
								QDomElement newFieldValue = info->doc->createElement("fieldValue");
								newFieldValue.setAttribute("name", nodeField);
								if (iterValue != fields.end())
									newFieldValue.setAttribute("value", iterValue->second);
								else if (iterDefFieldValue->second != "")
									newFieldValue.setAttribute("value", iterDefFieldValue->second);
								parent.insertAfter(newFieldValue, is);
							}
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
								parent.insertBefore(cloneDef, is);
							}
						}
						else 
						{
							std::map<QString, QDomElement>::const_iterator iterValueNode = fieldsNode.find(protoField);
							QDomElement fieldNode;
							if (iterValueNode != fieldsNode.end())
								fieldNode = iterValueNode->second;
							else
								fieldNode = iterDefField->second;
							QDomElement child = fieldNode.firstChildElement();
							while (!child.isNull())
							{
								QDomElement cloneNode = child.cloneNode().toElement();
								parent.insertBefore(cloneNode, is);
								child = child.nextSiblingElement();
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

		

		//Resolve DEF and USE attributes.
		inline static int solveDefUse(QDomElement root, std::map<QString, QDomElement>& defMap, QDomElement& dest, AdditionalInfoX3D* info)
		{
			if (root.isNull())
			{
				dest = root;
				return E_NOERROR;
			}
			QString use = root.attribute("USE");
			if (use != "")
			{
				QDomNode parent = root.parentNode();
				//check if exist a loop in the use of DEF and USE attribute
				while(!parent.isNull())
				{
					if (parent.toElement().attribute("DEF") == use && parent.toElement().tagName() == root.tagName())
					{
						info->lineNumberError = root.lineNumber();
						return E_INVALIDDEFUSE;
					}
					parent = parent.parentNode();
				}
				//Find the node with the DEF attribute equal to USE attribute
				std::map<QString, QDomElement>::const_iterator iter = defMap.find(use);
				if (iter != defMap.end())
				{
					if (root.tagName() == iter->second.tagName())
					{
						dest = iter->second; 
						return E_NOERROR;
					}
					else
					{
						info->lineNumberError = root.lineNumber();
						return E_MISMATCHDEFUSETYPE;
					}
				}
			}
			QString def = root.attribute("DEF");
			//Store the node with DEF attribute
			if (def != "" && defMap.find(def) == defMap.end())
				defMap[def] = root;
			dest = root;
			return E_NOERROR;
		}

		
		
		//Load default value of color and texture coordinate in the vertex
		inline static void loadDefaultValuePerVertex(typename OpenMeshType::VertexPointer vertex, const OpenMeshType &m, int mask)
		{
			if (mask & Mask::IOM_VERTCOLOR && m.HasPerVertexColor())
				vertex->C() = vcg::Color4b(vcg::Color4b::White);
			if (mask & Mask::IOM_VERTTEXCOORD && m.HasPerVertexTexCoord())
			{
				vertex->T() = vcg::TexCoord2<float>(0, 0);
				vertex->T().N() = -1;
			}
		}

		

		//Load default value of color and texture cooridinate per face and per wedge
		inline static void loadDefaultValuePerFace(typename OpenMeshType::FacePointer face, const OpenMeshType &m, int mask)
		{
			if (mask & Mask::IOM_FACECOLOR && HasPerFaceColor(m))
				face->C() = vcg::Color4b(vcg::Color4b::White);
			if (mask & Mask::IOM_WEDGCOLOR && HasPerWedgeColor(m))
				for(int i = 0; i < 3; i++)
					face->WC(i) = vcg::Color4b(vcg::Color4b::White);
			if (mask & Mask::IOM_WEDGTEXCOORD && HasPerWedgeTexCoord(m))
			{
				for(int i = 0; i < 3; i++)
				{
					face->WT(i) = vcg::TexCoord2<float>(0, 0);
					face->WT(i).N() = -1;
				}
			}
		}

		

		//Load components mesh info from DOM
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
			
			//search and replace Switch node
			ManageSwitchNode(doc);
			//search and replace LOD node
			ManageLODNode(doc);
			//search and check Inline node
			int result = ManageInlineNode(doc, info);
			if (result != E_NOERROR) return result;
			//search and check ExternProtoDeclare node
			result = ManageExternProtoDeclare(doc, info, filename);
			if (result != E_NOERROR) return result;
			
			QDomNodeList shapeNodes = doc->elementsByTagName("Shape");
			for (int s = 0; s < shapeNodes.size(); s++)
			{
				QDomElement shape = shapeNodes.at(s).toElement();
				QDomElement appearance = shape.firstChildElement("Appearance");
				std::vector<QString> textureFile;
				//If exist an Appearance node load texture info
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
									textureFile.push_back(path);
									load = true;
								}
								j++;
							}
							if (!load)
								textureFile.push_back(url);
						}
					}
					QDomElement materialNode = appearance.firstChildElement("Material");
					if (!materialNode.attribute("diffuseColor", "").isEmpty())
						bHasPerVertexColor = true; 
				}
				QDomElement geometry = shape.firstChildElement();
				while(!geometry.isNull())
				{
					QString tagName = geometry.tagName();
					QString coorTag[] = {"Coordinate", "CoordinateDouble"};
					QDomElement coordinate = findNode(coorTag, 2, geometry);
					//If geometry node is supported, get info on color, normal and texture coordinate (per vertex, per color, or per wedge)
					if ((!coordinate.isNull() && ((coordinate.attribute("point")!= "") || coordinate.attribute("USE", "") != "")) || (tagName == "ElevationGrid"))
					{
						bool copyTextureFile = true;
						QStringList colorList, normalList, textureList;
						QString colorTag[] = {"Color", "ColorRGBA"};
						QDomElement color = findNode(colorTag, 2, geometry);
						findAndParseAttribute(colorList, color, "color", "");
						QDomElement normal = geometry.firstChildElement("Normal");
						findAndParseAttribute(normalList, normal, "point", "");
						QString textCoorTag[] = {"TextureCoordinate", "MultiTextureCoordinate", "TextureCoordinateGenerator"};
						QDomElement textureCoor = findNode(textCoorTag, 3, geometry);
						findAndParseAttribute(textureList, textureCoor, "point", "");
						QString colorPerVertex = geometry.attribute("colorPerVertex", "true");
						QString normalPerVertex = geometry.attribute("normalPerVertex", "true");
 						bool textureGenSup = isTextCoorGenSupported(textureCoor);
						if (tagName == "IndexedTriangleFanSet" || tagName == "IndexedTriangleSet" || tagName == "IndexedTriangleStripSet" || tagName == "IndexedQuadSet")
						{
							QStringList coordIndex;
							findAndParseAttribute(coordIndex, geometry, "index", "");
							if (!coordIndex.isEmpty())
							{
								if (!color.isNull() && (!colorList.isEmpty() || color.attribute("USE", "") != "")) bHasPerVertexColor = true;
								if (!textureCoor.isNull() && (textureGenSup || !textureList.isEmpty() || textureCoor.attribute("USE", "") != "") && textureFile.size()>0) 
									bHasPerVertexText = true;
								else 
									copyTextureFile = false;
								if (!normal.isNull() && (!normalList.isEmpty() || normal.attribute("USE", "") != ""))
								{
									if (normalPerVertex == "true")
										bHasPerVertexNormal = true;
									else
										bHasPerFaceNormal = true;
								}
							}
						}
						else if (tagName == "TriangleFanSet" || tagName == "TriangleSet" || tagName == "TriangleStripSet" || tagName == "QuadSet")
						{
							if (!color.isNull() && (!colorList.isEmpty() || color.attribute("USE", "") != "")) bHasPerWedgeColor = true;
							if (!textureCoor.isNull() && (textureGenSup || !textureList.isEmpty() || textureCoor.attribute("USE", "") != "") && textureFile.size()>0)
								bHasPerWedgeTexCoord = true;
							else
								copyTextureFile = false;
							if (!normal.isNull() && (!normalList.isEmpty() || normal.attribute("USE", "") != ""))
							{
								if (normalPerVertex == "true")
									bHasPerWedgeNormal = true;
								else
									bHasPerFaceNormal = true;
							}
						}
						else if (tagName == "IndexedFaceSet")
						{
							QStringList colorIndex, normalIndex, texCoordIndex, coordIndex;
							findAndParseAttribute(colorIndex, geometry, "colorIndex", "");
							findAndParseAttribute(normalIndex, geometry, "normalIndex", "");
							findAndParseAttribute(texCoordIndex, geometry, "texCoordIndex", "");
							findAndParseAttribute(coordIndex, geometry, "coordIndex", "");
							if (!coordIndex.isEmpty())
							{
								if (!textureCoor.isNull() && (textureGenSup || !textureList.isEmpty() || textureCoor.attribute("USE", "") != "") && textureFile.size()>0)
								{
									if (!texCoordIndex.isEmpty())
										bHasPerWedgeTexCoord = true;
									else
										bHasPerVertexText = true;
								}
								else
									copyTextureFile = false;
								if (!color.isNull() && (!colorList.isEmpty() || color.attribute("USE", "") != ""))
								{
									if (colorPerVertex == "true" && !colorIndex.isEmpty())
										bHasPerWedgeColor = true;
									else if (colorPerVertex == "true" && colorIndex.isEmpty())
										bHasPerVertexColor = true;
									else
										bHasPerFaceColor = true;
								}
								if (!normal.isNull() && (!normalList.isEmpty() || normal.attribute("USE", "") != ""))
								{
									if (normalPerVertex == "true" && !normalIndex.isEmpty())
										bHasPerWedgeNormal = true;
									else if (normalPerVertex == "true" && normalIndex.isEmpty())
										bHasPerVertexNormal = true;
									else
										bHasPerFaceNormal = true;
								}
							}
						}
						else if (tagName == "ElevationGrid")
						{
							int xDimension = geometry.attribute("xDimension", "0").toInt();
							int zDimension = geometry.attribute("zDimension", "0").toInt();
							if (xDimension != 0 && zDimension!=0)
							{
								if (!textureCoor.isNull() && (textureGenSup || !textureList.isEmpty() || textureCoor.attribute("USE", "") != "") && textureFile.size()>0)
									bHasPerVertexText = true;
								else
									copyTextureFile = false;
								if (!color.isNull() && (!colorList.isEmpty() || color.attribute("USE", "") != ""))
								{
									if (colorPerVertex == "true")
										bHasPerVertexColor = true;
									else
										bHasPerFaceColor = true;
								}
								if (!normal.isNull() && (!normalList.isEmpty() || normal.attribute("USE", "") != ""))
								{
									if (normalPerVertex == "true")
										bHasPerVertexNormal = true;
									else
										bHasPerFaceNormal = true;
								}
							}
						}
						else if (tagName == "PointSet" && !color.isNull() && (!colorList.isEmpty() || color.attribute("USE", "") != ""))
						{
							bHasPerVertexColor = true;
							copyTextureFile = false;
						}
						else
							copyTextureFile = false;
						if (copyTextureFile)
						{
							for (size_t i = 0; i < textureFile.size(); i++)
								info->textureFile.push_back(textureFile.at(i));
						}
					}
					geometry = geometry.nextSiblingElement();
				}
			}
			
			std::map<QString, QDomElement> defUse;
			info->numface += countObject(doc->firstChildElement(), defUse);
			//set mask
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

		

		//Return the number of x3d object in the scene
		static int countObject(const QDomElement& root, std::map<QString, QDomElement>& defUse)
		{
			if (root.isNull()) return 0;
			int n = 0;
			if (root.tagName() == "Shape")
				return ++n;
			if (root.attribute("DEF") != "")
				defUse[root.attribute("DEF")] = root;
			else if (root.attribute("USE") != "")
			{
				std::map<QString, QDomElement>::const_iterator iter = defUse.find(root.attribute("USE"));
				if (iter != defUse.end())
				{
					n += countObject(iter->second, defUse);
					return n;
				}
			}
			QDomElement child = root.firstChildElement();
			while(!child.isNull())
			{
				n += countObject(child, defUse);
				child = child.nextSiblingElement();
			}
			return n;
		}
		

		//Load in the mesh the geometry defined in the nodes TriangleSet, TriangleFanSet, TriangleStripSet, and QuadSet
		static int LoadSet(QDomElement geometry,
									OpenMeshType& m,
									const vcg::Matrix44f tMatrix,
									const TextureInfo& texture,
									const QStringList& coordList,
									const QStringList& colorList,
									const QStringList& normalList,
									int colorComponent,
									AdditionalInfoX3D* info,
									CallBackPos *cb)
		{
			QString normalPerVertex = geometry.attribute("normalPerVertex", "true");
			QString ccw = geometry.attribute("ccw", "true");
			std::vector<vcg::Point4f> vertexSet;
			int index = 0;
			std::vector<int> vertexFaceIndex;
			//create list of vertex index
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
			//Load vertexs in the mesh
			int offset = m.vert.size();
			vcg::tri::Allocator<OpenMeshType>::AddVertices(m, vertexSet.size());
			for (size_t vv = 0; vv < vertexSet.size(); vv++)
			{
				vcg::Point4f tmp = tMatrix * vertexSet.at(vv);
				m.vert[offset + vv].P() = vcg::Point3f(tmp.X(),tmp.Y(),tmp.Z());
				loadDefaultValuePerVertex(&(m.vert[offset + vv]), m, info->mask);
				if (m.HasPerVertexColor() && (info->mask & vcg::tri::io::Mask::IOM_VERTCOLOR) && info->meshColor)
					m.vert[offset + vv].C() = info->color;
			}
			//Load faces in the mesh
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
						int vertIndexPerFace = tt;
						if (ccw == "false")
							vertIndexPerFace = 2 - tt;
						m.face[faceIndex].V(vertIndexPerFace) = &(m.vert[vertexFaceIndex.at(tt + ff*3) + offset]);
						//Load normal per wedge
						if (HasPerWedgeNormal(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGNORMAL) && normalPerVertex == "true")
							getNormal(normalList, (tt + ff*3)*3, m.face[faceIndex].WN(vertIndexPerFace), tMatrix);
						//Load color per wedge
						if (HasPerWedgeColor(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGCOLOR))
							getColor(colorList, colorComponent, (tt + ff*3)*colorComponent, m.face[faceIndex].WC(vertIndexPerFace), vcg::Color4b(Color4b::White));
						//Load textureCoordinate per wedge
						if (HasPerWedgeTexCoord(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD))
							getTextureCoord(texture, (tt + ff*3)*2, m.vert[vertexFaceIndex.at(tt + ff*3) + offset].cP(), m.face[faceIndex].WT(vertIndexPerFace), tMatrix);
					}
					//Load normal per face
					if (HasPerFaceNormal(m) && normalPerVertex == "false" && (info->mask & vcg::tri::io::Mask::IOM_FACENORMAL))
						getNormal(normalList, ff*3, m.face[faceIndex].N(), tMatrix);
						
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
				//Get the length of each fans or strip
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
				//For each fan or strip
				for (size_t ns = 0; ns < count.size() && ff < nFace; ns++)
				{
					//Get first and second vertex(center and last vertex in TriangleFanSet, last two vertex in TriangleStripSet) 
					int numVertex = count.at(ns);
					int firstVertexIndex = vertexFaceIndex.at(index) + offset;
					int secondVertexIndex = vertexFaceIndex.at(index + 1) + offset;
					vcg::Point3f firstNormal, secondNormal;
					if (normalPerVertex == "true")
					{
						getNormal(normalList, index*3, firstNormal, tMatrix);
						getNormal(normalList, index*3 + 3, secondNormal, tMatrix);
					}
					vcg::Color4b firstColor, secondColor;
					getColor(colorList, colorComponent, index*colorComponent, firstColor, vcg::Color4b(Color4b::White));
					getColor(colorList, colorComponent, index*colorComponent + colorComponent, secondColor, vcg::Color4b(Color4b::White));
					vcg::TexCoord2<float> firstTextCoord, secondTextCoord;
					getTextureCoord(texture, index*2, m.vert[vertexFaceIndex.at(index) + offset].cP(), firstTextCoord, tMatrix);
					getTextureCoord(texture, index*2, m.vert[vertexFaceIndex.at(index + 1) + offset].cP(), secondTextCoord, tMatrix);
					for(int vi = 2; vi < numVertex; vi++)
					{
						int vertIndexPerFace = 0;
						if (ccw == "false")
							vertIndexPerFace = 2;
						//Load vertex per face and load color, normal and texture coordinate per wedge
						int faceIndex = ff + offsetFace;
						m.face[faceIndex].V(vertIndexPerFace) = &(m.vert[firstVertexIndex]);
						m.face[faceIndex].V(1) = &(m.vert[secondVertexIndex]);
						if (HasPerWedgeNormal(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGNORMAL) && normalPerVertex == "true" )
						{
							m.face[faceIndex].WN(vertIndexPerFace) = firstNormal;
							m.face[faceIndex].WN(1) = secondNormal;
						}
						if (HasPerWedgeColor(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGCOLOR))
						{
							m.face[faceIndex].WC(vertIndexPerFace) = firstColor;
							m.face[faceIndex].WC(1) = secondColor;
						}
						if (HasPerWedgeTexCoord(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD))
						{
							m.face[faceIndex].WT(vertIndexPerFace) = firstTextCoord;
							m.face[faceIndex].WT(1) = secondTextCoord;
						}
						
						m.face[faceIndex].V(2 - vertIndexPerFace) = &(m.vert[vertexFaceIndex.at(index + vi) + offset]);
						if ((info->mask & vcg::tri::io::Mask::IOM_WEDGNORMAL) && normalPerVertex == "true" && HasPerWedgeNormal(m))
							getNormal(normalList, (index + vi)*3, m.face[faceIndex].WN(2 - vertIndexPerFace), tMatrix);
						if ((info->mask & vcg::tri::io::Mask::IOM_FACENORMAL) && normalPerVertex == "false" && HasPerFaceNormal(m))
							getNormal(normalList, ff*3, m.face[faceIndex].N(), tMatrix);
						if ((info->mask & vcg::tri::io::Mask::IOM_WEDGCOLOR) && HasPerWedgeColor(m))
							getColor(colorList, colorComponent, (index + vi)*colorComponent, m.face[faceIndex].WC(2 - vertIndexPerFace), vcg::Color4b(Color4b::White));
						if (HasPerWedgeTexCoord(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD))
							getTextureCoord(texture, (index + vi)*2, m.vert[vertexFaceIndex.at(index + vi) + offset].cP(), m.face[faceIndex].WT(2 - vertIndexPerFace), tMatrix); 
						
						//Update first two vertex for the next face
						if (geometry.tagName() == "TriangleStripSet")
						{
							firstVertexIndex = secondVertexIndex;
							firstColor = secondColor;
							firstNormal = secondNormal;
							firstTextCoord = secondTextCoord;
						}
						secondVertexIndex = vertexFaceIndex.at(index + vi) + offset;
						if (HasPerWedgeColor(m))
							secondColor = m.face[faceIndex].WC(2 - vertIndexPerFace);
						if (HasPerWedgeNormal(m))
							secondNormal = m.face[faceIndex].WN(2 - vertIndexPerFace);
						if (HasPerWedgeTexCoord(m))
								secondTextCoord = m.face[faceIndex].WT(2 - vertIndexPerFace);
						ff++;
					}
					index += numVertex;
				}
			}
			else if (geometry.tagName() == "QuadSet")
			{
				nFace = vertexFaceIndex.size()/4;
				int nTriFace = 0;
				std::vector<std::vector<int> > faceVect;
				for (int ff = 0; ff < nFace; ff++)
				{
					//Tesselate the quadrangular face
					std::vector<std::vector<vcg::Point3f> > polygonVect;
					std::vector<Point3f> polygon;
					for (int tt = 0; tt < 4; tt++)
						polygon.push_back(m.vert[vertexFaceIndex.at(tt + ff*4) + offset].cP());
					polygonVect.push_back(polygon);
					std::vector<int> indexVect;
					vcg::glu_tesselator::tesselate<vcg::Point3f>(polygonVect, indexVect);
					faceVect.push_back(indexVect);
					nTriFace += indexVect.size()/3;
				}
				vcg::tri::Allocator<OpenMeshType>::AddFaces(m, nTriFace);
				int faceIndex = offsetFace;
				for (int ff = 0; ff < nFace; ff++)
				{
					std::vector<int> indexVect = faceVect.at(ff);
					size_t iv = 0;
					//Load the triangular face obtained by tessellation
					while (iv + 2 < indexVect.size())
					{
						for (int tt = 0; tt < 3; tt++)
						{
							int vertIndexPerFace = tt;
							if (ccw == "false")
								vertIndexPerFace = 2 - tt;
							int indexVertex = indexVect.at(iv) + ff*4;
							m.face[faceIndex].V(vertIndexPerFace) = &(m.vert[vertexFaceIndex.at(indexVertex) + offset]);
							//Load normal per wedge
							if (HasPerWedgeNormal(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGNORMAL) && normalPerVertex == "true")
								getNormal(normalList, indexVertex*3, m.face[faceIndex].WN(vertIndexPerFace), tMatrix);
							//Load color per wedge
							if (HasPerWedgeColor(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGCOLOR))
								getColor(colorList, colorComponent, indexVertex*colorComponent, m.face[faceIndex].WC(vertIndexPerFace), vcg::Color4b(Color4b::White));
							//Load texture coordinate per wedge
							if (HasPerWedgeTexCoord(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD))
								getTextureCoord(texture, indexVertex*2, m.vert[vertexFaceIndex.at(indexVertex) + offset].cP(), m.face[faceIndex].WT(vertIndexPerFace), tMatrix); 
							iv++;
						}
						//Load normal per face
						if (HasPerFaceNormal(m) && normalPerVertex == "false" && (info->mask & vcg::tri::io::Mask::IOM_FACENORMAL))
							getNormal(normalList, ff*3, m.face[faceIndex].N(), tMatrix);
						faceIndex++;
					}
				}
			}
			info->numvert++;
			if (cb !=NULL) (*cb)(10 + 80*info->numvert/info->numface, "Loading X3D Object...");
			return E_NOERROR;
		}

		

		//Load in the mesh the geometry defined in the nodes IndexedTriangleSet, IndexedTriangleFanSet, IndexedTriangleStripSet, IndexedQuadSet
		static int LoadIndexedSet(QDomElement geometry,
									OpenMeshType& m,
									const vcg::Matrix44f tMatrix,
									const TextureInfo& texture,
									const QStringList& coordList,
									const QStringList& colorList,
									const QStringList& normalList,
									int colorComponent,
									AdditionalInfoX3D* info,
									CallBackPos *cb)
		{
			QStringList indexList;
			findAndParseAttribute(indexList, geometry, "index", "");
			if (!indexList.isEmpty())
			{
				QString normalPerVertex = geometry.attribute("normalPerVertex", "true");
				QString ccw = geometry.attribute("ccw", "true");
				//Load vertex in the mesh
				int offset = m.vert.size();
				int nVertex = coordList.size()/3;
				vcg::tri::Allocator<OpenMeshType>::AddVertices(m, nVertex);
				vcg::Color4b defValue;
				if (info->meshColor)
					defValue = info->color;
				else
					defValue = vcg::Color4b(Color4b::White);
				for (int vv = 0; vv < nVertex; vv++)
				{
					vcg::Point4f tmp = tMatrix * vcg::Point4f(coordList.at(vv*3).toFloat(), coordList.at(vv*3 + 1).toFloat(), coordList.at(vv*3 + 2).toFloat(), 1.0);
					m.vert[offset + vv].P() = vcg::Point3f(tmp.X(),tmp.Y(),tmp.Z());
					//Load normal per vertex
					if (m.HasPerVertexNormal() && (info->mask & vcg::tri::io::Mask::IOM_VERTNORMAL) && normalPerVertex == "true")
						getNormal(normalList, vv*3, m.vert[offset + vv].N(), tMatrix);
					//Load color per vertex
					if (m.HasPerVertexColor() && (info->mask & vcg::tri::io::Mask::IOM_VERTCOLOR))
						getColor(colorList, colorComponent, vv*3, m.vert[offset + vv].C(), defValue);
					//Load texture coordinate per vertex
					if (m.HasPerVertexTexCoord() && (info->mask & vcg::tri::io::Mask::IOM_VERTCOORD))
						getTextureCoord(texture, vv*2, m.vert[offset + vv].cP(), m.vert[offset + vv].T(), tMatrix);
				}
				//Load face in the mesh
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
							int vertIndexPerFace = tt;
							if (ccw == "false")
								vertIndexPerFace = 2 - tt;
							size_t vertIndex = indexList.at(tt + ff*3).toInt() + offset;
							if (vertIndex >= m.vert.size())
							{
								info->lineNumberError = geometry.lineNumber();
								return E_INVALIDINDEXED;
							}
							m.face[faceIndex].V(vertIndexPerFace) = &(m.vert[vertIndex]);
							//Load texture coordinate per wedge
							if (!m.HasPerVertexTexCoord() && HasPerWedgeTexCoord(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD))
								getTextureCoord(texture, indexList.at(tt + ff*3).toInt()*2, m.vert[vertIndex].cP(), m.face[faceIndex].WT(vertIndexPerFace), tMatrix);
							if (HasPerWedgeColor(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGCOLOR))
								m.face[faceIndex].WC(vertIndexPerFace) = vcg::Color4b(vcg::Color4b::White);
 						}
						//Load normal per face
						if (HasPerFaceNormal(m) && normalPerVertex == "false" && (info->mask & vcg::tri::io::Mask::IOM_FACENORMAL))
							getNormal(normalList, ff*3, m.face[faceIndex].N(), tMatrix);
						if (HasPerFaceColor(m) && (info->mask & vcg::tri::io::Mask::IOM_FACECOLOR))
							m.face[faceIndex].C() = vcg::Color4b(vcg::Color4b::White);
					}
				}								
				else if (geometry.tagName() == "IndexedTriangleFanSet" || geometry.tagName() == "IndexedTriangleStripSet")
				{									
					int count = 0;
					int pos = indexList.indexOf("-1");
					//Check it the fans or the strips is correct
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
					size_t vertIndex;
					for (int ls = 0; ls < indexList.size() && ff < nFace; ls++)
					{
						if (indexList.at(ls) == "-1" || ls == 0)
						{
							//Get the first two vertex
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
						//Load vertex per face and load color, normal and texture coordinate per wedge
						int vertIndexPerFace = 0;
						if (ccw == "false")
							vertIndexPerFace = 2;
						int faceIndex = ff + offsetFace;
						m.face[faceIndex].V(vertIndexPerFace) = &(m.vert[firstVertexIndex]);
						m.face[faceIndex].V(1) = &(m.vert[secondVertexIndex]);
						if(!m.HasPerVertexTexCoord() && HasPerWedgeTexCoord(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD))
						{
								getTextureCoord(texture, (firstVertexIndex - offset)*2, m.vert[firstVertexIndex].cP(), m.face[faceIndex].WT(vertIndexPerFace), tMatrix);
								getTextureCoord(texture, (secondVertexIndex - offset)*2, m.vert[firstVertexIndex].cP(), m.face[faceIndex].WT(1), tMatrix);
						}

 						vertIndex = indexList.at(ls).toInt() + offset;
						if (vertIndex >= m.vert.size())
						{
							info->lineNumberError = geometry.lineNumber();
							return E_INVALIDINDEXED;
						}
						m.face[faceIndex].V(2 - vertIndexPerFace) = &(m.vert[vertIndex]);
						//Load normal per face
						if (HasPerFaceNormal(m) && (info->mask & vcg::tri::io::Mask::IOM_FACENORMAL) && normalPerVertex == "false")
							getNormal(normalList, ff*3, m.face[faceIndex].N(), tMatrix);
						if(!m.HasPerVertexTexCoord() && HasPerWedgeTexCoord(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD))
							getTextureCoord(texture, (vertIndex - offset)*2, m.vert[vertIndex].cP(), m.face[faceIndex].WT(2 - vertIndexPerFace), tMatrix);
						if (geometry.tagName() == "IndexedTriangleStripSet")
							firstVertexIndex = secondVertexIndex;
						secondVertexIndex = vertIndex;
						if (HasPerFaceColor(m) && (info->mask & vcg::tri::io::Mask::IOM_FACECOLOR))
							m.face[faceIndex].C() = vcg::Color4b(vcg::Color4b::White);
						if (HasPerWedgeColor(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGCOLOR))
						{
								m.face[faceIndex].WC(0) = vcg::Color4b(vcg::Color4b::White);
								m.face[faceIndex].WC(1) = vcg::Color4b(vcg::Color4b::White);
								m.face[faceIndex].WC(2) = vcg::Color4b(vcg::Color4b::White);
						}
						ff++;
					}
				}
				else if (geometry.tagName() == "IndexedQuadSet")
				{
					nFace = indexList.size()/4;
					std::vector<std::vector<int> > faceVect;
					int nTriFace = 0;
					for (int ff = 0; ff < nFace; ff++)
					{
						//Tessellate the quadrangular face
						std::vector<std::vector<vcg::Point3f> > polygonVect;
						std::vector<Point3f> polygon;
						for (int tt = 0; tt < 4; tt++)
						{
							size_t vertIndex = indexList.at(tt + ff*4).toInt() + offset;
							if (vertIndex >= m.vert.size())
							{
								info->lineNumberError = geometry.lineNumber();
								return E_INVALIDINDEXED;
							}
							polygon.push_back(m.vert[indexList.at(tt + ff*4).toInt() + offset].cP());
						}
						polygonVect.push_back(polygon);
						std::vector<int> indexVect;
						vcg::glu_tesselator::tesselate<vcg::Point3f>(polygonVect, indexVect);
						faceVect.push_back(indexVect);
						nTriFace += indexVect.size()/3;
					}
					vcg::tri::Allocator<OpenMeshType>::AddFaces(m, nFace * 2);
					int faceIndex = offsetFace;
					for (int ff = 0; ff < nFace; ff++)
					{
						std::vector<int> indexVect = faceVect.at(ff);
						size_t iv = 0;
						//Load the triangular face obtained from tessellation
						while (iv + 2 < indexVect.size())
						{
							for (int tt = 0; tt < 3; tt++)
							{
								int vertIndexPerFace = tt;
								if (ccw == "false")
									vertIndexPerFace = 2 - tt;
								int indexVertex = indexVect.at(iv) + ff*4;
								m.face[faceIndex].V(vertIndexPerFace) = &(m.vert[indexList.at(indexVertex).toInt() + offset]);
								//Load texture coordinate per wedge
								if(!m.HasPerVertexTexCoord() && HasPerWedgeTexCoord(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD))
									getTextureCoord(texture, indexList.at(indexVertex).toInt()*2, m.vert[indexList.at(indexVertex).toInt() + offset].cP(), m.face[faceIndex].WT(vertIndexPerFace), tMatrix);
								if (HasPerWedgeColor(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGCOLOR))
									m.face[faceIndex].WC(vertIndexPerFace) = vcg::Color4b(vcg::Color4b::White);
								iv++;
							}
							//Load normal per face
							if (HasPerFaceNormal(m) && normalPerVertex == "false" && (info->mask & vcg::tri::io::Mask::IOM_FACENORMAL))
								getNormal(normalList, ff*3, m.face[faceIndex].N(), tMatrix);
							if (HasPerFaceColor(m) && (info->mask & vcg::tri::io::Mask::IOM_FACECOLOR))
								m.face[faceIndex].C() = vcg::Color4b(vcg::Color4b::White);
							faceIndex++;
						}
					}
				}
			}
			info->numvert++;
			if (cb !=NULL) (*cb)(10 + 80*info->numvert/info->numface, "Loading X3D Object...");
			return E_NOERROR;
		}

		
		
		//Load in the mesh the geometry defined in the node ElevationGrid		
		static int LoadElevationGrid(QDomElement geometry,
									OpenMeshType& m,
									const vcg::Matrix44f tMatrix,
									const TextureInfo& texture,
									const QStringList& colorList,
									const QStringList& normalList,
									int colorComponent,
									AdditionalInfoX3D* info,
									CallBackPos *cb)
		{
			QString colorPerVertex = geometry.attribute("colorPerVertex", "true");
			QString normalPerVertex = geometry.attribute("normalPerVertex", "true");
			QString ccw = geometry.attribute("ccw", "true");
			int xDimension = geometry.attribute("xDimension", "0").toInt();
			int zDimension = geometry.attribute("zDimension", "0").toInt();
			float xSpacing = geometry.attribute("xSpacing", "1.0").toFloat();
			float zSpacing = geometry.attribute("zSpacing", "1.0").toFloat();
			QStringList heightList;
			findAndParseAttribute(heightList, geometry, "height", "");
			if (xDimension <= 0 || zDimension <= 0) return E_NOERROR;
			if (heightList.size() < (xDimension * zDimension))
			{
				info->lineNumberError = geometry.lineNumber();
				return E_INVALIDELEVATIONGRID;
			}
			//Get heights vector
			std::vector<float> heightVector;
			for (int i = 0; i < heightList.size(); i++)
				heightVector.push_back(heightList.at(i).toFloat());
			int offsetVertex = m.vert.size();
			int offsetFace = m.face.size();
			vcg::tri::Allocator<OpenMeshType>::AddVertices(m, xDimension * zDimension);
			vcg::tri::Allocator<OpenMeshType>::AddFaces(m, (xDimension - 1)*(zDimension - 1)*2);
			//Load vertex in the mesh
			int index = 0;
			vcg::Color4b defValue;
			if (info->meshColor)
				defValue = info->color;
			else
				defValue = vcg::Color4b(Color4b::White);
			for (int i=0; i < zDimension; i++)
			{
				for (int j=0; j < xDimension; j++)
				{
					vcg::Point4f in(j * xSpacing, heightVector[i * xDimension + j], i * zSpacing, 1.0);
					in = tMatrix * in;
					index = i * xDimension + j;
					m.vert[index + offsetVertex].P()= vcg::Point3f(in.X(), in.Y(), in.Z());
					loadDefaultValuePerVertex(&(m.vert[index + offsetVertex]), m, info->mask);
					//Load color per vertex
					if (m.HasPerVertexColor() && (info->mask & vcg::tri::io::Mask::IOM_VERTCOLOR))
					{
						if (colorPerVertex == "true")
							getColor(colorList, colorComponent, index * colorComponent, m.vert[index + offsetVertex].C(), defValue);
						else
							m.vert[index + offsetVertex].C() = defValue;
					}
					//Load normal per vertex
					if (m.HasPerVertexNormal() && normalPerVertex == "true" && (info->mask & vcg::tri::io::Mask::IOM_VERTNORMAL))
						getNormal(normalList, index * 3, m.vert[index + offsetVertex].N(), tMatrix);
					//Load texture coordinate per vertex
					if (m.HasPerVertexTexCoord() && (info->mask & vcg::tri::io::Mask::IOM_VERTTEXCOORD))
						getTextureCoord(texture, index * 2, m.vert[index + offsetVertex].cP(), m.vert[index + offsetVertex].T(), tMatrix);
				}
			}
			//Load face in the mesh
			for (int i = 0; i < zDimension - 1; i++)
			{
				for (int j = 0; j < xDimension - 1; j++)
				{
					index = 2 * (i * (xDimension - 1) + j);
					//Load first face
					for (int ff = 0; ff < 2; ff++)
					{
						int val[3][2] = {{i, j}, {i+1, j+1}, {i, j+1}};
						if (ff == 1)
						{
							val[1][1] = j;
							val[2][0] = i+1;
						}
						loadDefaultValuePerFace(&(m.face[index + offsetFace]), m, info->mask);
						for (int tt = 0; tt < 3; tt++)
						{
							int vertIndexPerFace = tt;
							if (ccw == "false")
								vertIndexPerFace = 2 - tt;
							m.face[index + offsetFace].V(vertIndexPerFace) = &(m.vert[val[tt][0] * xDimension + val[tt][1] + offsetVertex]);
							//Load texture coordinate per wedge
							if (!m.HasPerVertexTexCoord() && HasPerWedgeTexCoord(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD))
								getTextureCoord(texture, (val[tt][0] * xDimension + val[tt][1])*2, m.vert[val[tt][0] * xDimension + val[tt][1] + offsetVertex].cP(), m.face[index + offsetFace].WT(vertIndexPerFace), tMatrix);
							if (HasPerWedgeColor(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGCOLOR))
								m.face[index + offsetFace].WC(vertIndexPerFace) = vcg::Color4b(vcg::Color4b::White);
						}
						//Load color per face
						if (HasPerFaceColor(m) && (info->mask & vcg::tri::io::Mask::IOM_FACECOLOR))
						{
							if (colorPerVertex == "false")
								getColor(colorList, colorComponent, (index + ff) * colorComponent, m.face[index + offsetFace].C(), vcg::Color4b(Color4b::White));
							else
								m.face[index + offsetFace].C() = vcg::Color4b(vcg::Color4b::White);
						}
						//Load normal per face
						if (HasPerFaceNormal(m) &&  (info->mask & vcg::tri::io::Mask::IOM_FACENORMAL) && normalPerVertex == "false")
							getNormal(normalList, (index + ff) * 3, m.face[index + offsetFace].N(), tMatrix);
						index++;
					}
				}
			}
			info->numvert++;
			if (cb !=NULL) (*cb)(10 + 80*info->numvert/info->numface, "Loading X3D Object...");
			return E_NOERROR;
		}

		
		
		//Load in the mesh the geometry defined in the node IndexedFaceSet
		static int LoadIndexedFaceSet(QDomElement geometry,
									OpenMeshType& m,
									const vcg::Matrix44f tMatrix,
									const TextureInfo& texture,
									const QStringList& coordList,
									const QStringList& colorList,
									const QStringList& normalList,
									int colorComponent,
									AdditionalInfoX3D* info,
									CallBackPos *cb)
		{
			QStringList coordIndex;
			findAndParseAttribute(coordIndex, geometry, "coordIndex", "");
			if (!coordIndex.isEmpty())
			{
				QString normalPerVertex = geometry.attribute("normalPerVertex", "true");
				QString colorPerVertex = geometry.attribute("colorPerVertex", "true");
				QString ccw = geometry.attribute("ccw", "true");
				QStringList colorIndex, normalIndex, texCoordIndex;
				findAndParseAttribute(colorIndex, geometry, "colorIndex", "");
				findAndParseAttribute(normalIndex, geometry, "normalIndex", "");
				findAndParseAttribute(texCoordIndex, geometry, "texCoordIndex", "");
			
				int offset = m.vert.size();
				int nVertex = coordList.size()/3;
				//Load vertex in the mesh
				vcg::tri::Allocator<OpenMeshType>::AddVertices(m, nVertex);
				vcg::Color4b defValue;
				if (info->meshColor)
					defValue = info->color;
				else
					defValue = vcg::Color4b(Color4b::White);
				for (int vv = 0; vv < nVertex; vv++)
				{
					vcg::Point4f tmp = tMatrix * vcg::Point4f(coordList.at(vv*3).toFloat(), coordList.at(vv*3 + 1).toFloat(), coordList.at(vv*3 + 2).toFloat(), 1.0);
					m.vert[offset + vv].P() = vcg::Point3f(tmp.X(),tmp.Y(),tmp.Z());
					//Load color per vertex
					if (m.HasPerVertexColor() && (info->mask & vcg::tri::io::Mask::IOM_VERTCOLOR))
					{
						if (colorPerVertex == "true")
							getColor(colorList, colorComponent, vv*colorComponent, m.vert[offset + vv].C(), defValue);
						else
							m.vert[offset + vv].C() = defValue;
					}
					//Load normal per vertex
					if (m.HasPerVertexNormal() && normalPerVertex == "true" && (info->mask & vcg::tri::io::Mask::IOM_VERTNORMAL))
						getNormal(normalList, vv * 3, m.vert[offset + vv].N(), tMatrix);
					//Load texture coordinate per vertex
					if (m.HasPerVertexTexCoord() && (info->mask & vcg::tri::io::Mask::IOM_VERTTEXCOORD))
						getTextureCoord(texture, vv * 2, m.vert[offset + vv].cP(), m.vert[offset + vv].T(), tMatrix);
					if (cb !=NULL && (vv%1000 == 0)) (*cb)(10 + 80*info->numvert/info->numface + 81*vv/(2*nVertex*info->numface), "Loading X3D Object...");
				}
				
				int ci = 0;
				int initPolygon;
				std::vector<std::pair<int, std::vector<int> > > objVect; 
				int nFace = 0;
				while(ci < coordIndex.size())
				{
					initPolygon = ci;
					std::vector<std::vector<vcg::Point3f> > polygonVect;
					std::vector<Point3f> polygon;
					//Check if polygon is correct
					while(ci < coordIndex.size() && coordIndex.at(ci) != "-1")
					{
						size_t n = coordIndex.at(ci).toInt() + offset;
						if (n >= m.vert.size())
						{
							info->lineNumberError = geometry.lineNumber();
							return E_INVALIDINDEXFACESETCOORD;
						}
						polygon.push_back(m.vert[n].cP());
						ci++;
					}
					if (ci - initPolygon < 3)
					{
						info->lineNumberError = geometry.lineNumber();
						return E_INVALIDINDEXFACESET;
					}
					ci++;
					//Tesselate polygon
					polygonVect.push_back(polygon);
					std::vector<int> indexVect;
					if (polygon.size() == 3)
					{
						indexVect.push_back(0);
						indexVect.push_back(1);
						indexVect.push_back(2);
					}
					else
						vcg::glu_tesselator::tesselate<vcg::Point3f>(polygonVect, indexVect);
					objVect.push_back(std::pair<int, std::vector<int> >(initPolygon, indexVect));
					nFace += indexVect.size()/3;
				}
				int offsetFace = m.face.size();
				int x = offsetFace;
				int y = nFace;
				vcg::tri::Allocator<OpenMeshType>::AddFaces(m, nFace);
				for (size_t j = 0; j < objVect.size(); j++)
				{
					std::pair<int, std::vector<int> > pair = objVect.at(j);
					initPolygon = pair.first;
					std::vector<int> indexVect = pair.second;
					nFace = indexVect.size()/3;
					//vcg::Point3f normals[3];
					for (int ff = 0; ff < nFace; ff++)
					{
						for (int tt = 0; tt < 3; tt++)
						{
							int vertIndexPerFace = tt;
							if (ccw == "false")
								vertIndexPerFace = 2 - tt;
							int index = coordIndex.at(indexVect.at(tt + ff*3) + initPolygon).toInt();
							m.face[ff + offsetFace].V(vertIndexPerFace) = &(m.vert[index + offset]);
							//Load per wedge color
							if (HasPerWedgeColor(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGCOLOR))
							{
								if (index < colorIndex.size() && colorPerVertex == "true")
									getColor(colorList, colorComponent, colorIndex.at(indexVect.at(tt + ff*3) + initPolygon).toInt() * colorComponent, m.face[ff + offsetFace].WC(vertIndexPerFace), vcg::Color4b(Color4b::White));
								else
									m.face[ff + offsetFace].WC(vertIndexPerFace) = vcg::Color4b(vcg::Color4b::White);
							}
							//Load per wedge normal
							if (HasPerWedgeNormal(m) && normalPerVertex == "true" && (info->mask & vcg::tri::io::Mask::IOM_WEDGNORMAL) && index < normalIndex.size())
								getNormal(normalList, normalIndex.at(indexVect.at(tt + ff*3) + initPolygon).toInt() * 3, m.face[ff + offsetFace].WN(vertIndexPerFace), tMatrix);
								
							//Load per wegde texture coordinate
							if(HasPerWedgeTexCoord(m) && (info->mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD))
							{
								if (texCoordIndex.isEmpty())// && !m.HasPerVertexTexCoord())
									getTextureCoord(texture, index*2, m.vert[index + offset].cP(), m.face[ff + offsetFace].WT(vertIndexPerFace), tMatrix);
								else if (!texCoordIndex.isEmpty() && (indexVect.at(tt + ff*3) + initPolygon) < texCoordIndex.size())
									getTextureCoord(texture, texCoordIndex.at(indexVect.at(tt + ff*3) + initPolygon).toInt()*2, m.vert[index + offset].cP(), m.face[ff + offsetFace].WT(vertIndexPerFace), tMatrix); 
								else
								{
									m.face[ff + offsetFace].WT(tt) = vcg::TexCoord2<float>(0, 0);
									m.face[ff + offsetFace].WT(tt).N() = -1;
								}
							}
						}
						//Load per face normal
						if (HasPerFaceNormal(m) && normalPerVertex == "false" && (info->mask & vcg::tri::io::Mask::IOM_FACENORMAL))
						{
							if (!normalIndex.isEmpty() && ff < normalIndex.size() && normalIndex.at(ff).toInt() > -1)
								getNormal(normalList, normalIndex.at(j).toInt() * 3,  m.face[ff + offsetFace].N(), tMatrix);
							else
								getNormal(normalList, j*3,  m.face[ff + offsetFace].N(), tMatrix);
						}
						//Load per face color
						if(HasPerFaceColor(m) && (info->mask & vcg::tri::io::Mask::IOM_FACECOLOR))
						{
							if (colorPerVertex == "false")
							{
								if (!colorIndex.isEmpty() && ff < colorIndex.size() && colorIndex.at(ff).toInt() > -1)
									getColor(colorList, colorComponent, colorIndex.at(j).toInt() * colorComponent, m.face[ff + offsetFace].C(), vcg::Color4b(Color4b::White));
								else
									getColor(colorList, colorComponent, j*colorComponent, m.face[ff + offsetFace].C(), vcg::Color4b(Color4b::White));
							}
							else
								m.face[ff + offsetFace].C() = vcg::Color4b(vcg::Color4b::White);
						}
						if (cb !=NULL && ((offsetFace - x + ff)%1000 == 0)) (*cb)(10 + 80*info->numvert/info->numface + 81/(2*info->numface) + 81*(offsetFace - x + ff)/(2*info->numface*y), "Loading X3D Object...");
					}
					offsetFace += nFace;
				}
			}
			info->numvert++;
			if (cb !=NULL) (*cb)(10 + 80*info->numvert/info->numface, "Loading X3D Object...");
			return E_NOERROR;
		}

		
		
		//Load in the mesh the geometry defined in the node PointSet
		static int LoadPointSet(QDomElement /* geometry */,
									OpenMeshType& m,
									const vcg::Matrix44f tMatrix,
									const QStringList& coordList,
									const QStringList& colorList,
									int colorComponent,
									AdditionalInfoX3D* info,
									CallBackPos *cb)
		{
			//Load vertex in the mesh
			int offset = m.vert.size();
			int nVertex = coordList.size()/3;
			vcg::tri::Allocator<OpenMeshType>::AddVertices(m, nVertex);
			vcg::Color4b defValue;
			if (info->meshColor)
				defValue = info->color;
			else
				defValue = vcg::Color4b(Color4b::White);
			for (int vv = 0; vv < nVertex; vv++)
			{
				vcg::Point4f tmp(coordList.at(vv*3).toFloat(), coordList.at(vv*3 + 1).toFloat(), coordList.at(vv*3 + 2).toFloat(), 1.0);
				tmp = tMatrix * tmp;			
				m.vert[vv + offset].P() = vcg::Point3f(tmp.X(), tmp.Y(), tmp.Z());
				//Load color per vertex
				if (m.HasPerVertexColor() && (info->mask & vcg::tri::io::Mask::IOM_VERTCOLOR))
					getColor(colorList, colorComponent, vv*colorComponent, m.vert[vv + offset].C(), defValue);
				if (m.HasPerVertexTexCoord() && (info->mask & vcg::tri::io::Mask::IOM_VERTCOORD))
				{
					m.vert[vv + offset].T() = vcg::TexCoord2<>();
					m.vert[vv + offset].T().N() = -1;
				}
			}
			info->numvert++;
			if (cb !=NULL) (*cb)(10 + 80*info->numvert/info->numface, "Loading X3D Object...");
			return E_NOERROR;
		}
		
		
		//Load in the mesh the geometry defined in the node Polypoint2D
		static int LoadPolypoint2D(QDomElement geometry,
									OpenMeshType& m,
									const vcg::Matrix44f tMatrix,
									AdditionalInfoX3D* info,
									CallBackPos *cb)
		{
			//Load vertex in the mesh
			int offset = m.vert.size();
			QStringList pointList;
			findAndParseAttribute(pointList, geometry, "point", "");
			if (!pointList.isEmpty())
			{
				int nVertex = pointList.size()/2;
				vcg::tri::Allocator<OpenMeshType>::AddVertices(m, nVertex);
				for (int vv = 0; vv < nVertex; vv++)
				{
					vcg::Point4f tmp(pointList.at(vv*2).toFloat(), pointList.at(vv*2 + 1).toFloat(), 0, 1.0);
					tmp = tMatrix * tmp;			
					m.vert[vv + offset].P() = vcg::Point3f(tmp.X(), tmp.Y(), tmp.Z());
					loadDefaultValuePerVertex(&(m.vert[vv + offset]), m, info->mask);
				}
			}
			info->numvert++;
			if (cb !=NULL) (*cb)(10 + 80*info->numvert/info->numface, "Loading X3D Object...");
			return E_NOERROR;
		}

		
		
		//Load in the mesh the geometry defined in the node TriangleSet2D
		static int LoadTriangleSet2D(QDomElement geometry,
									OpenMeshType& m,
									const vcg::Matrix44f tMatrix,
									AdditionalInfoX3D* info,
									CallBackPos *cb)
		{
			QStringList vertices;
			findAndParseAttribute(vertices, geometry, "vertices", "");
			if (!vertices.isEmpty())
			{
				//Create list of vertex index
				std::vector<vcg::Point4f> vertexSet;
				int index = 0;
				std::vector<int> vertexFaceIndex;
				while (index + 1 < vertices.size())
				{
					vcg::Point4f vertex(vertices.at(index).toFloat(), vertices.at(index + 1).toFloat(), 0, 1.0);
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
					index += 2;
				}
				//Load vertex in the mesh
				int offsetVertex = m.vert.size();
				vcg::tri::Allocator<OpenMeshType>::AddVertices(m, vertexSet.size());
				for (size_t vv = 0; vv < vertexSet.size(); vv++)
				{
					vcg::Point4f tmp = tMatrix * vertexSet.at(vv);
					m.vert[offsetVertex + vv].P() = vcg::Point3f(tmp.X(),tmp.Y(),tmp.Z());
					loadDefaultValuePerVertex(&(m.vert[offsetVertex + vv]), m, info->mask); 
				}
				//Load face in the mesh
				int offsetFace = m.face.size();
				int nFace = vertices.size()/6;
				vcg::tri::Allocator<OpenMeshType>::AddFaces(m, nFace);
				for (int ff = 0; ff < nFace; ff++)
				{
					int faceIndex = ff + offsetFace;
					loadDefaultValuePerFace(&(m.face[faceIndex]), m, info->mask);
					for (int tt = 0; tt < 3; tt++)
						m.face[faceIndex].V(tt) = &(m.vert[vertexFaceIndex.at(tt + ff*3) + offsetVertex]);
				}
			}
			info->numvert++;
			if (cb !=NULL) (*cb)(10 + 80*info->numvert/info->numface, "Loading X3D Object...");
			return E_NOERROR;
		}
		
		
		//Load texture info from Appearance node.
		static int LoadAppearance(const QDomElement& root, std::vector<bool>& validTexture, std::vector<TextureInfo>& textureInfo, QDomNodeList& textTransfList, std::map<QString, QDomElement>& defMap, AdditionalInfoX3D* info)
		{
			QDomElement appearance = root.firstChildElement("Appearance");
			if (!appearance.isNull())
			{
				int result = solveDefUse(appearance, defMap, appearance, info);
				if (result != E_NOERROR) return result;
				QDomNodeList texture;
				//Find all node ImageTexture
				QDomElement multiTexture = appearance.firstChildElement("MultiTexture");
				if (!multiTexture.isNull())
				{
					result= solveDefUse(multiTexture, defMap, multiTexture, info);
					if (result != E_NOERROR) return result;
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
				//Get info from ImageTexture node
				for (int i = 0; i < texture.size(); i++)
				{
					QDomElement imageTexture = texture.at(i).toElement();
					result = solveDefUse(imageTexture, defMap, imageTexture, info);
					if (result != E_NOERROR) return result;
					QString url = imageTexture.attribute("url");
					QStringList paths = url.split(" ", QString::SkipEmptyParts);
					int j = 0;
					bool found = false;
					while (j < paths.size() && !found)
					{
						QString path = paths.at(j).trimmed().remove('"');
						int indexTex = -1;
						size_t z = 0;
						while (z < info->textureFile.size() && !found)
						{
							if (info->textureFile.at(z) == path || info->textureFile.at(z) == url)
							{
								indexTex = z;
								if (!info->useTexture[z])
								{
									found = true;
									info->useTexture[z] = true;
								}
							}
							z++;
						}
						if (found || indexTex != -1)
						{
							TextureInfo tInfo = TextureInfo();
							tInfo.textureIndex = indexTex;
							tInfo.repeatS = (imageTexture.attribute("repeatS", "true") == "true");
							tInfo.repeatT = (imageTexture.attribute("repeatT", "true") == "true");
							textureInfo.push_back(tInfo);
							found = true;
						}
						j++;
					}
					if (!found){
						TextureInfo tInfo = TextureInfo();
						tInfo.isValid = false;
						textureInfo.push_back(tInfo);
					}
				}
				textTransfList = appearance.elementsByTagName("TextureTransform");
				QDomElement materialNode = appearance.firstChildElement("Material");
				QStringList list;
				findAndParseAttribute(list, materialNode, "diffuseColor", "");
				if (list.size() >= 3)
				{
					vcg::Color4f color(list.at(0).toFloat(), list.at( 1).toFloat(), list.at(2).toFloat(), 1); 
					vcg::Color4b colorB;
					colorB.Import(color);
					info->color = colorB;
				}
				else
					info->meshColor = false;

			}
			return E_NOERROR;
		}
		
		
		//Create the transformation matrix for texture coordinate from TextureTransform node
		inline static vcg::Matrix33f createTextureTrasformMatrix(QDomElement elem)
		{
			vcg::Matrix33f matrix, tmp;
			matrix.SetIdentity();
			QStringList coordList, center;
			findAndParseAttribute(center, elem, "center", "");
			if (center.size() == 2)
			{			
				matrix[0][2] = -center.at(0).toFloat();
				matrix[1][2] = -center.at(1).toFloat();
			}
			findAndParseAttribute(coordList, elem, "scale", "");
			if(coordList.size() == 2)
			{
				tmp.SetIdentity();
				tmp[0][0] = coordList.at(0).toFloat();
				tmp[1][1] = coordList.at(1).toFloat();
				matrix *= tmp;
			}
			findAndParseAttribute(coordList, elem, "rotation", "");
			if(coordList.size() == 1)
			{
				tmp.SetRotateDeg(math::ToDeg(coordList.at(0).toFloat()), vcg::Point3f(0, 0, 1));
				matrix *= tmp;
			}
			if (center.size() == 2)
			{
				tmp.SetIdentity();
				tmp[0][2] = center.at(0).toFloat();
				tmp[1][2] = center.at(1).toFloat();
				matrix *= tmp;
			}
			findAndParseAttribute(coordList, elem, "traslation", "");
			if(coordList.size() == 2)
			{
				tmp.SetIdentity();
				tmp[0][2] = coordList.at(0).toFloat();
				tmp[1][2] = coordList.at(1).toFloat();
				matrix *= tmp;
			}
			return matrix;
		}
		
		
		//Create the transformation matrix from Trasform node 
		inline static vcg::Matrix44f createTransformMatrix(QDomElement root, vcg::Matrix44f tMatrix)
		{
			vcg::Matrix44f t, tmp;
			t.SetIdentity();
			QStringList coordList, center, scale;
			findAndParseAttribute(coordList, root, "translation", "");
			if(coordList.size() == 3)
				t.SetTranslate(coordList.at(0).toFloat(), coordList.at(1).toFloat(), coordList.at(2).toFloat()); 
			findAndParseAttribute(center, root, "center", "");
			if(center.size() == 3)
			{
				tmp.SetTranslate(center.at(0).toFloat(), center.at(1).toFloat(), center.at(2).toFloat());
				t *= tmp;
			}
			findAndParseAttribute(coordList, root, "rotation", "");
			if(coordList.size() == 4)
			{
				tmp.SetRotateRad(coordList.at(3).toFloat(), vcg::Point3f(coordList.at(0).toFloat(), coordList.at(1).toFloat(), coordList.at(2).toFloat()));
				t *= tmp;
			}
			findAndParseAttribute(scale, root, "scaleOrientation", "");
			if(scale.size() == 4)
			{
				tmp.SetRotateRad(scale.at(3).toFloat(), vcg::Point3f(scale.at(0).toFloat(), scale.at(1).toFloat(), scale.at(2).toFloat()));
				t *= tmp;
			}
			findAndParseAttribute(coordList, root, "scale", "");
			if(coordList.size() == 3)
			{
				tmp.SetScale(coordList.at(0).toFloat(), coordList.at(1).toFloat(), coordList.at(2).toFloat());
				t *= tmp;
			}
			if(scale.size() == 4)
			{
				tmp.SetRotateRad(-scale.at(3).toFloat(), vcg::Point3f(scale.at(0).toFloat(), scale.at(1).toFloat(), scale.at(2).toFloat()));
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

		
		
		//Check the validity of Inline node and load the geometry from linked file
		static int NavigateInline(OpenMeshType& m,
									QDomElement root,
									const vcg::Matrix44f tMatrix,
									AdditionalInfoX3D* info,
									CallBackPos *cb)
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
					//Check if exist a loop in the dependences among files 
					for (size_t j = 0; j < info->filenameStack.size(); j++)
					{
						if ( info->filenameStack[j] == path ) 
						{
							info->lineNumberError = root.lineNumber();
							return E_LOOPDEPENDENCE;
						}
					}
					//Load geometry from linked file
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
		
		
		//Manage the ExternProtoDeclare node to permit the resolution of ProtoInstance node
		static int NavigateExternProtoDeclare(QDomElement root,
									const vcg::Matrix44f /* tMatrix */,
									std::map<QString, QDomElement>& protoDeclareMap,
									AdditionalInfoX3D* info)
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
		
		
		//Check the validity of ProtoInstance node and load the geometry from relating ProtoDeclare 
		static int NavigateProtoInstance(OpenMeshType& m,
									QDomElement root,
									const vcg::Matrix44f tMatrix,
									std::map<QString, QDomElement>& defMap,
									std::map<QString, QDomElement>& protoDeclareMap,
									AdditionalInfoX3D* info,
									CallBackPos *cb)
		{
			QString name = root.attribute("name");
			std::map<QString, QString> fields;
			std::map<QString, QDomElement> fieldsNode;
			//Get the initialization of ProtoInstance's field
			QDomElement child = root.firstChildElement("fieldValue");
			while (!child.isNull())
			{
				QString fName = child.attribute("name");
				QString fValue = child.attribute("value");
				if (fName =="" || (fValue == "" && child.firstChildElement().isNull()))
				{
					info->lineNumberError = root.lineNumber();
					return E_INVALIDINSTFIELD;
				}
				if (!child.firstChildElement().isNull())
					fieldsNode[fName] = child;
				else
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
			//Get the name of file that contains the prototype
			QString filename = "";
			for (std::map<QString, QDomNode*>::const_iterator i = info->protoDeclareNodeMap.begin(); i != info->protoDeclareNodeMap.end(); i++)
			{
				if (i->second->firstChildElement() == iter->second)
				{
					filename = i->first;
					break;
				}
			}
			//Check if exist a loop dependence among files
			for (size_t j = 0; j < info->filenameStack.size(); j++)
			{
				if ( info->filenameStack[j] == filename ) 
				{
					info->lineNumberError = root.lineNumber();
					return E_LOOPDEPENDENCE;
				}
			}
			
			std::map<QString, QDomElement> newProtoDeclMap;
			//Initialize ProtoDeclare node
			if (filename != "")
			{
				QDomDocument docChild(filename);
				QFile file(filename.split("#", QString::SkipEmptyParts).at(0));
				file.open(QIODevice::ReadOnly);
				docChild.setContent(&file);
				QDomNodeList exProtoDeclare = docChild.elementsByTagName("ExternProtoDeclare");
				for (int j = 0; j < exProtoDeclare.size(); j++)
					NavigateExternProtoDeclare(exProtoDeclare.at(j).toElement(), tMatrix, newProtoDeclMap, info); 
			}
			int result = InitializeProtoDeclare(protoInstance, fields, fieldsNode, defMap, info);
			if (result != E_NOERROR) return result;
			QDomElement body = protoInstance.firstChildElement("ProtoBody");
			std::map<QString, QDomElement> newDefMap;
			if (filename != "")
				info->filenameStack.push_back(filename);
			result = NavigateScene(m, body, tMatrix, newDefMap, newProtoDeclMap, info, cb);
			if (result != E_NOERROR)
				return result;
			if (filename != "")
				info->filenameStack.pop_back();
			return E_NOERROR;
		}		
		
		
		//If the index is valid, return the normal of index 'index'
		inline static void getNormal(const QStringList& list, int index, vcg::Point3f& dest, const vcg::Matrix44f& tMatrix)
		{
			if(!list.isEmpty() && (index + 2) < list.size())
			{
				vcg::Point3f normal(list.at(index).toFloat(), list.at(index + 1).toFloat(), list.at( index+ 2).toFloat());
				vcg::Matrix44f intr44 = vcg::Inverse(tMatrix);
				intr44.transposeInPlace();
				Matrix33f intr33;
				for(unsigned int rr = 0; rr < 2; ++rr)
				{
					for(unsigned int cc = 0;cc < 2;++cc)
					intr33[rr][cc] = intr44[rr][cc];
				}
				normal = intr33 * normal;
				dest = normal.Normalize();
			}
		}

		

		//If the index is valid, return the color of index 'index'
		inline static void getColor(const QStringList& list, int component, int index, vcg::Color4b& dest, const vcg::Color4b& defValue)
		{
			if(!list.isEmpty() && (index + component - 1) < list.size())
			{
				vcg::Color4f color;
				if (component == 3)
					color = vcg::Color4f(list.at(index).toFloat(), list.at(index + 1).toFloat(), list.at(index + 2).toFloat(), 1); 
				else
					color = vcg::Color4f(list.at(index).toFloat(), list.at(index + 1).toFloat(), list.at(index + 2).toFloat(), list.at(index + 3).toFloat());
				vcg::Color4b colorB;
				colorB.Import(color);
				dest = colorB;
			}
			else
				dest = defValue;
		}

		

		//If the index is valid, return the texture coordinate of index 'index'
		inline static bool getTextureCoord(const TextureInfo& textInfo, int index, const vcg::Point3f& vertex, vcg::TexCoord2<float>& dest, const vcg::Matrix44f& tMatrix)
		{
			vcg::Point3f point;
			vcg::TexCoord2<float> textCoord;
			if (textInfo.isCoordGenerator)
			{
				//TextureCoordinateGenerator
				if (textInfo.mode == "COORD")
				{
					vcg::Point4f tmpVertex(vertex.X(), vertex.Y(), vertex.Z(), 1.0);
					vcg::Matrix44f tmpMatrix = vcg::Inverse(tMatrix);
					tmpVertex = tmpMatrix * tmpVertex;
					point = vcg::Point3f(tmpVertex.X(), tmpVertex.Y(), 0.0);
					textCoord.N() = textInfo.textureIndex;
				}
				else
				{
					point = vcg::Point3f(0, 0, 0);
					textCoord.N() = -1;
				}
			}
			else if (!textInfo.textureCoordList.isEmpty() && (index + 1) < textInfo.textureCoordList.size())
			{
				point = vcg::Point3f(textInfo.textureCoordList.at(index).toFloat(), textInfo.textureCoordList.at(index + 1).toFloat(), 1.0);
				textCoord.N() = textInfo.textureIndex;
			}
			else
			{
				point = vcg::Point3f(0, 0, 0);
				textCoord.N() = -1;
			}
			//Apply trasform
			point = textInfo.textureTransform * point;
			//Apply clamb and repeat
			if (!textInfo.repeatS)
			{
				point.X() = point.X() < 0? 0: point.X();
				point.X() = point.X() > 1? 1: point.X();
			}
			else
				point.X() = (point.X() != floorf(point.X()))? (point.X() - floorf(point.X())): fmodf(point.X(), 2.0);
			if (!textInfo.repeatT)
			{
				point.Y() = point.Y() < 0? 0: point.Y();
				point.Y() = point.Y() > 1? 1: point.Y();
			}
			else
				point.Y() = (point.Y() != floorf(point.Y()))? (point.Y() - floorf(point.Y())): fmodf(point.Y(), 2.0);
			textCoord.U() = point.X();
			textCoord.V() = point.Y();
			dest = textCoord;
			return true;
		}

		
		
		//Navigate scene graph
		static int NavigateScene(OpenMeshType& m,
									QDomElement root,
									const vcg::Matrix44f tMatrix,
									std::map<QString, QDomElement>& defMap,
									std::map<QString, QDomElement>& protoDeclareMap,
									AdditionalInfoX3D* info,
									CallBackPos *cb)
		{
			if (root.isNull()) return E_NOERROR;
			int result = solveDefUse(root, defMap, root, info);
			if (result != E_NOERROR) return result;
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
				return NavigateExternProtoDeclare(root, tMatrix, protoDeclareMap, info);
			
			if (root.tagName() == "ProtoInstance")
				return NavigateProtoInstance(m, root, tMatrix, defMap, protoDeclareMap, info, cb);
			
			if (root.tagName() == "Shape")
			{
				std::vector<bool> validTexture;
				std::vector<TextureInfo> textureInfo;
				QDomNodeList textureTransformList;
				int result = LoadAppearance(root, validTexture, textureInfo, textureTransformList, defMap, info);
				if (result != E_NOERROR) return result;
				QDomElement geometryNode = root.firstChildElement();
				while (!geometryNode.isNull())
				{
					QDomElement geometry;
					result = solveDefUse(geometryNode, defMap, geometry, info);
					if (result != E_NOERROR) return result;
					QString coordTag[] = {"Coordinate", "CoordinateDouble"};
					QDomElement coordinate = findNode(coordTag, 2, geometry);
					result = solveDefUse(coordinate, defMap, coordinate, info);
					if (result != E_NOERROR) return result;
					if ((!coordinate.isNull() && (coordinate.attribute("point") != "")) || (geometry.tagName() == "ElevationGrid"))
					{
						//Get coordinate 
						QStringList coordList;
						findAndParseAttribute(coordList, coordinate, "point", "");
						//GetColor
						QString colorTag[] = {"Color", "ColorRGBA"};
						QDomElement color = findNode(colorTag, 2, geometry);
						result = solveDefUse(color, defMap, color, info);
						if (result != E_NOERROR) return result;
						QStringList colorList;
						findAndParseAttribute(colorList, color, "color", "");
						if (!colorList.isEmpty())
							info->meshColor = false;
						//GetNormal
						QDomElement normal = geometry.firstChildElement("Normal");
						result = solveDefUse(normal, defMap, normal, info);
						if (result != E_NOERROR) return result;
						QStringList normalList;
						findAndParseAttribute(normalList, normal, "vector", "");
						//Get texture coordinate
						QString textCoorTag[] = {"TextureCoordinate", "MultiTextureCoordinate", "TextureCoordinateGenerator"};
						QDomElement textureCoord = findNode(textCoorTag, 3, geometry);
						result = solveDefUse(textureCoord, defMap, textureCoord, info);
						//Get additional texture info
						if (textureCoord.tagName() == "MultiTextureCoordinate")
						{
							QDomElement child = textureCoord.firstChildElement();
							size_t i= 0;
							int j = 0;
							while (!child.isNull())
							{
								if (i >= validTexture.size()) break;
								QDomElement solveChild; 
								result = solveDefUse(child, defMap, solveChild, info);
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
						//Get first valid texture
						TextureInfo texture;
						for (size_t i = 0; i < textureInfo.size(); i++)
						{
							if (textureInfo.at(i).isValid)
							{
								texture = textureInfo.at(i);
								break;
							}
						}
						
						//Load geometry in the mesh
						if (geometry.tagName() == "TriangleSet" || geometry.tagName() == "TriangleFanSet" || geometry.tagName() == "TriangleStripSet" || geometry.tagName() == "QuadSet")
							return LoadSet(geometry, m, tMatrix, texture, coordList, colorList, normalList, colorComponent, info, cb);
						else if (geometry.tagName() == "IndexedTriangleSet" || geometry.tagName() == "IndexedTriangleFanSet" || geometry.tagName() == "IndexedTriangleStripSet" || geometry.tagName() == "IndexedQuadSet")
							return LoadIndexedSet(geometry, m, tMatrix, texture, coordList, colorList, normalList, colorComponent, info, cb);
						else if (geometry.tagName() == "ElevationGrid")
							return LoadElevationGrid(geometry, m, tMatrix, texture, colorList, normalList, colorComponent, info, cb);
						else if (geometry.tagName() == "IndexedFaceSet")
							return LoadIndexedFaceSet(geometry, m, tMatrix, texture, coordList, colorList, normalList, colorComponent, info, cb);
						else if (geometry.tagName() == "PointSet")
							return LoadPointSet(geometry, m, tMatrix, coordList, colorList, colorComponent, info, cb); 
					}
					else if (geometry.tagName() == "Polypoint2D")
						return LoadPolypoint2D(geometry, m, tMatrix, info, cb);
					else if (geometry.tagName() == "TriangleSet2D")
						return LoadTriangleSet2D(geometry, m, tMatrix, info, cb);
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

		
		
		//Tralate a VRML file in X3D file with XML encoding
		static int ParseVrml(const char * filename, QDomDocument* root)
		{
			wchar_t *file = coco_string_create(filename);
			try 
			{
				VrmlTranslator::Scanner scanner(file);
				VrmlTranslator::Parser parser(&scanner);
				//root = new QDomDocument(filename);
				parser.doc = root;
				parser.Parse();
				if (parser.errors->count != 0) 
				{
					errorStr = coco_string_create_char(parser.errors->stringError);
					return E_VRMLPARSERERROR;
				}
				//root = parser.doc;
			}
			catch (char* str)
			{
				errorStr = str;
				return E_VRMLPARSERERROR;
			}
			coco_string_delete(file);
			return E_NOERROR;
		}

	

public:	
	
		//Get information from x3d file about the components necessary in the mesh to load the geometry
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

		

		//Merge all meshes in the x3d's file in the templeted mesh m
		static int Open(OpenMeshType& m, const char* filename, AdditionalInfoX3D*& info, CallBackPos *cb = 0)
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
			info->useTexture = std::vector<bool>(info->textureFile.size(), false);
			if (cb !=NULL) (*cb)(10, "Loading X3D Object...");
			if (scene.size() == 0)
				return E_NO3DSCENE;
			if (scene.size() > 1)
				return E_MULTISCENE;
			return NavigateScene(m, scene.at(0).toElement(), tMatrix, defMap, protoDeclareMap, info, cb);
		}

	

		//Get information from vrml file about the components necessary in the mesh to load the geometry
		static int LoadMaskVrml(const char * filename, AdditionalInfoX3D*& addinfo)
		{
			AdditionalInfoX3D* info = new AdditionalInfoX3D();
			info->filenameStack.push_back(QString(filename));
			info->mask = 0;
			info->filename = QString(filename);
			addinfo = info;
			QDomDocument* document = new QDomDocument(filename);
			int result = ParseVrml(filename, document);
			if (result != E_NOERROR) 
			{	
				delete document;
				return result;
			}
			info->doc = document;
			/*QFile file("temp.x3d");
			file.open(QIODevice::WriteOnly | QIODevice::Text);
		    QTextStream out(&file);
			document->save(out, 1);*/
			return LoadMaskByDom(document, info, info->filename);
			/*wchar_t *file = coco_string_create(filename);
			try 
			{
				VrmlTranslator::Scanner scanner(file);
				VrmlTranslator::Parser parser(&scanner);
				parser.doc = new QDomDocument();
				parser.Parse();
				if (parser.errors->count != 0) 
				{
					errorStr = coco_string_create_char(parser.errors->stringError);
					return E_VRMLPARSERERROR;
				}
				info->doc = parser.doc;
				return LoadMaskByDom(parser.doc, info, info->filename);
			}
			catch (char* str)
			{
				errorStr = str;
				return E_VRMLPARSERERROR;
			}
			coco_string_delete(file);*/
			return E_NOERROR;
		}

	};
}
}
}
#endif