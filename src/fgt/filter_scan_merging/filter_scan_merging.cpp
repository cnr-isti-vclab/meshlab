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

#include <QtGui>

#include <meshlabplugins/edit_point/knnGraph.h>
#include <vcg/space/fitting3.h>
#include <vcg/complex/append.h>

#include "filter_scan_merging.h"

using namespace std;
using namespace vcg;

// Constructor
FilterScanMergingPlugin::FilterScanMergingPlugin()
{
    typeList << FSM_MERGELAYERS;

    foreach(FilterIDType tt , types())
            actionList << new QAction(filterName(tt), this);
}

// ST() return the very short string describing each filtering action
QString FilterScanMergingPlugin::filterName(FilterIDType filterId) const
{
   return QString("High Fidelity Scan Merging");
}

// Info() return the longer string describing each filtering action
QString FilterScanMergingPlugin::filterInfo(FilterIDType filterId) const
{
   return QString("Merge all the selected layers into one new mesh.<br>"
                  "The idea is to preserve point positions in non overlapping areas, and to make a fusion of the "
                  "scans on overlapping regions while keeping all raw points.<br>"
                  "See:<br\>"
                  "<b>High Fidelity Scan Merging</b><br>"
                  "<i>J. Digne, J. M. Morel, N. Audfray and C. Lartigue</i><br>"
                  "Eurographics Symposium on Geometry Processing 2010, CGF Vol 29(5), 2010");
}

// This function define the needed parameters for each filter.
void FilterScanMergingPlugin::initParameterSet(QAction *action, MeshDocument &md, RichParameterSet &parlst)
{
  parlst.addParam(new RichInt("numOfNeighbors", 30, "Number of neighbors",
                              "The number of neighbors the projection operator uses to compute the projection plane."));
  parlst.addParam(new RichInt("numOfIterations", 4, "Number of iteration",
                              "How many times the projection operator has to be applied."));
  parlst.addParam(new RichBool("modifyOriginalMesh",false, "Modify original meshes",
                               "If true also the original mesh vertexes are displaced according to the computed vector."
                               "Useful if you want to keep the result as separated entities."));
}

// Core Function doing the actual mesh processing.
bool FilterScanMergingPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
    bool modifyOriginalMeshFlag = par.getBool("modifyOriginalMesh");
    int nIterations = par.getInt("numOfIterations");
    int nNeighbors = par.getInt("numOfNeighbors");
    int nOp = 0;

    MeshModel *destMesh = md.addNewMesh("","MergedMesh");

    //CMeshO tempMesh[2];

    std::vector<Point3f> vertices[2];

    /* Compute the low frequencies mesh taking into account the union of the complete set of layers */
    cb(0, "High Fidelity Scan Merging: merging visible layers");
    foreach(MeshModel *mmp, md.meshList) {
        if (mmp->visible) {
            tri::Allocator<CMeshO>::CompactVertexVector(mmp->cm);
            tri::UpdatePosition<CMeshO>::Matrix(mmp->cm, mmp->cm.Tr, true);
            for (CMeshO::VertexIterator vi = mmp->cm.vert.begin(); vi != mmp->cm.vert.end(); vi++) {
                vertices[0].push_back(vi->cP());
            }
//            tri::Append<CMeshO, CMeshO>::Mesh(tempMesh[0], mmp->cm);
            tri::UpdatePosition<CMeshO>::Matrix(mmp->cm, Inverse(mmp->cm.Tr), true);
            nOp++;
        }
    }
    vertices[1].resize(vertices[0].size());
//    tri::Allocator<CMeshO>::CompactVertexVector(tempMesh[0]);
    nOp += 2;

    cb(100/nOp, "High Fidelity Scan Merging: computing low frequencies base");

    toLowFreq(vertices, nIterations, nNeighbors);
//    toLowFreq(tempMesh, nIterations, nNeighbors);

    tri::Allocator<CMeshO>::AddVertices(destMesh->cm, vertices[0].size());
    for (int i = 0; i < vertices[0].size(); i++) {
        destMesh->cm.vert[i].P() = vertices[0][i];
    }
//    tri::Append<CMeshO, CMeshO>::Mesh(destMesh->cm, tempMesh[0]);

    /* Compute the high frequencies mesh taking into account one layer at time */
    long vertexCnt = 0;
    int nMesh = 0;
    foreach(MeshModel *mmp, md.meshList) {
        if (mmp != destMesh && mmp->visible) {
            cb((nMesh+2)*100 / nOp, "High Fidelity Scan Merging: computing high frequencies");

            vertices[0].clear();
            vertices[1].clear();
//            tempMesh[0].Clear();
//            tempMesh[1].Clear();

            tri::UpdatePosition<CMeshO>::Matrix(mmp->cm, mmp->cm.Tr, true);
//            tri::Append<CMeshO, CMeshO>::Mesh(tempMesh[0], mmp->cm);
            for (CMeshO::VertexIterator vi = mmp->cm.vert.begin(); vi != mmp->cm.vert.end(); vi++) {
                vertices[0].push_back(vi->cP());
            }
            vertices[1].resize(vertices[0].size());

//            toLowFreq(tempMesh, nIterations, nNeighbors);
            toLowFreq(vertices, nIterations, nNeighbors);

            int localVertexCnt = 0;

            /* Correct the low frequencies mesh with high frequencies features */
            for (int i = 0; i < vertices[0].size(); i++) {
                destMesh->cm.vert[vertexCnt].P() = destMesh->cm.vert[vertexCnt].P() - (vertices[0][i] - mmp->cm.vert[localVertexCnt].cP());
                destMesh->cm.vert[vertexCnt].N() = mmp->cm.vert[localVertexCnt].N();
                if(modifyOriginalMeshFlag)
                  mmp->cm.vert[localVertexCnt].P() =  destMesh->cm.vert[vertexCnt].P();
                vertexCnt++;
                localVertexCnt++;
            }
//            for (CMeshO::VertexIterator vi = tempMesh[0].vert.begin(); vi != tempMesh[0].vert.end(); vi++) {
//                destMesh->cm.vert[vertexCnt].P() = destMesh->cm.vert[vertexCnt].P() - (vi->cP() - mmp->cm.vert[localVertexCnt].cP());
//                if(modifyOriginalMeshFlag)
//                  mmp->cm.vert[localVertexCnt].P() =  destMesh->cm.vert[vertexCnt].P();
//                vertexCnt++;
//                localVertexCnt++;
//            }
            tri::UpdatePosition<CMeshO>::Matrix(mmp->cm, Inverse(mmp->cm.Tr), true);

            nMesh++;
        }
    }
    tri::UpdateBounding<CMeshO>::Box(destMesh->cm);
    return true;
}

FilterScanMergingPlugin::FilterClass FilterScanMergingPlugin::getClass(QAction *a)
{
    return MeshFilterInterface::Layer;
}

/**
  This function computes the low frequencies mesh iterating the projection operator numOfIterations times.
  m has to be an array of 2 elements, the first contains the mesh to be processed, the second must be empty.
  For each vertex the projection operator needs numOfNeighbors neighbors to find the fitting plane.
  Once the function returns the result is in m[0]
  **/
void FilterScanMergingPlugin::toLowFreq(vector<Point3f>* vert, int numOfIterations, int numOfNeighbors) {
    Plane3<CMeshO::ScalarType> fittingPlane;
    std::vector<CMeshO::CoordType> pointToFit;
//    std::vector<CVertexO*>::iterator it;
    int act;
    int next;

    for (int i = 0; i < numOfIterations; i++) {
        act = i%2;
        next = (i+1)%2;

//        VertexConstDataWrapper<CMeshO> ww(m[act]);
        ConstDataWrapper<CMeshO::CoordType> ww(&(vert[act][0]), vert[act].size(), sizeof(Point3f));
        KdTree<float> tree(ww);
        tree.setMaxNofNeighbors(numOfNeighbors);

//        CMeshO::VertexIterator vertices = tri::Allocator<CMeshO>::AddVertices(m[next], m[act].VertexNumber());

        //perror("iteration\n");
        for (int j = 0; j < vert[act].size(); j++) {
            tree.doQueryK(vert[act][j]);
            int neighbours = tree.getNofFoundNeighbors();
            pointToFit.clear();
            for (int k = 0; k < neighbours; k++) {
                int neightId = tree.getNeighborId(k);
                pointToFit.push_back(vert[act][neightId]);
            }
            vcg::FitPlaneToPointSet(pointToFit, fittingPlane);
            vert[next][j] = fittingPlane.Projection(vert[act][j]);
        }
        //perror("iteration end\n");

//        for (CMeshO::VertexIterator vi = m[act].vert.begin(); vi != m[act].vert.end(); vi++) {
//            tree.doQueryK( vi->cP() );
//            int neighbours = tree.getNofFoundNeighbors();
//            pointToFit.clear();
//            for (int j = 0; j < neighbours; j++) {
//                int neightId = tree.getNeighborId(j);
//                pointToFit.push_back(m[act].vert[neightId].cP() );
//            }
//            vcg::FitPlaneToPointSet(pointToFit, fittingPlane);
//            vertices->P() = fittingPlane.Projection(vi->cP());
//            vertices->N() = vi->cN();
//            vertices++;
//        }
//        m[act].Clear();
    }
    if (act == 1) return;
    else {
        for (int i = 0; i < vert[0].size(); i++)
            vert[0][i] = vert[1][i];
//        tri::Append<CMeshO, CMeshO>::Mesh(m[0], m[1]);
        return;
    }
}

Q_EXPORT_PLUGIN(FilterScanMergingPlugin)
