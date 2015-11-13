//-------------------------------------------------------//
// File:  Utils.h                                        //
//                                                       //
// Description: Utils Source File                        //
//                                                       //
// Authors: Anthousis Andreadis - http://anthousis.com   //
//          Pavlos Mavridis     - http://pmavridis.com   //
// Date: 7-Oct-2015                                      //
//                                                       //
// Computer Graphics Group                               //
// http://graphics.cs.aueb.gr/graphics/                  //
// AUEB - Athens University of Economics and Business    //
//                                                       //
//                                                       //
// This work was funded by the EU-FP7 - PRESIOUS project //
//-------------------------------------------------------//

#include "Cluster.h"
#include "Segmenter.h"

#include <common/interfaces.h>

void runOperatorAtFace(CMeshO::FacePointer f_it, int faceID, int radius, MeshFaceFunctor* F, float euclidian, Cluster* centralSegment) {
    //get the 1-k neighbors
    Segmenter::faceTmpMarks[*f_it] = faceID;
    vcg::Point3f center;

    float thresholdDistance = euclidian * euclidian;

    //initialize processing on the center of the k-ring
    F->initCenter(f_it);

    if (euclidian > 0) { center = Segmenter::faceCentroid[*f_it]; }

    if (radius == 0){
        F->saveResults(f_it);
        return; //just to speed-up the special case
    }

    std::vector<std::pair<int, CMeshO::FacePointer> > localstack;
    localstack.push_back(std::make_pair(0, f_it));

    while (!localstack.empty()){
        std::pair<int, CMeshO::FacePointer> top = localstack.back();
        localstack.pop_back();
        int level = top.first;

        // For all face neighbors
        for (size_t i = 0; i < 3; ++i) {
            CMeshO::FacePointer nFacePointer = (*top.second).FFp(i);
            // Sanity checks
            if (!nFacePointer || nFacePointer == f_it || nFacePointer->IsD()) { continue; }

            //check if we have already visited this one
            int mark = Segmenter::faceTmpMarks[*nFacePointer];
            if (mark == faceID)
                continue;

            //mark this node as visited
            Segmenter::faceTmpMarks[*nFacePointer] = faceID;

            if (centralSegment != NULL){
                Cluster* neighborSegment = Segmenter::faceClusterPairs[*nFacePointer];;
                if (neighborSegment != centralSegment)
                    continue;
            }

            if (euclidian > 0.0){
                vcg::Point3f &inner_center = Segmenter::faceCentroid[*nFacePointer];
                if ((inner_center - center).SquaredNorm() < thresholdDistance){
                    std::pair<int, CMeshO::FacePointer> new_node = std::make_pair(level + 1, nFacePointer);
                    localstack.push_back(new_node);
                    F->operator()(nFacePointer);
                }
            }
            else if ((euclidian < 0.0) && (level < radius - 1)){
                std::pair<int, CMeshO::FacePointer> new_node = std::make_pair(level + 1, nFacePointer);
                localstack.push_back(new_node);
                //process the neighboring node
                F->operator()(nFacePointer);
            }
        }

    }
    //save the results
    F->saveResults(f_it);
}
