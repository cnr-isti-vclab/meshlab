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
$Log: samplefilter.cpp,v $
****************************************************************************/

#include <QtGui>
#include <QImage>
#include <QColor>

#include <math.h>
#include <stdlib.h>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include "filter_texture.h"
#include "rect_packer.h"

using namespace vcg;
using namespace std;
// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterTexturePlugin::FilterTexturePlugin() 
{ 
	typeList << FP_MAKE_ATLAS;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
const QString FilterTexturePlugin::filterName(FilterIDType filterId) 
{
  switch(filterId) {
		case FP_MAKE_ATLAS :  return QString("Build texture atlas"); 
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString FilterTexturePlugin::filterInfo(FilterIDType filterId)
{
  switch(filterId) {
		case FP_MAKE_ATLAS :  return QString("Compact all the texture of multitextured mesh into a single atlas"); 
		default : assert(0); 
	}
}

const PluginInfo &FilterTexturePlugin::pluginInfo()
{
   static PluginInfo ai;
   ai.Date=tr(__DATE__);
	 ai.Version = tr("1.0");
	 ai.Author = ("Imran Akbar");
   return ai;
 }

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterTexturePlugin::initParameterSet(QAction *action,MeshModel &m, FilterParameterSet &parlst) 
//void ExtraSamplePlugin::initParList(QAction *action, MeshModel &m, FilterParameterSet &parlst)
{
	 switch(ID(action))	 {
		default : 
			parlst.addInt  ("Xmax",2880 ,"Maximum width of texture atlas");//add to parList
			parlst.addInt  ("Ymax", 2880,"Maximum height of texture atlas");//add to parList
	}
}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool FilterTexturePlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet &par, vcg::CallBackPos * cb)
{
	//this function takes every texture in the model and creates a single texture composed of all the individual textures - necessary before calling the Quadric-with-texture filter
	//QT has support for all the major image formats (GIF/JPEG/PNG/BMP/TIFF) - though you need to compile QT with -qt-gif to get GIF support (on Ubuntu the package for qt already has this - check your /usr/share/qt4/plugins/imageformats folder)
	//can't do any file-format-specific calls, this has to be generalizable to any model
	//the general 'packing problem' is NP-complete, so you have to use a heuristic or genetic algorithm to get an approximate answer
	//this is not an optimal packing - we are not rotating textures, for example
	//TODO:how read image data from images already loaded into Meshlab, to prevent from opening files again?
	//TODO:improve atlas-generation algorithm so it tries to minimize whitespace (though transparent-background PNG files don't increase in file size due to whitespace)
	qDebug() << "called filterTexture applyFilter" << endl;
	int totalWidth=0, totalHeight=0;
	int numTextures = m.cm.textures.size();
	
	Point2i global_size;
	std::vector<Point2i> posiz;
	Point2i max_size;
	max_size[0] = par.getInt("Xmax");
	max_size[1] = par.getInt("Ymax");
	std::vector<Point2i> sizes;
	Point2i size;
	
	if ((numTextures == 0)||(numTextures == 1))
	{
		qDebug() << "filterTexture failed - # textures < 2" << endl;
		return false;//return false if numTextures == 0 or 1 (no need for an atlas)
	}
	QPixmap images[numTextures];//array of images
	for (unsigned textureIdx = 0; textureIdx < numTextures; ++textureIdx)//iterate through textures, loading each
	{
		images[textureIdx] = QPixmap(m.cm.textures[textureIdx].c_str());//loads image, if fails is a null image. will guess extension from file name
		size = Point2i(images[textureIdx].width(), images[textureIdx].height());
		sizes.push_back(size);
		qDebug() << "filterTexture loaded image: " << m.cm.textures[textureIdx].c_str() << endl;
	}
	
	rect_packer::pack(sizes, max_size, posiz, global_size);//will return UV offsets in posiz & minimum height/width required to cover all textures in global_size
	qDebug() << "global_size, x:" << global_size[0] << "global_size, y:" << global_size[1] << endl;
	
	QImage atlas = QImage(global_size[0], global_size[1], QImage::Format_ARGB32);//make a square texture of that dimension

	//insert textures one after another in one row, starting at 0,0
	QPainter painter(&atlas);
	for (int index=0; index<numTextures; ++index)
	{
		painter.drawPixmap(posiz[index][0], posiz[index][1], images[index]);//use drawPixmap instead of deprecated bitBlt() to paste image to a certain position in the texture atlas
		qDebug() << "inserted texture into atlas at position: " << posiz[index][0] << " " << posiz[index][1] << endl;
	}
	
	int tmpX=0,tmpY=0;
	CMeshO::FaceIterator fit;
	for (fit=m.cm.face.begin(); fit != m.cm.face.end(); ++fit)//iterate through faces with textures
	{
		if (!(*fit).IsD())//only iterates over non-deleted faces
		{
			int mat = fit->WT(0).N();//fit->cWT the 'c' is for const
			if (mat!=-1)
				fit->WT(0).N() = 0;//re-assign N to 0 (only one texture now)
			for(unsigned int ii = 0; ii < 3;++ii)//UVs are per-vertex?
			{
				qDebug() << "original U: " << fit->WT(ii).U() << "original V: " << fit->WT(ii).V() << endl;
				//if uv > 1 means want it tiled/repeated
				//is the 0,0 origin at top-left, bottom-left, or center?  seen all 3 - how else get negative UV coord?
				//adjust texture coordinates by half a pixel to avoid filtering artifacts?
				fit->WT(ii).U() = (fit->WT(ii).U()*images[mat].width() + posiz[mat][0])/global_size[0];//offset U by coord posiz, u*width is pixel, global_size is dimension - for normalizing (between 0 & 1 unless repeating)
				fit->WT(ii).V() = (fit->WT(ii).V()*images[mat].height() + posiz[mat][1])/global_size[1];//offset V by coord in posiz
				qDebug() << "new U: " << fit->WT(ii).U() << "new V: " << fit->WT(ii).V() << endl;
			}
		}
	}
	
	bool result = atlas.save("texture.png","PNG",0);//save image, highest compression
	qDebug() << "saved texture atlas" << endl;
	m.cm.textures.clear();//empty mm.cm.textures vector
	m.cm.textures.push_back("texture.png");//add the texture atlas (at position 0)
	//update display-load texture atlas?
	return result;
}

const int FilterTexturePlugin::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case FP_MAKE_ATLAS :	return MeshModel::MM_BORDERFLAG; // <-- ADD YOUR REQUIREMENT HERE 
    default: assert(0);
  }
  return 0;
}

const FilterTexturePlugin::FilterClass FilterTexturePlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_MAKE_ATLAS :
      return MeshFilterInterface::Generic;     
    default : return MeshFilterInterface::Generic;
  }
}


Q_EXPORT_PLUGIN(FilterTexturePlugin)
