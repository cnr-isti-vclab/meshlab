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

#include "filter_voronoi.h"
#include<vcg/complex/algorithms/voronoi_processing.h>
#include<vcg/complex/algorithms/update/curvature.h>
#include<vcg/complex/algorithms/update/quality.h>
#include<vcg/complex/algorithms/smooth.h>
#include<vcg/complex/algorithms/voronoi_volume_sampling.h>
#include<vcg/complex/algorithms/polygon_support.h>

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly
using namespace vcg;
using namespace std;
namespace vcg {
namespace tri {

template <class MeshType>
void BuildExtrudedFaceShell(MeshType &mIn, MeshType &mOut, float height=0, float inset=0, bool smoothFlag=true  )
{
  typedef typename MeshType::VertexPointer VertexPointer;
  typedef typename MeshType::CoordType CoordType;
  if(height==0) height = mIn.bbox.Diag()/100.0f;
  if(inset==0) inset = mIn.bbox.Diag()/100.0f;
  tri::UpdateFlags<MeshType>::FaceClearV(mIn);
  for(size_t i=0;i<mIn.face.size();++i) if(!mIn.face[i].IsV())
  {
    _SphMesh faceM;
    CoordType bary = Barycenter(mIn.face[i]);
    CoordType nf = mIn.face[i].N().Normalize();
    nf = nf*height/2.0f;
    std::vector<VertexPointer> vertVec;
    tri::PolygonSupport<MeshType,MeshType>::ExtractPolygon(&(mIn.face[i]),vertVec);
    size_t vn = vertVec.size();
    // Add vertices (alternated top and bottom)
    tri::Allocator<_SphMesh>::AddVertex(faceM, bary-nf);
    tri::Allocator<_SphMesh>::AddVertex(faceM, bary+nf);
    for(size_t j=0;j<vn;++j){
      tri::Allocator<_SphMesh>::AddVertex(faceM, vertVec[j]->P()-nf);
      tri::Allocator<_SphMesh>::AddVertex(faceM, vertVec[j]->P()+nf);
    }

    // Build top and bottom faces
    for(size_t j=0;j<vn;++j)
      tri::Allocator<_SphMesh>::AddFace(faceM, 0, 2+(j+0)*2, 2+((j+1)%vn)*2 );
    for(size_t j=0;j<vn;++j)
      tri::Allocator<_SphMesh>::AddFace(faceM, 1, 3+(j+0)*2, 3+((j+1)%vn)*2 );

    // Build side strip
    for(size_t j=0;j<vn;++j){
      size_t j0=j;
      size_t j1=(j+1)%vn;
      tri::Allocator<_SphMesh>::AddFace(faceM, 2+ j0*2 + 0 , 2+ j0*2+1, 2+j1*2+0);
      tri::Allocator<_SphMesh>::AddFace(faceM, 2+ j0*2 + 1 , 2+ j1*2+1, 2+j1*2+0);
    }

    for(size_t j=0;j<2*vn;++j)
      faceM.face[j].SetS();

    tri::UpdateTopology<_SphMesh>::FaceFace(faceM);
    tri::UpdateFlags<_SphMesh>::FaceBorderFromFF(faceM);
    tri::Refine(faceM, MidPoint<_SphMesh>(&faceM),0,true);
    tri::Refine(faceM, MidPoint<_SphMesh>(&faceM),0,true);

    tri::Append<MeshType,_SphMesh>::Mesh(mOut,faceM);

  } // end main loop for each face;
}


};
};

FilterVoronoiPlugin::FilterVoronoiPlugin()
{
}

bool FilterVoronoiPlugin::applyFilter( const QString& filterName,MeshDocument& md,EnvWrap& env, vcg::CallBackPos* cb)
{
  if(filterName == "Cross Field Coloring")
  {
    MeshModel &m=*md.mm();
    m.updateDataMask(MeshModel::MM_VERTCOLOR);
    m.updateDataMask(MeshModel::MM_VERTQUALITY);
    for(CMeshO::VertexIterator vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
    {
      vi->Q() = Norm(vi->PD1()) / Norm(vi->PD2());
    }

    tri::UpdateColor<CMeshO>::PerVertexQualityRamp(m.cm);
    return true;
  }
  if(filterName == "Cross Field Creation")
  {
    MeshModel &m=*md.mm();
    m.updateDataMask(MeshModel::MM_VERTCURVDIR);

    int crossType = env.evalEnum("crossType");
    if(crossType == 0) // Linear Y
    {
      float range = m.cm.bbox.DimY();
      for(int i=0;i<m.cm.vert.size();++i){
        float q01 = .25f+(m.cm.vert[i].P().Y() - m.cm.bbox.min.Y())/(2.0f*range);
        m.cm.vert[i].PD1()=Point3f(1,0,0)*q01;
        m.cm.vert[i].PD2()=Point3f(0,1,0)*(sqrt(1-q01*q01));
      }
    }

    if(crossType == 1) // Radial
    {
      tri::UpdateCurvature<CMeshO>::PerVertexBasicRadialCrossField(m.cm,2.0);
    }

    if(crossType == 2) // Curvature
    {
    }
    return true;
  }

  if(filterName == "Voronoi Sampling")
  {
    MeshModel *om=md.addOrGetMesh("voro", "voro",false);
    MeshModel *poly=md.addOrGetMesh("poly", "poly",false);

    om->updateDataMask(MeshModel::MM_VERTCOLOR);

    const int  sampleNum =env.evalInt("sampleNum");
    const int  iterNum =  env.evalInt("iterNum");
    const float radiusVariance = env.evalFloat("radiusVariance");
    const int distanceType=env.evalEnum("distanceType");

    MeshModel &m=*md.mm();

    m.updateDataMask(MeshModel::MM_FACEFACETOPO);
    m.updateDataMask(MeshModel::MM_VERTFACETOPO);
    m.updateDataMask(MeshModel::MM_VERTCURV);
    m.updateDataMask(MeshModel::MM_VERTCURVDIR);
    m.updateDataMask(MeshModel::MM_VERTCOLOR);
    tri::UpdateCurvature<CMeshO>::PerVertexBasicRadialCrossField(m.cm);

//   tri::EuclideanDistance<CMeshO> dd;
   vector<CVertexO *> seedVec;
//   tri::ClusteringSampler<CMeshO> cs(seedVec);
//   tri::SurfaceSampling<CMeshO, vcg::tri::ClusteringSampler<CMeshO> >::SamplingRandomGenerator().initialize(randSeed);
   vector<Point3f> pointVec;
   float radius=0;
   tri::PoissonSampling<CMeshO>(m.cm,pointVec,sampleNum,radius,radiusVariance);

   tri::VoronoiProcessingParameter vpp;
   vpp.geodesicRelaxFlag = (env.evalEnum("relaxType")==0);
   vpp.colorStrategy = env.evalEnum("colorStrategy");
   vpp.deleteUnreachedRegionFlag=true;
   vpp.refinementRatio=10;

   if(env.evalBool("preprocessFlag"))
   {
     tri::VoronoiProcessing<CMeshO>::PreprocessForVoronoi(m.cm,radius,vpp);
   }
   tri::VoronoiProcessing<CMeshO>::SeedToVertexConversion(m.cm,pointVec,seedVec);

   QList<int> meshlist; meshlist << m.id();

   if(distanceType==0) // Uniform Euclidean Distance
   {
     EuclideanDistance<CMeshO> dd;
     for(int i=0;i<iterNum;++i)
     {
       cb(100*i/iterNum, "Relaxing...");
       tri::VoronoiProcessing<CMeshO, EuclideanDistance<CMeshO> >::VoronoiRelaxing(m.cm, seedVec, 1,dd,vpp);
       md.updateRenderStateMeshes(meshlist,int(MeshModel::MM_VERTCOLOR));
       if (intteruptreq) return true;
     }
//     tri::VoronoiProcessing<CMeshO>::ConvertVoronoiDiagramToMesh(m.cm,om->cm,poly->cm,seedVec, vpp);
   }

   if(distanceType==1)
   {
     IsotropicDistance<CMeshO> id(m.cm,radiusVariance);
     for(int i=0;i<iterNum;++i)
     {
       cb(100*i/iterNum, "Relaxing...");
       tri::VoronoiProcessing<CMeshO, IsotropicDistance<CMeshO> >::VoronoiRelaxing(m.cm, seedVec, 1,id,vpp);
       md.updateRenderStateMeshes(meshlist,int(MeshModel::MM_VERTCOLOR));
       if (intteruptreq) return true;
     }
//     tri::VoronoiProcessing<CMeshO>::ConvertVoronoiDiagramToMesh(m.cm,om->cm,poly->cm,seedVec, vpp);
   }
   if(distanceType==2)
   {
     for(int i=0;i<iterNum;++i)
     {
       cb(100*i/iterNum, "Relaxing...");
       BasicCrossFunctor<CMeshO> bcf(m.cm);
       AnisotropicDistance<CMeshO> ad(m.cm,bcf);
       tri::VoronoiProcessing<CMeshO, AnisotropicDistance<CMeshO> >::VoronoiRelaxing(m.cm, seedVec, 1, ad, vpp);
       md.updateRenderStateMeshes(meshlist,int(MeshModel::MM_VERTCOLOR));
       if (intteruptreq) return true;
     }
     //      tri::VoronoiProcessing<CMeshO, AnisotropicDistance<CMeshO> >::ConvertVoronoiDiagramToMesh(m.cm,om->cm,seedVec, ad, vpp);
   }

   md.updateRenderStateMeshes(meshlist,int(MeshModel::MM_VERTCOLOR));
   if (intteruptreq) return true;

   om->UpdateBoxAndNormals();
   return true;
  }

  if(filterName=="Volumetric Sampling") /******************************************************/
  {
    MeshModel *m= md.mm();
    m->updateDataMask(MeshModel::MM_FACEMARK);

    float sampleSurfRadius = env.evalFloat("sampleSurfRadius");
    float poissonRadius = env.evalFloat("poissonRadius");
    int sampleVolNum = env.evalInt("sampleVolNum");
    int poissonFlag = env.evalBool("poissonFiltering");

    MeshModel *mcVm= md.addOrGetMesh("Montecarlo Volume","Montecarlo Volume",false,RenderMode(GLW::DMPoints));
    MeshModel  *pVm= md.addOrGetMesh("Poisson Sampling","Poisson Sampling",false,RenderMode(GLW::DMPoints));
    MeshModel  *pSm= md.addOrGetMesh("Surface Sampling","Surface Sampling",false,RenderMode(GLW::DMPoints));
    mcVm->updateDataMask(MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY);
    pSm->updateDataMask(MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY);
    VoronoiVolumeSampling<CMeshO> vvs(m->cm, pVm->cm);
    Log("Sampling Surface at a radius %f ",sampleSurfRadius);
    cb(1, "Init");
    vvs.Init(sampleSurfRadius);
    cb(30, "Sampling Volume...");
    vvs.BuildVolumeSampling(sampleVolNum,0,poissonRadius,cb);
    tri::Append<CMeshO,CMeshO>::MeshCopy(mcVm->cm,vvs.montecarloVolumeMesh);
    tri::UpdateColor<CMeshO>::PerVertexQualityRamp(mcVm->cm);
    vvs.ThicknessEvaluator();
    tri::Append<CMeshO,CMeshO>::MeshCopy(pSm->cm,vvs.poissonSurfaceMesh);
    return true;
  }
  if(filterName=="Voronoi Scaffolding") /******************************************************/
  {
    MeshModel *m= md.mm();
    m->updateDataMask(MeshModel::MM_FACEMARK);
    MeshModel   *pm= md.addOrGetMesh("Poisson-disk Samples","Poisson-disk Samples",false,RenderMode(GLW::DMPoints));
    MeshModel *mcVm= md.addOrGetMesh("Montecarlo Volume","Montecarlo Volume",false,RenderMode(GLW::DMPoints));
    MeshModel  *vsm= md.addOrGetMesh("Voronoi Seeds","Voronoi Seeds",false,RenderMode(GLW::DMPoints));
    MeshModel   *sm= md.addOrGetMesh("Scaffolding","Scaffolding",false,RenderMode(GLW::DMFlat));

    pm->updateDataMask(m);
    cb(10, "Sampling Surface...");
    float sampleSurfRadius = env.evalFloat("sampleSurfRadius");
    int sampleVolNum = env.evalInt("sampleVolNum");
    int voronoiSeed = env.evalInt("voronoiSeed");
    int voxelRes = env.evalInt("voxelRes");
    float isoThr = env.evalFloat("isoThr");
    int smoothStep = env.evalInt("smoothStep");
    int relaxStep = env.evalInt("relaxStep");
    int surfFlag = env.evalBool("surfFlag");
    int elemType = env.evalEnum("elemType");

    VoronoiVolumeSampling<CMeshO> vvs(m->cm,vsm->cm);
    Log("Sampling Surface at a radius %f ",sampleSurfRadius);
    vvs.Init(sampleSurfRadius);
    cb(30, "Sampling Volume...");
    float poissonVolumeRadius=0;
    vvs.BuildVolumeSampling(sampleVolNum,voronoiSeed,poissonVolumeRadius);
    Log("Base Poisson volume sampling at a radius %f ",poissonVolumeRadius);

    cb(40, "Relaxing Volume...");
    vvs.RelaxVoronoiSamples(relaxStep);

    cb(50, "Building Scaffloding Volume...");
    vvs.BuildScaffoldingMesh(sm->cm,voxelRes,isoThr,elemType,surfFlag);
    cb(90, "Final Smoothing...");
    tri::Smooth<CMeshO>::VertexCoordLaplacian(sm->cm, smoothStep);
    sm->UpdateBoxAndNormals();
   tri::Append<CMeshO,CMeshO>::MeshCopy(mcVm->cm,vvs.montecarloVolumeMesh);
   tri::Append<CMeshO,CMeshO>::MeshCopy(pm->cm,vvs.poissonSurfaceMesh);
    return true;
  }

  if(filterName=="Create Solid Wireframe") /******************************************************/
  {
    MeshModel *m= md.mm();
    m->updateDataMask(MeshModel::MM_FACEFACETOPO);
    MeshModel *sm= md.addOrGetMesh("Shell Mesh","Shell Mesh",false,RenderMode(GLW::DMFlat));
    float edgeCylRadius = env.evalFloat("edgeCylRadius");
    float vertCylRadius = env.evalFloat("vertCylRadius");
    float vertSphRadius = env.evalFloat("vertSphRadius");
    float faceExtHeight = env.evalFloat("faceExtHeight");
    bool edgeCylFlag = env.evalBool("edgeCylFlag");
    bool vertCylFlag = env.evalBool("vertCylFlag");
    bool vertSphFlag = env.evalBool("vertSphFlag");
    bool faceExtFlag = env.evalBool("faceExtFlag");

    sm->cm.Clear();
    sm->updateDataMask(MeshModel::MM_FACEFACETOPO);
    tri::RequireFFAdjacency(sm->cm);
    tri::UpdateNormal<CMeshO>::PerVertexNormalizedPerFace(m->cm);
    tri::Clean<CMeshO>::RemoveUnreferencedVertex(m->cm);
    tri::Allocator<CMeshO>::CompactEveryVector(m->cm);
    if(edgeCylFlag) tri::BuildCylinderEdgeShell(m->cm,sm->cm,edgeCylRadius);
    if(vertCylFlag) tri::BuildCylinderVertexShell(m->cm,sm->cm,vertCylRadius,edgeCylRadius);
    if(vertSphFlag) tri::BuildSphereVertexShell(m->cm,sm->cm,vertSphRadius);
    if(faceExtFlag) tri::BuildExtrudedFaceShell(m->cm,sm->cm,faceExtHeight);

    sm->UpdateBoxAndNormals();
     return true;
}
  return false;
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterVoronoiPlugin)
