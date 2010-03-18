/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

$Log: FilterCreateIso.cpp,v $
Revision 1.1  2007/12/02 07:57:48  cignoni
Added the new sample filter plugin that removes border faces


*****************************************************************************/
#include <Qt>
#include <QtGui>
#include "filtercreateiso.h"

#include <vcg/math/perlin_noise.h>
#include <vcg/complex/trimesh/create/marching_cubes.h>
#include <vcg/complex/trimesh/create/mc_trivial_walker.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>

using namespace std;
using namespace vcg;

FilterCreateIso::FilterCreateIso() 
{
  typeList << FP_CREATEISO;
	
  FilterIDType tt;
  foreach(tt , types())
		actionList << new QAction(filterName(tt), this);
	
}

FilterCreateIso::~FilterCreateIso() {
	for (int i = 0; i < actionList.count() ; i++ ) 
		delete actionList.at(i);
}

 QString FilterCreateIso::filterName(FilterIDType filter) const
{
	switch(filter)
  {
	  case FP_CREATEISO :								return QString("Noisy Isosurface");
  	default: assert(0);
  }
  return QString("error!");
}

 QString FilterCreateIso::filterInfo(FilterIDType filterId) const
{
  switch(filterId)
  {
		case FP_CREATEISO:	     return tr("Create a isosurface perturbed by a noisy isosurface."); 
  	default: assert(0);
  }
  return QString("error!");
}

 FilterCreateIso::FilterClass FilterCreateIso::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_CREATEISO : return MeshFilterInterface::MeshCreation;     
    default					  : return MeshFilterInterface::Generic;
  }
}

 int FilterCreateIso::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case FP_CREATEISO :	return MeshModel::MM_NONE;
    default: assert(0);
  }
  return 0;
}

bool FilterCreateIso::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos * cb)
{
    MeshModel &m=*(md.mm());
    CMeshO::FaceIterator fi;
	CMeshO::VertexIterator vi;
	if(filter->text() == filterName(FP_CREATEISO) )
	{
		
		SimpleVolume<SimpleVoxel> 	volume;
		
		typedef vcg::tri::TrivialWalker<CMeshO, SimpleVolume<SimpleVoxel> >	MyWalker;
		typedef vcg::tri::MarchingCubes<CMeshO, MyWalker>	MyMarchingCubes;
		MyWalker walker;
		
		const int gridSize=par.getInt("Resolution");
		// Simple initialization of the volume with some cool perlin noise
		volume.Init(Point3i(gridSize,gridSize,gridSize));
		for(int i=0;i<gridSize;i++)
			for(int j=0;j<gridSize;j++)
				for(int k=0;k<gridSize;k++)
					volume.Val(i,j,k)=(j-gridSize/2)*(j-gridSize/2)+(k-gridSize/2)*(k-gridSize/2)  + i*gridSize/5*(float)math::Perlin::Noise(i*.2,j*.2,k*.2);
		
		
		// MARCHING CUBES
		printf("[MARCHING CUBES] Building mesh...");
		MyMarchingCubes					mc(m.cm, walker);
		walker.BuildMesh<MyMarchingCubes>(m.cm, volume, mc, (gridSize*gridSize)/10);
		vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);																																			 
		vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);					// updates bounding box		
	}
	return true;
}
void FilterCreateIso::initParameterSet(QAction *action,MeshModel &m, RichParameterSet & parlst)
{ 
	pair<float,float> qualityRange;
  switch(ID(action))
  {
    case FP_CREATEISO :
		  parlst.addParam(new RichInt("Resolution",64,"Grid Resolution","Resolution of the side of the cubic grid used for the volume creation"));
 		  break;
  default: break; // do not add any parameter for the other filters
  }
}


Q_EXPORT_PLUGIN(FilterCreateIso)
