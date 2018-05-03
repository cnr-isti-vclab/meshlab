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
#include "filter_isoparametrization.h"

#include <QStringList>
#include <QFileInfo>
#include "defines.h"

#include "../../common/meshmodel.h"
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
  case ISOP_PARAM :			return "Iso Parametrization: Main";
  case ISOP_REMESHING :     return "Iso Parametrization Remeshing";
  case ISOP_DIAMPARAM :     return "Iso Parametrization Build Atlased Mesh";
  case ISOP_TRANSFER:		return "Iso Parametrization transfer between meshes";
  default: assert(0);
  }
  return QString("error!");
}
static const QString bibRef("For more details see: <br>"
                            "<b>N. Pietroni, M. Tarini and P. Cignoni</b>, <br><a href=\"http://vcg.isti.cnr.it/Publications/2010/PTC10/\">'Almost isometric mesh parameterization through abstract domains'</a> <br>"
                            "IEEE Transaction of Visualization and Computer Graphics, 2010");

QString FilterIsoParametrization::filterInfo(FilterIDType filterId) const
{
  switch(filterId)
  {
  case ISOP_PARAM : return "The filter builds the abstract domain mesh representing the Isoparameterization of a watertight two-manifold triangular mesh. <br>"
                           "This abstract mesh can be used to uniformly remesh the input mesh, or to build a atlased texture parametrization. Abstract Mesh can be also loaded and saved. <br>"
        "In short this filter build a very coarse almost regular triangulation such that original mesh can be reprojected from this abstract mesh with minimal distortion.<br>"+bibRef;
  case ISOP_REMESHING : return "Uniform Remeshing based on Isoparameterization, each triangle of the domain is recursively subdivided. <br>"
                               +bibRef;
  case ISOP_DIAMPARAM : return "The filter build a new mesh with a standard atlased per wedge texture. The atlas is simply done by "
                               "exploiting the low distortion, coarse, regular, mesh of the abstract domain<br>" +bibRef;
  case ISOP_TRANSFER:return "Transfer the Isoparametrization between two meshes, the two meshes must be reasonably similar and well aligned."
                            " It is useful to transfer back an isoparam onto the original mesh after having computed it on a dummy, clean watertight model.<br>"
                            +bibRef;
  default: assert(0);
  }
  return QString("error!");
}

int FilterIsoParametrization::getRequirements(QAction *)
{
	return MeshModel::MM_NONE;
}

void FilterIsoParametrization::initParameterSet(QAction *a, MeshDocument& md, RichParameterSet & par)
{
  
  switch(ID(a))
  {
  case ISOP_PARAM:
  {
    par.addParam(new RichInt("targetAbstractMinFaceNum",150,"AM  Min Size",
                             "This number and the following one indicate the range face number of the abstract mesh that is used for the parametrization process.<br>"
                             "The algorithm will choose the best abstract mesh with the number of triangles within the specified interval.<br>"
                             "If the mesh has a very simple structure this range can be very low and strict;"
                             "for a roughly spherical object if you can specify a range of [8,8] faces you get a octahedral abstract mesh, e.g. a geometry image.<br>"
                             "Large numbers (greater than 400) are usually not of practical use."));
    par.addParam(new RichInt("targetAbstractMaxFaceNum",200,"AM Max Size", "Please notice that a large interval requires huge amount of memory to be allocated, in order save the intermediate results. <br>"
                                                                                      "An interval of 50 should be fine."));
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
    par.addParam(new RichString("AbsLoadName", "", "Load AM", "The filename of the abstract mesh that has to be loaded. If empty, the abstract mesh will be computed according to the above parameters (suggested extension '.abs')."));
    par.addParam(new RichString("AbsSaveName", "", "Save AM", "The filename where the computed abstract mesh will be saved. If empty, nothing will be done."));    
    break;
  }
  case ISOP_REMESHING :
  {
    par.addParam(new RichInt("SamplingRate",10,"Sampling Rate", "This specify the sampling rate for remeshing. Must be greater than 2"));
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
  CMeshO::ScalarType minE,maxE,avE,stdE;
  CMeshO::ScalarType minAr,maxAr,avAr,stdAr;
  CMeshO::ScalarType minAn,maxAn,avAn,stdAn;
  
  StatEdge<CMeshO>(*mesh,minE,maxE,avE,stdE);
  StatArea<CMeshO>(*mesh,minAr,maxAr,avAr,stdAr);
  StatAngle<CMeshO>(*mesh,minAn,maxAn,avAn,stdAn);
  Log(" REMESHED ");
  Log("Irregular Vertices:%d ",non_reg);
  Log("stdDev Area:  %5.2f",stdAr/avAr);
  Log("stdDev Angle: %5.2f",stdAn/avAn);
  Log("stdDev Edge:  %5.2f",stdE/avE);
}

bool FilterIsoParametrization::applyFilter(QAction *filter, MeshDocument& md, RichParameterSet & par, vcg::CallBackPos  *cb)
{
  MeshModel* m = md.mm();  //get current mesh from document
  CMeshO *mesh=&m->cm;
  switch(ID(filter))
  {
  case ISOP_PARAM :  //////////////// the main filter //////////////////////
  {
    int targetAbstractMinFaceNum = par.getInt("targetAbstractMinFaceNum");
    int targetAbstractMaxFaceNum = par.getInt("targetAbstractMaxFaceNum");
    int convergenceSpeed = par.getInt("convergenceSpeed");
    int stopCriteria=par.getEnum("stopCriteria");
    bool doublestep=par.getBool("DoubleStep");
    
    m->updateDataMask(MeshModel::MM_FACEFACETOPO);
    m->updateDataMask(MeshModel::MM_VERTQUALITY); // needed to store patch index
    m->updateDataMask(MeshModel::MM_VERTMARK | MeshModel::MM_FACEMARK);
    
    bool isTXTenabled=m->hasDataMask(MeshModel::MM_VERTTEXCOORD);
    if (!isTXTenabled)
      m->updateDataMask(MeshModel::MM_VERTTEXCOORD);
    
    bool isVColorenabled=m->hasDataMask(MeshModel::MM_VERTCOLOR);
    if (!isVColorenabled)
      m->updateDataMask(MeshModel::MM_VERTCOLOR);
    
    bool isFColorenabled=m->hasDataMask(MeshModel::MM_FACECOLOR);
    if (!isFColorenabled)
      m->updateDataMask(MeshModel::MM_FACECOLOR);
    int tolerance = targetAbstractMaxFaceNum-targetAbstractMinFaceNum;

    CMeshO::PerMeshAttributeHandle<IsoParametrization> isoPHandle = 
        tri::Allocator<CMeshO>::GetPerMeshAttribute<IsoParametrization>(*mesh,"isoparametrization");
    
    QString AbsLoadName = par.getString("AbsLoadName");
    if(AbsLoadName.isEmpty())
    {    
      IsoParametrizator Parametrizator;
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
      
      bool isOK=isoPHandle().Init(abs_mesh,para_mesh);
      if (!isOK) {
        this->errorMessage="Problems gathering parameterization \n";
        return false;
      }
      
      isoPHandle().CopyParametrization<CMeshO>(mesh); ///copy back to original mesh
    }
    else
    {
      AbstractMesh *abs_mesh = new AbstractMesh();
      ParamMesh *para_mesh = new ParamMesh();
      bool Done=isoPHandle().LoadBaseDomain<CMeshO>(qUtf8Printable(AbsLoadName),mesh,para_mesh,abs_mesh,true);
      if (!Done)
      {
        this->errorMessage="Abstract domain doesnt fit well with the parametrized mesh";
        delete para_mesh;
        delete abs_mesh;
        return false;
      }
    }
    
    QString AbsSaveName = par.getString("AbsSaveName");
    if(!AbsSaveName.isEmpty())
    {
      isoPHandle().SaveBaseDomain(qUtf8Printable(AbsSaveName));
    }
    return true;
  }
  case ISOP_REMESHING :
  {
    CMeshO::PerMeshAttributeHandle<IsoParametrization> isoPHandle =
        tri::Allocator<CMeshO>::FindPerMeshAttribute<IsoParametrization>(*mesh,"isoparametrization");
    
    bool b=tri::Allocator<CMeshO>::IsValidHandle<IsoParametrization>(*mesh,isoPHandle);
    if (!b)
    {
      this->errorMessage="You must compute the abstract mesh before remeshing. Use the Isoparametrization main filter.";
      return false;
    }
    
    
    int SamplingRate=par.getInt("SamplingRate");
    if (SamplingRate<2)
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
    mm->UpdateBoxAndNormals();
    return true;
  }
  case ISOP_DIAMPARAM :
  {
    CMeshO::PerMeshAttributeHandle<IsoParametrization> isoPHandle =
        tri::Allocator<CMeshO>::FindPerMeshAttribute<IsoParametrization>(*mesh,"isoparametrization");
    bool b=tri::Allocator<CMeshO>::IsValidHandle<IsoParametrization>(*mesh,isoPHandle);
    if (!b)
    {
      this->errorMessage="You must compute the abstract mesh before remeshing. Use the Isoparametrization main filter.";
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
    tri::UpdateNormal<CMeshO>::PerFace(*rem);
    return true;
  }
//  case ISOP_LOAD :
//  {
//    QString AbsName = par.getString("AbsName");
//    m->updateDataMask(MeshModel::MM_WEDGTEXCOORD);
//    m->updateDataMask(MeshModel::MM_VERTTEXCOORD);
//    m->updateDataMask(MeshModel::MM_FACECOLOR);
//    m->updateDataMask(MeshModel::MM_VERTQUALITY);
//    m->updateDataMask(MeshModel::MM_FACEMARK);
//    if(!QFile(m->fullName()).exists())
//    {
//      this->errorMessage="File not exists";
//      return false;
//    }
//    CMeshO::PerMeshAttributeHandle<IsoParametrization> isoPHandle =
//        tri::Allocator<CMeshO>::FindPerMeshAttribute<IsoParametrization>(*mesh,"isoparametrization");
    
//    bool b=tri::Allocator<CMeshO>::IsValidHandle<IsoParametrization>(*mesh,isoPHandle);
//    if (!b)
//      isoPHandle=tri::Allocator<CMeshO>::AddPerMeshAttribute<IsoParametrization>(*mesh,"isoparametrization");
    
//    QByteArray ba = AbsName.toLatin1();
//    char *path=ba.data();
//    AbstractMesh *abs_mesh = new AbstractMesh();
//    ParamMesh *para_mesh = new ParamMesh();
//    bool Done=isoPHandle().LoadBaseDomain<CMeshO>(path,mesh,para_mesh,abs_mesh,true);
//    if (!Done)
//    {
//      this->errorMessage="Abstract domain doesnt fit well with the parametrized mesh";
//      delete para_mesh;
//      delete abs_mesh;
//      return false;
//    }
//    return true;
//  }
//  case ISOP_SAVE :
//  {
//    m->updateDataMask(MeshModel::MM_VERTQUALITY);
//    CMeshO::PerMeshAttributeHandle<IsoParametrization> isoPHandle =
//        tri::Allocator<CMeshO>::FindPerMeshAttribute<IsoParametrization>(*mesh,"isoparametrization");
    
//    bool b=tri::Allocator<CMeshO>::IsValidHandle<IsoParametrization>(*mesh,isoPHandle);
//    if (!b)
//    {
//      this->errorMessage="You must compute the Base domain before remeshing. Use the Isoparametrization command.";
//      return false;
//    }
//    /*QString Qpath=m->fullName();*/
    
//    QString AbsName = par.getString("AbsName");
    
//    QByteArray ba = AbsName.toLatin1();
//    char *path=ba.data();
//    isoPHandle().SaveBaseDomain(path);
//    return true;
//  }
  case ISOP_TRANSFER:
  {
    MeshModel *mmtrg = par.getMesh("targetMesh");
    MeshModel *mmsrc = par.getMesh("targetMesh");
    CMeshO *trgMesh=&mmtrg->cm;
    CMeshO *srcMesh=&mmsrc->cm;
    
    CMeshO::PerMeshAttributeHandle<IsoParametrization> isoPHandle =
        tri::Allocator<CMeshO>::FindPerMeshAttribute<IsoParametrization>(*mesh,"isoparametrization");
    
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

MeshFilterInterface::FilterClass FilterIsoParametrization::getClass(QAction *)
{
  return MeshFilterInterface::Remeshing;
}

int FilterIsoParametrization::postCondition( QAction* /*filter*/ ) const
{
	return MeshModel::MM_WEDGTEXCOORD | MeshModel::MM_VERTTEXCOORD;
}

MeshFilterInterface::FILTER_ARITY FilterIsoParametrization::filterArity( QAction* filter) const
{
    switch(ID(filter))
    {
    case ISOP_PARAM :	
    case ISOP_REMESHING :
    case ISOP_DIAMPARAM :
        return MeshFilterInterface::SINGLE_MESH;
    case ISOP_TRANSFER:	
        return MeshFilterInterface::FIXED;
    }
    return MeshFilterInterface::NONE;
}


MESHLAB_PLUGIN_NAME_EXPORTER(FilterIsoParametrization)
