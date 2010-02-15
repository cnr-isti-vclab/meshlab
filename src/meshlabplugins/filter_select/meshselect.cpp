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
#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include "meshselect.h"
#include <vcg/complex/trimesh/update/selection.h>
#include <vcg/complex/trimesh/stat.h>
#include <vcg/space/colorspace.h>

using namespace vcg;

 QString SelectionFilterPlugin::filterName(FilterIDType filter) const
{
 switch(filter)
  {
	  case FP_SELECT_ALL :		             return QString("Select All");
	  case FP_SELECT_NONE :		             return QString("Select None");
	  case FP_SELECT_INVERT :		           return QString("Invert Selection");
	  case FP_SELECT_DELETE_FACE :		     return QString("Delete Selected Faces");
	  case FP_SELECT_DELETE_FACEVERT :		 return QString("Delete Selected Faces and Vertices");
	  case FP_SELECT_ERODE :		           return QString("Erode Selection");
	  case FP_SELECT_DILATE :		           return QString("Dilate Selection");
	  case FP_SELECT_BORDER_FACES:		     return QString("Select Border Faces");
	  case FP_SELECT_BY_QUALITY :		       return QString("Select by Vertex Quality");
	  case FP_SELECT_BY_RANGE:						 return QString("Select by Coord range");
	  case FP_SELECT_BY_COLOR:						 return QString("Select Face by Vertex Color");
  }
  return QString("Unknown filter");
}

SelectionFilterPlugin::SelectionFilterPlugin() 
{
  typeList << 
    FP_SELECT_ALL <<
    FP_SELECT_NONE <<
    FP_SELECT_DELETE_FACE <<
    FP_SELECT_DELETE_FACEVERT <<
    FP_SELECT_ERODE <<
    FP_SELECT_DILATE <<
    FP_SELECT_BORDER_FACES <<
		FP_SELECT_INVERT <<
		FP_SELECT_BY_QUALITY <<
		FP_SELECT_BY_COLOR;
  
  FilterIDType tt;
  
  foreach(tt , types())
    {
      actionList << new QAction(filterName(tt), this);
      if(tt==FP_SELECT_DELETE_FACE){
            actionList.last()->setShortcut(QKeySequence (Qt::Key_Delete));
            actionList.last()->setIcon(QIcon(":/images/delete_face.png"));
      }
      if(tt==FP_SELECT_DELETE_FACEVERT){
            actionList.last()->setShortcut(QKeySequence ("Shift+Del"));
            actionList.last()->setIcon(QIcon(":/images/delete_facevert.png"));
      }
    }	  
}
SelectionFilterPlugin::~SelectionFilterPlugin() 
{
	for (int i = 0; i < actionList.count() ; i++ ) {
		delete actionList.at(i);
	}
}
void SelectionFilterPlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst)
{
		switch(ID(action))
		{
			case FP_SELECT_BY_QUALITY: 
					{
						std::pair<float,float> minmax =  tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.cm); 
						float minq=minmax.first;
						float maxq=minmax.second;
						
						parlst.addParam(new RichDynamicFloat("minQ", minq*0.75+maxq*.25, minq, maxq,  tr("Min Quality"), tr("Minimum acceptable quality value") ));
						parlst.addParam(new RichDynamicFloat("maxQ", minq*0.25+maxq*.75, minq, maxq,  tr("Max Quality"), tr("Maximum acceptable quality value") ));
					}
					break;
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
			}
			break;
		}
}

bool SelectionFilterPlugin::applyFilter(QAction *action, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos * /*cb*/)
{
    MeshModel &m=*(md.mm());
    CMeshO::FaceIterator fi;
	CMeshO::VertexIterator vi;
	switch(ID(action))
  {
  case FP_SELECT_DELETE_FACE : 
        for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
      if(!(*fi).IsD() && (*fi).IsS() ) tri::Allocator<CMeshO>::DeleteFace(m.cm,*fi);
			m.clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
    break;
  case FP_SELECT_DELETE_FACEVERT : 
		tri::UpdateSelection<CMeshO>::ClearVertex(m.cm);
		tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);  
    for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
      if(!(*fi).IsD() && (*fi).IsS() )
                    tri::Allocator<CMeshO>::DeleteFace(m.cm,*fi);
        for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
            if(!(*vi).IsD() && (*vi).IsS() )
                    tri::Allocator<CMeshO>::DeleteVertex(m.cm,*vi);
			m.clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
    break;
  case FP_SELECT_ALL    : tri::UpdateSelection<CMeshO>::AllFace(m.cm);     break;
  case FP_SELECT_NONE   : tri::UpdateSelection<CMeshO>::ClearFace(m.cm);   break;
  case FP_SELECT_INVERT : tri::UpdateSelection<CMeshO>::InvertFace(m.cm);  break;
  case FP_SELECT_ERODE  : tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);  
                          tri::UpdateSelection<CMeshO>::FaceFromVertexStrict(m.cm); 
  break;
  case FP_SELECT_DILATE : tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(m.cm);  
                          tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m.cm); 
  break;
  case FP_SELECT_BORDER_FACES: tri::UpdateSelection<CMeshO>::FaceFromBorder(m.cm);  
  break;
  case FP_SELECT_BY_QUALITY: 
		{
			float minQ = par.getDynamicFloat("minQ");	
			float maxQ = par.getDynamicFloat("maxQ");	
			bool inclusiveFlag = par.getBool("Inclusive");
			tri::UpdateSelection<CMeshO>::VertexFromQualityRange(m.cm, minQ, maxQ);  
			if(inclusiveFlag) tri::UpdateSelection<CMeshO>::FaceFromVertexStrict(m.cm);
			else tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m.cm);
		}
	break;
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

            tri::UpdateSelection<CMeshO>::ClearFace(m.cm);
            tri::UpdateSelection<CMeshO>::ClearVertex(m.cm);

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
            if(par.getBool("Inclusive")) tri::UpdateSelection<CMeshO>::FaceFromVertexStrict(m.cm);
                                     else tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(m.cm);
		}
		break;

  default:  assert(0);
  }
  return true;
}

QString SelectionFilterPlugin::filterInfo(FilterIDType filterId) const 
 {
  switch(filterId)
  {
    case FP_SELECT_DILATE : return tr("Dilate (expand) the current set of selected faces");  
    case FP_SELECT_DELETE_FACE : return tr("Delete the current set of selected faces, vertices that remains unreferenced are not deleted.");  
    case FP_SELECT_DELETE_FACEVERT : return tr("Delete the current set of selected faces and all the vertices surrounded by that faces.");  
    case FP_SELECT_ERODE  : return tr("Erode (reduce) the current set of selected faces");  
    case FP_SELECT_INVERT : return tr("Invert the current set of selected faces");  
    case FP_SELECT_NONE   : return tr("Clear the current set of selected faces");  
    case FP_SELECT_ALL    : return tr("Select all the faces of the current mesh");  
    case FP_SELECT_BORDER_FACES    : return tr("Select all the faces on the boundary");  
    case FP_SELECT_BY_QUALITY    : return tr("Select all the faces with all the vertexes within the specified quality range");
    case FP_SELECT_BY_COLOR:  return tr("Select part of the mesh based on its color.");
  }  
  assert(0);
  return QString();
 }

 int SelectionFilterPlugin::getRequirements(QAction *action)
{
 switch(ID(action))
  {
   case FP_SELECT_BORDER_FACES:   return  MeshModel::MM_FACEFLAGBORDER;
   case FP_SELECT_BY_COLOR:		return MeshModel::MM_VERTCOLOR;
	 default: return 0;
  }
}

Q_EXPORT_PLUGIN(SelectionFilterPlugin)
