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

const QString SelectionFilterPlugin::filterName(FilterIDType filter) 
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
	  case FP_SELECT_BY_QUALITY :		       return QString("Select by Quality");
	  case FP_SELECT_BY_RANGE:						 return QString("Select by Coord range");
	  case FP_SELECT_BY_COLOR:				return QString("Select Face by color");
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
void SelectionFilterPlugin::initParameterSet(QAction *action, MeshModel &m, FilterParameterSet &parlst)
{
		switch(ID(action))
		{
			case FP_SELECT_BY_QUALITY: 
					{
						std::pair<float,float> minmax =  tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.cm); 
						float minq=minmax.first;
						float maxq=minmax.second;
						parlst.addAbsPerc("minQ",minq*.75+maxq*.25,minq,maxq,"Min Quality", "Minimum acceptable quality value");
						parlst.addAbsPerc("maxQ",minq*.25+maxq*.75,minq,maxq,"Max Quality", "Maximum acceptable quality value");
					}
					break;
			case FP_SELECT_BY_COLOR:
			{
				parlst.addColor("Color",Color4b::Black, tr("Color To Select"), tr("Color that you want to be selected.") );
				
				QStringList colorspace;
				colorspace << "HSV" << "RGB";
				parlst.addEnum("ColorSpace", 0, colorspace, tr("Pick Color Space"), tr("The color space that the sliders will manipulate.") );

				QStringList mode;
				mode << "Start Over" << "Add" << "Subtract";
				parlst.addEnum("Mode", 0, mode, tr("Mode:"), tr("The mode of this filter.  Start Over clears the selection completely before selecting based on the color.  Add just adds to the current selection bases on color and subtract takes away from the selection the triangles with a vertex matching the color.") );

				
				parlst.addDynamicFloat("PercentRH", 0.2f, 0.0f, 1.0f, MeshModel::MM_FACESELECTION, tr("Variation from Red or Hue"), tr("A float between 0 and 1 that represents the percent variation from this color that will be selected.  For example if the R was 200 and you put 0.1 then any color with R 200+-25.5 will be selected.") );
				parlst.addDynamicFloat("PercentGS", 0.2f, 0.0f, 1.0f, MeshModel::MM_FACESELECTION, tr("Variation from Green or Saturation"), tr("A float between 0 and 1 that represents the percent variation from this color that will be selected.  For example if the R was 200 and you put 0.1 then any color with R 200+-25.5 will be selected.") );
				parlst.addDynamicFloat("PercentBV", 0.2f, 0.0f, 1.0f, MeshModel::MM_FACESELECTION, tr("Variation from Blue or Value"), tr("A float between 0 and 1 that represents the percent variation from this color that will be selected.  For example if the R was 200 and you put 0.1 then any color with R 200+-25.5 will be selected.") );
			}
			break;
		}
}
// Return true if the specified action has an automatic dialog.
// return false if the action has no parameters or has an self generated dialog.
bool SelectionFilterPlugin::autoDialog(QAction *action)
{
	 switch(ID(action))
	 {
		 case  FP_SELECT_BY_COLOR:
		 case FP_SELECT_BY_QUALITY:
			 return true;
	 }
  return false;
}


bool SelectionFilterPlugin::applyFilter(QAction *action, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos * cb) 
{
	CMeshO::FaceIterator fi;
	CMeshO::VertexIterator vi;
	switch(ID(action))
  {
  case FP_SELECT_DELETE_FACE : 
		for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
      if(!(*fi).IsD() && (*fi).IsS() ) tri::Allocator<CMeshO>::DeleteFace(m.cm,*fi);
			m.clearDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);
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
			m.clearDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);
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
			float minQ = par.getAbsPerc("minQ");	
			float maxQ = par.getAbsPerc("maxQ");	
			tri::UpdateSelection<CMeshO>::VertexFromQualityRange(m.cm, minQ, maxQ);  
			tri::UpdateSelection<CMeshO>::FaceFromVertexStrict(m.cm);
		}
	break;
  case FP_SELECT_BY_COLOR:
		{
			int mode = par.getEnum("Mode");
			int colorSpace = par.getEnum("ColorSpace");
			QColor targetColor = par.getColor("Color");
			
			float red = targetColor.redF();
			float green = targetColor.greenF();
			float blue = targetColor.blueF();
			
			float hue = (targetColor.hue()+1)/360.0f;// because in QT hueF() returns a value -1 to 0.7 
			float saturation = targetColor.saturationF();
			float value = targetColor.valueF();
			
			//qDebug() << "r " << red << " g " << green << " b " << blue;
			//qDebug() << "h " << hue << " s " << saturation << " v " << value;
			
			//like fuzz factor in photoshop
			float valueRH = par.getDynamicFloat("PercentRH");
			float valueGS = par.getDynamicFloat("PercentGS");
			float valueBV = par.getDynamicFloat("PercentBV");
			
			float lowRH = (colorSpace ? red - valueRH : hue - valueRH );
			float lowGS = (colorSpace ? green - valueGS : saturation - valueGS );
			float lowBV = (colorSpace ? blue - valueBV : value - valueBV );
			float highRH = (colorSpace ? red + valueRH : hue + valueRH );
			float highGS = (colorSpace ? green + valueGS : saturation + valueGS );
			float highBV = (colorSpace ? blue + valueBV : value + valueBV );
			
			//qDebug() << lowRH << " " << lowGS << " " << lowBV;
			//qDebug() << highRH << " " << highGS << " " << highBV;
						
			//now modify the selection
			if(mode == 0)
			{
				//clear any existing selection
				tri::UpdateSelection<CMeshO>::ClearFace(m.cm);
			}
					
			//now loop through all the faces
			CMeshO::FaceIterator fi;
			for(fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
			{
				if(!(*fi).IsD())
				{
					Color4f colorv0 = Color4f::Construct((*fi).V(0)->C());
					Color4f colorv1 = Color4f::Construct((*fi).V(1)->C());
					Color4f colorv2 = Color4f::Construct((*fi).V(2)->C());
					
					colorv0 = ColorSpace<float>::RGBtoHSV(colorv0);
					colorv1 = ColorSpace<float>::RGBtoHSV(colorv1);
					colorv2 = ColorSpace<float>::RGBtoHSV(colorv2);
						
					//if(colorv0[0] < 0) qDebug() << "value was less than 0";
					//qDebug() << colorv0[0] << " " << colorv0[1] << " " << colorv0[2];
					//qDebug() << colorv1[0] << " " << colorv1[1] << " " << colorv1[2];
					//qDebug() << colorv2[0] << " " << colorv2[1] << " " << colorv2[2];
					
					if( (colorv0[0] > lowRH && colorv0[0] < highRH &&
						colorv0[1] > lowGS && colorv0[1] < highGS &&
						colorv0[2] > lowBV && colorv0[2] < highBV) ||
						(colorv1[0] > lowRH && colorv1[0] < highRH &&
						colorv1[1] > lowGS && colorv1[1] < highGS &&
						colorv1[2] > lowBV && colorv1[2] < highBV) ||
						(colorv2[0] > lowRH && colorv2[0] < highRH &&
						colorv2[1] > lowGS && colorv2[1] < highGS &&
						colorv2[2] > lowBV && colorv2[2] < highBV)
							)
					{
						//add
						if(mode <= 1)
							(*fi).SetS();
						else if(mode == 2)
							(*fi).ClearS();
					}
					
				}
			}
		}
		break;

  default:  assert(0);
  }
  return true;
}

 const QString SelectionFilterPlugin::filterInfo(FilterIDType filterId) 
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

const int SelectionFilterPlugin::getRequirements(QAction *action)
{
 switch(ID(action))
  {
   case FP_SELECT_BORDER_FACES:   return  MeshModel::MM_BORDERFLAG;
   case FP_SELECT_BY_COLOR:		return MeshModel::MM_VERTCOLOR;
  }
}

Q_EXPORT_PLUGIN(SelectionFilterPlugin)
