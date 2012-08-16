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
//#include <QTime>

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
    Plane3<CMeshO::ScalarType> fittingPlane = Plane3<CMeshO::ScalarType>();

    MeshModel *destMesh = md.addNewMesh("","MergedMesh");
    md.meshList.front();

    CMeshO tempMesh[2];

    foreach(MeshModel *mmp, md.meshList) {
        tri::UpdatePosition<CMeshO>::Matrix(mmp->cm, mmp->cm.Tr, true);
        tri::Append<CMeshO, CMeshO>::Mesh(tempMesh[0], mmp->cm);
        tri::UpdatePosition<CMeshO>::Matrix(mmp->cm, Inverse(mmp->cm.Tr), true);
    }
    tri::Allocator<CMeshO>::CompactVertexVector(tempMesh[0]);

    std::vector<CMeshO::CoordType> pointToFit = std::vector<CMeshO::CoordType>();
    std::vector<CVertexO*>::iterator it;

    int act;
    int next;
    for (int i = 0; i < par.getInt("numOfIterations"); i++) {
        act = i%2;
        next = (i+1)%2;

        tri::KNNGraph<CMeshO>::MakeKNNTree(tempMesh[act], par.getInt("numOfNeighbors"));

        CMeshO::PerVertexAttributeHandle<std::vector<CVertexO*>* > neighbors;
        neighbors = tri::Allocator<CMeshO>::GetPerVertexAttribute<std::vector<CVertexO*>* >(tempMesh[act], std::string("KNNGraph"));

        CMeshO::VertexIterator vertices = tri::Allocator<CMeshO>::AddVertices(tempMesh[next], tempMesh[act].VertexNumber());

        for (CMeshO::VertexIterator vi = tempMesh[act].vert.begin(); vi != tempMesh[act].vert.end(); vi++) {
            pointToFit.clear();

            for (it = neighbors[vi]->begin(); it != neighbors[vi]->end(); it++) {
                pointToFit.push_back((*it)->cP());
            }

            vcg::PlaneFittingPoints(pointToFit, fittingPlane);

            vertices->P() = fittingPlane.Projection(vi->cP());
            vertices->N() = vi->cN();
            vertices++;
        }

        tri::KNNGraph<CMeshO>::DeleteKNNTree(tempMesh[act]);
        tempMesh[act].Clear();
    }

    tri::Append<CMeshO, CMeshO>::Mesh(destMesh->cm, tempMesh[next]);

    long vertexCnt = 0;
    md.meshList.front();
    foreach(MeshModel *mmp, md.meshList) {
        if (mmp != destMesh) {
            tempMesh[act].Clear();
            tempMesh[next].Clear();

            tri::UpdatePosition<CMeshO>::Matrix(mmp->cm, mmp->cm.Tr, true);
            tri::Append<CMeshO, CMeshO>::Mesh(tempMesh[0], mmp->cm);

            for (int i = 0; i < par.getInt("numOfIterations"); i++) {
                act = i%2;
                next = (i+1)%2;

                tri::KNNGraph<CMeshO>::MakeKNNTree(tempMesh[act], par.getInt("numOfNeighbors"));

                CMeshO::PerVertexAttributeHandle<std::vector<CVertexO*>* > neighbors;
                neighbors = tri::Allocator<CMeshO>::GetPerVertexAttribute<std::vector<CVertexO*>* >(tempMesh[act], std::string("KNNGraph"));

                CMeshO::VertexIterator vertices = tri::Allocator<CMeshO>::AddVertices(tempMesh[next], tempMesh[act].VertexNumber());

                for (CMeshO::VertexIterator vi = tempMesh[act].vert.begin(); vi != tempMesh[act].vert.end(); vi++) {
                    pointToFit.clear();

                    for (it = neighbors[vi]->begin(); it != neighbors[vi]->end(); it++) {
                        pointToFit.push_back((*it)->cP());
                    }

                    vcg::PlaneFittingPoints(pointToFit, fittingPlane);

                    vertices->P() = fittingPlane.Projection(vi->cP());
                    vertices->N() = vi->cN();
                    vertices++;
                }

                tri::KNNGraph<CMeshO>::DeleteKNNTree(tempMesh[act]);
                tempMesh[act].Clear();
            }

            int localVertexCnt = 0;
            for (CMeshO::VertexIterator vi = tempMesh[next].vert.begin(); vi != tempMesh[next].vert.end(); vi++) {
                destMesh->cm.vert[vertexCnt].P() = destMesh->cm.vert[vertexCnt].P() + (vi->cP() - mmp->cm.vert[localVertexCnt].cP());

                vertexCnt++;
                localVertexCnt++;
            }
            tri::UpdatePosition<CMeshO>::Matrix(mmp->cm, Inverse(mmp->cm.Tr), true);
        }
    }

    return true;
}

FilterScanMergingPlugin::FilterClass FilterScanMergingPlugin::getClass(QAction *a)
{
    return MeshFilterInterface::Generic;
}

Q_EXPORT_PLUGIN(FilterScanMergingPlugin)
