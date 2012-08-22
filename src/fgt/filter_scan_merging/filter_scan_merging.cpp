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
   return QString("All the layers are merged into one mesh without loss of the high frequencies details.");
}

// This function define the needed parameters for each filter.
void FilterScanMergingPlugin::initParameterSet(QAction *action, MeshDocument &md, RichParameterSet &parlst)
{
    parlst.addParam(new RichInt("numOfNeighbors", 30, "Number of neighbors",
                                "The number of neighbors the projection operator uses to compute the projection plane."));
    parlst.addParam(new RichInt("numOfIterations", 4, "Number of iteration",
                                "How many times the projection operator has to be applied."));
}

// Core Function doing the actual mesh processing.
bool FilterScanMergingPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
    int nIterations = par.getInt("numOfIterations");
    int nNeighbors = par.getInt("numOfNeighbors");

    MeshModel *destMesh = md.addNewMesh("","MergedMesh");
    md.meshList.front();

    CMeshO tempMesh[2];

    /* Compute the low frequencies mesh taking into account the union of the complete set of layers */
    foreach(MeshModel *mmp, md.meshList) {
        tri::UpdatePosition<CMeshO>::Matrix(mmp->cm, mmp->cm.Tr, true);
        tri::Append<CMeshO, CMeshO>::Mesh(tempMesh[0], mmp->cm);
        tri::UpdatePosition<CMeshO>::Matrix(mmp->cm, Inverse(mmp->cm.Tr), true);
    }
    tri::Allocator<CMeshO>::CompactVertexVector(tempMesh[0]);

    toLowFrequecies(tempMesh, nIterations, nNeighbors);

    tri::Append<CMeshO, CMeshO>::Mesh(destMesh->cm, tempMesh[0]);

    /* Compute the high frequencies mesh taking into account one layer at time */
    long vertexCnt = 0;
    md.meshList.front();
    foreach(MeshModel *mmp, md.meshList) {
        if (mmp != destMesh) {
            tempMesh[0].Clear();
            tempMesh[1].Clear();

            tri::UpdatePosition<CMeshO>::Matrix(mmp->cm, mmp->cm.Tr, true);
            tri::Append<CMeshO, CMeshO>::Mesh(tempMesh[0], mmp->cm);

            toLowFrequecies(tempMesh, nIterations, nNeighbors);

            int localVertexCnt = 0;

            /* Correct the low frequencies mesh with high frequencies features */
            for (CMeshO::VertexIterator vi = tempMesh[0].vert.begin(); vi != tempMesh[0].vert.end(); vi++) {
                destMesh->cm.vert[vertexCnt].P() = destMesh->cm.vert[vertexCnt].P() + (vi->cP() - mmp->cm.vert[localVertexCnt].cP());

                vertexCnt++;
                localVertexCnt++;
            }
            tri::UpdatePosition<CMeshO>::Matrix(mmp->cm, Inverse(mmp->cm.Tr), true);
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
void FilterScanMergingPlugin::toLowFrequecies(CMeshO* m, int numOfIterations, int numOfNeighbors) {
    Plane3<CMeshO::ScalarType> fittingPlane = Plane3<CMeshO::ScalarType>();
    std::vector<CMeshO::CoordType> pointToFit = std::vector<CMeshO::CoordType>();
    std::vector<CVertexO*>::iterator it;
    int act;
    int next;

    for (int i = 0; i < numOfIterations; i++) {
        act = i%2;
        next = (i+1)%2;

        tri::KNNGraph<CMeshO>::MakeKNNTree(m[act], numOfNeighbors);

        CMeshO::PerVertexAttributeHandle<std::vector<CVertexO*>* > neighbors;
        neighbors = tri::Allocator<CMeshO>::GetPerVertexAttribute<std::vector<CVertexO*>* >(m[act], std::string("KNNGraph"));

        CMeshO::VertexIterator vertices = tri::Allocator<CMeshO>::AddVertices(m[next], m[act].VertexNumber());

        for (CMeshO::VertexIterator vi = m[act].vert.begin(); vi != m[act].vert.end(); vi++) {
            pointToFit.clear();

            for (it = neighbors[vi]->begin(); it != neighbors[vi]->end(); it++) {
                pointToFit.push_back((*it)->cP());
            }

            vcg::PlaneFittingPoints(pointToFit, fittingPlane);

            vertices->P() = fittingPlane.Projection(vi->cP());
            vertices->N() = vi->cN();
            vertices++;
        }

        tri::KNNGraph<CMeshO>::DeleteKNNTree(m[act]);
        m[act].Clear();
    }
    if (act == 1) return;
    else {
        tri::Append<CMeshO, CMeshO>::Mesh(m[0], m[1]);
        return;
    }
}

Q_EXPORT_PLUGIN(FilterScanMergingPlugin)
