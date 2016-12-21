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
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#include "filter_multiscale_align.h"
#include <vcg/complex/algorithms/point_sampling.h>
#include <vcg/complex/algorithms/create/resampler.h>
#include <math.h>
#include <stdio.h>

using namespace vcg;
using namespace std;

class BaseSampler
{
public:
  BaseSampler(CMeshO* _m){m=_m; uvSpaceFlag = false; qualitySampling=false; tex=0;}
  CMeshO *m;
  QImage* tex;
  int texSamplingWidth;
  int texSamplingHeight;
  bool uvSpaceFlag;
  bool qualitySampling;

  void AddVert(CMeshO::VertexType &p)
  {
    tri::Allocator<CMeshO>::AddVertices(*m,1);
    m->vert.back().ImportData(p);
	//p.C()=Color4b(0,0,0,1);
	p.SetS();
  }

}; // end class BaseSampler


// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterMultiscaleAlign::FilterMultiscaleAlign()
{
	typeList <<MS_ALIGN;

  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

QString FilterMultiscaleAlign::filterName(FilterIDType filterId) const
{
  switch(filterId) {
    case MS_ALIGN : return QString("Multiscale Align");
		default : assert(0);
	}
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
 QString FilterMultiscaleAlign::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
    case MS_ALIGN : return QString("Create a Multiscale Align");
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
 void FilterMultiscaleAlign::initParameterSet(QAction *action,MeshDocument & md, RichParameterSet & parlst)
 {
     MeshModel *target;
     switch(ID(action))	 {

     case MS_ALIGN :
         target= md.mm();
         foreach (target, md.meshList)
             if (target != md.mm())  break;

         parlst.addParam(new RichMesh ("RefMesh", md.mm(),&md, "Reference Mesh",
                                       "The mesh with correct scale values and high level of details"));
         parlst.addParam(new RichMesh ("SecondMesh", target,&md, "Second Mesh",
                                       "The mesh for which a scale factor is searched and then used for align in the reference frame."));
		 parlst.addParam(new RichEnum("Seed extraction method", 0, QStringList()<<"Descriptive"<<"Random", tr("Seed extraction method"),
			 QString("The seed extraction method")));
         parlst.addParam(new RichBool("Check Borders",true,"Check for border points","Check for border points"));
		 parlst.addParam(new RichBool("Use Quadriplets",true,"Use quadriplets","Use quadriplets"));
		 parlst.addParam(new RichFloat("Error Threshold",4.0,"Average alignment error for seleted seeds","Average alignment error for seleted seeds"));
		 parlst.addParam(new RichFloat("Expected scale",2.0,"Expected scale","Expected scale (to fasten the search)"));

         parlst.addParam(new RichInt ("Seed Number", 2000, "Seed Number", "Seed Number"));
        
         break;
     default : return;
     }
 }

// The Real Core Function doing the actual mesh processing.
bool FilterMultiscaleAlign::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos * /*cb*/)
{
    switch(ID(filter))
    {
    case MS_ALIGN:
    {

        MeshModel *refMesh     = par.getMesh("RefMesh");
        MeshModel *toAlignMesh = par.getMesh("SecondMesh");

		Options opt;

		bool useDescriptive;

		switch(par.getEnum("Seed extraction method"))
		{
		case 0: opt.useDescriptive=true;   break;
		case 1: opt.useDescriptive=false; break;
		}

		opt.checkBorders=par.getBool("Check Borders");
		opt.alignError=par.getFloat("Error Threshold");
		opt.useQuadriplets=par.getBool("Use Quadriplets");
        opt.expectedScale=par.getFloat("Expected scale");

        opt.seedNumber= par.getInt("Seed Number");
        
        tri::RequirePerVertexNormal(refMesh->cm);
        tri::RequirePerVertexNormal(toAlignMesh->cm);

		float scaleFact=1.0;

		////// Poisson sampling to create the models and define the seams
        float radiusRef, radiusToAlign, refCoarse, alignCoarse;
        MeshModel* refSampled;
        MeshModel* refAlign;

        if (refMesh->cm.vn>SUBSAMPLES)
        {
            radiusRef = 2*poissonDiskSampling(md, refMesh, "refSampled", SUBSAMPLES, false);
            refSampled=md.getMesh(md.size()-1);
        }
        else
        {
            refSampled=refMesh;
            radiusRef= tri::SurfaceSampling<CMeshO,BaseSampler>::ComputePoissonDiskRadius(refMesh->cm, refMesh->cm.vn);
        }
        poissonDiskSampling(md, refSampled, "refSampledSampled", opt.seedNumber, opt.checkBorders);
       
        if (toAlignMesh->cm.vn>SUBSAMPLES)
        {
           radiusToAlign = 2*poissonDiskSampling(md, toAlignMesh, "refToAlign", SUBSAMPLES, false);
           refAlign=md.getMesh(md.size()-1);
        }
        else
        {
            refAlign=toAlignMesh;
            radiusToAlign= tri::SurfaceSampling<CMeshO,BaseSampler>::ComputePoissonDiskRadius(toAlignMesh->cm, toAlignMesh->cm.vn);
        }
        poissonDiskSampling(md, refAlign, "refToAlignSampled", opt.seedNumber, opt.checkBorders);
        

		Log("Min radius reference: %f , Min radius toAlign: %f",radiusRef,radiusToAlign);
		
		/////////////////
		////PRE-SCALING
		/////////////////
//   Here there could be a pre-scaling operation, that brings the two models in similar space. The ration could be given by radiusRef/radiusToAlign, since these two values define the "detail" of the models
		/////////////////
		///END PRE-SCALING
		///////////////

        refCoarse   = MAX_SCALE_MULTIPLIER*refMesh->cm.bbox.Diag()/2.0f;
        alignCoarse = MAX_SCALE_MULTIPLIER*toAlignMesh->cm.bbox.Diag()/2.0f;

        Log("Max radius reference: %f , Max radius toAlign: %f",refCoarse,alignCoarse);
		
        double logBase=std::log((double)MULTIPLIER);

		///// This creates the scales vector
		int numScales= (int)(std::log(max(refCoarse,alignCoarse)/min(radiusRef,radiusToAlign)) / logBase) + 2; 

        Log("Nb scales: %i",numScales);

        _msa._seedNumber = opt.seedNumber;
        scaleFact=_msa.process_scale(refSampled,radiusRef,refCoarse,refAlign,radiusToAlign,alignCoarse, MULTIPLIER, numScales, opt);

		if (scaleFact==-1)
			Log("Sorry, alignment not found",scaleFact);
		else
			Log("Alignment found, final scale factor: %f ",scaleFact);

        refMesh->updateDataMask(MeshModel::MM_VERTQUALITY);
        toAlignMesh->updateDataMask(MeshModel::MM_VERTQUALITY);
        refSampled->updateDataMask(MeshModel::MM_VERTQUALITY);
        refAlign->updateDataMask(MeshModel::MM_VERTQUALITY);

        break;
    }
    }



    return true;
}

float FilterMultiscaleAlign::selectAllPoints(MeshDocument &md, MeshModel *mesh, QString label)
{

    CMeshO::VertexIterator vj;
    for(vj = mesh->cm.vert.begin(); vj != mesh->cm.vert.end(); ++vj)
    {
        (*vj).SetS();
        (*vj).Q()=MAX_SCALE_MULTIPLIER*mesh->cm.bbox.Diag();
    }
    MeshModel *mm= md.addNewMesh("", label);
    mm->updateDataMask(mesh);
    mm->updateDataMask(MeshModel::MM_VERTQUALITY);
    mm->updateDataMask(MeshModel::MM_VERTCOLOR);

    // workaround to estimate radius
    return tri::SurfaceSampling<CMeshO,BaseSampler>::
            ComputePoissonDiskRadius(mesh->cm,
                                     mesh->cm.vert.size());
}

///// This calculates the poisson sampling
float FilterMultiscaleAlign::poissonDiskSampling(MeshDocument &md, MeshModel *mesh, QString label, float numSamples, bool checkBord)
{

	CMeshO::VertexIterator vj;
	for(vj = mesh->cm.vert.begin(); vj != mesh->cm.vert.end(); ++vj)
		{
			(*vj).ClearS();
			
		}

	MeshModel *mm= md.addNewMesh("",label); // After Adding a mesh to a MeshDocument the new mesh is the current one
    mm->updateDataMask(mesh);
    mm->updateDataMask(MeshModel::MM_VERTQUALITY);
    mm->updateDataMask(MeshModel::MM_VERTCOLOR);
	
	int sampleNum = numSamples;
	float radius = tri::SurfaceSampling<CMeshO,BaseSampler>::ComputePoissonDiskRadius(mesh->cm,sampleNum);

	Log("Computing %i Poisson Samples for an expected radius of %f",sampleNum,radius);

	// first of all generate montecarlo samples for fast lookup
    CMeshO *presampledMesh=&(mesh->cm);

    //CMeshO MontecarloMesh; // this mesh is used only if we need real poisson sampling (and therefore we need to choose points different from the starting mesh vertices)

    
      //QTime tt;tt.start();
      //BaseSampler sampler(&MontecarloMesh);
      //sampler.qualitySampling =true;
      //tri::SurfaceSampling<CMeshO,BaseSampler>::Montecarlo(mesh->cm, sampler, sampleNum*1);
      //MontecarloMesh.bbox = mesh->cm.bbox; // we want the same bounding box
      //presampledMesh=&MontecarloMesh;
      //Log("Generated %i Montecarlo Samples (%i msec)",MontecarloMesh.vn,tt.elapsed());
    

    BaseSampler mps(&(mm->cm));

    tri::SurfaceSampling<CMeshO,BaseSampler>::PoissonDiskParam pp;
    //tri::SurfaceSampling<CMeshO,BaseSampler>::PoissonDiskParam::Stat pds; pp.pds=&pds;
    pp.bestSampleChoiceFlag=true;
    pp.bestSamplePoolSize =10;
    
    tri::SurfaceSampling<CMeshO,BaseSampler>::PoissonDiskPruning(mps, *presampledMesh, radius,pp);
    //tri::SurfaceSampling<CMeshO,BaseSampler>::PoissonDisk(curMM->cm, mps, *presampledMesh, radius,pp);
    vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
    //Point3i &g=pp.pds->gridSize;
    //Log("Grid size was %i %i %i (%i allocated on %i)",g[0],g[1],g[2], pp.pds->gridCellNum, g[0]*g[1]*g[2]);
    Log("Sampling created a new mesh of %i points",md.mm()->cm.vn);

 //   if (checkBord)
	//{
	//	_msa.checkBorders(md.mm(), radius, MULTIPLIER);

	//	/*Log("Borders checked");
	//	Log("Orig size %i Sample size %i",mesh->cm.vn, md.mm()->cm.vn);*/

	//	for(CMeshO::VertexIterator vi = mesh->cm.vert.begin(); vi != mesh->cm.vert.end(); ++vi)
	//	{
	//		if((*vi).IsS())
	//		{
	//			for(CMeshO::VertexIterator vj = md.mm()->cm.vert.begin(); vj != md.mm()->cm.vert.end(); ++vj)
	//			{
	//				if ((*vj).P()==(*vi).P())
	//				{
	//					if((*vj).Q()==0.0)
	//					{
	//						(*vi).ClearS();
	//					}
	//					else
	//					{
	//						(*vi).Q()=(*vj).Q();
	//						(*vi).C()=(*vj).C();
	//					}
	//					//Log("One done");
	//					break;
	//				}
	//			}
	//		}
	//		

	//	}
	//}
	//else
	//{
		for(CMeshO::VertexIterator vi = mesh->cm.vert.begin(); vi != mesh->cm.vert.end(); ++vi)
		{
			if((*vi).IsS())
			{
                (*vi).Q()=MAX_SCALE_MULTIPLIER*mesh->cm.bbox.Diag();
			}
		}

	//}

    //we have to use the indices of the vertices, and they MUST be continuous
    tri::Allocator<CMeshO>::CompactVertexVector(mesh->cm);

	return radius;

}

 MeshFilterInterface::FilterClass FilterMultiscaleAlign::getClass(QAction *a)
{
  switch(ID(a))
  {

    case MS_ALIGN:
      return FilterClass(MeshFilterInterface::Layer+MeshFilterInterface::RangeMap);
      break;
    default: assert(0);
      return MeshFilterInterface::Generic;
  }
}


 MeshFilterInterface::FILTER_ARITY FilterMultiscaleAlign::filterArity(QAction* filter) const
 {
	 switch (ID(filter))
	 {
	 case MS_ALIGN:
		 return MeshFilterInterface::SINGLE_MESH;
	  }
	 return MeshFilterInterface::NONE;
 }



MESHLAB_PLUGIN_NAME_EXPORTER(FilterMultiscaleAlign)
