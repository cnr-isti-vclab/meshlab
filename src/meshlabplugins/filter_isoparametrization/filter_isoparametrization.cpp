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

#include "../../common/meshmodel.h"
#include <filter_isoparametrization.h>
#include <iso_transfer.h>


#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace vcg;

FilterIsoParametrization::FilterIsoParametrization()
{
	typeList << ISOP_PARAM
		<< ISOP_REMESHING
		<< ISOP_DIAMPARAM
		<< ISOP_LOAD
		<< ISOP_SAVE
		<< ISOP_TRANSFER;

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
	case ISOP_PARAM :			return "Iso Parametrization";
	case ISOP_REMESHING : return "Iso Parametrization Remeshing";
  case ISOP_DIAMPARAM : return "Iso Parametrization Build Atlased Mesh";
	case ISOP_LOAD :			return "Iso Parametrization Load Abstract Domain";
	case ISOP_SAVE :			return "Iso Parametrization Save Abstract Domain";
	case ISOP_TRANSFER:		return "Iso Parametrization transfer between meshes";
	default: assert(0);
	}
	return QString("error!");
}

QString FilterIsoParametrization::filterInfo(FilterIDType filterId) const
{
	switch(filterId)
	{
  case ISOP_PARAM : return "The filter build the abstract Isoparameterization of a two-manifold triangular mesh   <br>"
											"An adaptively chosen abstract domain of the parameterization is built. For more details see: <br>"
											"Pietroni, Tarini and Cignoni, 'Almost isometric mesh parameterization through abstract domains' <br>"
                      "IEEE Transaction of Visualization and Computer Graphics 2010";
  case ISOP_REMESHING : return "Remeshing based on an Abstract Isoparameterization, each triangle of the domain is recursively subdivided. <br>"
													"For more details see: <br>"
													"Pietroni, Tarini and Cignoni, 'Almost isometric mesh parameterization through abstract domains' <br>"
                          "IEEE Transaction of Visualization and Computer Graphics 2010";
  case ISOP_DIAMPARAM : return "The filter build a new mesh with a standard atlased per wedge texture. The atlas is simply done by "
                         "splitting each triangle of the abstract domain<br>"
													"For more details see: <br>"
													"Pietroni, Tarini and Cignoni, 'Almost isometric mesh parameterization through abstract domains' <br>"
                          "IEEE Transaction of Visualization and Computer Graphics 2010";
  case ISOP_LOAD : return "Load the Isoparameterization from a saved Abstract Mesh  <br>"
										 "For more details see: <br>"
										 "Pietroni, Tarini and Cignoni, 'Almost isometric mesh parameterization through abstract domains' <br>"
                     "IEEE Transaction of Visualization and Computer Graphics 2010";
  case ISOP_SAVE : return "Save the Isoparameterization on an Abstract Mesh  <br>"
										 "For more details see: <br>"
										 "Pietroni, Tarini and Cignoni, 'Almost isometric mesh parameterization through abstract domains' <br>"
                     "IEEE Transaction of Visualization and Computer Graphics 2010";
  case ISOP_TRANSFER:return "Transfer the Isoparametrization between two meshes, the two meshes must be reasonably similar and well aligned."
                    " It is useful to transfer back an isoparam onto the original mesh after having computed it on a dummy, clean watertight model.<br>"
											 "For more details see: <br>"
											 "Pietroni, Tarini and Cignoni, 'Almost isometric mesh parameterization through abstract domains' <br>"
                       "IEEE Transaction of Visualization and Computer Graphics 2010";
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

void FilterIsoParametrization::initParameterSet(QAction *a, MeshDocument& md, RichParameterSet & par)
{

	switch(ID(a))
	{
	case ISOP_PARAM:
		{
			par.addParam(new RichInt("targetAbstractMinFaceNum",140,"Abstract Min Mesh Size",
				"This number and the following one indicate the range face number of the abstract mesh that is used for the parametrization process.<br>"
        "The algorithm will choose the best abstract mesh with the number of triangles within the specified interval.<br>"
				"If the mesh has a very simple structure this range can be very low and strict;"
        "for a roughly spherical object if you can specify a range of [8,8] faces you get a octahedral abstract mesh, e.g. a geometry image.<br>"
				"Large numbers (greater than 400) are usually not of practical use."));
			par.addParam(new RichInt("targetAbstractMaxFaceNum",180,"Abstract Max Mesh Size", "Please notice that a large interval requires huge amount of memory to be allocated, in order save the intermediate results. <br>"
				"An interval of 40 should be fine."));
			QStringList stopCriteriaList;
			stopCriteriaList.push_back("Best Heuristic");
			stopCriteriaList.push_back("Area + Angle");
			stopCriteriaList.push_back("Regularity");
			stopCriteriaList.push_back("L2");

      par.addParam(new RichEnum("stopCriteria", 1, stopCriteriaList, tr("Optimization Criteria"),
        tr(//"<p style=\'white-space:pre\'>"
				"Choose a metric to stop the parametrization within the interval<br>"
				"1: Best Heuristic : stop considering both isometry and number of faces of base domain<br>"
				"2: Area + Angle : stop at minimum area and angle distorsion<br>"
				"3: Regularity : stop at minimum number of irregular vertices<br>"
				"4: L2 : stop at minimum OneWay L2 Stretch Eff")));

      par.addParam(new RichInt("convergenceSpeed",1, "Convergence Precision", "This parameter controls the convergence speed/precision of the optimization of the texture coordinates. Larger the number slower the processing and ,eventually, slighly better results"));
			par.addParam(new RichBool("DoubleStep",true,"Double Step","Use this bool to divide the parameterization in 2 steps. Double step makes the overall process faster and robust."
				"<br> Consider to disable this bool in case the object has topologycal noise or small handles."));
			break;
		}
	case ISOP_REMESHING :
		{
			par.addParam(new RichInt("SamplingRate",10,"Sampling Rate", "This specify the sampling rate for remeshing."));
			break;
		}
	case ISOP_DIAMPARAM :
		{
      par.addParam(new RichDynamicFloat("BorderSize",0.1f,0.01f,0.5f,"BorderSize ratio",
          "This parameter controls the amount of space that must be left between each diamond when building the atlas."
          "It directly affects how many triangle are splitted during this conversion. <br>"
          "In abstract parametrization mesh triangles can naturally cross the triangles of the abstract domain, so when converting "
          "to a standard parametrization we must cut all the triangles that protrudes outside each diamond more than the specified threshold."
          "The unit of the threshold is in percentage of the size of the diamond,"
          "The bigger the threshold the less triangles are splitted, but the more UV space is used (wasted)."));
			break;										
		}
	case ISOP_LOAD : 
		{
			QFileInfo fi(md.mm()->fullName());
			QString fileName = fi.baseName();

			fileName = fileName.append(".abs");
      par.addParam(new RichString("AbsName", fileName, "Abstract Mesh file", "The filename of the abstract mesh that has to be loaded"));
			break;
		}
	case ISOP_SAVE : 
		{
			QFileInfo fi(md.mm()->fullName());
			QString fileName = fi.baseName();

			fileName = fileName.append(".abs");
      par.addParam(new RichString("AbsName", fileName, "Abstract Mesh file", "The filename where the abstract mesh has to be saved"));
			break;
		}
	case ISOP_TRANSFER:
		{
      par.addParam(new RichMesh ("sourceMesh",md.mm(),&md, "Source Mesh",	"The mesh already having an Isoparameterization"));
      par.addParam(new RichMesh ("targetMesh",md.mm(),&md, "Target Mesh",	"The mesh to be Isoparameterized"));
    }
	}
}

void FilterIsoParametrization::PrintStats(CMeshO *mesh)
{
  tri::UpdateTopology<CMeshO>::FaceFace(*mesh);
  tri::UpdateTopology<CMeshO>::VertexFace(*mesh);
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
	MeshModel* m = md.mm();  //get current mesh from document
	CMeshO *mesh=&m->cm;
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

			m->updateDataMask(MeshModel::MM_FACEFACETOPO);
      m->updateDataMask(MeshModel::MM_VERTQUALITY); // needed to store patch index

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
      tri::ParamEdgeCollapseParameter pecp;
      IsoParametrizator::ReturnCode ret=Parametrizator.Parametrize<CMeshO>(mesh,pecp,doublestep);

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
        if (!isTXTenabled)    m->clearDataMask(MeshModel::MM_VERTTEXCOORD);
        if (!isFMarkenabled)  m->clearDataMask(MeshModel::MM_FACEMARK);
        if (!isVMarkenabled)  m->clearDataMask(MeshModel::MM_VERTMARK);
        if (!isVColorenabled) m->clearDataMask(MeshModel::MM_VERTCOLOR);
        if (!isFColorenabled) m->clearDataMask(MeshModel::MM_FACECOLOR);
        switch(ret)
        {
        case IsoParametrizator::MultiComponent:
          this->errorMessage="non possible parameterization because of multi componet mesh"; return false;
        case IsoParametrizator::NonSizeCons:
          this->errorMessage="non possible parameterization because of non size consistent mesh"; return false;
        case IsoParametrizator::NonManifoldE:
          this->errorMessage="non possible parameterization because of non manifold edges"; return false;
        case IsoParametrizator::NonManifoldV:
          this->errorMessage="non possible parameterization because of non manifold vertices";return false;
        case IsoParametrizator::NonWatertigh:
          this->errorMessage="non possible parameterization because of non watertight mesh"; return false;
        case IsoParametrizator::FailParam:
          this->errorMessage="non possible parameterization cause one of the following reasons:\n Topologycal noise \n Too Low resolution mesh \n Too Bad triangulation \n"; return false;
        default:
          this->errorMessage="unknown error"; return false;
        }
			}
      // At this point we are sure that everithing went ok so we can allocate surely the abstract
      AbstractMesh *abs_mesh = new AbstractMesh();
      ParamMesh *para_mesh = new ParamMesh();
      Parametrizator.ExportMeshes(*para_mesh,*abs_mesh);
      CMeshO::PerMeshAttributeHandle<IsoParametrization> isoPHandle;
      isoPHandle=tri::Allocator<CMeshO>::AddPerMeshAttribute<IsoParametrization>(*mesh,"isoparametrization");

      bool isOK=isoPHandle().Init(abs_mesh,para_mesh);

			///copy back to original mesh
			isoPHandle().CopyParametrization<CMeshO>(mesh);

			if (!isOK)
			{
        this->errorMessage="Problems gathering parameterization \n";
				return false;
			}
			if (!isVMarkenabled)
				m->clearDataMask(MeshModel::MM_VERTMARK);
			if (!isFMarkenabled)
				m->clearDataMask(MeshModel::MM_FACEMARK);
			return true;
		}
	case ISOP_REMESHING :
		{
      CMeshO::PerMeshAttributeHandle<IsoParametrization> isoPHandle =
          tri::Allocator<CMeshO>::GetPerMeshAttribute<IsoParametrization>(*mesh,"isoparametrization");

      bool b=tri::Allocator<CMeshO>::IsValidHandle<IsoParametrization>(*mesh,isoPHandle);
			if (!b)
			{
				this->errorMessage="You must compute the Base domain before remeshing. Use the Isoparametrization command.";
				return false;
			}
			

			int SamplingRate=par.getInt("SamplingRate");
			if (!SamplingRate>2)
			{
				this->errorMessage="Sampling rate must be >1";
				return false;
			}
      MeshModel* mm=md.addNewMesh("","Re-meshed");

			CMeshO *rem=&mm->cm;
      DiamSampler DiamSampl;
			DiamSampl.Init(&isoPHandle());
			bool done=DiamSampl.SamplePos(SamplingRate);
			assert(done);
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
      tri::UpdateNormals<CMeshO>::PerFace(*rem);
			return true;
		}
	case ISOP_DIAMPARAM :
		{
      CMeshO::PerMeshAttributeHandle<IsoParametrization> isoPHandle =
          tri::Allocator<CMeshO>::GetPerMeshAttribute<IsoParametrization>(*mesh,"isoparametrization");
      bool b=tri::Allocator<CMeshO>::IsValidHandle<IsoParametrization>(*mesh,isoPHandle);
			if (!b)
			{
				this->errorMessage="You must compute the Base domain before remeshing. Use the Isoparametrization command.";
				return false;
			}

			float border_size=par.getDynamicFloat("BorderSize");
      MeshModel* mm=md.addNewMesh("","Diam-Parameterized");
			mm->updateDataMask(MeshModel::MM_WEDGTEXCOORD);
			mm->updateDataMask(MeshModel::MM_VERTCOLOR);
			CMeshO *rem=&mm->cm;
			DiamondParametrizator DiaPara;
			DiaPara.Init(&isoPHandle());
			DiaPara.SetCoordinates<CMeshO>(*rem,border_size);
      tri::UpdateNormals<CMeshO>::PerFace(*rem);
			return true;
		}
	case ISOP_LOAD : 
		{
			QString AbsName = par.getString("AbsName");
			m->updateDataMask(MeshModel::MM_WEDGTEXCOORD);
			m->updateDataMask(MeshModel::MM_VERTTEXCOORD);
			m->updateDataMask(MeshModel::MM_FACECOLOR);
			m->updateDataMask(MeshModel::MM_VERTQUALITY);
			m->updateDataMask(MeshModel::MM_FACEMARK);
			if(!QFile(m->fullName()).exists())
			{
				this->errorMessage="File not exists";
				return false;
			}
      CMeshO::PerMeshAttributeHandle<IsoParametrization> isoPHandle =
          tri::Allocator<CMeshO>::GetPerMeshAttribute<IsoParametrization>(*mesh,"isoparametrization");

      bool b=tri::Allocator<CMeshO>::IsValidHandle<IsoParametrization>(*mesh,isoPHandle);
			if (!b)
        isoPHandle=tri::Allocator<CMeshO>::AddPerMeshAttribute<IsoParametrization>(*mesh,"isoparametrization");

			QByteArray ba = AbsName.toLatin1();
			char *path=ba.data();
      AbstractMesh *abs_mesh = new AbstractMesh();
      ParamMesh *para_mesh = new ParamMesh();
      bool Done=isoPHandle().LoadBaseDomain<CMeshO>(path,mesh,para_mesh,abs_mesh,true);
			if (!Done)
			{
				this->errorMessage="Abstract domain doesnt fit well with the parametrized mesh";
        delete para_mesh;
        delete abs_mesh;
				return false;
			}
			return true;
		}
	case ISOP_SAVE : 
		{
			m->updateDataMask(MeshModel::MM_VERTQUALITY);
      CMeshO::PerMeshAttributeHandle<IsoParametrization> isoPHandle =
          tri::Allocator<CMeshO>::GetPerMeshAttribute<IsoParametrization>(*mesh,"isoparametrization");

      bool b=tri::Allocator<CMeshO>::IsValidHandle<IsoParametrization>(*mesh,isoPHandle);
			if (!b)
			{
				this->errorMessage="You must compute the Base domain before remeshing. Use the Isoparametrization command.";
				return false;
			}
			/*QString Qpath=m->fullName();*/

			QString AbsName = par.getString("AbsName");

			QByteArray ba = AbsName.toLatin1();
			char *path=ba.data();
			isoPHandle().SaveBaseDomain(path);
			return true;
		}
	case ISOP_TRANSFER:
		{
      MeshModel *mmtrg = par.getMesh("targetMesh");
      MeshModel *mmsrc = par.getMesh("targetMesh");
      CMeshO *trgMesh=&mmtrg->cm;
      CMeshO *srcMesh=&mmsrc->cm;

      CMeshO::PerMeshAttributeHandle<IsoParametrization> isoPHandle =
          tri::Allocator<CMeshO>::GetPerMeshAttribute<IsoParametrization>(*mesh,"isoparametrization");

      bool b=tri::Allocator<CMeshO>::IsValidHandle<IsoParametrization>(*srcMesh,isoPHandle);
			if (!b)
			{
        this->errorMessage="Your source mesh must have the abstract isoparametrization. Use the Isoparametrization command.";
				return false;
			}
			IsoTransfer IsoTr;
      AbstractMesh *abs_mesh = isoPHandle().AbsMesh();
      ParamMesh *para_mesh = isoPHandle().ParaMesh();

      mmtrg->updateDataMask(MeshModel::MM_WEDGTEXCOORD);
      mmtrg->updateDataMask(MeshModel::MM_VERTTEXCOORD);
      mmtrg->updateDataMask(MeshModel::MM_FACECOLOR);
      mmtrg->updateDataMask(MeshModel::MM_VERTQUALITY);
      mmtrg->updateDataMask(MeshModel::MM_FACEMARK);
      IsoTr.Transfer<CMeshO>(isoPHandle(),*trgMesh);
			
			isoPHandle().Clear();
      tri::Allocator<CMeshO>::DeletePerMeshAttribute(*srcMesh,isoPHandle);
			
      isoPHandle=tri::Allocator<CMeshO>::AddPerMeshAttribute<IsoParametrization>(*trgMesh,"isoparametrization");
      isoPHandle().AbsMesh()=abs_mesh;
      isoPHandle().SetParamMesh<CMeshO>(trgMesh,para_mesh);
			
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
}

int FilterIsoParametrization::postCondition( QAction* /*filter*/ ) const
{
	return MeshModel::MM_UNKNOWN;
}

Q_EXPORT_PLUGIN(FilterIsoParametrization)
