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
			QStringList algoList;
			algoList.push_front("resampled tiles");
			algoList.push_front("copied tiles");
			parlst.addEnum( "algoList", 0, algoList, "which algorithm to use for the texture atlas", "choose the algorithm to use for the texture atlas" );//parlst.addBool use high resolution atlas (large file size) or lowest-common-denominator resolution (smaller file size)
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
	int numTextures = m.cm.textures.size();
	
	Point2i global_size;
	std::vector<Point2i> posiz;
	Point2i max_size;
	max_size[0] = par.getInt("Xmax");
	max_size[1] = par.getInt("Ymax");
	int algo = par.getEnum("algoList");
	std::vector<Point2i> sizes, splitsize;
	std::vector< std::vector<Point2i> > splitSizes;//vector of vectors, for when textures won't all fit into one atlas
	std::vector< std::vector<Point2i> > splitPosiz;//vector of vectors, for when textures won't all fit into one atlas
	std::vector<Point2i> splitglobalSizes;//vector of points for when textures won't all fit into one atlas
	Point2i size;
	
	if (numTextures < 2)
	{
		qDebug() << "filterTexture failed - # textures < 2" << endl;
		return false;//return false if numTextures == 0 or 1 (no need for an atlas)
	}
	
	//1)  generate an array of Point2f that will store the maximum difference in the u & v coordinates for each texture index
	float maxdiffUV[numTextures][2];
	//2)  iterate through all faces, and for each face do 6 subtractions to figure out the maximum difference (absolute value?) for the U & V coordinates of vertexes in that face
	CMeshO::FaceIterator fit;
	int mat;
	float diff1,diff2,diff3;
	for (fit=m.cm.face.begin(); fit != m.cm.face.end(); ++fit)//iterate through faces with textures
	{
		if (!(*fit).IsD())//only iterates over non-deleted faces
		{
			mat = fit->WT(0).N();//fit->cWT the 'c' is for const
			if (mat!=-1)//only do this for faces with textures
			{
				diff1 = fabs(fit->WT(0).U() - fit->WT(1).U());//fabs is floating-point absolute value function call
				diff2 = fabs(fit->WT(1).U() - fit->WT(2).U());
				diff3 = fabs(fit->WT(0).U() - fit->WT(2).U());
				maxdiffUV[mat][0] = diff1;
				if (diff2 > maxdiffUV[mat][0]) maxdiffUV[mat][0] = diff2;//3)  if the value computed in 2) is greater than the value stored in the array from 1), assign that value to the array
				if (diff3 > maxdiffUV[mat][0]) maxdiffUV[mat][0] = diff3;
				
				diff1 = fabs(fit->WT(0).V() - fit->WT(1).V());
				diff2 = fabs(fit->WT(1).V() - fit->WT(2).V());
				diff3 = fabs(fit->WT(0).V() - fit->WT(2).V());
				maxdiffUV[mat][1] = diff1;
				if (diff2 > maxdiffUV[mat][1]) maxdiffUV[mat][1] = diff2;
				if (diff3 > maxdiffUV[mat][1]) maxdiffUV[mat][1] = diff3;
			}
		}
	}
	//4)  when done, copy the texture the necessary number of times (as indicated in the array), with values rounded up to the nearest integer - do resampling here, if wanted
	//iterate through all values in maxdiffUV - if ceiling of value > 1, do copy
	int c;
	QPixmap images[numTextures];//array of images
	QImage tiledimages[numTextures];
	int xPos, yPos;
	for (c=0; c<numTextures; c++)//iterate through textures, loading each
	{
		qDebug() << "@ top of numtextures for loop" << endl;
		images[c] = QPixmap(m.cm.textures[c].c_str());//loads image, if fails is a null image. will guess extension from file name
		if ((maxdiffUV[c][0]>1) || (maxdiffUV[c][1]>1))//tiling texture, if uv > 1 means want it tiled/repeated
		{
			maxdiffUV[c][0]++;//incrementing by one unit to allow for the original unit to still be present - so when span is 2, will create an image of 3* the original
			maxdiffUV[c][1]++;//this will allow for a UV coordinate of .9 with a span of 2 = 2.9
			tiledimages[c] = QImage(maxdiffUV[c][0]*images[c].width(), maxdiffUV[c][1]*images[c].height(), QImage::Format_ARGB32);//doesn't need to be ceiling (ceil function) - doesn't matter if texture is not a complete copy
			QPainter painter(&tiledimages[c]);//TODO: how move initialization outside of for loop?
			//now draw into the image however many times necessary
			for (xPos = 0; xPos < maxdiffUV[c][0]*images[c].width(); xPos += images[c].width())//nested for loop in order to fill whole grid, does one column at a time
			{
				for (yPos = 0; yPos < maxdiffUV[c][1]*images[c].height(); yPos += images[c].height())
				{
					painter.drawPixmap(xPos, yPos, images[c]);//x & y position to insert top-left corner at, which image to insert
				}
			}
			painter.~QPainter();//destroys - otherwise will crash if resample tiledimage
			if (algo==1)//resampled tiles
			{
				qDebug() << "resampling" << endl;
				tiledimages[c] = tiledimages[c].scaled(images[c].width(), images[c].height());//resample image to original size
			}
			images[c] = QPixmap::fromImage(tiledimages[c]);//not necessary to write file if stays in memory
			qDebug() << "converted image to pixmap" << endl;
		}
		qDebug() << "trying 2 intiialize size" << endl;
		size = Point2i(images[c].width(), images[c].height());
		sizes.push_back(size);
		qDebug() << "filterTexture loaded image: " << m.cm.textures[c].c_str() << "with (tiled) size: " << size[0] << " " << size[1] << endl;
	}
	
	bool made = false;
	int numAtlases = 0;
	int sizesIndeces = sizes.size();
	int length;
	std::map<int, int> whichAtlas;//an stl map that maps material numbers to texture atlas numbers
	splitSizes.push_back(sizes);
	while (made==false && numAtlases<=sizesIndeces)
	{
		numAtlases++;
		qDebug() << "numAtlases" << numAtlases << endl;
		whichAtlas.clear();//for re-use
		for (c= 0; c<numAtlases; c++)
		{
			splitPosiz.push_back(sizes);//add a placeholder that will be written over
			splitglobalSizes.push_back(Point2i(0,0));//add a placeholder that will be written over
			if (c==0)
				made = rect_packer::pack(splitSizes[c], max_size, splitPosiz[c], splitglobalSizes[c]);//first time, assign result to made
			else
				made = made && rect_packer::pack(splitSizes[c], max_size, splitPosiz[c], splitglobalSizes[c]);//will return UV offsets in posiz & minimum height/width required to cover all textures in global_size.  returns false if failed to fit all the textures into the atlas.  && with itself to make sure all atlases were generated ok, not just 1.  each has own posiz & global_size for using to offset UVs later
			if (made)
			{
				for (int subcounter = 0; subcounter<splitSizes[c].size(); subcounter++)
				{
					whichAtlas[subcounter] = numAtlases;
				}
			}
			qDebug() << "rect_packer::pack returned: " << made << endl;
		}
		length = sizesIndeces/numAtlases;//length of each split, numAtlases is how many times have to split
		if (made==false)//need to split the vector of sizes into half
		{
			whichAtlas.clear();
			for (c=0; c<splitSizes.size(); c+=2)//iterate through each vector in splitSizes, increment by two so will not land on the vector you just inserted
			{
				splitsize = splitSizes[c];//first make a copy of the vector
				splitsize.erase(splitsize.begin(),splitsize.begin()+splitsize.size()/2);//from which to delete the first half of the elements.  will this delete the last element?
				splitSizes[c].resize(splitSizes[c].size()/2);//then resize the original vector by half
				splitSizes.insert(splitSizes.begin()+c+1, splitsize);//add the new vector after the original, c starts at 0 so need to add 1
			}
		}
	}
	if (made==false)
		return false;//textures just couldn't fit in given texture atlas dimensions, even one texture in its own atlas
	
	qDebug() << "returned false, continuing" << endl;
	QImage atlas;
	int index;
	QString filename;
	bool result;
	for (c=0; c<numAtlases; c++)
	{
		atlas = QImage(splitglobalSizes[c][0], splitglobalSizes[c][1], QImage::Format_ARGB32);//make a texture of that dimension
		qDebug() << "created atlas of dimensions:" << splitglobalSizes[c][0] << ", " << splitglobalSizes[c][1] << endl;
		//insert textures one after another at position returned by rect_packer, starting at 0,0 (origin in top-left corner)
		QPainter painter(&atlas);
		for (index=0; index<length; ++index)
		{
			painter.drawPixmap(splitPosiz[c][index][0], splitPosiz[c][index][1], images[c*length+index]);//use drawPixmap instead of deprecated bitBlt() to paste image to a certain position in the texture atlas
			qDebug() << "inserted texture " << c*length+index << "into atlas at position: " << splitPosiz[c][index][0] << " " << splitPosiz[c][index][1] << endl;
		}
		filename = "texture" + QString::number(numAtlases) + ".png";
		result = atlas.save(filename,"PNG",0);//save image, highest compression
		qDebug() << "saved texture atlas" << endl;
		m.cm.textures.clear();//empty mm.cm.textures vector
		m.cm.textures.push_back(filename.toStdString());//add the texture atlas (at position 0)
		//update display-load texture atlas?
	}

	float minU, minV;
	int minUwhole, minVwhole;
	float minUdec, minVdec;
	for (fit=m.cm.face.begin(); fit != m.cm.face.end(); ++fit)//iterate through faces with textures
	{
		if (!(*fit).IsD())//only iterates over non-deleted faces
		{
			mat = fit->WT(0).N();//fit->cWT the 'c' is for const
			if (mat!=-1)//only re-assign texture if face has a texture material to begin with
			{
				fit->WT(0).N() = whichAtlas[mat];//re-assign N to number of texture atlas
				//if all UV coordinates are less than the maximum span of U or V and >=0, just divide by max span of U or V and have the new normalized coordinate
				if ((((fit->WT(0).U() <= maxdiffUV[mat][0]) && (fit->WT(0).V() <= maxdiffUV[mat][1])) && ((fit->WT(1).U() <= maxdiffUV[mat][0]) && (fit->WT(1).V() <= maxdiffUV[mat][1])) && ((fit->WT(2).U() <= maxdiffUV[mat][0]) && (fit->WT(2).V() <= maxdiffUV[mat][1]))) && (((fit->WT(0).U()>0) && (fit->WT(0).V()>0)) && ((fit->WT(1).U()>0) && (fit->WT(1).V()>0)) && ((fit->WT(2).U()>0) && (fit->WT(2).V()>0))))
				{
					for (c=0; c<3; ++c)
					{
						fit->WT(c).U() /= maxdiffUV[mat][0];
						fit->WT(c).V() /= maxdiffUV[mat][1];
					}
				}
				else//otherwise, need to reposition UV coordinates close to the origin by letting smallest U,V be between 0-1 (by using its decimal) and subtracting its whole number from the other values
				{
					//find minimum U
					minU = fit->WT(0).U();
					if (fit->WT(1).U() < minU) minU = fit->WT(1).U();
					if (fit->WT(2).U() < minU) minU = fit->WT(2).U();
					//find minimum V
					minV = fit->WT(0).V();
					if (fit->WT(1).V() < minV) minV = fit->WT(1).V();
					if (fit->WT(2).V() < minV) minV = fit->WT(2).V();
					//get minimum U's whole number
					minUwhole = int(minU);
					//get minimum V's whole number
					minVwhole = int(minV);
					//get minimum U's decimal
					minUdec = minU - minUwhole;
					//get minimum V's decimal
					minVdec = minV - minVwhole;
					
					//now reposition UVs so that the smallest U,V is in the 0-1 box and others are translated accordingly
					for (c=0; c<3; ++c)
					{
						//negative UV coordinates do not mirror the texture, just continue in u-v plane
						//negative U coordinate of -.2 equivalent to + U coordinate of 1-.2 = .8
						//4 cases depending on whether or not minU/minV is positive or negative - 4 quadrants
						//can't just flip negative values along an axis - as that would reverse the direction of the triangle's texture
						if (minUwhole<0)
						{
							//correct U
							minUwhole = minU - (1 + minUdec);//subtracting min U gets you to 0, (1+ (-)minUdec) gets you to the negative correction
						}
						if (minVwhole<0)
						{
							//correct V
							minVwhole = minV - (1 + minVdec);
						}
						//do subtractions now - should get a result between 0 & 1
						fit->WT(c).U() -= minUwhole;
						fit->WT(c).V() -= minVwhole;
						if (((fit->WT(c).U() > 1)||(fit->WT(c).V() > 1))||((fit->WT(c).U() < 0)||(fit->WT(c).V() < 0)))
							qDebug() << "uv outside of 0-1 box" << endl;
					}
				}
				
				for (c= 0; c < 3;++c)//UVs are per-vertex?
				{
					qDebug() << "original U: " << fit->WT(c).U() << "original V: " << fit->WT(c).V() << endl;
					fit->WT(c).U() = (fit->WT(c).U()*images[mat].width() + splitPosiz[whichAtlas[mat]][mat % length][0])/splitglobalSizes[whichAtlas[mat]][0];//offset U by coord posiz, u*width is pixel, global_size is dimension - for normalizing (between 0 & 1 unless repeating)
					fit->WT(c).V() = (fit->WT(c).V()*images[mat].height() + splitPosiz[whichAtlas[mat]][mat % length][1])/splitglobalSizes[whichAtlas[mat]][1];//offset V by coord in posiz
					qDebug() << "new U: " << fit->WT(c).U() << "new V: " << fit->WT(c).V() << endl;
				}
			}
		}
	}
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
