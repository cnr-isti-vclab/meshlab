/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
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

//-------------------------------------------------------//
// File:  meshsegmentation.cpp                           //
//                                                       //
// Description: Mesh Segmentation Filter Source File     //
//                                                       //
// Authors: Anthousis Andreadis - http://anthousis.com   //
// Date: 7-Oct-2015                                      //
//                                                       //
// Computer Graphics Group                               //
// http://graphics.cs.aueb.gr/graphics/                  //
// AUEB - Athens University of Economics and Business    //
//                                                       //
//                                                       //
// This work was funded by the EU-FP7 - PRESIOUS project //
//-------------------------------------------------------//

#include "meshsegmentation.h"
#include "Segmenter.h"
#include "RG_Segmenter.h"
#include "HC_Segmenter.h"

using namespace std;
using namespace vcg;

float degreesToError(float angle) {
    return 1.f - cosf(angle*M_PI / 180.f);
}

CMeshO::PerFaceAttributeHandle<int> Segmenter::faceMarks = CMeshO::PerFaceAttributeHandle<int>();
CMeshO::PerFaceAttributeHandle<int> Segmenter::faceTmpMarks = CMeshO::PerFaceAttributeHandle<int>();
CMeshO::PerFaceAttributeHandle<Cluster *> Segmenter::faceClusterPairs = CMeshO::PerFaceAttributeHandle<Cluster *>();
CMeshO::PerFaceAttributeHandle<float> Segmenter::faceArea = CMeshO::PerFaceAttributeHandle<float>();
CMeshO::PerFaceAttributeHandle<Point3f> Segmenter::faceCentroid = CMeshO::PerFaceAttributeHandle<Point3f>();

SegmentationPlugin::SegmentationPlugin(void)
{
    typeList << FP_SEGMENTATION;

    FilterIDType tt;

    foreach(tt, types())
        actionList << new QAction(filterName(tt), this);
    

    seg_eThresholdDegrees = 15.f;
    //seg_eThreshold = degreesToError(seg_eThresholdDegrees);
    seg_areaThreshold = 2.f;
    seg_localNeighbAware = false;
    seg_localNeighbDist = 6.f;
    seg_hierarchical = false;
    seg_generateDecomp = false;
    seg_fifoRegionGrowing = false;
}

SegmentationPlugin::FilterClass SegmentationPlugin::getClass(QAction *action)
{
    switch (ID(action))
    {
    case FP_SEGMENTATION:       return FilterClass(MeshFilterInterface::FaceColoring + MeshFilterInterface::Remeshing);
    default                     : assert(0);
    }

    return MeshFilterInterface::Generic;
}

QString SegmentationPlugin::filterName(FilterIDType filter) const
{
    switch (filter)
    {
    case FP_SEGMENTATION      : return tr("Mesh Segmentation");
    default                   : assert(0);
    }

    return tr("error!");
}

QString SegmentationPlugin::filterInfo(FilterIDType filterID) const
{
    switch (filterID)
    {
	case FP_SEGMENTATION	  : return tr("Segment a mesh into distinct facets using a region-growing or a hierarchical "
                                          "bottom-up approach. The approach is tailored to distinguish the regions of a fragmented object "
                                          "that belong to the original surface from the fractured ones using the algorithms described in:<br><br>"
                                          "<b>A. Andreadis,  P. Mavridis and G. Papaioannou</b><br>"
                                          "<i>\"Facet Extraction and Classification for the Reassembly of Fractured 3D Objects\"</i><br>"
                                          "<a href=\"http://dx.doi.org/10.2312/egp.20141060\">DOI = 10.2312/egp.20141060</a> Eurographics (Posters) 2014<br>"
                                          "<br>"
                                          "The developement of this plugin was partially supported by the EC FP7 STREP project PRESIOUS, no. 600533.");
    default                   : assert(0);
    }

    return QString();
}

// this function builds and intializes with the default values (that can depend on the current mesh or selection)
// the list of parameters that a filter requires.
// return
//      true if has some parameters
//      false is has no params
void SegmentationPlugin::initParameterSet(QAction * action, MeshModel & m, RichParameterSet & parlst)
{
    switch(ID(action))
    {
    case FP_SEGMENTATION:
        // Here add the specific parameters for the segmentation
        parlst.addParam(new RichBool  ( "Hierarchical",
                                        seg_hierarchical,
                                        "Hierarchical Bottom-Up",
                                        "Instead of the region-growing perform an agglomerative process for the segmentation.\n"
                                        "This is more exhaustive and much slower approach, especially if Local Neighborhood aware metric is used."));
        parlst.addParam(new RichBool  ( "RgFIFO",
                                        seg_fifoRegionGrowing,
                                        "FIFO Region Growing",
                                        "Only for the Region Growing approach. The next face that is picked and tested for inclusion on a segment\n"
                                        "is picked in FIFO order. Otherwise the decision is based on Best-Candidate (FIFO is much faster but more unstable)"));
        parlst.addParam(new RichFloat ( "ErrorThreshold", 
                                        seg_eThresholdDegrees, 
                                        "Error threshold (angle in degrees) (0..90)", 
                                        "This is the threshold angle based on which a vertex is merged or not on a segment."));
        parlst.addParam(new RichFloat ( "AreaThreshold", 
                                        seg_areaThreshold, 
                                        "Area ratio (%) threshold for the generated Segments", 
                                        "Segments with lower area ratio than the specified limit, will be decomposed."));
        parlst.addParam(new RichBool  ( "LocalNeighbAware", 
                                        seg_localNeighbAware, 
                                        "Local Neighborhood Aware", 
                                        "The decision for the inclusion of each vertex to a segment\nis based on a local neighborhood aware metric. Slower but handles curved surfaces."));
        parlst.addParam(new RichBool  ( "GenerateDecomposition", 
                                        seg_generateDecomp, 
                                        "Generate Decomposition", 
                                        "If true a new layer is generated for each segment, otherwise the original mesh is colored according to segmentation and cluster id is written into face quality."));
        parlst.addParam(new RichAbsPerc("LocalNeighbDist", 
                                        m.cm.bbox.Diag()/50.0, 0,m.cm.bbox.Diag(),
                                        "Distance in units for the local neighborhood", 
                                        "This is used only when the Local Neighborhood Aware metric is enabled."));
        break;
    default:
        break;
    }
}

int SegmentationPlugin::getRequirements(QAction *action)
{    return 0; }

bool SegmentationPlugin::applyFilter(QAction * filter, MeshDocument & md, RichParameterSet & par, vcg::CallBackPos * cb)
{
    MeshModel & m = *md.mm();
    md.mm()->updateDataMask(MeshModel::MM_VERTFACETOPO);
    md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);
    md.mm()->visible = false;
    switch (ID(filter))
    {
    case FP_SEGMENTATION:
    {
        seg_hierarchical       = par.getBool("Hierarchical");
        seg_fifoRegionGrowing  = par.getBool("RgFIFO");
        seg_eThresholdDegrees  = par.getFloat("ErrorThreshold");
        seg_areaThreshold      = par.getFloat("AreaThreshold");
        seg_localNeighbAware   = par.getBool("LocalNeighbAware");
        seg_generateDecomp     = par.getBool("GenerateDecomposition");
        seg_localNeighbDist    = par.getFloat("LocalNeighbDist");

        Segmenter *segmenter;
        if (!seg_hierarchical) {
            segmenter = new RG_Segmenter(cb);
            if (seg_fifoRegionGrowing) {
                ((RG_Segmenter *)segmenter)->regionGrowindMethod(RG_Segmenter::FIFO);
                if (seg_localNeighbAware) { ((RG_Segmenter *)segmenter)->setMetric(RG_Segmenter::local_boundary_norm); }
                else                      { ((RG_Segmenter *)segmenter)->setMetric(RG_Segmenter::orientation_norm); }
            }
            else {
                if (seg_localNeighbAware) { ((RG_Segmenter *)segmenter)->regionGrowindMethod(RG_Segmenter::BF_LOCAL_NEIGHBORS); }
                else                      { ((RG_Segmenter *)segmenter)->regionGrowindMethod(RG_Segmenter::BEST_FIRST); }
            }       
        }
        else {
            segmenter = new HC_Segmenter(cb);
            if (seg_localNeighbAware) { ((HC_Segmenter *)segmenter)->setMetric(HC_Segmenter::local_boundary_norm); }
            else                      { ((HC_Segmenter *)segmenter)->setMetric(HC_Segmenter::orientation_norm); }
        }
        
        segmenter->setMesh(&m.cm);
        segmenter->setWeightThreshold(degreesToError(seg_eThresholdDegrees));
        segmenter->setLnDistance(seg_localNeighbDist);

        clock_t start = clock();
        segmenter->createClusters();
        clock_t end = clock();
        Log("Segmentation finished (%f sec). Number of segments %i", (float)(end - start) / (float)CLOCKS_PER_SEC, segmenter->getNumClusters());
        start = clock(); 
        segmenter->postProcessClusters(seg_areaThreshold);
        end = clock();
        Log("Post-Processing finished (%f sec). Number of segments %i", (float)(end - start) / (float)CLOCKS_PER_SEC, segmenter->getNumClusters());

        if( seg_generateDecomp )
        {
        if (cb) { cb(100, "Creating Mesh Layers"); }        
        // Now copy each cluster to a new mesh object
        for (size_t i = 0; i < segmenter->getNumClusters(); ++i) {
            char name[256];
            sprintf(name,"Segment_%d",(int)i);
            MeshModel &pm = *md.addNewMesh("", name);

            tri::Allocator<CMeshO>::AddVertices(pm.cm, segmenter->clusterList_[i]->faces_.size()*3);
            tri::Allocator<CMeshO>::AddFaces(pm.cm, segmenter->clusterList_[i]->faces_.size());

            for (size_t f_i = 0; f_i < segmenter->clusterList_[i]->faces_.size(); ++f_i){
                CMeshO::FacePointer fPointer = segmenter->clusterList_[i]->faces_[f_i];
                pm.cm.vert[f_i*3 + 0].P() = fPointer->cP(0);
                pm.cm.face[f_i].V(0) = &(pm.cm.vert[f_i * 3 + 0]);

                pm.cm.vert[f_i*3 + 1].P() = fPointer->cP(1);
                pm.cm.face[f_i].V(1) = &(pm.cm.vert[f_i * 3 + 1]);

                pm.cm.vert[f_i*3 + 2].P() = fPointer->cP(2);
                pm.cm.face[f_i].V(2) = &(pm.cm.vert[f_i * 3 + 2]);
            }
            tri::Clean<CMeshO>::RemoveDuplicateVertex(pm.cm);
            tri::Allocator<CMeshO>::CompactEveryVector(pm.cm);
            pm.updateDataMask(MeshModel::MM_FACECOLOR);
            tri::UpdateColor<CMeshO>::PerFaceConstant(pm.cm, Color4b::Scatter( segmenter->getNumClusters(),i), false);  //calls the function that does the real job
            pm.UpdateBoxAndNormals();
        }
        }
        else
        {
          for (size_t i = 0; i < segmenter->getNumClusters(); ++i) {
            for (size_t f_i = 0; f_i < segmenter->clusterList_[i]->faces_.size(); ++f_i){
              segmenter->clusterList_[i]->faces_[f_i]->C()=Color4b::Scatter( segmenter->getNumClusters(),i); 
            }
          }
        }
    } break;
    }
    return true;
}

int SegmentationPlugin::postCondition(QAction * filter) const
{
    switch (ID(filter))
    {
    case FP_SEGMENTATION:  return MeshModel::MM_FACECOLOR;
    default: return MeshModel::MM_UNKNOWN;
    }
}

MESHLAB_PLUGIN_NAME_EXPORTER(SegmentationPlugin)
