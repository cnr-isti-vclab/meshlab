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

#include <Qt>
#include <QtGui>
#include "defines.h"
#include <filter_isoparametrization.h>
#include <vcg/complex/trimesh/update/normal.h>

//#include <vcg/complex/trimesh/update/flag.h>
//#include <vcg/complex/trimesh/update/color.h>
//#include <vcg/space/colorspace.h>
//#include <vcg/math/random_generator.h>

#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace vcg;

FilterIsoParametrization::FilterIsoParametrization()
{
  typeList << ISOP_PARAM
           << ISOP_REMESHING
					 << ISOP_DIAMPARAM;

  FilterIDType tt;
  foreach(tt , types())
    actionList << new QAction(filterName(tt), this);

}

FilterIsoParametrization::~FilterIsoParametrization()
{
	for (int i = 0; i < actionList.count() ; i++ )
		delete actionList.at(i);
}

 QString FilterIsoParametrization::filterName(FilterIDType filter) const
{
  switch(filter)
  {
    case ISOP_PARAM : return "Iso Parametrization";
    case ISOP_REMESHING : return "Iso Parametrization Remeshing";
    case ISOP_DIAMPARAM : return "Iso Parametrization transfer to Original mesh";
    default: assert(0);
  }
  return QString("error!");
}

 QString FilterIsoParametrization::filterInfo(FilterIDType filterId) const
{
  switch(filterId)
  {
    case ISOP_PARAM : return "<br>   Parameterization of a two-manifold triangular mesh   <br>"
			"An adaptively chosen abstract domain of the parameterization is built. For more details see: <br>"
			"Pietroni, Tarini and Cignoni, 'Almost isometric mesh parameterization through abstract domains' <br>"
			"IEEE Transaction of Visualization and Computer Graphics 2009";
    case ISOP_REMESHING : return "<br>	 Remeshing based on an Abstract domain   <br>"
			"For more details see: <br>"
			"Pietroni, Tarini and Cignoni, 'Almost isometric mesh parameterization through abstract domains' <br>"
			"IEEE Transaction of Visualization and Computer Graphics 2009";
    case ISOP_DIAMPARAM : return "<br>	 Transfer the Isoparametrinzation to a Diamond-parametrization over the original mesh  <br>"
			"For more details see: <br>"
			"Pietroni, Tarini and Cignoni, 'Almost isometric mesh parameterization through abstract domains' <br>"
			"IEEE Transaction of Visualization and Computer Graphics 2009";
   default: assert(0);
  }
  return QString("error!");
}

 int FilterIsoParametrization::getRequirements(QAction *action)
{
	(void) action;
    /*switch(ID(action))
    {
        case CP_SCATTER_PER_MESH : return MeshModel::MM_COLOR;
        default : return MeshModel::MM_VERTCOLOR;
    }*/
	return MeshModel::MM_UNKNOWN;
}

void FilterIsoParametrization::initParameterSet(QAction *a, MeshDocument& /*md*/, RichParameterSet & par)
{
  
  switch(ID(a))
  {
    case ISOP_PARAM:
    {
		par.addParam(new RichInt("targetAbstractMinFaceNum",100,"Abstract Min Mesh Size",
			"This number and the following one indicate the range face number of the abstract mesh that is used for the parametrization process.<br>"
			"The algorithm will choose the abstract mesh with the best number of triangles within the specified interval.<br>"
			"If the mesh has a very simple structure this range can be very low and strict;"
			"for a roughly spherical object if you specify a range of [8,8] faces you get a octahedral abstract mesh, e.g. a geometry image.<br>"
			"Large numbers (greater than 400) are usually not of practical use."));
		par.addParam(new RichInt("targetAbstractMaxFaceNum",200,"Abstract Max Mesh Size", "See above."));
		QStringList stopCriteriaList;
		stopCriteriaList.push_back("Best Heuristic");
		stopCriteriaList.push_back("Area + Angle");
		stopCriteriaList.push_back("Regularity");
		stopCriteriaList.push_back("L2");
		
		par.addParam(new RichEnum("stopCriteria", 0, stopCriteriaList, tr("Optimization Criteria"),
		        tr("<p style=\'white-space:pre\'>"
			        "Choose a metric to stop the parametrization within the interval<br>"
					"1: Best Heuristic : stop considering both isometry and number of faces of base domain<br>"
					"2: Area + Angle : stop at minimum area and angle distorsion<br>"
			        "3: Regularity : stop at minimum number of irregular vertices<br>"
			        "4: L2 : stop at minimum OneWay L2 Stretch Eff")));
		
		//par.addParam(new RichDynamicFloat("convergenceSpeed", 1, 1,4, "Convergence Speed", "This parameter controls the convergence speed/precision of the optimization of the texture coordinates. Larger the number slower the processing and ,eventually, slighly better results");
		par.addParam(new RichInt("convergenceSpeed",2, "Convergence Speed", "This parameter controls the convergence speed/precision of the optimization of the texture coordinates. Larger the number slower the processing and ,eventually, slighly better results"));
		par.addParam(new RichBool("DoubleStep",true,"Double Step","Use this bool to divide the parameterization in 2 steps. Double step makes the overall process faster and robust, but it may increase the distorsion"));
			break;
	}
	 case ISOP_REMESHING :
	{
		 par.addParam(new RichInt("SamplingRate",10,"Sampling Rate", "This specify the sampling rate for remeshing."));
		 break;
  }
	case ISOP_DIAMPARAM :
	{
		 par.addParam(new RichDynamicFloat("BorderSize",0.05,0.01,0.5,"BorderSize ratio", "This specify the border for each diamond.<br>"
																				"The bigger is the less triangles are splitted, but the more UV space is used."));								 
		break;										
	}
}
}

void FilterIsoParametrization::PrintStats(CMeshO *mesh)
{
  vcg::tri::UpdateTopology<CMeshO>::FaceFace(*mesh);
  vcg::tri::UpdateTopology<CMeshO>::VertexFace(*mesh);
  int non_reg=NumRegular<CMeshO>(*mesh);
  float minE,maxE,avE,stdE;
  float minAr,maxAr,avAr,stdAr;
  float minAn,maxAn,avAn,stdAn;
  int minEi,maxEi,stdEi;
  int minAri,maxAri,stdAri;
  int minAni,maxAni,stdAni;
  
	StatEdge<CMeshO>(*mesh,minE,maxE,avE,stdE);
	StatArea<CMeshO>(*mesh,minAr,maxAr,avAr,stdAr);
	StatAngle<CMeshO>(*mesh,minAn,maxAn,avAn,stdAn);
	minE/=avE;
	minE*=100.f;
	maxE/=avE;
	maxE*=100.f;
	stdE/=avE;
	stdE*=100.f;

	minAn/=avAn;
	minAn*=100.f;
	maxAn/=avAn;
	maxAn*=100.f;
	stdAn/=avAn;
	stdAn*=100.f;

	minAr/=avAr;
	minAr*=100.f;
	maxAr/=avAr;
	maxAr*=100.f;
	stdAr/=avAr;
	stdAr*=100.f;

	minEi=(int)minE;
	maxEi=(int)maxE;
	stdEi=(int)stdE;

	minAni=(int)minAn;
	maxAni=(int)maxAn;
	stdAni=(int)stdAn;
	
	minAri=(int)minAr;
	maxAri=(int)maxAr;
	stdAri=(int)stdAr;

	Log(" REMESHED ");
	Log("Irregular Vertices:%d ",non_reg);
	Log("stdDev Area:%d",stdAri);
	Log("stdDev Angle:%d",stdAni);
	Log("stdDev Edge:%d",stdEi);
}

bool FilterIsoParametrization::applyFilter(QAction *filter, MeshDocument& md, RichParameterSet & par, vcg::CallBackPos  *cb)
{
  (void) par;
  MeshModel* m = md.mm();  //get current mesh from document
  

 
  CMeshO *mesh=&m->cm;
  

  //return true;
  switch(ID(filter))
  {
	case ISOP_PARAM :
	{

		int targetAbstractMinFaceNum = par.getInt("targetAbstractMinFaceNum");
		int targetAbstractMaxFaceNum = par.getInt("targetAbstractMaxFaceNum");
		int convergenceSpeed = par.getInt("convergenceSpeed");
		int stopCriteria=par.getEnum("stopCriteria");
		bool doublestep=par.getBool("DoubleStep");
		IsoParametrizator Parametrizator;
		
		bool isTXTenabled=m->hasDataMask(MeshModel::MM_VERTTEXCOORD);
		if (!isTXTenabled)
			m->updateDataMask(MeshModel::MM_VERTTEXCOORD);

		bool isVMarkenabled=m->hasDataMask(MeshModel::MM_VERTMARK);
		if (!isVMarkenabled)
			m->updateDataMask(MeshModel::MM_VERTMARK);

		bool isFMarkenabled=m->hasDataMask(MeshModel::MM_FACEMARK);
		if (!isFMarkenabled)
			m->updateDataMask(MeshModel::MM_FACEMARK);

		bool isVColorenabled=m->hasDataMask(MeshModel::MM_VERTCOLOR);
		if (!isVColorenabled)
			m->updateDataMask(MeshModel::MM_VERTCOLOR);

		bool isFColorenabled=m->hasDataMask(MeshModel::MM_FACECOLOR);
		if (!isFColorenabled)
			m->updateDataMask(MeshModel::MM_FACECOLOR);
		int tolerance = targetAbstractMaxFaceNum-targetAbstractMinFaceNum;
		switch (stopCriteria)
		{
			case 0:Parametrizator.SetParameters(cb,targetAbstractMinFaceNum,tolerance,IsoParametrizator::SM_Euristic,convergenceSpeed);break;
			case 1:Parametrizator.SetParameters(cb,targetAbstractMinFaceNum,tolerance,IsoParametrizator::SM_Corr,convergenceSpeed);break;
			case 2:Parametrizator.SetParameters(cb,targetAbstractMinFaceNum,tolerance,IsoParametrizator::SM_Reg,convergenceSpeed);break;
			case 3:Parametrizator.SetParameters(cb,targetAbstractMinFaceNum,tolerance,IsoParametrizator::SM_L2,convergenceSpeed);break;
			default:Parametrizator.SetParameters(cb,targetAbstractMinFaceNum,tolerance,IsoParametrizator::SM_Euristic,convergenceSpeed);break;
		}
		IsoParametrizator::ReturnCode ret=Parametrizator.Parametrize<CMeshO>(mesh,doublestep);
		
		if (ret==IsoParametrizator::Done)
		{
			Parametrizator.PrintAttributes();
			float aggregate,L2;
			int n_faces;
			Parametrizator.getValues(aggregate,L2,n_faces);
			Log("Num Faces of Abstract Domain: %d, One way stretch efficiency: %.4f, Area+Angle Distorsion %.4f  ",n_faces,L2,aggregate*100.f);
		}
		else
		{
			if (!isTXTenabled)
				m->clearDataMask(MeshModel::MM_VERTTEXCOORD);
			if (!isFMarkenabled)
				m->clearDataMask(MeshModel::MM_FACEMARK);
			if (!isVMarkenabled)
				m->clearDataMask(MeshModel::MM_VERTMARK);
			if (!isVColorenabled)
				m->clearDataMask(MeshModel::MM_VERTCOLOR);
			if (!isFColorenabled)
				m->clearDataMask(MeshModel::MM_FACECOLOR);
			if (ret==IsoParametrizator::NonPrecondition)
				this->errorMessage="non possible parameterization because of violated preconditions";
			else
			if (ret==IsoParametrizator::FailParam)
				this->errorMessage="non possible parameterization cause because missing the intepolation for some triangle of original the mesh (maybe due to topologycal noise)";
			return false;
		}
		Parametrizator.ExportMeshes(para_mesh,abs_mesh);
		isoPHandle=vcg::tri::Allocator<CMeshO>::AddPerMeshAttribute<IsoParametrization>(*mesh,"isoparametrization");
		bool isOK=isoPHandle().Init(&abs_mesh,&para_mesh);
		if (!isOK)
		{
			Log("Problems gathering parameterization \n");
			return false;
		}
		if (!isTXTenabled)
				m->clearDataMask(MeshModel::MM_VERTTEXCOORD);
		if (!isVMarkenabled)
				m->clearDataMask(MeshModel::MM_VERTMARK);
		if (!isFMarkenabled)
				m->clearDataMask(MeshModel::MM_FACEMARK);
		if (!isVColorenabled)
				m->clearDataMask(MeshModel::MM_VERTCOLOR);
		if (!isFColorenabled)
				m->clearDataMask(MeshModel::MM_FACECOLOR);
		return true;
	}
	case ISOP_REMESHING :
	{
		bool b=vcg::tri::Allocator<CMeshO>::IsValidHandle<IsoParametrization>(*mesh,isoPHandle);
		if (!b)
		{
			this->errorMessage="You must compute the Base domain before remeshing. Use the Isoparametrization command.";
			return false;
		}
		

		int SamplingRate=par.getInt("SamplingRate");
		MeshModel* mm=md.addNewMesh("Re-meshed");
		
		CMeshO *rem=&mm->cm;
		
		DiamSampl.Init(&isoPHandle());
		DiamSampl.SamplePos(SamplingRate);
		DiamSampl.GetMesh<CMeshO>(*rem);

		int n_diamonds,inFace,inEdge,inStar,n_merged;
		DiamSampl.getResData(n_diamonds,inFace,inEdge,inStar,n_merged);

		Log("INTERPOLATION DOMAINS");
		Log("In Face: %d \n",inFace);
		Log("In Diamond: %d \n",inEdge);
		Log("In Star: %d \n",inStar);
		Log("Merged %d vertices\n",n_merged);
		mm->updateDataMask(MeshModel::MM_FACEFACETOPO);
		mm->updateDataMask(MeshModel::MM_VERTFACETOPO);
		PrintStats(rem);
		vcg::tri::UpdateNormals<CMeshO>::PerFace(*rem);
		return true;
	}
	case ISOP_DIAMPARAM :
	{
		bool b=vcg::tri::Allocator<CMeshO>::IsValidHandle<IsoParametrization>(*mesh,isoPHandle);
		if (!b)
		{
			this->errorMessage="You must compute the Base domain before remeshing. Use the Isoparametrization command.";
			return false;
		}
		
		float border_size=par.getDynamicFloat("BorderSize");
		MeshModel* mm=md.addNewMesh("Diam-Parameterized");
		mm->updateDataMask(MeshModel::MM_WEDGTEXCOORD);
		mm->updateDataMask(MeshModel::MM_VERTCOLOR);
		CMeshO *rem=&mm->cm;
		DiamondParametrizator DiaPara;
		DiaPara.Init(&isoPHandle());
		DiaPara.SetCoordinates<CMeshO>(*rem,border_size);
		vcg::tri::UpdateNormals<CMeshO>::PerFace(*rem);
		return true;
	}
  }
  return false;
}

 MeshFilterInterface::FilterClass FilterIsoParametrization::getClass(QAction *a)
{
  switch(ID(a))
  {
    case ISOP_PARAM : return MeshFilterInterface::Remeshing;
	case ISOP_REMESHING : return MeshFilterInterface::Remeshing;
	case ISOP_DIAMPARAM : return MeshFilterInterface::Remeshing;
    default: return MeshFilterInterface::Remeshing;
  }
  //return (MeshFilterInterface::Remeshing);
}

int FilterIsoParametrization::postCondition( QAction* filter ) const
{
	(void)filter;
   /* switch(ID(filter))
    {
        case CP_SCATTER_PER_MESH : return MeshModel::MM_COLOR;
        default : return MeshModel::MM_VERTCOLOR;
    }*/
	 return MeshModel::MM_UNKNOWN;
}

int FilterIsoParametrization::getPreConditions( QAction * filter ) const
{
	(void)filter;
	/*switch(ID(filter))
	{
		case CP_FILLING :
		case CP_COLOURISATION :
		case CP_SCATTER_PER_MESH :
		case CP_PERLIN_COLOR :
		case CP_COLOR_NOISE :
		{
			return MeshFilterInterface::FP_Generic;
			break;
		}
		case CP_THRESHOLDING :
		case CP_BRIGHTNESS :
		case CP_CONTRAST :
		case CP_CONTR_BRIGHT :
		case CP_GAMMA :
		case CP_INVERT :
		case CP_EQUALIZE :
		case CP_DESATURATION :
		case CP_WHITE_BAL :
		case CP_LEVELS :
		{
			return MeshFilterInterface::FP_VertexColor;	
			break;
		}
		default: assert(0); return MeshFilterInterface::FP_Generic;
	}*/
	return MeshFilterInterface::FP_Generic;
}

Q_EXPORT_PLUGIN(FilterIsoParametrization)
