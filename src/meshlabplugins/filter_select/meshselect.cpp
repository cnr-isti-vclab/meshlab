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
#include <vcg/space/colorspace.h>
#include "meshselect.h"
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/point_outlier.h>

using namespace vcg;

// ERROR CHECKING UTILITY
#define CheckError(x,y); if ((x)) {this->errorMessage = (y); return false;}
///////////////////////////////////////////////////////

SelectionFilterPlugin::SelectionFilterPlugin()
{
  typeList <<
    FP_SELECT_ALL <<
    FP_SELECT_NONE <<
    FP_SELECTBYANGLE <<
	FP_SELECT_UGLY <<
    FP_SELECT_DELETE_VERT <<
	FP_SELECT_DELETE_ALL_FACE <<
    FP_SELECT_DELETE_FACE <<
    FP_SELECT_DELETE_FACEVERT <<
    FP_SELECT_FACE_FROM_VERT <<
    FP_SELECT_VERT_FROM_FACE <<
    FP_SELECT_ERODE <<
    FP_SELECT_DILATE <<
    FP_SELECT_BORDER <<
    FP_SELECT_INVERT <<
    FP_SELECT_CONNECTED <<
    FP_SELECT_BY_VERT_QUALITY <<
    FP_SELECT_BY_FACE_QUALITY <<
    CP_SELFINTERSECT_SELECT <<
    CP_SELECT_TEXBORDER <<
    CP_SELECT_NON_MANIFOLD_FACE <<
    CP_SELECT_NON_MANIFOLD_VERTEX <<
    FP_SELECT_FACES_BY_EDGE <<            
    FP_SELECT_BY_COLOR <<
    FP_SELECT_FOLD_FACE << 
    FP_SELECT_OUTLIER;

  FilterIDType tt;

  foreach(tt , types())
    {
      actionList << new QAction(filterName(tt), this);
      if(tt==FP_SELECT_DELETE_VERT){
            actionList.last()->setShortcut(QKeySequence ("Ctrl+Del"));
            actionList.last()->setIcon(QIcon(":/images/delete_vert.png"));
			actionList.last()->setPriority(QAction::HighPriority);
      }
      if(tt==FP_SELECT_DELETE_FACE){
            actionList.last()->setShortcut(QKeySequence (Qt::Key_Delete));
            actionList.last()->setIcon(QIcon(":/images/delete_face.png"));
			actionList.last()->setPriority(QAction::HighPriority);
      }
      if(tt==FP_SELECT_DELETE_FACEVERT){
            actionList.last()->setShortcut(QKeySequence ("Shift+Del"));
            actionList.last()->setIcon(QIcon(":/images/delete_facevert.png"));
			actionList.last()->setPriority(QAction::HighPriority);
      }
	  if (tt == FP_SELECT_ALL){
		  actionList.last()->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_A);
		  actionList.last()->setIcon(QIcon(":/images/sel_all.png"));
		  actionList.last()->setPriority(QAction::LowPriority);
	  }
	  if (tt == FP_SELECT_NONE){
		  actionList.last()->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_D);
		  actionList.last()->setIcon(QIcon(":/images/sel_none.png"));
		  actionList.last()->setPriority(QAction::LowPriority);
	  }
	  if (tt == FP_SELECT_INVERT){
		  actionList.last()->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_I);
		  actionList.last()->setIcon(QIcon(":/images/sel_inv.png"));
		  actionList.last()->setPriority(QAction::LowPriority);
	  }
	  if (tt == FP_SELECT_DILATE){
		  actionList.last()->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Plus);
		  actionList.last()->setIcon(QIcon(":/images/sel_plus.png"));
		  actionList.last()->setPriority(QAction::LowPriority);
	  }
	  if (tt == FP_SELECT_ERODE){
		  actionList.last()->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Minus);
		  actionList.last()->setIcon(QIcon(":/images/sel_minus.png"));
		  actionList.last()->setPriority(QAction::LowPriority);
	  }
    }
}

 QString SelectionFilterPlugin::filterName(FilterIDType filter) const
{
 switch(filter)
 {
	case FP_SELECT_ALL :                  return tr("Select All");
	case FP_SELECT_NONE :                 return tr("Select None");
	case FP_SELECT_INVERT :               return tr("Invert Selection");
    case FP_SELECT_CONNECTED :            return tr("Select Connected Faces");
	case FP_SELECT_DELETE_VERT :          return tr("Delete Selected Vertices");
	case FP_SELECT_DELETE_ALL_FACE :      return tr("Delete ALL Faces");
	case FP_SELECT_DELETE_FACE :          return tr("Delete Selected Faces");
	case FP_SELECT_DELETE_FACEVERT :      return tr("Delete Selected Faces and Vertices");
	case FP_SELECTBYANGLE :               return tr("Select Faces by view angle");
	case FP_SELECT_UGLY :                 return tr("Select 'problematic' faces");
	case FP_SELECT_FACE_FROM_VERT :       return tr("Select Faces from Vertices");
	case FP_SELECT_VERT_FROM_FACE :       return tr("Select Vertices from Faces");
	case FP_SELECT_ERODE :                return tr("Erode Selection");
	case FP_SELECT_DILATE :               return tr("Dilate Selection");
	case FP_SELECT_BORDER :               return tr("Select Border");
	case FP_SELECT_BY_VERT_QUALITY :      return tr("Select by Vertex Quality");
	case FP_SELECT_BY_FACE_QUALITY :      return tr("Select by Face Quality");
	case FP_SELECT_BY_COLOR :             return tr("Select Faces by Color");
	case CP_SELFINTERSECT_SELECT :        return tr("Select Self Intersecting Faces");
	case CP_SELECT_TEXBORDER :            return tr("Select Vertex Texture Seams");
	case CP_SELECT_NON_MANIFOLD_FACE :    return tr("Select non Manifold Edges ");
	case CP_SELECT_NON_MANIFOLD_VERTEX :  return tr("Select non Manifold Vertices");
	case FP_SELECT_FACES_BY_EDGE:         return tr("Select Faces with edges longer than...");
	case FP_SELECT_FOLD_FACE :            return tr("Select Folded Faces");
	case  FP_SELECT_OUTLIER:              return tr("Select Outliers");
 }
 assert(0);
 return QString("Unknown filter");
}

QString SelectionFilterPlugin::filterInfo(FilterIDType filterId) const
{
 switch(filterId)
 {
	case FP_SELECT_DILATE :             return tr("Dilate (expand) the current set of selected faces.");
	case FP_SELECT_ERODE :              return tr("Erode (reduce) the current set of selected faces.");
	case FP_SELECT_INVERT :             return tr("Invert the current set of selected faces/vertices.");
    case FP_SELECT_CONNECTED:           return tr("Expand the current face selection so that it includes all the faces in the connected components where there is at least a selected face.");
	case FP_SELECT_NONE :               return tr("Clear the current set of selected faces/vertices.");
	case FP_SELECT_ALL :                return tr("Select all the faces/vertices of the current mesh.");
	case FP_SELECT_DELETE_VERT :        return tr("Delete the current set of selected vertices; faces that share one of the deleted vertexes are deleted too.");
	case FP_SELECT_DELETE_ALL_FACE:     return tr("Delete ALL faces, turning the mesh into a pointcloud. May be applied also to all visible layers.");
	case FP_SELECT_DELETE_FACE :        return tr("Delete the current set of selected faces, vertices that remains unreferenced are not deleted.");
	case FP_SELECT_DELETE_FACEVERT :    return tr("Delete the current set of selected faces and all the vertices surrounded by that faces.");
	case FP_SELECTBYANGLE :             return tr("Select faces according to the angle between their normal and the view direction. It is used in range map processing to select and delete steep faces parallel to viewdirection.");
	case FP_SELECT_UGLY :               return tr("Select faces with 'problems', like normal inverted w.r.t the surrounding areas, or extremely elongated");
	case CP_SELFINTERSECT_SELECT :      return tr("Select only self intersecting faces.");
	case FP_SELECT_FACE_FROM_VERT :     return tr("Select faces from selected vertices.");
	case FP_SELECT_VERT_FROM_FACE :     return tr("Select vertices from selected faces.");
	case FP_SELECT_FACES_BY_EDGE :      return tr("Select all triangles having an edge with lenght greater or equal than a given threshold.");     
	case FP_SELECT_BORDER :             return tr("Select vertices and faces on the boundary.");
	case FP_SELECT_BY_VERT_QUALITY :    return tr("Select all the faces/vertexes within the specified vertex quality range.");
	case FP_SELECT_BY_FACE_QUALITY :    return tr("Select all the faces/vertexes with within the specified face quality range.");
	case FP_SELECT_BY_COLOR :           return tr("Select part of the mesh based on its color.");
	case CP_SELECT_TEXBORDER :          return tr("Colorize only border edges.");
	case CP_SELECT_NON_MANIFOLD_FACE:   return tr("Select the faces and the vertices incident on non manifold edges (e.g. edges where more than two faces are incident); note that this function select the components that are related to non manifold edges. The case of non manifold vertices is specifically managed by the pertinent filter.");
	case CP_SELECT_NON_MANIFOLD_VERTEX: return tr("Select the non manifold vertices that do not belong to non manifold edges. For example two cones connected by their apex. Vertices incident on non manifold edges are ignored.");
	case FP_SELECT_FOLD_FACE:           return tr("Select the folded faces created by the Quadric Edge Collapse decimation. The face is selected if the angle between the face normal and the normal of the best fitting plane of the neighbor vertices is above the selected threshold.");
	case  FP_SELECT_OUTLIER:            return tr("Select the vertex classified as outlier using Local Outlier Propabilty measure described in:<br> <b>'LoOP: Local Outlier Probabilities'</b> Kriegel et al.<br>CIKM 2009");
 }
 assert(0);
 return QString("Unknown filter");
}

void SelectionFilterPlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst)
{
 switch(ID(action))
 {
	case FP_SELECT_FACES_BY_EDGE:
	{
		float maxVal = m.cm.bbox.Diag()/2.0f;
		parlst.addParam(new RichDynamicFloat("Threshold",maxVal*0.01,0,maxVal,"Edge Threshold", "All the faces with an edge <b>longer</b> than this threshold will be deleted. Useful for removing long skinny faces obtained by bad triangulation of range maps."));
	} break;
        
	case FP_SELECT_BORDER:
      //parlst.addParam(new RichInt("Iteration", true, "Inclusive Sel.", "If true only the faces with <b>all</b> selected vertices are selected. Otherwise any face with at least one selected vertex will be selected."));
	break;

	case FP_SELECTBYANGLE :
	{
		parlst.addParam(new RichDynamicFloat("anglelimit", 75.0f, 0.0f, 180.0f, "angle threshold (deg)", "faces with normal at higher angle w.r.t. the view direction are selected"));
		parlst.addParam(new RichBool ("usecamera", false, "Use ViewPoint from Mesh Camera", "Uses the ViewPoint from the camera associated to the current mesh\n if there is no camera, an error occurs"));
		parlst.addParam(new RichPoint3f("viewpoint", Point3f(0.0f, 0.0f, 0.0f), "ViewPoint", "if UseCamera is true, this value is ignored"));
	} break;

	case FP_SELECT_UGLY :
		parlst.addParam(new RichBool("useAR", true, "select by Aspect Ratio", "if true, faces with aspect ratio below the limit will be selected"));
		parlst.addParam(new RichDynamicFloat("ARatio", 0.02, 0.0, 1.0, tr("Aspect Ratio"), tr("Triangle face aspect ratio [1 (equilateral) - 0 (line)]: face will be selected if BELOW this threshold")));
		parlst.addParam(new RichBool("useNF", false, "select by Normal Angle", "if true, adjacent faces with normals forming an angle above the limit will be selected"));
		parlst.addParam(new RichDynamicFloat("NFRatio", 60, 0.0, 180.0, tr("Angle flip"), tr("angle between the adjacent faces: face will be selected if ABOVE this threshold")));
	break;

	case FP_SELECT_FACE_FROM_VERT:
		parlst.addParam(new RichBool("Inclusive", true, "Strict Selection", "If true only the faces with <b>all</b> selected vertices are selected. Otherwise any face with at least one selected vertex will be selected."));
	break;

    case FP_SELECT_VERT_FROM_FACE:
		parlst.addParam(new RichBool("Inclusive", true, "Strict Selection", "If true only the vertices with <b>all</b> the incident face selected are selected. Otherwise any vertex with at least one incident selected face will be selected."));
	break;

    case FP_SELECT_BY_VERT_QUALITY:
	{
		std::pair<float,float> minmax =  tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.cm);
		float minq=minmax.first;
		float maxq=minmax.second;

		parlst.addParam(new RichDynamicFloat("minQ", minq*0.75+maxq*.25, minq, maxq,  tr("Min Quality"), tr("Minimum acceptable quality value") ));
		parlst.addParam(new RichDynamicFloat("maxQ", minq*0.25+maxq*.75, minq, maxq,  tr("Max Quality"), tr("Maximum acceptable quality value") ));
		parlst.addParam(new RichBool("Inclusive", true, "Inclusive Sel.", "If true only the faces with <b>all</b> the vertices within the specified range are selected. Otherwise any face with at least one vertex within the range is selected."));
	} break;

	case FP_SELECT_BY_FACE_QUALITY:
	{
		std::pair<float,float> minmax =  tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(m.cm);
		float minq=minmax.first;
		float maxq=minmax.second;

		parlst.addParam(new RichDynamicFloat("minQ", minq*0.75+maxq*.25, minq, maxq,  tr("Min Quality"), tr("Minimum acceptable quality value") ));
		parlst.addParam(new RichDynamicFloat("maxQ", minq*0.25+maxq*.75, minq, maxq,  tr("Max Quality"), tr("Maximum acceptable quality value") ));
		parlst.addParam(new RichBool("Inclusive", true, "Inclusive Sel.", "If true only the vertices with <b>all</b> the adjacent faces within the specified range are selected. Otherwise any vertex with at least one face within the range is selected."));
	} break;
      
	case FP_SELECT_BY_COLOR:
	{
		parlst.addParam(new RichColor("Color",Color4b::Black, tr("Color To Select"), tr("Color that you want to be selected.") ));

		QStringList colorspace;
		colorspace << "HSV" << "RGB";
		parlst.addParam(new RichEnum("ColorSpace", 0, colorspace, tr("Pick Color Space"), tr("The color space that the sliders will manipulate.") ));

		parlst.addParam(new RichBool("Inclusive", true, "Inclusive Sel.", "If true only the faces with <b>all</b> the vertices within the specified range are selected. Otherwise any face with at least one vertex within the range is selected."));

		parlst.addParam(new RichDynamicFloat("PercentRH", 0.2f, 0.0f, 1.0f,  tr("Variation from Red or Hue"), tr("A float between 0 and 1 that represents the percent variation from this color that will be selected.  For example if the R was 200 and you put 0.1 then any color with R 200+-25.5 will be selected.") ));
		parlst.addParam(new RichDynamicFloat("PercentGS", 0.2f, 0.0f, 1.0f,  tr("Variation from Green or Saturation"), tr("A float between 0 and 1 that represents the percent variation from this color that will be selected.  For example if the R was 200 and you put 0.1 then any color with R 200+-25.5 will be selected.") ));
		parlst.addParam(new RichDynamicFloat("PercentBV", 0.2f, 0.0f, 1.0f,  tr("Variation from Blue or Value"), tr("A float between 0 and 1 that represents the percent variation from this color that will be selected.  For example if the R was 200 and you put 0.1 then any color with R 200+-25.5 will be selected.") ));
	} break;

	case FP_SELECT_ALL:
	{
		parlst.addParam(new RichBool("allFaces", true, "Select all Faces", "If true the filter will select all the faces."));
		parlst.addParam(new RichBool("allVerts", true, "Select all Vertices", "If true the filter will select all the vertices."));
	} break;

	case FP_SELECT_NONE:
	{
		parlst.addParam(new RichBool("allFaces", true, "De-select all Faces", "If true the filter will de-select all the faces."));
		parlst.addParam(new RichBool("allVerts", true, "De-select all Vertices", "If true the filter will de-select all the vertices."));
	} break;

	case FP_SELECT_INVERT:
	{
		bool defF = (m.cm.sfn > 0) ? true : false;
		bool defV = (m.cm.svn > 0) ? true : false;

		parlst.addParam(new RichBool("InvFaces", defF, "Invert Faces", "If true the filter will invert the set of selected faces."));
		parlst.addParam(new RichBool("InvVerts", defV, "Invert Vertices", "If true the filter will invert the set of selected vertices."));
	} break;

	case FP_SELECT_FOLD_FACE:
	{
		parlst.addParam(new RichDynamicFloat("AngleThreshold", 160.0f, 90.0f, 180.0f, tr("Angle Threshold"), tr("Angle between the face and the best fitting plane of the neighbours vertices. If it is above the threshold the face is selected.")));
	} break;

	case FP_SELECT_OUTLIER:
	{
		parlst.addParam(new RichDynamicFloat("PropThreshold", 0.8, 0.0, 1.0, tr("Probability"), tr("Threshold to select the vertex. The vertex is selected if the LoOP value is above the threshold.")));
		parlst.addParam(new RichInt("KNearest", 32, tr("Number of neighbors"), tr("Number of neighbours used to compute the LoOP")));
	} break;

	case FP_SELECT_DELETE_ALL_FACE:
	{
		parlst.addParam(new RichBool("allLayers", false, "Apply to all visible Layers", "If selected, the filter will be applied to all visible mesh Layers."));
	} break;
 }
}

bool SelectionFilterPlugin::applyFilter(QAction *action, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos * /*cb*/)
{
if (md.mm() == NULL)
	return false;

MeshModel &m=*(md.mm());
CMeshO::FaceIterator fi;
CMeshO::VertexIterator vi;

switch(ID(action))
{
	case FP_SELECT_DELETE_VERT:
	{
		if (m.cm.svn == 0) { Log("Nothing done: no vertex selected"); break; }
		tri::UpdateSelection<CMeshO>::FaceClear(m.cm);
		tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m.cm);
		int vvn = m.cm.vn;
		int ffn = m.cm.fn;
		for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
			if (!(*fi).IsD() && (*fi).IsS())
				tri::Allocator<CMeshO>::DeleteFace(m.cm, *fi);
		for (vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
			if (!(*vi).IsD() && (*vi).IsS())
				tri::Allocator<CMeshO>::DeleteVertex(m.cm, *vi);
		m.clearDataMask(MeshModel::MM_FACEFACETOPO);
		m.clearDataMask(MeshModel::MM_VERTFACETOPO);
		m.UpdateBoxAndNormals();
		Log("Deleted %i vertices, %i faces.", vvn - m.cm.vn, ffn - m.cm.fn);
	} break;

	case FP_SELECT_DELETE_ALL_FACE:
	{
		if (par.getBool("allLayers"))
		{
			MeshModel   *ml = NULL;
			while (ml = md.nextVisibleMesh(ml))
			{
				int ffn = ml->cm.fn;
				for (fi = ml->cm.face.begin(); fi != ml->cm.face.end(); ++fi)
				if (!(*fi).IsD())
					tri::Allocator<CMeshO>::DeleteFace(ml->cm, *fi);
				ml->clearDataMask(MeshModel::MM_FACEFACETOPO);
				ml->clearDataMask(MeshModel::MM_VERTFACETOPO);
				ml->UpdateBoxAndNormals();
				Log("Layer %i: deleted all %i faces.", ml->id(), ffn - ml->cm.fn);
			}
		}
		else
		{
			int ffn = m.cm.fn;
			for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
			if (!(*fi).IsD())
				tri::Allocator<CMeshO>::DeleteFace(m.cm, *fi);
			m.clearDataMask(MeshModel::MM_FACEFACETOPO);
			m.clearDataMask(MeshModel::MM_VERTFACETOPO);
			m.UpdateBoxAndNormals();
			Log("Deleted all %i faces.", ffn - m.cm.fn);
		}
	} break;

	case FP_SELECT_DELETE_FACE:
	{
		if (m.cm.sfn == 0) { Log("Nothing done: no faces selected"); break; }
		int ffn = m.cm.fn;
		for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
			if (!(*fi).IsD() && (*fi).IsS())
				tri::Allocator<CMeshO>::DeleteFace(m.cm, *fi);
		m.clearDataMask(MeshModel::MM_FACEFACETOPO);
		m.clearDataMask(MeshModel::MM_VERTFACETOPO);
		m.UpdateBoxAndNormals();
		Log("Deleted %i faces.", ffn - m.cm.fn);
	} break;

	case FP_SELECT_DELETE_FACEVERT:
	{
		if (m.cm.sfn == 0) { Log("Nothing done: no faces selected"); break; }
		tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
		tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
		int vvn = m.cm.vn;
		int ffn = m.cm.fn;
		for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
			if (!(*fi).IsD() && (*fi).IsS())
				tri::Allocator<CMeshO>::DeleteFace(m.cm, *fi);
		for (vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
			if (!(*vi).IsD() && (*vi).IsS())
				tri::Allocator<CMeshO>::DeleteVertex(m.cm, *vi);
		m.clearDataMask(MeshModel::MM_FACEFACETOPO);
		m.clearDataMask(MeshModel::MM_VERTFACETOPO);
		m.UpdateBoxAndNormals();
		Log("Deleted %i faces, %i vertices.", ffn - m.cm.fn, vvn - m.cm.vn);
	} break;

	case FP_SELECT_CONNECTED:
		tri::UpdateSelection<CMeshO>::FaceConnectedFF(m.cm);
	break;

	case FP_SELECTBYANGLE :
	{
		CMeshO::FaceIterator   fi;
		bool usecam = par.getBool("usecamera");
		Point3m viewpoint =  par.getPoint3m("viewpoint");

		// if usecamera but mesh does not have one
		if( usecam && !m.hasDataMask(MeshModel::MM_CAMERA) )
		{
			errorMessage = "Mesh has not a camera that can be used to compute view direction. Please set a view direction."; // text
			return false;
		}
		if(usecam)
		{
			viewpoint = m.cm.shot.GetViewPoint();
		}

		// angle threshold in radians
		float limit = cos( math::ToRad(par.getDynamicFloat("anglelimit")) );
		Point3m viewray;

		for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
			if(!(*fi).IsD())
			{
				viewray = Barycenter(*fi) - viewpoint;
				viewray.Normalize();

				if((viewray.dot((*fi).N().Normalize())) < limit)
					fi->SetS();
			}

	} break;

	case FP_SELECT_UGLY :
	{
		// elongated
		if (par.getBool("useAR"))
			for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
				if (!(*fi).IsD())
				{
					float QQ = QualityRadii((*fi).V(0)->P(), (*fi).V(1)->P(), (*fi).V(2)->P());
					if (QQ<par.getFloat("ARatio"))
						fi->SetS();
				}

		// flipped normal
		if (par.getBool("useNF"))
		{
			md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);
			for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
				if (!(*fi).IsD())
				{
					float QQ = 0;
					for (int ei = 0; ei < 3; ei++)
					{
						CMeshO::FacePointer adjf = (*fi).FFp(ei);
						float nangle = (*adjf).N().Normalize() * (*fi).N().Normalize();
						nangle = math::ToDeg(fabsf(acos(nangle)));
						if (nangle > QQ) QQ = nangle;
					}

					if (QQ > par.getFloat("NFRatio"))
						fi->SetS();
				}
			m.clearDataMask(MeshModel::MM_FACEFACETOPO);
		}

	} break;

	case FP_SELECT_ALL    :
		if (par.getBool("allVerts"))
			tri::UpdateSelection<CMeshO>::VertexAll(m.cm);
		if (par.getBool("allFaces"))
			tri::UpdateSelection<CMeshO>::FaceAll(m.cm);
	break;

	case FP_SELECT_NONE   :
		if (par.getBool("allVerts"))
			tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
		if (par.getBool("allFaces"))
			tri::UpdateSelection<CMeshO>::FaceClear(m.cm);   
	break;

	case FP_SELECT_INVERT :
		if (par.getBool("InvVerts"))
			tri::UpdateSelection<CMeshO>::VertexInvert(m.cm);
		if (par.getBool("InvFaces"))
			tri::UpdateSelection<CMeshO>::FaceInvert(m.cm);  
	break;

	case FP_SELECT_VERT_FROM_FACE:
		if (par.getBool("Inclusive"))
			tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
		else 
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);
	break;

	case FP_SELECT_FACE_FROM_VERT:
		if (par.getBool("Inclusive"))
			tri::UpdateSelection<CMeshO>::FaceFromVertexStrict(m.cm);
		else 
			tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m.cm);
	break;

	case FP_SELECT_ERODE: 
		tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
		tri::UpdateSelection<CMeshO>::FaceFromVertexStrict(m.cm);
	break;

	case FP_SELECT_DILATE: 
		tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);
		tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m.cm);
	break;

	case FP_SELECT_BORDER:
		tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m.cm);
		tri::UpdateFlags<CMeshO>::VertexBorderFromFaceBorder(m.cm);
		tri::UpdateSelection<CMeshO>::FaceFromBorderFlag(m.cm);
		tri::UpdateSelection<CMeshO>::VertexFromBorderFlag(m.cm);
	break;

	case FP_SELECT_BY_VERT_QUALITY:
	{
		float minQ = par.getDynamicFloat("minQ");
		float maxQ = par.getDynamicFloat("maxQ");
		bool inclusiveFlag = par.getBool("Inclusive");
		tri::UpdateSelection<CMeshO>::VertexFromQualityRange(m.cm, minQ, maxQ);
		if (inclusiveFlag) 
			tri::UpdateSelection<CMeshO>::FaceFromVertexStrict(m.cm);
		else 
			tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m.cm);
	} break;

	case FP_SELECT_BY_FACE_QUALITY:
	{
		float minQ = par.getDynamicFloat("minQ");
		float maxQ = par.getDynamicFloat("maxQ");
		bool inclusiveFlag = par.getBool("Inclusive");
		tri::UpdateSelection<CMeshO>::FaceFromQualityRange(m.cm, minQ, maxQ);
		if (inclusiveFlag) 
			tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
		else 
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);
	} break;

	case FP_SELECT_BY_COLOR:
	{
		int colorSpace = par.getEnum("ColorSpace");
		QColor targetColor = par.getColor("Color");

		float red = targetColor.redF();
		float green = targetColor.greenF();
		float blue = targetColor.blueF();

		float hue = targetColor.hue()/360.0f;// Normalized into [0..1) range
		float saturation = targetColor.saturationF();
		float value = targetColor.valueF();

		//like fuzz factor in photoshop
		float valueRH = par.getDynamicFloat("PercentRH");
		float valueGS = par.getDynamicFloat("PercentGS");
		float valueBV = par.getDynamicFloat("PercentBV");

		tri::UpdateSelection<CMeshO>::FaceClear(m.cm);
		tri::UpdateSelection<CMeshO>::VertexClear(m.cm);

		//now loop through all the faces
		for(vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
		{
			if(!(*vi).IsD())
			{
                    Color4f colorv = Color4f::Construct((*vi).C());
                    if(colorSpace == 0){
                        colorv = ColorSpace<float>::RGBtoHSV(colorv);
                        if( fabsf(colorv[0] - hue) <= valueRH &&
                            fabsf(colorv[1] - saturation) <= valueGS &&
                            fabsf(colorv[2] - value) <= valueBV  )
                                   (*vi).SetS();
                    }
                    else    {
                    if( fabsf(colorv[0] - red) <= valueRH &&
                        fabsf(colorv[1] - green) <= valueGS &&
                        fabsf(colorv[2] - blue) <= valueBV  )
                                    (*vi).SetS();
                    }
			}
		}
		if(par.getBool("Inclusive")) 
			tri::UpdateSelection<CMeshO>::FaceFromVertexStrict(m.cm);
		else 
			tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m.cm);
	} break;

	case CP_SELECT_TEXBORDER:
		tri::UpdateTopology<CMeshO>::FaceFaceFromTexCoord(m.cm);
		tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
		tri::UpdateFlags<CMeshO>::VertexBorderFromFaceBorder(m.cm);
		tri::UpdateSelection<CMeshO>::VertexFromBorderFlag(m.cm);
		// Just to be sure restore standard topology and border flags
		tri::UpdateTopology<CMeshO>::FaceFace(m.cm);
		tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);
		tri::UpdateFlags<CMeshO>::VertexBorderFromFaceBorder(m.cm);
	break;

	case CP_SELECT_NON_MANIFOLD_FACE:
		tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm,true);
	break;

	case CP_SELECT_NON_MANIFOLD_VERTEX:
		tri::Clean<CMeshO>::CountNonManifoldVertexFF(m.cm,true);
	break;

	case CP_SELFINTERSECT_SELECT:
	{
		std::vector<CFaceO *> IntersFace;
		tri::Clean<CMeshO>::SelfIntersections(m.cm, IntersFace);
		tri::UpdateSelection<CMeshO>::FaceClear(m.cm);
		std::vector<CFaceO *>::iterator fpi;
		for (fpi = IntersFace.begin(); fpi != IntersFace.end(); ++fpi)
			(*fpi)->SetS();
	} break;

	case FP_SELECT_FACES_BY_EDGE:
	{
		float threshold = par.getDynamicFloat("Threshold");
		int selFaceNum = tri::UpdateSelection<CMeshO>::FaceOutOfRangeEdge(m.cm,0,threshold );
		Log( "Selected %d faces with and edge longer than %f",selFaceNum,threshold);
	} break;

	case FP_SELECT_FOLD_FACE:
	{
		float angle = math::ToRad(par.getDynamicFloat("AngleThreshold"));
		m.updateDataMask(MeshModel::MM_VERTFACETOPO);
		tri::Clean<CMeshO>::SelectFoldedFaceFromOneRingFaces(m.cm, cos(angle));
		m.clearDataMask(MeshModel::MM_VERTFACETOPO);
	} break;

	case FP_SELECT_OUTLIER:
	{
		float threshold = par.getDynamicFloat("PropThreshold");
		int kNearest = par.getInt("KNearest");
		VertexConstDataWrapper<CMeshO> wrapper(m.cm);
		KdTree<typename CMeshO::ScalarType> kdTree(wrapper);
		int selVertexNum = tri::OutlierRemoval<CMeshO>::SelectLoOPOutliers(m.cm, kdTree, kNearest, threshold);
		Log("Selected %d outlier vertices", selVertexNum);
	} break;

	default: assert(0);
}
return true;
}

MeshFilterInterface::FilterClass SelectionFilterPlugin::getClass(QAction *action)
{
  switch(ID(action))
  {
	case CP_SELFINTERSECT_SELECT:	return FilterClass(MeshFilterInterface::Selection + MeshFilterInterface::Cleaning);

	case CP_SELECT_TEXBORDER : return FilterClass(MeshFilterInterface::Selection + MeshFilterInterface::Texture);

	case FP_SELECT_BY_FACE_QUALITY :
	case FP_SELECT_BY_VERT_QUALITY : return FilterClass(MeshFilterInterface::Selection + MeshFilterInterface::Quality);

	case FP_SELECTBYANGLE : return MeshFilterInterface::FilterClass(MeshFilterInterface::RangeMap + MeshFilterInterface::Selection);

	case FP_SELECT_ALL :
	case FP_SELECT_NONE :
	case FP_SELECT_CONNECTED:
	case FP_SELECT_UGLY:
	case FP_SELECT_DELETE_VERT:
	case FP_SELECT_DELETE_ALL_FACE:
	case FP_SELECT_DELETE_FACE:
	case FP_SELECT_DELETE_FACEVERT:
	case FP_SELECT_FACE_FROM_VERT:
	case FP_SELECT_VERT_FROM_FACE:
	case FP_SELECT_ERODE:
	case FP_SELECT_DILATE:
	case FP_SELECT_BORDER:
	case FP_SELECT_INVERT:
	case FP_SELECT_FACES_BY_EDGE:
	case FP_SELECT_FOLD_FACE:
	case FP_SELECT_OUTLIER:
	case FP_SELECT_BY_COLOR: 
	case CP_SELECT_NON_MANIFOLD_VERTEX:
	case CP_SELECT_NON_MANIFOLD_FACE:  return FilterClass(MeshFilterInterface::Selection);
  }
  return MeshFilterInterface::Selection;
}

 int SelectionFilterPlugin::getRequirements(QAction *action)
{
 switch(ID(action))
  {
	case CP_SELECT_NON_MANIFOLD_FACE:
	case CP_SELECT_NON_MANIFOLD_VERTEX:       
    case FP_SELECT_CONNECTED: return MeshModel::MM_FACEFACETOPO;
  
	case CP_SELECT_TEXBORDER: return MeshModel::MM_FACEFACETOPO;
	case CP_SELFINTERSECT_SELECT: return MeshModel::MM_FACEMARK | MeshModel::MM_FACEFACETOPO;

	case FP_SELECT_FOLD_FACE: return MeshModel::MM_VERTFACETOPO;

	default: return MeshModel::MM_NONE;
  }
}

int SelectionFilterPlugin::postCondition(QAction *action) const
{
	switch(ID(action))
	{
		case FP_SELECT_ALL                  :
		case FP_SELECT_FACE_FROM_VERT       :
		case FP_SELECT_VERT_FROM_FACE       :
		case FP_SELECT_NONE                 :
		case FP_SELECT_INVERT               :
		case FP_SELECT_ERODE                :
		case FP_SELECT_CONNECTED            :
		case FP_SELECT_DILATE               :
		case FP_SELECT_BORDER               :
		case FP_SELECT_BY_VERT_QUALITY      :
		case FP_SELECT_BY_FACE_QUALITY      :
		case FP_SELECT_BY_COLOR             :
		case CP_SELFINTERSECT_SELECT        :
		case CP_SELECT_TEXBORDER            :
		case CP_SELECT_NON_MANIFOLD_FACE    :
		case CP_SELECT_NON_MANIFOLD_VERTEX  :
		case FP_SELECT_FACES_BY_EDGE        :
		case FP_SELECT_UGLY                 :
		case FP_SELECT_FOLD_FACE            :
		case FP_SELECT_OUTLIER              : return MeshModel::MM_VERTFLAGSELECT | MeshModel::MM_FACEFLAGSELECT;
		case FP_SELECT_DELETE_VERT          :
		case FP_SELECT_DELETE_ALL_FACE      :
		case FP_SELECT_DELETE_FACE          :
		case FP_SELECT_DELETE_FACEVERT      : return MeshModel::MM_GEOMETRY_AND_TOPOLOGY_CHANGE;
  }
  return MeshModel::MM_ALL;
}

int SelectionFilterPlugin::getPreConditions( QAction * action) const
{
  switch(ID(action))
  {
	case   CP_SELECT_NON_MANIFOLD_VERTEX:
	case   CP_SELECT_NON_MANIFOLD_FACE:
	case   CP_SELFINTERSECT_SELECT:
	case   FP_SELECT_FACES_BY_EDGE:
	case   FP_SELECT_FACE_FROM_VERT:
	case   FP_SELECT_BORDER:
	case   FP_SELECT_ERODE:
	case   FP_SELECT_DILATE:
	case   FP_SELECT_FOLD_FACE:
	case   FP_SELECT_CONNECTED:        return MeshModel::MM_FACENUMBER;
	case   FP_SELECT_BY_COLOR:         return MeshModel::MM_VERTCOLOR;
	case   FP_SELECT_BY_VERT_QUALITY:  return MeshModel::MM_VERTQUALITY;
	case   FP_SELECT_BY_FACE_QUALITY:  return MeshModel::MM_FACEQUALITY;
	case   CP_SELECT_TEXBORDER:        return MeshModel::MM_WEDGTEXCOORD;
  }
  return 0;
}
MESHLAB_PLUGIN_NAME_EXPORTER(SelectionFilterPlugin)
