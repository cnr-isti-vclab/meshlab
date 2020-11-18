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

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "filter_layer.h"

#include<vcg/complex/append.h>
#include <QImageReader>
#include <QDir>
#include <QXmlStreamWriter>



using namespace std;
using namespace vcg;

// Constructor
FilterLayerPlugin::FilterLayerPlugin()
{
    typeList <<
        FP_FLATTEN <<
        FP_MESH_VISIBILITY <<        
        FP_DELETE_MESH <<
        FP_DELETE_NON_VISIBLE_MESH <<
        FP_DELETE_RASTER <<
        FP_DELETE_NON_SELECTED_RASTER <<
        FP_SPLITSELECTEDFACES <<
        FP_SPLITSELECTEDVERTICES <<
        FP_SPLITCONNECTED <<
        FP_RENAME_MESH <<
        FP_RENAME_RASTER <<
        FP_DUPLICATE <<
        FP_SELECTCURRENT <<
		FP_EXPORT_CAMERAS <<
		FP_IMPORT_CAMERAS;

    foreach(FilterIDType tt , types())
        actionList << new QAction(filterName(tt), this);
}

QString FilterLayerPlugin::pluginName() const
{
    return "FilterLayer";
}

// ST() return the very short string describing each filtering action
QString FilterLayerPlugin::filterName(FilterIDType filterId) const
{
    switch(filterId) {
    case FP_SPLITSELECTEDFACES :  return QString("Move selected faces to another layer");
    case FP_SPLITSELECTEDVERTICES : return QString("Move selected vertices to another layer") ;
    case FP_SPLITCONNECTED :  return QString("Split in Connected Components");
    case FP_DUPLICATE :  return QString("Duplicate Current layer");
    case FP_DELETE_MESH :  return QString("Delete Current Mesh");
    case FP_DELETE_NON_VISIBLE_MESH :  return QString("Delete all non visible Mesh Layers");
    case FP_DELETE_RASTER :  return QString("Delete Current Raster");
    case FP_DELETE_NON_SELECTED_RASTER :  return QString("Delete all Non Selected Rasters");
    case FP_FLATTEN :  return QString("Flatten Visible Layers");
    case FP_RENAME_MESH :  return QString("Rename Current Mesh");
    case FP_RENAME_RASTER :  return QString("Rename Current Raster");
    case FP_SELECTCURRENT :  return QString("Change the current layer");
    case FP_MESH_VISIBILITY :  return QString("Change Visibility of layer(s)");
    case FP_EXPORT_CAMERAS:  return QString("Export active rasters cameras to file");
    case FP_IMPORT_CAMERAS:  return QString("Import cameras for active rasters from file");
    default : assert(0);
    }
	return NULL;
}

// Info() return the longer string describing each filtering action
QString FilterLayerPlugin::filterInfo(FilterIDType filterId) const
{
    switch(filterId) {
    case FP_SPLITSELECTEDFACES :  return QString("Selected faces are moved (or duplicated) in a new layer. Warning! per-vertex and per-face user defined attributes will not be transferred.");
    case FP_SPLITSELECTEDVERTICES :  return QString("Selected vertices are moved (or duplicated) in a new layer. Warning! per-vertex user defined attributes will not be transferred.");
    case FP_DELETE_MESH :  return QString("The current mesh layer is deleted");
    case FP_DELETE_NON_VISIBLE_MESH :  return QString("All the non visible mesh layers are deleted");
    case FP_DELETE_RASTER :  return QString("The current raster layer is deleted");
    case FP_DELETE_NON_SELECTED_RASTER :  return QString("All non selected raster layers are deleted");
    case FP_SPLITCONNECTED:  return QString("Split current Layer into many layers, one for each connected components");
    case FP_DUPLICATE :  return QString("Create a new layer containing the same model as the current one");
    case FP_FLATTEN :  return QString("Flatten all or only the visible layers into a single new mesh. <br> Transformations are preserved. Existing layers can be optionally deleted");
    case FP_RENAME_MESH :  return QString("Explicitly change the label shown for a given mesh");
    case FP_RENAME_RASTER :  return QString("Explicitly change the label shown for a given raster");
    case FP_SELECTCURRENT :  return QString("Change the current layer to a chosen one");
    case FP_MESH_VISIBILITY :  return QString("Make layer(s) visible/invisible. Useful for scripting.");
    case FP_EXPORT_CAMERAS:  return QString("Export active cameras to file, in the .out or Agisoft .xml formats");
    case FP_IMPORT_CAMERAS:  return QString("Import cameras for active rasters from .out or Agisoft .xml formats");
    default : assert(0);
    }
	return NULL;
}

// This function define the needed parameters for each filter.
void FilterLayerPlugin::initParameterList(const QAction *action, MeshDocument &md, RichParameterList & parlst)
{
    MeshModel *mm=md.mm();
    RasterModel *rm=md.rm();
    switch(ID(action))
    {
    case FP_SPLITSELECTEDVERTICES:
    case FP_SPLITSELECTEDFACES :
        {
            parlst.addParam(RichBool ("DeleteOriginal",
                true,
                "Delete original selection",
                "Deletes the original selected faces/vertices, thus splitting the mesh among layers. \n\n"
                "if false, the selected faces/vertices are duplicated in the new layer"));
        }
        break;
    case FP_FLATTEN :
        parlst.addParam(RichBool ("MergeVisible", true, "Merge Only Visible Layers",
            "If true, flatten only visible layers, otherwise, all layers are used"));
        parlst.addParam(RichBool ("DeleteLayer", true, "Delete Layers ",
            "Delete all the layers used as source in flattening. <br>If all layers are visible only a single layer will remain after the invocation of this filter"));
        parlst.addParam(RichBool ("MergeVertices", true,  "Merge duplicate vertices",
            "Merge the vertices that are duplicated among different layers. \n\n"
            "Very useful when the layers are spliced portions of a single big mesh."));
        parlst.addParam(RichBool ("AlsoUnreferenced", false, "Keep unreferenced vertices",
            "Do not discard unreferenced vertices from source layers\n\n"
            "Necessary for point-cloud layers"));
        break;
    case FP_RENAME_MESH :
        parlst.addParam(RichString ("newName",
            mm->label(),
            "New Label",
            "New Label for the mesh"));
        break;
    case FP_RENAME_RASTER :
        parlst.addParam(RichString ("newName",
            rm?rm->label():"",
            "New Label",
            "New Label for the raster"));
        break;
    case FP_SELECTCURRENT :
        parlst.addParam(RichMesh ("layer",md.mm(),&md, "Layer Name",
            "The name of the current layer"));
        break;
    case FP_MESH_VISIBILITY :
        parlst.addParam(RichMesh ("layer",md.mm(),&md, "Layer Name", "The name of the layer that has to change visibility. If second parameter is not empty, this parameter is ignored"));
        parlst.addParam(RichString("lName", "", "Substring match", "Apply visibility to all layers with name substring matching the entered string. If not empty, the first parameter is ignored."));
        parlst.addParam(RichBool ("isMeshVisible", true,  "Visible", "It makes the selected layer(s) visible or invisible."));
        break;
        
	case FP_EXPORT_CAMERAS:
		parlst.addParam(RichEnum("ExportFile", 0, QStringList("Bundler .out") << "Agisoft xml", "Output format", "Choose the output format, The filter enables to export the cameras to both Bundler and Agisoft Photoscan."));
		parlst.addParam(RichString("newName",
			"cameras",
			"Export file name (the right extension will be added at the end)",
			"Name of the output file, it will be saved in the same folder as the project file"));
		break;
	case FP_IMPORT_CAMERAS:
		parlst.addParam(RichOpenFile("ImportFile", 0, QStringList("All Project Files (*.out *.xml);;Bundler Output (*.out);;Agisoft xml (*.xml)"),"Choose the camera file to be imported", "It's possible to import both Bundler .out and Agisoft .xml files. In both cases, distortion parameters won't be imported. In the case of Agisoft, it's necessary to undistort the images before exporting the xml file"));
		break;
    default: break; // do not add any parameter for the other filters
    }
}

// Core Function doing the actual mesh processing.
bool FilterLayerPlugin::applyFilter(const QAction *filter, MeshDocument &md, std::map<std::string, QVariant>&, unsigned int& /*postConditionMask*/, const RichParameterList & par, vcg::CallBackPos *cb)
{
 CMeshO::FaceIterator fi;
 int numFacesSel,numVertSel;

 switch(ID(filter))
 {
	case  FP_RENAME_MESH:     md.mm()->setLabel(par.getString("newName")); break;

	case  FP_RENAME_RASTER:
	{
    if (md.rm()) 
    {
      md.rm()->setLabel(par.getString("newName"));
    }
    else
    {
      this->errorMessage = "Error: Call to Rename Current Raster with no valid raster.";
      return false;
    }
    
	} break;

	case  FP_SELECTCURRENT:   md.setCurrent(par.getMesh("layer")); break;

	case  FP_MESH_VISIBILITY:   
	{
		QString match = par.getString("lName");
		if (match == "")
		{
			MeshModel *mm = par.getMesh("layer");
			if (mm)
				md.setVisible(mm->id(), par.getBool("isMeshVisible"));
		}
		else
		{
			foreach(MeshModel *mmp, md.meshList)
			{
				if (mmp->label().contains(match))
					md.setVisible(mmp->id(), par.getBool("isMeshVisible"));
			}
		}
	} break;

	case  FP_DELETE_MESH:
	{
		if (md.mm())
			md.delMesh(md.mm());
	} break;

	case  FP_DELETE_NON_VISIBLE_MESH:
	{
		foreach(MeshModel *mmp, md.meshList)
		{
			if (!mmp->visible)
				md.delMesh(mmp);
		}
	} break;

    case  FP_DELETE_RASTER :  if(md.rm()) md.delRaster(md.rm()); break;

	case  FP_DELETE_NON_SELECTED_RASTER:
	{
		foreach(RasterModel *rmp, md.rasterList)
		{
			if (!rmp->visible)
			   md.delRaster(rmp);
		}
	} break;

	case FP_SPLITSELECTEDVERTICES :
	{
		MeshModel* currentModel = md.mm();

		MeshModel* destModel = md.addNewMesh("","SelectedVerticesSubset",true);
		destModel->updateDataMask(currentModel);

		numVertSel = (int)tri::UpdateSelection<CMeshO>::VertexCount(currentModel->cm);

		tri::Append<CMeshO, CMeshO>::Mesh(destModel->cm, currentModel->cm, true);

		if(par.getBool("DeleteOriginal"))	// delete original vert/faces
		{
			CMeshO::VertexIterator vi;

			int delfaces = (int)tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(currentModel->cm);
			for (CMeshO::FaceIterator fi = currentModel->cm.face.begin(); fi != currentModel->cm.face.end(); ++fi)
				if(!(*fi).IsD() && (*fi).IsS() )
					tri::Allocator<CMeshO>::DeleteFace(currentModel->cm, *fi);
			for (vi = currentModel->cm.vert.begin(); vi != currentModel->cm.vert.end(); ++vi)
				if(!(*vi).IsD() && (*vi).IsS() )
					tri::Allocator<CMeshO>::DeleteVertex(currentModel->cm, *vi);

			tri::UpdateSelection<CMeshO>::VertexClear(currentModel->cm);
			currentModel->clearDataMask(MeshModel::MM_FACEFACETOPO);

			log("Moved %i vertices to layer %i, deleted %i faces", numVertSel, delfaces, md.meshList.size());
		}
		else								// keep original faces
		{
			log("Copied %i vertices to layer %i", numVertSel, md.meshList.size());
		}
		vcg::tri::UpdateFlags<CMeshO>::VertexClear(destModel->cm, CMeshO::VertexType::SELECTED);

		// init new layer
		destModel->UpdateBoxAndNormals();
		destModel->cm.Tr = currentModel->cm.Tr;
		destModel->updateDataMask(currentModel);
	} break;

    case FP_SPLITSELECTEDFACES :
	{
		MeshModel *currentModel = md.mm();

		MeshModel *destModel = md.addNewMesh("", "SelectedFacesSubset", true);
		destModel->updateDataMask(currentModel);

		// select all points involved
		tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(currentModel->cm);
		numFacesSel = (int)tri::UpdateSelection<CMeshO>::FaceCount(currentModel->cm);
		numVertSel = (int)tri::UpdateSelection<CMeshO>::VertexCount(currentModel->cm);

		tri::Append<CMeshO, CMeshO>::Mesh(destModel->cm, currentModel->cm, true);

		if(par.getBool("DeleteOriginal"))	// delete original faces
		{
			CMeshO::VertexIterator vi;
			CMeshO::FaceIterator   fi;
			tri::UpdateSelection<CMeshO>::VertexClear(currentModel->cm);
			tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(currentModel->cm);
			for (fi = currentModel->cm.face.begin(); fi != currentModel->cm.face.end(); ++fi)
				if(!(*fi).IsD() && (*fi).IsS() )
					tri::Allocator<CMeshO>::DeleteFace(currentModel->cm, *fi);
			for (vi = currentModel->cm.vert.begin(); vi != currentModel->cm.vert.end(); ++vi)
				if(!(*vi).IsD() && (*vi).IsS() )
					tri::Allocator<CMeshO>::DeleteVertex(currentModel->cm, *vi);

			tri::UpdateSelection<CMeshO>::VertexClear(currentModel->cm);
			tri::UpdateSelection<CMeshO>::FaceClear(currentModel->cm);
			currentModel->clearDataMask(MeshModel::MM_FACEFACETOPO);

			log("Moved %i faces and %i vertices to layer %i", numFacesSel, numVertSel, md.meshList.size());
		}
		else								// keep original faces
		{
			log("Copied %i faces and %i vertices to layer %i", numFacesSel, numVertSel, md.meshList.size());
		}
		vcg::tri::UpdateFlags<CMeshO>::VertexClear(destModel->cm, CMeshO::VertexType::SELECTED);
		vcg::tri::UpdateFlags<CMeshO>::FaceClear(destModel->cm, CMeshO::FaceType::SELECTED);

		// init new layer
		destModel->UpdateBoxAndNormals();
		destModel->cm.Tr = currentModel->cm.Tr;
		destModel->updateDataMask(currentModel);
	} break;

	case FP_DUPLICATE :
	{
		MeshModel *currentModel = md.mm();				// source = current
		QString newName = currentModel->label() + "_copy";
		MeshModel *destModel = md.addNewMesh("", newName, true); // After Adding a mesh to a MeshDocument the new mesh is the current one
		destModel->updateDataMask(currentModel);
		tri::Append<CMeshO, CMeshO>::Mesh(destModel->cm, currentModel->cm);

		log("Duplicated current model to layer %i", md.meshList.size());

		// init new layer
		destModel->UpdateBoxAndNormals();
		destModel->cm.Tr = currentModel->cm.Tr;
	} break;

    case FP_FLATTEN :
	{
		bool deleteLayer = par.getBool("DeleteLayer");
		bool mergeVisible = par.getBool("MergeVisible");
		bool mergeVertices = par.getBool("MergeVertices");
		bool alsoUnreferenced = par.getBool("AlsoUnreferenced");

		MeshModel *destModel = md.addNewMesh("", "Merged Mesh", true);

		QList<MeshModel *> toBeDeletedList;

		int cnt=0;
		foreach(MeshModel *mmp, md.meshList)
		{ 
			++cnt;
            if(mmp->visible || !mergeVisible)
            {
				if (mmp != destModel)
                {
                    cb(cnt*100/md.meshList.size(), "Merging layers...");
                    tri::UpdatePosition<CMeshO>::Matrix(mmp->cm,mmp->cm.Tr,true);
                    toBeDeletedList.push_back(mmp);
                    if(!alsoUnreferenced)
                    {
                        vcg::tri::Clean<CMeshO>::RemoveUnreferencedVertex(mmp->cm);
                    }
					destModel->updateDataMask(mmp);
					tri::Append<CMeshO, CMeshO>::Mesh(destModel->cm, mmp->cm);
                    tri::UpdatePosition<CMeshO>::Matrix(mmp->cm,Inverse(mmp->cm.Tr),true);
                    
                }
            }
		}
            
		if( deleteLayer )
		{
			log( "Deleted %d merged layers", toBeDeletedList.size());
			foreach(MeshModel *mmp,toBeDeletedList)
				md.delMesh(mmp);
			md.setCurrent(destModel); // setting again newly created model as current
		}
		
		if( mergeVertices )
		{
			int delvert = tri::Clean<CMeshO>::RemoveDuplicateVertex(destModel->cm);
			log( "Removed %d duplicated vertices", delvert);
		}

		destModel->UpdateBoxAndNormals();
		log("Merged all the layers to single mesh of %i vertices",md.mm()->cm.vn);
	} break;

    case FP_SPLITCONNECTED :
	{
		MeshModel *currentModel = md.mm();
		CMeshO &cm = md.mm()->cm;
		md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);
		std::vector< std::pair<int,CMeshO::FacePointer> > connectedCompVec;
		int numCC = tri::Clean<CMeshO>::ConnectedComponents(cm,  connectedCompVec);
		log("Found %i Connected Components",numCC);
		
		for(size_t i=0; i<connectedCompVec.size();++i)
		{
			tri::UpdateSelection<CMeshO>::FaceClear(cm);
			connectedCompVec[i].second->SetS();
			tri::UpdateSelection<CMeshO>::FaceConnectedFF(cm/*,true*/);
			tri::UpdateSelection<CMeshO>::VertexClear(cm);
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(cm);

			MeshModel *destModel= md.addNewMesh("",QString("CC %1").arg(i), true);
			destModel->updateDataMask(currentModel);
			tri::Append<CMeshO, CMeshO>::Mesh(destModel->cm, cm, true);

			// init new layer
			destModel->UpdateBoxAndNormals();
			destModel->cm.Tr = currentModel->cm.Tr;
		}
	} break;

	case FP_EXPORT_CAMERAS:
	{
		int output = par.getEnum("ExportFile");
		QString name = par.getString("newName");
				
		if (output ==0)
		{ 
			FILE* outfile = NULL;
			
			name=name.append(".out");
			outfile = fopen(name.toStdString().c_str(), "wb");

			if (outfile == NULL)
				return false;

			int active = 0;
			for (int i = 0; i < md.rasterList.size(); i++)
			{
				if (md.rasterList[i]->visible)
					active++;
			}
		
			fprintf(outfile, "# Bundle file v0.3\n");
			fprintf(outfile, "%d %d\n", active, 0);

			for (int i = 0; i < md.rasterList.size(); i++)
			{
				if (md.rasterList[i]->visible)
				{
					fprintf(outfile, "%f %d %d\n", md.rasterList[i]->shot.Intrinsics.FocalMm / md.rasterList[i]->shot.Intrinsics.PixelSizeMm[0], 0, 0);

					Matrix44m mat = md.rasterList[i]->shot.Extrinsics.Rot();

					Matrix33m Rt = Matrix33m(Matrix44m(mat), 3);
				
					Point3f pos = Rt * md.rasterList[i]->shot.Extrinsics.Tra();
					Rt.Transpose();

					fprintf(outfile, "%f %f %f\n", Rt[0][0], Rt[1][0], Rt[2][0]);
					fprintf(outfile, "%f %f %f\n", Rt[0][1], Rt[1][1], Rt[2][1]);
					fprintf(outfile, "%f %f %f\n", Rt[0][2], Rt[1][2], Rt[2][2]);
					fprintf(outfile, "%f %f %f\n", -pos[0], -pos[1], -pos[2]);

				}
				
			}
			fprintf(outfile, "%d %d %d\n", 0, 0,0);
			fclose(outfile);
			
		}
		else if (output==1)
		{
			name = name.append(".xml");
			
			QFileInfo fi(name);
			QDir tmpDir = QDir::current();
			QDir::setCurrent(fi.absoluteDir().absolutePath());

			//QDomDocument doc("AgisoftXML");
			QFile file(name);
			file.open(QIODevice::WriteOnly);

			QXmlStreamWriter xmlWriter(&file);
			xmlWriter.setAutoFormatting(true);
			xmlWriter.writeStartDocument();

			xmlWriter.writeStartElement("document");
			xmlWriter.writeAttribute("version", "1.2.0");

			xmlWriter.writeStartElement("chunk");
			xmlWriter.writeStartElement("sensors");
			
			for (int i = 0; i < md.rasterList.size(); i++)
			{
				if (md.rasterList[i]->visible)
				{
					float focal, pixelX, pixelY;
					if (md.rasterList[i]->shot.Intrinsics.FocalMm > 1000)
					{
						focal = md.rasterList[i]->shot.Intrinsics.FocalMm / 500;
						pixelX = md.rasterList[i]->shot.Intrinsics.PixelSizeMm[0] / 500;
						pixelY = md.rasterList[i]->shot.Intrinsics.PixelSizeMm[1] / 500;
					}
					else
					{
						focal = md.rasterList[i]->shot.Intrinsics.FocalMm;
						pixelX = md.rasterList[i]->shot.Intrinsics.PixelSizeMm[0];
						pixelY = md.rasterList[i]->shot.Intrinsics.PixelSizeMm[1];
					}

					xmlWriter.writeStartElement("sensor");
					xmlWriter.writeAttribute("id", QString::number(i));
					xmlWriter.writeAttribute("label", "unknown"+QString::number(i));
					xmlWriter.writeAttribute("type", "frame");
					xmlWriter.writeStartElement("resolution");
					xmlWriter.writeAttribute("width", QString::number(md.rasterList[i]->shot.Intrinsics.ViewportPx[0]));
					xmlWriter.writeAttribute("height", QString::number(md.rasterList[i]->shot.Intrinsics.ViewportPx[1]));
					xmlWriter.writeEndElement();
					xmlWriter.writeStartElement("property");
					xmlWriter.writeAttribute("name", "pixel_width");
					xmlWriter.writeAttribute("value", QString::number(pixelX));
					xmlWriter.writeEndElement();
					xmlWriter.writeStartElement("property");
					xmlWriter.writeAttribute("name", "pixel_height");
					xmlWriter.writeAttribute("value", QString::number(pixelY));
					xmlWriter.writeEndElement();
					xmlWriter.writeStartElement("property");
					xmlWriter.writeAttribute("name", "focal_length");
					xmlWriter.writeAttribute("value", QString::number(focal));
					xmlWriter.writeEndElement();
					xmlWriter.writeStartElement("property");
					xmlWriter.writeAttribute("name", "fixed");
					xmlWriter.writeAttribute("value", "false");
					xmlWriter.writeEndElement();
					xmlWriter.writeStartElement("calibration");
					xmlWriter.writeAttribute("type", "frame");
					xmlWriter.writeAttribute("class", "adjusted");
					xmlWriter.writeStartElement("resolution");
					xmlWriter.writeAttribute("width", QString::number(md.rasterList[i]->shot.Intrinsics.ViewportPx[0]));
					xmlWriter.writeAttribute("height", QString::number(md.rasterList[i]->shot.Intrinsics.ViewportPx[1]));
					xmlWriter.writeEndElement();
					xmlWriter.writeTextElement("fx", QString::number(md.rasterList[i]->shot.Intrinsics.FocalMm / md.rasterList[i]->shot.Intrinsics.PixelSizeMm[0]));
					xmlWriter.writeTextElement("fy", QString::number(md.rasterList[i]->shot.Intrinsics.FocalMm / md.rasterList[i]->shot.Intrinsics.PixelSizeMm[1]));
					xmlWriter.writeTextElement("cx", QString::number(md.rasterList[i]->shot.Intrinsics.CenterPx[0]));
					xmlWriter.writeTextElement("cy", QString::number(md.rasterList[i]->shot.Intrinsics.CenterPx[1]));
					xmlWriter.writeTextElement("k1", "0");
					xmlWriter.writeTextElement("k2", "0");
					xmlWriter.writeTextElement("p1", "0");
					xmlWriter.writeTextElement("p2", "0");
					xmlWriter.writeEndElement();
					xmlWriter.writeEndElement();
				}
			}
			xmlWriter.writeEndElement();
			xmlWriter.writeStartElement("cameras");
			for (int i = 0; i < md.rasterList.size(); i++)
			{
				if (md.rasterList[i]->visible)
				{
					xmlWriter.writeStartElement("camera");
					xmlWriter.writeAttribute("id", QString::number(i));
					xmlWriter.writeAttribute("label", md.rasterList[i]->currentPlane->shortName());
					xmlWriter.writeAttribute("sensor_id", QString::number(i));
					xmlWriter.writeAttribute("enabled", "true");
					Matrix44m mat = md.rasterList[i]->shot.Extrinsics.Rot();
					Point3f pos = md.rasterList[i]->shot.Extrinsics.Tra();
					QString transform= QString::number(mat[0][0]);
					transform.append(" " + QString::number(-mat[1][0]));
					transform.append(" " + QString::number(-mat[2][0]));
					transform.append(" " + QString::number(pos[0]));
					transform.append(" " + QString::number(mat[0][1]));
					transform.append(" " + QString::number(-mat[1][1]));
					transform.append(" " + QString::number(-mat[2][1]));
					transform.append(" " + QString::number(pos[1]));
					transform.append(" " + QString::number(mat[0][2]));
					transform.append(" " + QString::number(-mat[1][2]));
					transform.append(" " + QString::number(-mat[2][2]));
					transform.append(" " + QString::number(pos[2]));
					transform.append(" 0");
					transform.append(" 0");
					transform.append(" 0");
					transform.append(" 1");
					
					xmlWriter.writeTextElement("transform", transform);
					xmlWriter.writeEndElement();
				}
			}
			xmlWriter.writeEndElement();
			xmlWriter.writeEndDocument();

			file.close();
		}
	} break;

	case FP_IMPORT_CAMERAS:
	{
		QString fileName = par.getOpenFileName("ImportFile");
		if (fileName.isEmpty())
		{	
			this->errorMessage = "No file to open";
			return false;
		}
		QFileInfo fi(fileName);
		
		if ((fi.suffix().toLower() == "out"))
		{
			unsigned int   num_cams, num_points;
			FILE *fp = fopen(qUtf8Printable(fileName), "r");
			if (!fp) return false;

			////Read header

			char line[100];
			fgets(line, 100, fp);
			if (line[0] == '\0') return false;
			line[18] = '\0';
			if (0 != strcmp("# Bundle file v0.3", line))  return false;
			fgets(line, 100, fp);
			if (line[0] == '\0') return false;
			sscanf(line, "%d %d", &num_cams, &num_points);
			
			///// Check if the number of active rasters and cameras is the same
			unsigned active = 0;
			for (int i = 0; i < md.rasterList.size(); i++)
			{
				if (md.rasterList[i]->visible)
					active++;
			}

			if (active != num_cams)
			{
				this->errorMessage = "Wait! The number of active rasters and the number of cams in the Bundler file is not the same!";
				return false;
			}

			//// Import cameras
			for (uint i = 0; i < num_cams; ++i)
			{
				float f, k1, k2;
				MESHLAB_SCALAR R[16] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 };
				vcg::Point3f t;

				fgets(line, 100, fp);; if (line[0] == '\0') return false; sscanf(line, "%f %f %f", &f, &k1, &k2);

				fgets(line, 100, fp);; if (line[0] == '\0') return false; sscanf(line, "%f %f %f", &(R[0]), &(R[1]), &(R[2]));  R[3] = 0;
				fgets(line, 100, fp);; if (line[0] == '\0') return false; sscanf(line, "%f %f %f", &(R[4]), &(R[5]), &(R[6]));  R[7] = 0;
				fgets(line, 100, fp);; if (line[0] == '\0') return false; sscanf(line, "%f %f %f", &(R[8]), &(R[9]), &(R[10])); R[11] = 0;

				fgets(line, 100, fp);; if (line[0] == '\0') return false; sscanf(line, "%f %f %f", &(t[0]), &(t[1]), &(t[2]));

				Matrix44m mat(R);

				Matrix33m Rt = Matrix33m(Matrix44m(mat), 3);
				Rt.Transpose();

				Point3f pos = Rt * Point3f(t[0], t[1], t[2]);

				md.rasterList[i]->shot.Extrinsics.SetTra(Point3f(-pos[0], -pos[1], -pos[2]));
				md.rasterList[i]->shot.Extrinsics.SetRot(mat);

				md.rasterList[i]->shot.Intrinsics.FocalMm = f;
				md.rasterList[i]->shot.Intrinsics.k[0] = 0.0;//k1; To be uncommented when distortion is taken into account reliably
				md.rasterList[i]->shot.Intrinsics.k[1] = 0.0;//k2;
				md.rasterList[i]->shot.Intrinsics.PixelSizeMm = vcg::Point2f(1, 1);
				QSize size;
				QImageReader sizeImg(md.rasterList[i]->currentPlane->fullPathFileName);
				size = sizeImg.size();
				md.rasterList[i]->shot.Intrinsics.ViewportPx = vcg::Point2i(size.width(), size.height());
				md.rasterList[i]->shot.Intrinsics.CenterPx[0] = (int)((double)md.rasterList[i]->shot.Intrinsics.ViewportPx[0] / 2.0f);
				md.rasterList[i]->shot.Intrinsics.CenterPx[1] = (int)((double)md.rasterList[i]->shot.Intrinsics.ViewportPx[1] / 2.0f);	
			}
		}
		else if ((fi.suffix().toLower() == "xml"))
		{
			QDomDocument doc;
			QFile file(fileName);
			if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file))
				return false;
			std::vector<Shotf >   shots;

			////// Read and store sensors list
			QDomNodeList sensors = doc.elementsByTagName("sensor");

			if (sensors.size() == 0)
			{
				this->errorMessage = "Error!";
				return false;
			}

			shots.resize(sensors.size());
			for (int i = 0; i < sensors.size(); i++)
			{
				QDomNode n = sensors.item(i);
				int id = n.attributes().namedItem("id").nodeValue().toInt();

				QDomNode node = n.firstChild();
				//Devices
				while (!node.isNull())
				{
					if (QString::compare(node.nodeName(), "calibration") == 0)
					{
						QDomNode node1 = node.firstChild();
						//Devices
						while (!node1.isNull())
						{
							if (QString::compare(node1.nodeName(), "resolution") == 0)
							{
								int width = node1.attributes().namedItem("width").nodeValue().toInt();
								int height = node1.attributes().namedItem("height").nodeValue().toInt();
								shots[id].Intrinsics.ViewportPx[0] = width;
								shots[id].Intrinsics.ViewportPx[1] = height;
								shots[id].Intrinsics.CenterPx[0] = (float)width / 2.0f;
								shots[id].Intrinsics.CenterPx[1] = (float)height / 2.0f;
								//Log("Width %f, Height %f", shots[id].Intrinsics.CenterPx[0], shots[id].Intrinsics.CenterPx[1]);
							}
							else if (QString::compare(node1.nodeName(), "fx") == 0)
							{
								float fx = node1.toElement().text().toFloat();
								if (fx > 100)
								{
									fx = fx / 100;
									shots[id].Intrinsics.FocalMm = fx;
									shots[id].Intrinsics.PixelSizeMm[0] = 0.01f;
									shots[id].Intrinsics.PixelSizeMm[1] = 0.01f;
								}
								else
								{
									shots[id].Intrinsics.FocalMm = fx;
									shots[id].Intrinsics.PixelSizeMm[0] = 1;
									shots[id].Intrinsics.PixelSizeMm[1] = 1;
								}
								//Log("Focal %f", fx);
							}
							else if (QString::compare(node1.nodeName(), "k1") == 0)
							{
								float k1 = node1.toElement().text().toFloat();
								if (k1 != 0.0f)
								{
									this->errorMessage = "Distortion is not supported";
									log("Warning! Distortion parameters won't be imported! Please undistort the images in Photoscan before!"); // text

								}

							}
							node1 = node1.nextSibling();
						}

					}
					node = node.nextSibling();
				}
			}

			///////// Read and import cameras 
			QDomNodeList cameras = doc.elementsByTagName("camera");

			if (cameras.size() == 0)
			{
				this->errorMessage = "Error!";
				return false;
			}

			for (int i = 0; i < cameras.size(); i++)
			{
				QDomNode n = cameras.item(i);
				//int id = n.attributes().namedItem("id").nodeValue().toInt();
				int sensor_id = n.attributes().namedItem("sensor_id").nodeValue().toInt();
				QString name = n.attributes().namedItem("label").nodeValue();

				int rasterId = -1;
				for (int i = 0; i < md.rasterList.size(); i++)
				{
					if (md.rasterList[i]->currentPlane->shortName() == name)
					{
						rasterId = i;
						break;
					}
				}

				QDomNode node = n.firstChild();
				
				while (!node.isNull() && rasterId != -1)
				{
					if (QString::compare(node.nodeName(), "transform") == 0)
					{
						md.rasterList[rasterId]->shot.Intrinsics.FocalMm = shots[sensor_id].Intrinsics.FocalMm;
						md.rasterList[rasterId]->shot.Intrinsics.ViewportPx[0] = shots[sensor_id].Intrinsics.ViewportPx[0];
						md.rasterList[rasterId]->shot.Intrinsics.ViewportPx[1] = shots[sensor_id].Intrinsics.ViewportPx[1];
						md.rasterList[rasterId]->shot.Intrinsics.CenterPx[0] = shots[sensor_id].Intrinsics.CenterPx[0];
						md.rasterList[rasterId]->shot.Intrinsics.CenterPx[1] = shots[sensor_id].Intrinsics.CenterPx[1];
						md.rasterList[rasterId]->shot.Intrinsics.PixelSizeMm[0] = shots[sensor_id].Intrinsics.PixelSizeMm[0];
						md.rasterList[rasterId]->shot.Intrinsics.PixelSizeMm[1] = shots[sensor_id].Intrinsics.PixelSizeMm[1];

						QStringList values = node.toElement().text().split(" ", QString::SkipEmptyParts);
						Matrix44m mat = md.rasterList[i]->shot.Extrinsics.Rot();
						Point3f pos = md.rasterList[i]->shot.Extrinsics.Tra();
						
						mat[0][0] = values[0].toFloat();
						mat[1][0] = -values[1].toFloat();
						mat[2][0] = -values[2].toFloat();
						pos[0] = values[3].toFloat();
						mat[0][1] = values[4].toFloat();
						mat[1][1] = -values[5].toFloat();
						mat[2][1] = -values[6].toFloat();
						pos[1] = values[7].toFloat();
						mat[0][2] = values[8].toFloat();
						mat[1][2] = -values[9].toFloat();
						mat[2][2] = -values[10].toFloat();
						pos[2] = values[11].toFloat();
						md.rasterList[i]->shot.Extrinsics.SetRot(mat);
						md.rasterList[i]->shot.Extrinsics.SetTra(pos);
					}
					node = node.nextSibling();
				}
			}
		}

		else
		{
			this->errorMessage = "No file to open";
			errorMessage = "Unknown file type";
			return false;
		}
	} break;

 } // end case
 return true;
}

FilterLayerPlugin::FilterClass FilterLayerPlugin::getClass(const QAction *a) const
{
	switch(ID(a))
	{
		case FP_RENAME_MESH :
		case FP_SPLITSELECTEDFACES :
		case FP_SPLITSELECTEDVERTICES:
		case FP_DUPLICATE :
		case FP_FLATTEN :
		case FP_SELECTCURRENT :
		case FP_MESH_VISIBILITY :
		case FP_SPLITCONNECTED :
		case FP_DELETE_MESH :
		case FP_DELETE_NON_VISIBLE_MESH :        return FilterPluginInterface::Layer;
		case FP_RENAME_RASTER :
		case FP_DELETE_RASTER :
		case FP_DELETE_NON_SELECTED_RASTER :
		case FP_EXPORT_CAMERAS:	                 return FilterPluginInterface::RasterLayer;
		case FP_IMPORT_CAMERAS:                  return FilterClass(FilterPluginInterface::Camera + FilterPluginInterface::RasterLayer);
		default :  assert(0);
    }
		return FilterPluginInterface::Generic;
}

FilterPluginInterface::FILTER_ARITY FilterLayerPlugin::filterArity(const QAction* filter) const
{
    switch(ID(filter))
    {
    case FP_RENAME_MESH :
    case FP_MESH_VISIBILITY:
    case FP_SPLITSELECTEDFACES :
    case FP_SPLITSELECTEDVERTICES:
    case FP_DUPLICATE :
    case FP_SELECTCURRENT :
    case FP_SPLITCONNECTED :
    case FP_DELETE_MESH :
        return FilterPluginInterface::SINGLE_MESH;
    case FP_RENAME_RASTER :
    case FP_DELETE_RASTER :
    case FP_DELETE_NON_SELECTED_RASTER :
	case FP_EXPORT_CAMERAS:
	case FP_IMPORT_CAMERAS:
        return FilterPluginInterface::NONE;
    case FP_FLATTEN :
    case FP_DELETE_NON_VISIBLE_MESH :
        return FilterPluginInterface::VARIABLE;
    }
    return FilterPluginInterface::NONE;
}

int FilterLayerPlugin::postCondition(const QAction* filter) const
{
	switch (ID(filter))
	{
		case FP_RENAME_MESH:
		case FP_DUPLICATE:
		case FP_SELECTCURRENT:
		case FP_DELETE_MESH:
		case FP_DELETE_NON_VISIBLE_MESH:
		case FP_FLATTEN:
		case FP_RENAME_RASTER:
		case FP_DELETE_RASTER:
		case FP_DELETE_NON_SELECTED_RASTER:
		case FP_EXPORT_CAMERAS:
		case FP_IMPORT_CAMERAS:
		case FP_SPLITCONNECTED:
		case FP_MESH_VISIBILITY:            return MeshModel::MM_NONE;

		case FP_SPLITSELECTEDFACES:
		case FP_SPLITSELECTEDVERTICES:      return MeshModel::MM_GEOMETRY_AND_TOPOLOGY_CHANGE;     

		default:  assert(0);
	}
	return FilterPluginInterface::Generic;
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterLayerPlugin)
