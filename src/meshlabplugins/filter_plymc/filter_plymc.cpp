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

#include "filter_plymc.h"
#include <vcg/complex/trimesh/append.h>
#include <wrap/io_trimesh/export_vmi.h>

#include "plymc.h"
#include "simplemeshprovider.h"

using namespace vcg;

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

PlyMCPlugin::PlyMCPlugin() 
{ 
    typeList << FP_PLYMC;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
 QString PlyMCPlugin::filterName(FilterIDType filterId) const 
{
  switch(filterId) {
  case FP_PLYMC :  return QString("Surface Reconstruction: VCG");
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
 QString PlyMCPlugin::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
        case FP_PLYMC :  return QString(
              "The surface reconstrction algorithm that have been used for a long time inside the ISTI-Visual Computer Lab."
              "It is mostly a variant of the Curless et al. e.g. a volumetric approach with some original weighting schemes,"
              "a different expansion rule, and another approach to hole filling through volume dilation/relaxations.");
		default : assert(0); 
	}
	return QString("Unknown Filter");
}

// The FilterClass describes in which generic class of filters it fits. 
// This choice affect the submenu in which each filter will be placed 
// More than a single class can be choosen.
 PlyMCPlugin::FilterClass PlyMCPlugin::getClass(QAction *a)
{
  switch(ID(a))
	{
        case FP_PLYMC :  return MeshFilterInterface::Remeshing;
		default : assert(0); 
	}
	return MeshFilterInterface::Generic;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void PlyMCPlugin::initParameterSet(QAction *action,MeshModel &m, RichParameterSet & parlst) 
{
     switch(ID(action))
     {
        case FP_PLYMC :
          parlst.addParam(new RichAbsPerc("voxSize",m.cm.bbox.Diag()/100.0,0,m.cm.bbox.Diag(),"Voxel Side", "VoxelSide"));
          parlst.addParam(    new RichInt("subdiv",1,"SubVol Splitting","The level of recursive splitting of the subvolume reconstruction process. A value of '3' means that a 3x3x3 regular space subdivision is created and the reconstruction process generate 8 matching meshes. It is useful for reconsruction objects at a very high resolution. Default value (1) means no splitting."));
          parlst.addParam(  new RichFloat("geodesic",3.0,"Geodesic Weighting","The influence of each range map is weighted with its geodesic distance from the borders. In this way when two (or more ) range maps overlaps their contribution blends smoothly hiding possible misalignments. "));
          parlst.addParam(   new RichBool("openResult",true,"Show Result","if not checked the result is only saved into the current directory"));
          parlst.addParam(    new RichInt("smoothNum",1,"Volume Laplacian iter","How many volume smoothing step are performed to clean out the eventually noisy borders"));
          parlst.addParam(    new RichInt("wideNum",3,"Widening" ," How many voxel the field is expanded. Larger this value more holes will be filled"));
          parlst.addParam(   new RichBool("mergeColor",false,"Vertex Splatting","This option use a different way to build up the volume, instead of using rasterization of the triangular face it splat the vertices into the grids. It works under the assumption that you have at least one sample for each voxel of your reconstructed volume."));
          parlst.addParam(   new RichBool("simplification",false,"Post Merge simplification","After the merging an automatic simplification step is performed."));
        break;
     default: break; // do not add any parameter for the other filters
  }
}

// The Real Core Function doing the actual mesh processing.
bool PlyMCPlugin::applyFilter(QAction */*filter*/, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos * cb)
{
    srand(time(NULL));

    tri::PlyMC<SMesh,SimpleMeshProvider<SMesh> > pmc;
    tri::PlyMC<SMesh,SimpleMeshProvider<SMesh> >::Parameter &p = pmc.p;

    int subdiv=par.getInt("subdiv");

    p.IDiv=Point3i(subdiv,subdiv,subdiv);
    p.IPosS=Point3i(0,0,0);
    p.IPosE[0]=p.IDiv[0]-1; p.IPosE[1]=p.IDiv[1]-1; p.IPosE[2]=p.IDiv[2]-1;
    printf("AutoComputing all subVolumes on a %ix%ix%i\n",p.IDiv[0],p.IDiv[1],p.IDiv[2]);

    p.VoxSize=par.getAbsPerc("voxSize");
    p.QualitySmoothVox = par.getFloat("geodesic");
    p.SmoothNum = par.getInt("smoothNum");
    p.WideNum = par.getInt("wideNum");
    p.NCell=0;
    p.MergeColor=p.VertSplatFlag=par.getBool("mergeColor");
    p.SimplificationFlag = par.getBool("simplification");
    foreach(MeshModel*mm, md.meshList)
    {
        if(mm->visible)
        {
            SMesh sm;
            mm->updateDataMask(MeshModel::MM_FACEQUALITY);
            tri::Append<SMesh,CMeshO>::Mesh(sm, mm->cm,false,p.VertSplatFlag); // note the last parameter of the append to prevent removal of unreferenced vertices...
            tri::UpdatePosition<SMesh>::Matrix(sm, mm->cm.Tr,true);
            tri::UpdateBounding<SMesh>::Box(sm);
            tri::UpdateNormals<SMesh>::NormalizeVertex(sm);
            //QString mshTmpPath=QDir::tempPath()+QString("/")+QString(mm->shortName())+QString(".vmi");
            QString mshTmpPath=QString("__TMP")+QString(mm->shortName())+QString(".vmi");
            qDebug("Saving tmp file %s",qPrintable(mshTmpPath));
            int retVal = tri::io::ExporterVMI<SMesh>::Save(sm,qPrintable(mshTmpPath) );
            if(retVal!=0)
            {
                qDebug("Failed to write vmi temp file %s",qPrintable(mshTmpPath));
                return false;
            }
            pmc.MP.AddSingleMesh(qPrintable(mshTmpPath));
        }
    }

    pmc.Process(cb);

    if(par.getBool("openResult"))
    {
    for(size_t i=0;i<p.OutNameVec.size();++i)
        {string name;
            if(!p.SimplificationFlag) name = p.OutNameVec[i].c_str();
            else name = p.OutNameSimpVec[i].c_str();

            MeshModel *mp=md.addNewMesh(name.c_str(),0,false);
            tri::io::ImporterPLY<CMeshO>::Open(mp->cm,name.c_str());
            tri::UpdateBounding<CMeshO>::Box(mp->cm);
            tri::UpdateNormals<CMeshO>::PerVertexPerFace(mp->cm);
        }
    }

    for(int i=0;i<pmc.MP.size();++i)
            QFile::remove(pmc.MP.MeshName(i).c_str());

   return true;
}

Q_EXPORT_PLUGIN(PlyMCPlugin)
