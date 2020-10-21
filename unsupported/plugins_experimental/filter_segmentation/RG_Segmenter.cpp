//-------------------------------------------------------//
// File:  RG_Segmenter.cpp                               //
//                                                       //
// Description: Region Growing Segmenter Source File     //
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

#include "RG_Segmenter.h"
#include "Cluster.h"
#include <deque>

using namespace vcg;
using namespace std;

float(*RG_Segmenter::error_metric_ptr_)(Cluster *, CMeshO::FacePointer &, Segmenter *) = 0;

CMeshO::FacePointer(*RG_Segmenter::next_candidate_)(Cluster *cl, void *) = 0;

RG_Segmenter::RG_Segmenter(CallBackPos *cb) : Segmenter(cb) {
    meshObj_ = NULL;
    rgFunction_ = &RG_Segmenter::greedyBestFirstSegmentation;
}

void RG_Segmenter::regionGrowindMethod(RG_Segmenter::rg_type_t type) {
    if (type == FIFO)
        rgFunction_ = &RG_Segmenter::greedyFIFOSegmentation;
    else if (type == BEST_FIRST)
        rgFunction_ = &RG_Segmenter::greedyBestFirstSegmentation;
    else // BF_LOCAL_NEIGHBORS
        rgFunction_ = &RG_Segmenter::greedyBestFirstSegmentationLocalNeighbors;
}

void RG_Segmenter::clusterAddFace(Cluster *to, CMeshO::FacePointer &face) {
    const float area = Segmenter::faceArea[*face];
    const Point3f &centroid = Segmenter::faceCentroid[*face];

    to->faces_.push_back(face);
    to->area_ += area;

    vcg::Point3f tmpNormalSum = face->N() * area;
    vcg::Point3f tmpCentroid = centroid * area;
    to->faceNormalSum_[0] += tmpNormalSum[0];
    to->faceNormalSum_[1] += tmpNormalSum[1];
    to->faceNormalSum_[2] += tmpNormalSum[2];
    to->centroidSum_[0] += tmpCentroid[0];
    to->centroidSum_[1] += tmpCentroid[1];
    to->centroidSum_[2] += tmpCentroid[2];

    // Set the cluster_id of the face and the pointer to the actual Cluster
    Segmenter::faceClusterPairs[*face] = to;
    faceMarks[face] = to->getId();

    to->updateAvgNormal();
    to->updateCentroid();
}

Cluster *RG_Segmenter::finalizeCluster(Cluster *seg) {
    // Add already existing segment to list
    clusterList_.push_back(seg);
    // Create a new segment
    Cluster *newSeg = new Cluster();
    newSeg->setId((int)(clusterList_.size()));
    return newSeg;
}

void RG_Segmenter::updateProcessList(Cluster *seg, CMeshO::FacePointer &face, vector<CMeshO::FacePointer> &processList) {
    for (size_t i = 0; i < 3; ++i) {
        CMeshO::FacePointer nFacePointer = face->cFFp(i);
        // Sanity checks
        if (nFacePointer == face || nFacePointer->IsD()) { continue; }

        // If the neighbor face does not belong yet to a segment
        if (!faceClusterPairs[nFacePointer]) {
            if (faceMarks[nFacePointer] != seg->getId()) {
                // Mark as tested
                faceMarks[nFacePointer] = seg->getId();
                processList.push_back(nFacePointer);
            }
        }
    }
}

void RG_Segmenter::updateProcessList(Cluster *seg, CMeshO::FacePointer &face, deque<CMeshO::FacePointer> &processList) {
    for (size_t i = 0; i < 3; ++i) {
        CMeshO::FacePointer nFacePointer = face->FFp(i);
        // Sanity checks
        if (!nFacePointer || nFacePointer == face || nFacePointer->IsD()) { continue; }

        // If the neighbor face does not belong yet to a segment
        if (!faceClusterPairs[nFacePointer]) {
            if (faceMarks[nFacePointer] != seg->getId()) {
                // Mark as tested
                faceMarks[nFacePointer] = seg->getId();
                processList.push_back(nFacePointer);
            }
        }
    }
}

void RG_Segmenter::greedyFIFOSegmentation() {
    setNexCandidate(RG_Segmenter::fifo);
    Cluster *seg = new Cluster();
    seg->setId((unsigned int)(clusterList_.size()));

    deque<CMeshO::FacePointer> fQueue;

    int index = -1;
    int totalFaces = meshObj_->face.size();
    // For all faces
    for (CMeshO::FaceIterator f_it = meshObj_->face.begin(); f_it != meshObj_->face.end(); ++f_it)  {
        // If face is deleted ignore it
        if ((*f_it).IsD()) { continue; }
        CMeshO::FacePointer facePointer = &(*f_it);
        
        // If the face does not belong to a segment
        if (!faceClusterPairs[*facePointer]) {
            // If the segment is empty add it to segment and neighbors to queue based on tests
            if (seg->faces_.empty()) {
                clusterAddFace(seg, facePointer);
                updateProcessList(seg, facePointer, fQueue);
            }
            // Else if not marked - add it to queue
            else if (!faceMarks[*facePointer]) {
                fQueue.push_back(facePointer);
            }
        }
        // Else ignore it
        // Now process the Queue while it still has items
        while (!fQueue.empty()) {   
            // Get next item
            //CMeshO::FacePointer fqPointer = fQueue.front();
            //fQueue.pop_front();
            CMeshO::FacePointer fqPointer = RG_Segmenter::next_candidate_(seg,reinterpret_cast<void *>(&fQueue));

            // MERGE CRITERION
            float minWeight = RG_Segmenter::error_metric_ptr_(seg, fqPointer, this);
            //////////////////////////////////////////////////////////////////////
            // If angle is within the permitted range add the face to the segment
            if (errorThreshold_ >= minWeight) {
                // Update progress bar
                if (cb_ && (++index % 5000) == 0) { cb_(100 * index / totalFaces, "Performing Segmentation..."); }
                clusterAddFace(seg, fqPointer);
                updateProcessList(seg, fqPointer, fQueue);
            }   // End If
        }   // End While
        // When the queue is empty it means that no other neighbor is valid for the segment.
        // If the segment has at least on face
        if (!seg->faces_.empty()) {
            seg = finalizeCluster(seg);
        }
    }
    // Delete the last allocated segment if it is empty
    if (seg->faces_.empty()) delete seg;
    if (cb_) { cb_(100, "Performing Segmentation..."); }
}

void RG_Segmenter::greedyBestFirstSegmentation(void) {
    setNexCandidate(RG_Segmenter::best);
    Cluster *seg = new Cluster();
    seg->setId((int)(clusterList_.size()));

    int index = -1;
    int totalFaces = meshObj_->face.size();

    vector<CMeshO::FacePointer> fVector;
    // For all faces
    for (CMeshO::FaceIterator f_it = meshObj_->face.begin(); f_it != meshObj_->face.end(); ++f_it)  {
        // If face is deleted ignore it
        if ((*f_it).IsD()) { continue; }
        CMeshO::FacePointer facePointer = &(*f_it);
            
        // If the face does not belong to a segment
        if (!faceClusterPairs[*facePointer]) {
            // If the segment is empty add it to segment and neighbors to queue based on tests
            if (seg->faces_.empty()) {
                clusterAddFace(seg, facePointer);
                updateProcessList(seg, facePointer, fVector);
            } // Else ignore it
        }
        // Else ignore it
        // Now process the vector while it still has items
        while (!fVector.empty()) {
            float minWeight = FLT_MAX;
            CMeshO::FacePointer f_best = RG_Segmenter::next_candidate_(seg, reinterpret_cast<void *>(&fVector));
            // Sanity Check
            if (f_best->IsD()) continue;

            // MERGE CRITERION
            float weight = orientation_norm(seg, f_best, this);
            //////////////////////////////////////////////////////////////////////

            // If angle is within the permitted range add the face to the segment
            if (errorThreshold_ > weight) {
                if (cb_ && (++index % 5000) == 0) { cb_(100 * index / totalFaces, "Performing Segmentation..."); }
                clusterAddFace(seg, f_best);
                updateProcessList(seg, f_best, fVector);
            }   // End If
        }   // End While
        // When the queue is empty it means that no other neighbor is valid for the segment.
        // If the segment has at least one face
        if (!seg->faces_.empty()) {
            seg = finalizeCluster(seg);
        }
    }
    // Delete the last allocated segment if it is empty
    if (seg->faces_.empty()) delete seg;

    if (cb_) { cb_(100, "Performing Segmentation..."); }
}

void RG_Segmenter::greedyBestFirstSegmentationLocalNeighbors(void) {
    Cluster *seg = new Cluster();
    seg->setId((int)clusterList_.size());

    map<CMeshO::FacePointer, lnFaceHandle> sfMap;
    vector<CMeshO::FacePointer> fVector;

    int index = -1;
    int totalFaces = meshObj_->face.size();
    int mark_counter = -1;
    // For all faces
    for (CMeshO::FaceIterator f_it = meshObj_->face.begin(); f_it != meshObj_->face.end(); ++f_it) {
        // If face is deleted ignore it
        if ((*f_it).IsD()) { continue; }
        CMeshO::FacePointer facePointer = &(*f_it);

        // If the face does not belong to a segment
        if (Segmenter::faceClusterPairs[*facePointer] == NULL) {
            // If the segment is empty add it to segment and neighbors to queue based on tests
            if (seg->faces_.empty()) {
                clusterAddFace(seg, facePointer);
                sfMap.insert(make_pair(facePointer, lnFaceHandle(facePointer)));
                updateProcessList(seg, facePointer, fVector);
            } // Else ignore it
        }
        // Else ignore it
        // Now process the Queue while it still has items
        while (!fVector.empty()) {
            if (cb_ && (index % 1000) == 0) {
                cb_(100 * index / totalFaces, "Performing Segmentation...");
            }
            float minWeight = FLT_MAX;
            CMeshO::FacePointer f_best;
            int i = 0, erase_index = 0;
            // Now for each under examination Face search the best Neighbor that is Already in the Segment
            std::vector<CMeshO::FacePointer>::iterator it;
            for (it = fVector.begin(); it != fVector.end(); ++it) {
                CMeshO::FacePointer vFacePointer = (*it);
                // For all neighbor Faces
                for (size_t k = 0; k < 3; ++k) {
                    CMeshO::FacePointer nvFacePointer = vFacePointer->cFFp(k);
                    // Sanity checks
                    if (nvFacePointer == vFacePointer || nvFacePointer->IsD()) { continue; }
                    if (Segmenter::faceClusterPairs[*nvFacePointer] == seg) {
                        std::map<CMeshO::FacePointer, lnFaceHandle>::iterator sf_it = sfMap.find(nvFacePointer);
                        if (sf_it != sfMap.end()) {
                            float weight = metricNormalDeviation(sf_it->second.lnAvgNormal, vFacePointer->N());
                            // We store the it as this is the face that will be added to the segment
                            if (minWeight > weight) {
                                minWeight = weight;
                                f_best = *it;
                                erase_index = i;
                            }
                            break;
                        }
                    }
                }
                i++;
            }
            // Remove the item from the Vector
            fVector.erase(fVector.begin() + erase_index);
            // Sanity Check
            if (f_best->IsD()) continue;

            // If angle is within the permitted range add the face to the segment
            if (errorThreshold_ >= minWeight) {
                ++index;
                clusterAddFace(seg, f_best);

                UpdateLocalNeighborsStructure *ulns = new UpdateLocalNeighborsStructure(&sfMap);
                runOperatorAtFace(f_best, ++mark_counter, 4, ulns, eDist_, seg);
                delete ulns;

                ComputeNormalFaceFunctor *nff = new ComputeNormalFaceFunctor;
                runOperatorAtFace(f_best, ++mark_counter, 4, nff, eDist_, seg);
                vcg::Point3f fbAvgNormal = nff->getAverageNormal();
                float fbLnArea = nff->getSummedArea();
                delete nff;
                sfMap.insert(make_pair(f_best, lnFaceHandle(f_best, fbLnArea, fbAvgNormal)));
                updateProcessList(seg, f_best, fVector);
            }   // End If
            // If the best find was not valid, none other will be
            else {
                fVector.clear();
                sfMap.clear();
            }   // End Else
        }   // End While
        sfMap.clear();
        // When the queue is empty it means that no other neighbor is valid for the segment.
        // If the segment has at least on face
        if (!seg->faces_.empty()) {
            seg = finalizeCluster(seg);
        }
    }   // End for
    // Delete the last allocated segment if it is empty
    if (seg->faces_.empty()) delete seg;
}

void RG_Segmenter::createClusters() {
    (this->*rgFunction_) ();
}
