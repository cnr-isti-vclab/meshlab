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

#include <QtScript>

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/inertia.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/update/selection.h>
#include <vcg/complex/append.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/complex/algorithms/bitquad_support.h>
#include <vcg/complex/algorithms/bitquad_optimization.h>

#include "filter_info.h"

using namespace std;
using namespace vcg;

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterInfoVMustPlugin::FilterInfoVMustPlugin() 
{ 
	typeList << FP_EXTRACT_INFO;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
QString FilterInfoVMustPlugin::filterName(FilterIDType filterId) const
{
  switch(filterId) 
	{
		case FP_EXTRACT_INFO :  return QString("Extract Information"); 
	
		default : assert(0); 
	}

  return QString();
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
 QString FilterInfoVMustPlugin::filterInfo(FilterIDType filterId) const
{
  switch(filterId) 
	{
		case FP_EXTRACT_INFO : return QString("Extract information about the input 3D model. This filter is part of the V-MUST CIF PIPELINE."); 
		default : assert(0); 
	}

	return QString("Unknown Filter");
}

// The FilterClass describes in which generic class of filters it fits. 
// This choice affect the submenu in which each filter will be placed 
// More than a single class can be choosen.
FilterInfoVMustPlugin::FilterClass FilterInfoVMustPlugin::getClass(QAction *a)
{
  switch(ID(a))
	{
		case FP_EXTRACT_INFO :  return MeshFilterInterface::Measure; 
		default : assert(0); 
	}
	return MeshFilterInterface::Generic;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterInfoVMustPlugin::initParameterSet(QAction *action,MeshModel &m, RichParameterSet & parlst) 
{
	// Nothing to set..
}

QString FilterInfoVMustPlugin::filterScriptFunctionName( FilterIDType filterID )
{
	switch(filterID) 
	{
		case FP_EXTRACT_INFO :  return QString("Extract Information"); 

		default : assert(0); 
	}
	return QString();
}

// Core Function doing the actual mesh processing.
bool FilterInfoVMustPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb)
{
	if (ID(filter) == FP_EXTRACT_INFO)
	{
		// INFORMATION EXTRACTED
		int nv=0;                                      // number of vertices
		int ne=0;                                      // number of edges
		int nf=0;                                      // number of faces
		int unreferencedVert=0;                        // number of unreferenced vertices
		int boundaryEdges=0;                           // number of boundary edges
		int connectedComponents=0;                     // number of connected components
		bool isPointCloud = false;                     // true: the input model is a point cloud
		bool isManifold = false;                       // true: mesh is two-manifold
		int nonmanifoldVertices=0;                     // number of non-manifold vertices
		int nonmanifoldEdges=0;                        // number of non-manifold edges
		int facesOnNonmanifoldVertices=0;              // number of faces incident on the non-manifold vertices
		int facesOnNonmanifoldEdges=0;                 // number of faces incident on the non-manifold edges
		int borders=0;                                 // number of borders
		int genus=0;                                   // genus
		float bbox_sx=0.0f,bbox_sy=0.0f,bbox_sz=0.0f;  // bounding box size
		float bbox_diagonal=0.0f;                      // bounding box diagonal
		float area=0.0f;                               // surface area
		float volume=0.0f;                             // enclosed volume
		Point3f barycenter;                            // thin shell barycenter
		Point3f masscenter;                            // center of mass
		bool flagVN=false;                             // per-vertex normal
		bool flagFN=false;                             // per-face normal
		bool flagVC=false;                             // per-vertex color
		bool flagFC=false;                             // per-face color
		bool flagVT=false;                             // per-vertex texture coordinates

		// replace the extension with 'txt'
		QString modelname = md.mm()->shortName();
		QFileInfo finfo(md.mm()->shortName());
		QString ext = finfo.suffix();
		QString filename(modelname);
		filename.replace(QString(ext), QString("txt"));

		QFile data(filename);
		if (data.open(QFile::WriteOnly | QFile::Truncate)) 
		{
			QTextStream fout(&data);

			/**** COMPUTE TOPOLOGICAL INFORMATION
			/***********************************************************************************/

			CMeshO &m=md.mm()->cm;

			tri::Allocator<CMeshO>::CompactFaceVector(m);
			tri::Allocator<CMeshO>::CompactVertexVector(m);
			md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);
			md.mm()->updateDataMask(MeshModel::MM_VERTFACETOPO);				

			nv = m.vn;
			nf = m.fn;

			tri::Clean<CMeshO>::CountEdges(m, ne, boundaryEdges);

			// IMPORTANT! A point cloud is a mesh composed by vertices only. 
			//            This implies that a mesh composed by few triangles and a lot of
			//            vertices is considered as a mesh with many unreferenced vertices.
			if (ne == 0 && nf == 0)
			{
				isPointCloud = true;
			}

			if (!isPointCloud)
			{
				// ONLY FOR MESHES (!!)

				nonmanifoldEdges = tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m,true);
				facesOnNonmanifoldEdges = tri::UpdateSelection<CMeshO>::FaceCount(m);
				tri::UpdateSelection<CMeshO>::VertexClear(m);
				tri::UpdateSelection<CMeshO>::FaceClear(m);

				nonmanifoldVertices = tri::Clean<CMeshO>::CountNonManifoldVertexFF(m,true);
				tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m);
				facesOnNonmanifoldVertices = tri::UpdateSelection<CMeshO>::FaceCount(m);

				unreferencedVert = tri::Clean<CMeshO>::CountUnreferencedVertex(m);

				connectedComponents = tri::Clean<CMeshO>::CountConnectedComponents(m);

				if (nonmanifoldVertices == 0 && nonmanifoldEdges == 0)
				{
					isManifold = true;
				}

				// For a two-manifold mesh additional stuffs are computed
				if (isManifold)
				{
					borders = tri::Clean<CMeshO>::CountHoles(m);
					genus = tri::Clean<CMeshO>::MeshGenus(m.vn - unreferencedVert, ne, m.fn, borders, connectedComponents);
				}
			}

			/***** COMPUTE GEOMETRIC INFORMATION
			/***********************************************************************************/

			bbox_sx = m.bbox.DimX();
			bbox_sy = m.bbox.DimY();
			bbox_sz = m.bbox.DimZ();
			bbox_diagonal = m.bbox.Diag();

			if (isPointCloud)
			{
				// computer center of mass
				
				CMeshO::VertexIterator it;
				CMeshO::VertexType v;
				v.P()[0] = v.P()[1] = v.P()[2] = 0.0f;
				for (it = m.vert.begin(); it != m.vert.end(); it++)
				{
					v.P() += (*it).P();
				}

				masscenter = barycenter = v.P() / static_cast<float>(m.vert.size());
			}
			else
			{
				// ONLY FOR MESHES (!!)

				tri::Inertia<CMeshO> I(m);
				volume = I.Mass();
				masscenter = I.CenterOfMass();

				area = tri::Stat<CMeshO>::ComputeMeshArea(m);

				barycenter = tri::Stat<CMeshO>::ComputeShellBarycenter(m);
			}
			
			/***** CHECK ATTRIBUTES
			/***********************************************************************************/

			if (tri::HasPerVertexNormal(m))
				flagVN = true;
			else
				flagVN = false;

			if (tri::HasPerFaceNormal(m))
				flagFN = true;
			else
				flagFN = false;

			if (tri::HasPerVertexColor(m)) 
				flagVC = true;
			else
				flagVC = false;

			if (tri::HasPerFaceColor(m))
				flagFC = true;
			else
				flagFC = false;

			if (tri::HasPerVertexTexCoord(m))
				flagVT = true;
			else
				flagVT = false;


			/***** OUTPUT
			/***********************************************************************************/

			fout << "Model name: " << modelname.toStdString().c_str() << "  [ ";
			
			if (flagVN) fout << "VN ";
			if (flagFN) fout << "FN ";
			if (flagVC) fout << "VC ";
			if (flagFC) fout << "FC ";
			if (flagVT) fout << "VT ";

			fout << "]";

			fout << endl << endl;
			fout << "Number of Vertices: " << nv << endl;
			fout << "Number of Edges: " << ne << endl;
			fout << "Number of Faces: " << nf << endl << endl;

			if (isPointCloud)
			{
				fout << "Point Cloud: YES" << endl;

				fout << "Number of unreferenced vertices: N/A" << endl;
				fout << "Number of boundary edges: N/A" << endl;
				fout << "Number of connected components: " << connectedComponents << endl; // it must be zero for a point cloud

				fout << "Two-manifold: N/A" << endl;
				fout << "Non-manifold vertices: N/A" << endl;
				fout << "Non-manifold edges: N/A" << endl;
				fout << "Faces incident on non-manifold vertices: N/A" << endl; 
				fout << "Faces incident on non-manifold edges: N/A " << endl;
				fout << "Borders: N/A" << endl;
				fout << "Genus: N/A" << endl;

				fout << endl;
				fout << "Bounding box size: " << bbox_sx << " " << bbox_sy << " " << bbox_sz << endl;
				fout << "Bounding box diagonal: " << bbox_diagonal << endl;
				fout << "Volume: N/A" << endl;
				fout << "Surface area: N/A" << endl;
				fout << "Mass center: " << masscenter[0] << " " << masscenter[1] << " " << masscenter[2] << endl;
				fout << "Barycenter: " << barycenter[0] << " " << barycenter[1] << " " << barycenter[2] << endl;

			}
			else
			{
				fout << "Point Cloud: NO" << endl;

				fout << "Number of unreferenced vertices: " << unreferencedVert << endl;
				fout << "Number of boundary edges: " << boundaryEdges << endl;
				fout << "Number of connected components: " << connectedComponents << endl;
			
				if (isManifold)
				{
					fout << "Two-manifold: YES" << endl;
					fout << "Non-manifold vertices: " << nonmanifoldVertices << endl; // it must be zero for two-manifold meshes
					fout << "Non-manifold edges: " << nonmanifoldEdges << endl;   // it must be zero for two-manifold meshes
					fout << "Faces incident on non-manifold vertices: " << facesOnNonmanifoldVertices << endl; // it must be zero for two-manifold meshes
					fout << "Faces incident on non-manifold edges: " << facesOnNonmanifoldEdges << endl; // it must be zero for two-manifold meshes
					fout << "Borders: " << borders << endl;
					fout << "Genus: " << genus << endl;
				}
				else
				{
					fout << "Two-manifold: NO" << endl;
					fout << "Non-manifold vertices: " << nonmanifoldVertices << endl;
					fout << "Non-manifold edges: " << nonmanifoldEdges << endl;
					fout << "Faces incident on non-manifold vertices: " << facesOnNonmanifoldVertices << endl;
					fout << "Faces incident on non-manifold edges: " << facesOnNonmanifoldEdges << endl;
					fout << "Borders: N/A" << endl;
					fout << "Genus: N/A" << endl;
				}

				fout << endl;
				fout << "Bounding box size: " << bbox_sx << " " << bbox_sy << " " << bbox_sz << endl;
				fout << "Bounding box diagonal: " << bbox_diagonal << endl;
				fout << "Volume: " << volume << endl;
				fout << "Surface area: " << area << endl;
				fout << "Mass center: " << masscenter[0] << " " << masscenter[1] << " " << masscenter[2] << endl;
				fout << "Barycenter: " << barycenter[0] << " " << barycenter[1] << " " << barycenter[2] << endl;
			}

			data.close();

			return true;
		}
		else
			return false;
	}

	return true;
}


Q_EXPORT_PLUGIN(FilterInfoVMustPlugin)


