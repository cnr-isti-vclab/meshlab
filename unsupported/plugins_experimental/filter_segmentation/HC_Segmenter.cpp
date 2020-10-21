//-------------------------------------------------------//
// File:  HC_Segmenter.cpp                               //
//                                                       //
// Description: Hierarchical Segmentation Source File    //
//                                                       //
// Authors: Pavlos Mavridis     - http://pmavridis.com   //
//          Anthousis Andreadis - http://anthousis.com   //
// Date: 7-Oct-2015                                      //
//                                                       //
// Computer Graphics Group                               //
// http://graphics.cs.aueb.gr/graphics/                  //
// AUEB - Athens University of Economics and Business    //
//                                                       //
//                                                       //
// This work was funded by the EU-FP7 - PRESIOUS project //
//-------------------------------------------------------//

#include "HC_Segmenter.h"
#include "Cluster.h"
#include <deque>

using namespace std;
using namespace vcg;

float(*HC_Segmenter::error_metric_ptr_)(Cluster*, Cluster*, Segmenter *) = 0;

HC_Segmenter::HC_Segmenter(CallBackPos *cb) : Segmenter(cb) {
    meshObj_ = NULL;
    HC_Segmenter::error_metric_ptr_ = HC_Segmenter::orientation_norm;
}

void HC_Segmenter::clusterAddFace(Cluster *to, CMeshO::FacePointer &face) {
    const float area = Segmenter::faceArea[*face];
    const Point3f &centroid = Segmenter::faceCentroid[*face];

    to->faces_.push_back(face);
    to->area_ = to->area_ + area;

    vcg::Point3f tmpNormalSum = face->N() * area;
    vcg::Point3f tmpCentroid = centroid * area;
    to->faceNormalSum_[0] = tmpNormalSum[0];
    to->faceNormalSum_[1] = tmpNormalSum[1];
    to->faceNormalSum_[2] = tmpNormalSum[2];
    to->centroidSum_[0] = tmpCentroid[0];
    to->centroidSum_[1] = tmpCentroid[1];
    to->centroidSum_[2] = tmpCentroid[2];
}

// Used only on InitClusters -- Not a general function
void HC_Segmenter::clusterAddNeighbor(Cluster *to, Cluster* c, CMeshO::FacePointer &fh){
    assert(to != NULL);
    assert(c != NULL);
    assert(c != to);

    //std::map<Cluster*, set<CMeshMyMesh::FaceHandle> >::iterator it;
    std::map<Cluster*, std::set<CMeshO::FacePointer> >::iterator it = to->neighbors_.find(c);
    if (it == to->neighbors_.end()){
        set<CMeshO::FacePointer> new_set;
        new_set.insert(fh);
        to->neighbors_.insert(pair<Cluster*, set<CMeshO::FacePointer> >(c, new_set));
    }
}

void HC_Segmenter::clusterUpdateIfNearestOneWay(Cluster *to, float& minWeight, Cluster** A, Cluster** B, Segmenter *segmenter){
    std::map<Cluster*, std::set<CMeshO::FacePointer> >::iterator innerIter = to->neighbors_.begin();
    for (; innerIter != to->neighbors_.end() && (*innerIter).first < to; ++innerIter){
        //find the weight between the two clusters
        Cluster* c2 = (*innerIter).first;
        float weight = HC_Segmenter::error_metric_ptr_(to, c2, segmenter);
        if (weight < minWeight){
            minWeight = weight;
            *A = to;
            *B = c2;
        }
    }
}

void HC_Segmenter::clusterComputeNearestClusters(Cluster *to) {
    to->nearestDistance_ = FLT_MAX;
    to->nearestCluster_ = NULL;
    Cluster* foo;
    clusterUpdateIfNearestOneWay(to, to->nearestDistance_, &foo, &to->nearestCluster_, this);
}

Cluster* HC_Segmenter::clusterGetNearestCluster(Cluster *to, float *d){
    *d = to->nearestDistance_;
    return to->nearestCluster_;
}

void HC_Segmenter::clusterMerge(Cluster *cl, Cluster* c) {
    assert(c != NULL);

    cl->area_ += c->area_;
    cl->faceNormalSum_ += c->faceNormalSum_;
    cl->centroidSum_ += c->centroidSum_;

    map<Cluster*, std::set<CMeshO::FacePointer> >::iterator iter;
    // merge the list of edge-facets for the two maps
    std::map<Cluster*, std::set<CMeshO::FacePointer> >::iterator I;
    for (I = c->neighbors_.begin(); I != c->neighbors_.end(); ++I){
        iter = cl->neighbors_.find(I->first);
        if (iter == cl->neighbors_.end())
            cl->neighbors_.insert(*I);
        else
            iter->second.insert(I->second.begin(), I->second.end());
    }

    // remove myself from the list of neighbors
    iter = cl->neighbors_.find(cl);
    if (iter != cl->neighbors_.end()){
        cl->neighbors_.erase(iter);
    }

    // remove c from the list of neighbors
    iter = cl->neighbors_.find(c);
    if (iter != cl->neighbors_.end()){
        cl->neighbors_.erase(iter);
    }

    cl->faces_.insert(cl->faces_.end(), c->faces_.begin(), c->faces_.end());
}

// Cluster old_c is removed from the list of neighbors 
// and cluster new_c is added to the list of neighbors 
void HC_Segmenter::clusterExchange(Cluster *to, Cluster* old_cluster, Cluster* new_cluster){
    std::map<Cluster*, std::set<CMeshO::FacePointer> >::iterator itB(to->neighbors_.find(old_cluster));
    std::map<Cluster*, std::set<CMeshO::FacePointer> >::iterator itA(to->neighbors_.find(new_cluster));

    if (itB != to->neighbors_.end() && itA != to->neighbors_.end()){
        // merge the two sets
        itA->second.insert(itB->second.begin(), itB->second.end());
        to->neighbors_.erase(itB);
    }
    else if (itB != to->neighbors_.end() && itA == to->neighbors_.end()){
        std::set<CMeshO::FacePointer> old_set = itB->second;
        // merge the two sets
        to->neighbors_.insert(pair<Cluster*, set<CMeshO::FacePointer> >(new_cluster, old_set));
        to->neighbors_.erase(itB);
    }
}

void HC_Segmenter::initClusters() {
    int totalClusters = 0;
    CMeshO::FaceIterator fIter, fBegin(meshObj_->face.begin()), fEnd(meshObj_->face.end());

    clusterArray_ = new Cluster*[meshObj_->face.size()];
    clusterPool_ = new Cluster[meshObj_->face.size()];

    //initialize clusters
    for (fIter = fBegin; fIter != fEnd; ++fIter) {
        CMeshO::FacePointer facePointer = &(*fIter);
        //create a new cluster
        Cluster *nc = &clusterPool_[totalClusters];
        //Cluster *nc = new Cluster;
        nc->setId(totalClusters);

        clusterAddFace(nc, facePointer);
        faceClusterPairs[*fIter] = nc;

        clusterArray_[totalClusters] = nc;

        totalClusters++;
    }

    initialFaces_ = totalClusters;
    active_clusters_ = totalClusters;

    //initialize the neighbor list
    for (fIter = fBegin; fIter != fEnd; ++fIter) {
        CMeshO::FacePointer facePointer = &(*fIter);
        Cluster* nc = faceClusterPairs[*fIter];

        // For all face neighbors
        for (size_t i = 0; i < 3; ++i) {
            CMeshO::FacePointer nFacePointer = (*fIter).FFp(i);
            // Sanity checks
            if (!nFacePointer || nFacePointer == facePointer || nFacePointer->IsD()) { continue; }

            Cluster* nc2 = faceClusterPairs[*nFacePointer];
            clusterAddNeighbor(nc, nc2, facePointer);
        }
    }
}

void HC_Segmenter::finalizeClusters() {
    clusterList_.reserve(active_clusters_);
    for (int i = 0; i < active_clusters_; i++) {
        Cluster * from = clusterArray_[i];
        Cluster * to = new Cluster;
        clusterList_.push_back(to);
        to->area_ = from->area_;
        Point3d clusterCentroid = from->centroidSum_ / from->area_;
        to->centroid_[0] = clusterCentroid[0];
        to->centroid_[1] = clusterCentroid[1];
        to->centroid_[2] = clusterCentroid[2];
        to->id_ = i;
        Point3d clusterNormal = from->faceNormalSum_ / from->area_;
        to->normal_[0] = clusterNormal[0];
        to->normal_[1] = clusterNormal[1];
        to->normal_[2] = clusterNormal[2];
        to->faces_ = from->faces_;

        std::vector<CMeshO::FacePointer>::iterator fi;
        for (fi = to->faces_.begin(); fi != to->faces_.end(); ++fi) {
            CMeshO::FacePointer facePointer = *fi;
            faceMarks[*facePointer] = to->id_;
            faceClusterPairs[*facePointer] = to;
        }
    }
    delete[] clusterPool_;
    delete[] clusterArray_;
}

void HC_Segmenter::hierarchicalMerging() {
    for (int i = 0; i < active_clusters_; ++i){
        clusterComputeNearestClusters(clusterArray_[i]);
    }

    int L2_size = (initialFaces_ + BUCKET_SIZE - 1) / BUCKET_SIZE;  //ceil(active_clusters/BUCKET_SIZE)

    level2_nodes = new TreeNode[L2_size];

    //initialize level 2 nodes
    for (int i = 0; i < L2_size; ++i){
        level2_nodes[i].weight = FLT_MAX;
        level2_nodes[i].index = -1;
    }
    update_level2_range(0, initialFaces_);

    int mergeNum = 0;
    while (mergeNum < initialFaces_){
        int L2_active_clusters = (active_clusters_ + BUCKET_SIZE - 1) / BUCKET_SIZE;  //ceil(active_clusters/BUCKET_SIZE)
        if (cb_ && (mergeNum % 5000) == 0) {
            char buffer[512];
            sprintf(buffer, "Hierarchical Clustering :: Active Clusters %d | Total merges %d", active_clusters_, mergeNum);
            cb_(mergeNum * 100 / initialFaces_, buffer);
        }

        float minWeight = FLT_MAX;
        Cluster* A = NULL;
        Cluster* B = NULL;

        for (int i = 0; i < L2_active_clusters; ++i){
            if (level2_nodes[i].weight < minWeight){
                int level1_index = level2_nodes[i].index;
                minWeight = level2_nodes[i].weight;
                A = clusterArray_[level1_index];
            }
        }

        float d;
        clusterComputeNearestClusters(A);
        B = clusterGetNearestCluster(A, &d);
        //assert(minWeight == d);

        if (minWeight > errorThreshold_ || active_clusters_ < 7)
            break;

        // merge the two clusters with the lowest weight
        assert(A != NULL);
        assert(B != NULL);

        //A is the new merged cluster
        clusterMerge(A, B);
        //          cout<< "Merge:" <<mergeNum<<endl;
        mergeNum++;

        //for all the faces in cluster B, the pointers should now point to cluster A 
        //(not sure if we should keep these pointers up-to-date)
        std::vector<CMeshO::FacePointer>::iterator faceIter(B->faces_.begin());
        std::vector<CMeshO::FacePointer>::iterator faceEnd(B->faces_.end());
        for (; faceIter != faceEnd; ++faceIter){
            faceClusterPairs[*faceIter] = A;
        }

        // update: all the neighbors of B should now point to A
        std::map<Cluster*, std::set<CMeshO::FacePointer> >::iterator nIter(B->neighbors_.begin());
        std::map<Cluster*, std::set<CMeshO::FacePointer> >::iterator nEnd(B->neighbors_.end());
        for (; nIter != nEnd; ++nIter){
            Cluster* tmp = (*nIter).first;
            if (tmp != A){ //we don't have to change anything in node A yet
                clusterExchange(tmp, B, A);
            }
        }

        // (updates the neighbors only if necessary 
        A->nearestDistance_ = FLT_MAX;
        A->nearestCluster_ = NULL;

        nIter = A->neighbors_.begin();
        for (; nIter != A->neighbors_.end(); ++nIter){
            Cluster* c2 = (*nIter).first;
            float weight = HC_Segmenter::error_metric_ptr_(A, c2, this);
            if (c2 < A && (weight < A->nearestDistance_)){
                A->nearestDistance_ = weight;
                A->nearestCluster_ = c2;
            }

            if (c2->nearestCluster_ == A || c2->nearestCluster_ == B) {
                clusterComputeNearestClusters(c2);
            }
            else{
                if (A<c2 && (c2->nearestDistance_>weight)) {
                    c2->nearestDistance_ = weight;
                    c2->nearestCluster_ = A;
                }
            }
        }

        int to_delete = B->getId();

        B->faces_.clear();
        B->neighbors_.clear();

        clusterArray_[to_delete] = clusterArray_[active_clusters_ - 1];
        //update the index of the moved node
        clusterArray_[to_delete]->setId(to_delete);
        active_clusters_--;

        int l2_A_index = A->getId() / BUCKET_SIZE;
        int l2_B_index = to_delete / BUCKET_SIZE;

        //update the upper level node corresponding to A
        recompute_level2_at(l2_A_index);
        //update the upper level node corresponding to B
        if (l2_B_index != l2_A_index)
            recompute_level2_at(l2_B_index);

        int new_l2_active_nodes = (active_clusters_ + BUCKET_SIZE - 1) / BUCKET_SIZE; //ceil(active_clusters/BUCKET_SIZE)

        // slow path: the node we have moved was in the upper hierarchy
        if (level2_nodes[new_l2_active_nodes - 1].index == active_clusters_){
            //recompute the last L2 node
            int l2_index = active_clusters_ / BUCKET_SIZE;
            if ((l2_index != l2_A_index) || (l2_index != l2_B_index))
                recompute_level2_at(l2_index);
        }

        //update L2 for all the neighbors of A 
        nIter = A->neighbors_.begin();
        nEnd = A->neighbors_.end();
        for (; nIter != nEnd; ++nIter){
            //update second level hierarchy
            int level1_index = (*nIter).first->getId();
            int level2_index = level1_index / BUCKET_SIZE;
            if (level2_nodes[level2_index].index == level1_index)
                recompute_level2_at(level2_index); //brute force
            else
                update_level2_item(level1_index);
        }
    }
    delete[] level2_nodes;
}

void HC_Segmenter::createClusters() {
    initClusters();
    hierarchicalMerging();
    finalizeClusters();
}
