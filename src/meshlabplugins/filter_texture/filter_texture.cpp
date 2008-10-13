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
#include <vcg/complex/trimesh/clean.h>

#include "filter_texture.h"

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

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterTexturePlugin::initParameterSet(QAction *action,MeshModel &m, FilterParameterSet &parlst)
{
	 switch(ID(action))	 {
		default : 
			parlst.addInt  ("Xmax",4096,"Maximum width of texture atlas");// values are large enough so realistically should never have to resize to get the textures to fit in.  empty space in the encoded texture atlas image will not affect PNG file size, and can be cropped out later if desired
			parlst.addInt  ("Ymax",4096,"Maximum height of texture atlas");//add to parList
			QStringList algoList;
			algoList.push_front("resampled tiles");
			algoList.push_front("copied tiles");
			parlst.addEnum( "algoList", 0, algoList, "which algorithm to use for the texture atlas", "choose the algorithm to use for the texture atlas" );//use high resolution atlas (large file size that copies repeated tiles) or lowest-common-denominator resolution (smaller file size that resamples repeated tiles into the original space for its dimensions)
	}
}
//this function determines the maximum u & v coordinates for each texture, by iterating through every face in the model.  this is needed because it is common for models to have UV coordinates outside of the normalized range 0-1 in order to repeat the texture.  this becomes a problem when we try to stitch all the textures together into one texture atlas (the solution is to copy the texture however many times is necessary to encompass the maximum UV coords, or to clip triangles - we currently only support tiling.  to clip triangles you need to look at the refine.h, edgelen class).  if a model of a wall has a texture tiled 30x along the U and 40x along the V, there is no need to tile the texture 30 and 40 times because each triangle will only span over a few copies of the texture, so we tile based on the maximum span of each polygon
void FilterTexturePlugin::maxFaceSpan(int &c, float maxdiffUV[][2], MeshModel &m, int &mat, CMeshO::FaceIterator &fit)//c++ passes arrays by reference, don't need &
{
	int whole;
	float dec, diff1,diff2,diff3;//'dec' stores decimal components
	for (fit=m.cm.face.begin(); fit != m.cm.face.end(); ++fit)//iterate through faces with textures
	{	
		mat = fit->WT(0).N();//fit->cWT the 'c' is for const
		if (mat!=-1)//only do this for faces with textures
		{//you have 3 vertices in every triangle, so we determine the difference between the uv coordinates of all 3 combinations to find the maximum	
			//first thing to do is to transform negative UV coords into positive ones, to make life simpler
			//way to do this is to negate the whole number component, and to do (1-decimal component)
			//need to do this BEFORE calculate maximum spans (maxdiffUV) otherwise will have incorrect span: ie -1.4 to 0 different distance than 1.6 to 0
			/*for (c= 0; c < 3;++c)//UVs are per-vertex
			{
				if (fit->WT(c).U()<0)//is negative
				{
					whole = int(fit->WT(c).U());//cast to integer.  -0.9 -> 0, or -1.4 -> -1
					//qDebug() << "correcting negative:" << fit->WT(c).U() << "to";
					dec = fit->WT(c).U() - whole;//-.9, or -.4
					fit->WT(c).U() = abs(whole)+(1+dec);//0 + (1+-.9) = 0.1, or 1 + (1+-.4) = 1.6
					//qDebug() << fit->WT(c).U() << endl;
				}
				if (fit->WT(c).V()<0)//is negative
				{
					whole = int(fit->WT(c).V());//cast to integer.  -0.9 -> 0, or -1.4 -> -1
					//qDebug() << "correcting negative:" << fit->WT(c).U() << "to";
					dec = fit->WT(c).V() - whole;//-.9, or -.4
					fit->WT(c).V() = abs(whole)+(1+dec);//0 + (1+-.9) = 0.1, or 1 + (1+-.4) = 1.6
					//qDebug() << fit->WT(c).U() << endl;
				}
			}*/
			
			//need fabs() to ensure result of subtraction is positive
			diff1 = fabs(fit->WT(0).U() - fit->WT(1).U());//fabs is floating-point absolute value function call
			diff2 = fabs(fit->WT(1).U() - fit->WT(2).U());
			diff3 = fabs(fit->WT(0).U() - fit->WT(2).U());
			//find the max of the first two, then the max of that result and diff3, then the max of that with the previous highest diffUV value
			//(so end up storing the highest span across all faces that use that material)
			maxdiffUV[mat][0] = max(maxdiffUV[mat][0],max(max(diff1, diff2),diff3));
			//now do the same for the v-coordinate
			diff1 = fabs(fit->WT(0).V() - fit->WT(1).V());
			diff2 = fabs(fit->WT(1).V() - fit->WT(2).V());
			diff3 = fabs(fit->WT(0).V() - fit->WT(2).V());
			maxdiffUV[mat][1] = max(maxdiffUV[mat][1],max(max(diff1, diff2),diff3));
		}
	}
}

//this function does not create the texture atlas, rather it just copies each image to itself for however many repetitions is necessary based on its uv coordinates
void FilterTexturePlugin::copyTiles(QPixmap images[], QImage tiledimages[], int &numTextures, int &c, float maxdiffUV[][2], MeshModel &m, int &algo, std::vector<Point2i> &sizes, Point2i &size)
{
	int xPos, yPos;
	QPixmap tempImage;//for use in cropping to fill area at top of tiledimage
	for (c=0; c<numTextures; c++)//iterate through textures, loading each
	{
		images[c] = QPixmap(m.cm.textures[c].c_str());//loads image, if fails is a null image. will guess extension from file name
		if (!images[c].isNull())
		{
			qDebug() << "loaded image" << m.cm.textures[c].c_str() << endl;
			//we do this for all textures so that even if span is not >1 (in need of tiling) will get span of 2 to allow for uvs from 0.9 to 1.1, etc.
			maxdiffUV[c][0]++;//incrementing by one unit to allow for the original unit to still be present - so when span is 2, will create an image of 3* the original
			maxdiffUV[c][1]++;//this will allow for a UV coordinate of .9 with a span of 2 = 2.9
			if ((maxdiffUV[c][0]>1) || (maxdiffUV[c][1]>1))//tiling texture, if uv > 1 means want it is tiled/repeated (even though added one by incrementing)
			{
				tiledimages[c] = QImage(maxdiffUV[c][0]*images[c].width(), maxdiffUV[c][1]*images[c].height(), QImage::Format_ARGB32);//doesn't need to be ceiling (ceil function) - doesn't matter if texture is not a complete copy
				QPainter painter(&tiledimages[c]);//TODO: how move initialization outside of for loop, so can re-use the painter?
				//now draw into the image however many times necessary
				//meshlab considers the (0,0) texture coordinate to be at the bottom-left, not the top-left, so that's where we start drawing
				for (xPos = 0; xPos < maxdiffUV[c][0]*images[c].width(); xPos += images[c].width())//nested for loop in order to fill whole grid, does one column at a time
				{
					for (yPos = (maxdiffUV[c][1]-1)*images[c].height(); yPos > 0; yPos -= images[c].height())
					{
						qDebug() << "drawing mat:" << c << "into position:" << xPos << "," << yPos << endl;
						painter.drawPixmap(xPos, yPos, images[c]);//x & y position to insert top-left corner at, which image to insert
					}
					if (yPos<0)//this will be executed after the drawer is done painting all the textures from the bottom, until there's possibly a bit of space left at the top
					{
						yPos += images[c].height();//now check if there is any space left at the top of the tiled image, yPos is this amount
						if (yPos>0)//if so, crop the texture (from the top) and insert it in that space
						{
							tempImage = images[c].copy(0,(images[c].height()-yPos),images[c].width(),yPos);//x,y,width,height
							painter.drawPixmap(xPos, 0, tempImage);//x,y,image
						}
					}
				}
				if (algo==1)//resampled tiles
				{
					qDebug() << "resampling" << endl;
					images[c] = QPixmap::fromImage(tiledimages[c].scaled(images[c].width(), images[c].height()));//resample image to original size
				}
				else
					images[c] = QPixmap::fromImage(tiledimages[c]);//not necessary to write file if stays in memory, so just re-assign the QPixmap to the new tiled image
			}
		}
		if (images[c].width()!=0 && images[c].height()!=0)
		{
			size = Point2i(images[c].width(), images[c].height());//regardless of whether or not it was tiled, now store the image's dimensions
			sizes.push_back(size);
			qDebug() << "copyTiles loaded image into sizes vector: " << m.cm.textures[c].c_str() << "with (potentially tiled) size: " << size[0] << " " << size[1] << endl;		
		}
	}
}

bool FilterTexturePlugin::createAtlas(int &numTextures, int &c, MeshModel &m, QPixmap images[], std::vector<Point2i> &posiz, Point2i &global_size)
{
	QImage atlas;
	int index;
	QString filename;
	
	atlas = QImage(global_size[0], global_size[1], QImage::Format_ARGB32);//make a texture of that dimension
	//insert textures one after another at position returned by rect_packer, starting at 0,0 (origin in top-left corner)
	QPainter painter(&atlas);
	for (index=0; index<numTextures; ++index)
	{
		painter.drawPixmap(posiz[index][0], posiz[index][1], images[index]);//use drawPixmap instead of deprecated bitBlt() to paste image to a certain position in the texture atlas
		qDebug() << "inserted texture " << index << "into atlas at position: " << posiz[index][0] << " " << posiz[index][1] << endl;
	}
	filename = "textureatlas.png";
	m.cm.textures.clear();//empty mm.cm.textures vector
	m.cm.textures.push_back(filename.toStdString());//add the texture atlas (at position 0)
	//updateTexture();//TODO: slot in mainwindow?  update display-load texture atlas?
	return atlas.save(filename,"PNG",0);//save image, highest compression
}

void FilterTexturePlugin::adjustUVCoords(int &mat, int &c, CMeshO::FaceIterator &fit, std::vector<Point2i> &posiz, Point2i &global_size, MeshModel &m, float maxdiffUV[][2], QPixmap images[])
{
	float minU, minV;
	int minUwhole, minVwhole;//stores whole-number components
	//bool UwasNegative, VwasNegative;
	for (fit=m.cm.face.begin(); fit != m.cm.face.end(); ++fit)//iterate through faces with textures
	{
		mat = fit->WT(0).N();//fit->cWT the 'c' is for const
		if (mat!=-1)//only re-assign texture if face has a texture material to begin with
		{
			fit->WT(0).N() = 0;//re-assign N to texture atlas's index (0, as only one texture now)
			//if all UV coordinates are less than the maximum span of U or V and >=0, just divide by max span of U or V and have the new normalized coordinate
			//ie if maximum span is 2, and have u coordinates .5 and 1, can just divide
			//but if have maximum span 2, and have u coordinates 3.7 and 3.9 (which is possible because their span is still 0.2) need to correct for, same case if negative
			//tiled texture coordinates (3.7 and 3.9): start position determined by .7, how many times you repeat determined by difference?
			//negative UV coordinates do not mirror the texture, just continue in u-v plane
			//negative U coordinate of -.2 equivalent to + U coordinate of 1-.2 = .8
			//4 cases depending on whether or not minU/minV is positive or negative - 4 quadrants
			//can't just flip negative values along an axis - as that would reverse the direction of the triangle's texture
			//cannot clamp values otherwise lose its ability to wrap i.e. repeat
			
			//3 ways of rendering values outside 0-1: clamp, wrap, or mirror
			//CLAMP - any value outside 0-1 gets clamped to 0 or 1
			//MIRROR - if whole number is odd, is frac(U) and whole number is even, is 1-frac(U), so 1.1 -> 0.9
			//WRAP - repeat as frac(U), so 1.1 -> 0.1
			//this assumes we're doing WRAP-around
			//texture gets 'flipped' whenever the UVs cross-product is negative
			
			//fabs to allow for negative uv coords
			if ((((fabs(fit->WT(0).U()) <= maxdiffUV[mat][0]) && (fabs(fit->WT(0).V()) <= maxdiffUV[mat][1])) && ((fabs(fit->WT(1).U()) <= maxdiffUV[mat][0]) && (fabs(fit->WT(1).V()) <= maxdiffUV[mat][1])) && ((fabs(fit->WT(2).U()) <= maxdiffUV[mat][0]) && (fabs(fit->WT(2).V()) <= maxdiffUV[mat][1])))){}
			else//otherwise, need to reposition UV coordinates close to the 0,0 by letting smallest U,V be between 0-1 (by subtracting its whole number component, leaving its decimal) and subtracting its whole number component from the other values (maintaining the offsets between them).  this corrects for UV coords outside the range of the span.  the resulting coordinates (before the last for loop) should be in the span, but not necessarily 0-1
			{
				qDebug() << "doing adjustment for > maxdiffUV" << endl;
				
				//find minimum U
				minU = min(fit->WT(0).U(), fit->WT(1).U());
				minU = min(fit->WT(2).U(), minU);
				//find minimum V
				minV = min(fit->WT(0).V(), fit->WT(1).V());
				minV = min(fit->WT(2).V(), minV);
				//if two points are both minimum values, valid to pick either one
				//float origU, origV;

				//now reposition UVs so that the smallest U & V coordinates are in the 0-1 box and others are translated accordingly
				//because you're minimizing the U & Vs separately, you won't necessarily end up with one vertex in the 0-1 box

				//4 cases: need to maintain the sign of each coordinate, and the spans between them (by translation)
				//all 3 coordinates are positive: below
				//2 are positive, 1 negative: offset negative coordinate to between 0 & -1
				//1 is positive, 2 negative: offset the LEAST negative coordinate to between 0 & -1
				//0 is positive, 3 negative: " "
				//the last 3 (those with at least one negative coord) are in effect the same algorithm, as when there is only one negative coord the least negative coord is that one
				
				//do for U, then repeat for V
				
				//first case
				if (fit->WT(0).U()>0 && fit->WT(1).U()>0 && fit->WT(2).U()>0){}
				else//other 3 cases (with at least one negative coord)
				{
					//find the least negative value - you already have the minimum, so just compare all values until find a negative one that's greater than it
					for (c=0; c<3; ++c)
					{
						if (fit->WT(c).U()<0 && fit->WT(c).U()>minU)
							minU=fit->WT(c).U();
					}
					qDebug() << "correcting negative, new minU:" << minU << endl;
				}
				minUwhole = int(minU);//get minimum U's whole number (equivalent to the floor)
				for (c=0; c<3; ++c)
					fit->WT(c).U() -= minUwhole;//offset by this value
				
				
				//first case
				if (fit->WT(0).V()>0 && fit->WT(1).V()>0 && fit->WT(2).V()>0){}
				else//other 3 cases (with at least one negative coord)
				{
					//find the least negative value - you already have the minimum, so just compare all values until find a negative one that's greater than it
					for (c=0; c<3; ++c)
					{
						if (fit->WT(c).V()<0 && fit->WT(c).V()>minV)
							minV=fit->WT(c).V();
					}
					qDebug() << "correcting negative, new minV:" << minV << endl;
				}
				minVwhole = int(minV);//get minimum V's whole number
				for (c=0; c<3; ++c)
					fit->WT(c).V() -= minVwhole;//offset by this value
				
				/*for (c=0; c<3; ++c)
				{
					//origU=fit->WT(c).U();
					//origV=fit->WT(c).V();
					fit->WT(c).U() -= minUwhole;
					fit->WT(c).V() -= minVwhole;
					/*if ((fit->WT(c).U()>maxdiffUV[mat][0])||(fit->WT(c).V()>maxdiffUV[mat][1]))
					{
						qDebug() << "-------------------" << endl;
						qDebug() << "maxdiffuv:" << maxdiffUV[mat][0] << "," << maxdiffUV[mat][1] << endl;
						qDebug() << "original u,v:" << origU << "," << origV << endl;
						qDebug() << "minu,minuwhole,minv,minvwhole:" << minU << "," << minUwhole << "," << minV << "," << minVwhole << endl;
						qDebug() << "shifted u,v:" << fit->WT(c).U() << "," << fit->WT(c).V() << endl;
					}/*
				}*/
			}
			
			for (c= 0; c < 3;++c)//UVs are per-vertex
			{
				//UwasNegative=VwasNegative=false;
				qDebug() << "------------------" << endl;
				qDebug() << "material " << mat << "position:" << posiz[mat][0] << "," << posiz[mat][1] << endl;
				qDebug() << "U: " << fit->WT(c).U() << "V: " << fit->WT(c).V() << endl;
				qDebug() << "maxdiffs:" << maxdiffUV[mat][0] << "," << maxdiffUV[mat][1] << endl;
				if (maxdiffUV[mat][0]>=1)//so that division by 0 won't yield coordinate of 'inf': infinity, and only do for textures you tiled
					fit->WT(c).U() /= maxdiffUV[mat][0];
				if (maxdiffUV[mat][1]>=1)
					fit->WT(c).V() /= maxdiffUV[mat][1];
				qDebug() << "new (first division) U: " << fit->WT(c).U() << "new V: " << fit->WT(c).V() << endl;
				
				//these values should all be between 0 & 1, as should the final outputs
				if ((fabs(fit->WT(c).U()) > 1)||(fabs(fit->WT(c).V()) > 1))
					qDebug() << "atlas's UV coords outside 0-1 range!" << endl;
				//if (fit->WT(c).U()<0)
				//	UwasNegative=true;
				//if (fit->WT(c).V()<0)
				//	VwasNegative=true;
				fit->WT(c).U() = (fabs(fit->WT(c).U())*images[mat].width() + posiz[mat][0])/global_size[0];//offset U by coord posiz, u*width is pixel, global_size is dimension - for normalizing (between 0 & 1)
				//if (UwasNegative)
				//	fit->WT(c).U() *= -1;
				//extra adjustment here because V coordinate is measured from the bottom, whereas the rect packer gives us the position from the top
				fit->WT(c).V() = (fabs(fit->WT(c).V())*images[mat].height() + (global_size[1]-posiz[mat][1]-images[mat].height()))/global_size[1];//offset V by coord in posiz
				//if (VwasNegative)
				//	fit->WT(c).V() *= -1;
				qDebug() << "new (second division) U: " << fit->WT(c).U() << "new V: " << fit->WT(c).V() << endl;
			}
		}
	}
	qDebug() << "global:" << global_size[0] << "," << global_size[1] << endl;
}

// The Real Core Function doing the actual mesh processing
bool FilterTexturePlugin::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet &par, vcg::CallBackPos * cb)
{
	//this function takes every texture in the model and creates a single texture composed of all the individual textures - necessary before calling the Quadric-with-texture filter
	//QT has support for all the major image formats (GIF/JPEG/PNG/BMP/TIFF) - though you need to compile QT with -qt-gif to get GIF support (on Ubuntu the package for qt already has this - check your /usr/share/qt4/plugins/imageformats folder)
	//can't do any file-format-specific calls, this has to be generalizable to any model
	//the general 'packing problem' is NP-complete, so you have to use a heuristic or genetic algorithm to get an approximate answer
	//this is not an optimal packing - we are not rotating textures, for example
	//TODO:how read image data from images already loaded into Meshlab, to prevent from opening files again?
	qDebug() << "called filterTexture applyFilter" << endl;
	
	//vcg::tri::Allocator<CMeshO>::CompactVertexVector(m);
	//vcg::tri::Allocator<CMeshO>::CompactFaceVector(m);
	
	int numTextures = m.cm.textures.size();
	
	Point2i global_size;
	Point2i max_size;
	Point2i size;
	
	std::vector<Point2i> posiz;
	std::vector<Point2i> sizes;
	max_size[0] = par.getInt("Xmax");
	max_size[1] = par.getInt("Ymax");
	int algo = par.getEnum("algoList");
	
	if (numTextures < 2)
	{
		qDebug() << "filterTexture failed - # textures < 2" << endl;
		return false;//return false if numTextures == 0 or 1 (no need for an atlas)
	}
	
	//1)  generate an array of Point2f that will store the maximum difference in the u & v coordinates for each texture index
	float maxdiffUV[numTextures][2];
	//2)  iterate through all faces, and for each face do 6 subtractions to figure out the maximum difference (absolute value?) for the U & V coordinates of vertices in that face
	CMeshO::FaceIterator fit;
	int mat,c;
	maxFaceSpan(c,maxdiffUV,m,mat,fit);
	
	//4)  when done, copy the texture the necessary number of times (as indicated in the array), - do resampling here, if wanted
	//iterate through all values in maxdiffUV - if either value > 1, tile texture
	QPixmap images[numTextures];//array of images
	QImage tiledimages[numTextures];
	copyTiles(images, tiledimages, numTextures, c, maxdiffUV, m, algo, sizes, size);
	
	if (rect_packer::pack(sizes, max_size, posiz, global_size)==false)
		return false;//textures just couldn't fit in given texture atlas dimensions
	
	adjustUVCoords(mat, c, fit, posiz, global_size, m, maxdiffUV, images);
	
	return createAtlas(numTextures, c, m, images, posiz, global_size);
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
