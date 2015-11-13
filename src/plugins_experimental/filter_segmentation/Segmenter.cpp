//-------------------------------------------------------//
// File:  Segmenter.cpp                                  //
//                                                       //
// Description: Segmenter Source File                    //
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

#include "Segmenter.h"

using namespace std;
using namespace vcg;

#define PI 3.14159265f

void Segmenter::setMesh(CMeshO *meshObj) {
    meshObj_ = meshObj;
    Segmenter::faceClusterPairs = tri::Allocator<CMeshO>::AddPerFaceAttribute<Cluster *>(*meshObj_, string("Segment"));
    Segmenter::faceMarks = tri::Allocator<CMeshO>::AddPerFaceAttribute<int>(*meshObj_, string("Mark"));
    Segmenter::faceTmpMarks = tri::Allocator<CMeshO>::AddPerFaceAttribute<int>(*meshObj_, string("TmpMark"));
    Segmenter::faceArea = tri::Allocator<CMeshO>::AddPerFaceAttribute<float>(*meshObj_, string("Area"));
    Segmenter::faceCentroid = tri::Allocator<CMeshO>::AddPerFaceAttribute<vcg::Point3f>(*meshObj_, string("Centroid"));

    for (CMeshO::FaceIterator i = meshObj_->face.begin(); i != meshObj_->face.end(); ++i) {
        Segmenter::faceClusterPairs[*i] = NULL;
        Segmenter::faceMarks[*i] = -1;
        Segmenter::faceTmpMarks[*i] = -1;
        Segmenter::faceArea[*i] = computeFaceArea(*i);
        Segmenter::faceCentroid[*i] = computeFaceCentroid(*i);
    }
    computeTotalAreaAndInitPFA();
}

void Segmenter::postProcessClusters(float areaThreshold) {
    int nClustersBelowThr = 0;
    vector<Cluster *>::iterator s_it, merge_it;
    for (s_it = clusterList_.begin(); s_it != clusterList_.end(); ++s_it) {
        float ratio = 100.f*(*s_it)->getArea() / totalArea_;
        if (ratio < areaThreshold) {
            ++nClustersBelowThr;
        }
    }

    initNumClusters_ = clusterList_.size();
    // Calculate total mesh area
    bool finished;
    do {
        if (cb_) {
            cb_(100 * (initNumClusters_ - clusterList_.size()) / nClustersBelowThr, "Merging segments based on area threshold");
        }
        finished = true;
        Cluster *segToMerge = NULL;
        // Search for a segment that is below the area threshold
        float lowestRatio = FLT_MAX;
        vector<Cluster *>::iterator s_it, merge_it;
        for (s_it = clusterList_.begin(); s_it != clusterList_.end(); ++s_it) {
            float ratio = 100.f*(*s_it)->getArea() / totalArea_;
            if (ratio < areaThreshold && !(*s_it)->disjoined_) {
                if (lowestRatio >= ratio) {
                    lowestRatio = ratio;
                    segToMerge = (*s_it);
                    merge_it = s_it;
                    finished = false;
                }
            }
        }   // end for

        // If we found a valid for merging segment
        if (!finished) {
            decomposeClusterBestFirst(segToMerge);
            // Now clear and remove the merged segment
            merge_it = clusterList_.erase(merge_it);
            delete segToMerge;
            segToMerge = NULL;
        }   // end if
    } while (!finished);    // end do-while
}

void Segmenter::computeTotalAreaAndInitPFA() {
    totalArea_ = 0.f;
    CMeshO::FaceIterator f_it, f_end(meshObj_->face.end());
    for (f_it = meshObj_->face.begin(); f_it != f_end; ++f_it)  {
        faceMarks[*f_it] = -1;
        faceClusterPairs[*f_it] = NULL;

        // If face is deleted ignore it
        if ((*f_it).IsD()) { continue; }
        totalArea_ += Segmenter::faceArea[*f_it];
    }
}

void Segmenter::decomposeClusterBestFirst(Cluster *segToMerge) {
    Cluster *uniqueFNSegIds[100];
    int uIds_pos;
    int *segmentIds = new int[initNumClusters_];
    memset(segmentIds, -1, initNumClusters_ * sizeof(int));
    int round = 0;

    do {
        bool foundFace = false;
        Cluster *bestNeighborSeg = NULL;
        float minWeight = FLT_MAX;
        // Find the best face to merge with a valid neighbor segment
        std::vector<CMeshO::FacePointer>::iterator bestFace;
        size_t bestIndex = 0;
        size_t tmpIndex = 0;
        for (std::vector<CMeshO::FacePointer>::iterator fl_it = segToMerge->faces_.begin(); fl_it != segToMerge->faces_.end(); ++fl_it) {
            CMeshO::FacePointer facePointer = *fl_it;
            if (facePointer->IsD()) { continue; }

            uIds_pos = 0;
            round++;

            // Find unique neighbor segments in 1-ring Face Neighbors
            for (size_t i = 0; i < 3; ++i) {
                CMeshO::FacePointer nFacePointer = facePointer->FFp(i);
                // Sanity checks
                if (!nFacePointer || nFacePointer == facePointer || nFacePointer->IsD()) { continue; }

                int ffSegId = faceMarks[nFacePointer];
                //if (ffSegId != segToMerge->getId() && segmentIds[ffSegId] != round) {
                if (faceClusterPairs[nFacePointer] != segToMerge && segmentIds[ffSegId] != round) {
                    uniqueFNSegIds[uIds_pos] = faceClusterPairs[nFacePointer];
                    uIds_pos++;
                    segmentIds[ffSegId] = round;
                }
            }   // end for

            if (!uIds_pos) {
                ++tmpIndex;
                continue;
            }
            foundFace = true;

            Point3f &faceNormal = facePointer->N();
            //Point3f &faceCentroid = Segmenter::faceCentroid[*facePointer];
            // For each unique segment neighbor of the face search for the best matching one
            for (int i = 0; i < uIds_pos; ++i) {
                Cluster *cn_seg = uniqueFNSegIds[i];
                float weight = metricNormalDeviation(cn_seg->getAvgNormal(), faceNormal);

                if (minWeight >= weight) {
                    minWeight = weight;
                    bestNeighborSeg = cn_seg;
                    bestFace = fl_it;
                    bestIndex = tmpIndex;
                }
                /////////////////////////////////////////////////////////////////
            }   // For that finds the best matching segment for the current Face
            ++tmpIndex;
        }   // For that finds the best face for merging (included the matching segment of it)
        if (!foundFace) {
            segToMerge->disjoined_ = true;
            break;
        }
        faceMarks[*bestFace] = bestNeighborSeg->getId();
        faceClusterPairs[*bestFace] = bestNeighborSeg;
        clusterAddFace(bestNeighborSeg, (*bestFace));
        // Erase the bestFace from the previous segment
        segToMerge->faces_.erase(bestFace);
    } while (!segToMerge->faces_.empty());
    delete[]segmentIds;
}
