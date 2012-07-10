#ifndef CONNECTEDCOMPONENT_H
#define CONNECTEDCOMPONENT_H

#include <QTime>

#include "knnGraph.h"

#include <vector>
#include <stack>

#include <vcg/complex/complex.h>
#include <vcg/complex/allocate.h>
#include <vcg/complex/algorithms/update/flag.h>

#include <vcg/space/fitting3.h>


using namespace std;
using namespace vcg;

template <typename _MyMeshType, typename _MyVertexType>
class ComponentFinder {
public:
    static std::vector<_MyVertexType*> &FindComponent(_MyMeshType& m, float dim, vector<_MyVertexType*> &borderVect, vector<_MyVertexType*> &notReachableVect, bool fitting = false, float planeDim = 0.0, float distanceFromPlane = 0.0, Plane3<typename _MyMeshType::ScalarType> *fittingPlane = NULL);

    static void DeletePerVertexAttribute(_MyMeshType& m);

    static void Dijkstra(_MyMeshType& m, _MyVertexType& v, int numOfNeighbours, float maxHopDist, vector<_MyVertexType*> &notReachableVect);
};


/** This function returns a vector which stores pointers to the vertices of the connected component with
  * max distance 'dim' from the center. It assumes that the vertices have the DistParam attribute, so
  * we don't have to know where is the center.
  * The border is also computed, using the notReachableVect parameter, which stores the vertices we
  * couldn't reach because of the hop distance.
  *
  * We can specify to fit some points: in this case first we compute the fitting plane (vcg library method),
  * then we calculate the connected component and the border.
  **/
template <typename _MyMeshType, typename _MyVertexType>
std::vector<_MyVertexType*> &ComponentFinder<_MyMeshType, _MyVertexType>::FindComponent(_MyMeshType& m, float dim, vector<_MyVertexType*> &borderVect, vector<_MyVertexType*> &notReachableVect, bool fitting, float planeDim, float distanceFromPlane, Plane3<typename _MyMeshType::ScalarType> *fittingPlane) {
    vector<_MyVertexType*> *resultVect = new vector<_MyVertexType*>();
    vector<typename _MyMeshType::CoordType> pointToFit = vector<typename _MyMeshType::CoordType>();

    tri::UpdateFlags<_MyMeshType>::VertexClearV(m);

    bool hasDistParam = vcg::tri::HasPerVertexAttribute(m, "DistParam");

    typename _MyMeshType::template PerVertexAttributeHandle<float> distFromCenter;

    if (hasDistParam) distFromCenter = vcg::tri::Allocator<_MyMeshType>::template GetPerVertexAttribute<float>(m, std::string("DistParam"));
    else return *resultVect;

    for (typename _MyMeshType::VertexIterator vi = m.vert.begin(); vi != m.vert.end(); vi++) {
        if (fitting) {
            if (distFromCenter[vi] < planeDim) {
                pointToFit.push_back(vi->cP());
            }
        }
        else if (distFromCenter[vi] < dim) resultVect->push_back(&*vi);
    }

    typename vector<_MyVertexType*>::iterator it;
    if (fitting) {
        vcg::PlaneFittingPoints(pointToFit, *fittingPlane);

        for (typename _MyMeshType::VertexIterator vi = m.vert.begin(); vi != m.vert.end(); vi++) {
            if (distFromCenter[vi] < dim && math::Abs(vcg::SignedDistancePlanePoint<typename _MyMeshType::ScalarType>(*fittingPlane, vi->cP())) < distanceFromPlane) resultVect->push_back(&*vi);
        }
        for (it = notReachableVect.begin(); it != notReachableVect.end(); it++) {
            if (distFromCenter[*it] < dim && math::Abs(vcg::SignedDistancePlanePoint<typename _MyMeshType::ScalarType>(*fittingPlane, (*it)->cP())) < distanceFromPlane) borderVect.push_back(*it);
        }
    }
    else {
        for (it = notReachableVect.begin(); it != notReachableVect.end(); it++) {
            if (distFromCenter[*it] < dim) borderVect.push_back(*it);
        }
    }

    return *resultVect;
}




/* This class is used in the priority queue to order the nodes */
template <typename _MyMeshType, typename _MyVertexType>
class Compare {
private:
    typename _MyMeshType::template PerVertexAttributeHandle<float> *distFromCenter;

public:
    Compare(typename _MyMeshType::template PerVertexAttributeHandle<float> *distFromCenter) {
        this->distFromCenter = distFromCenter;
    }

    bool operator() (const _MyVertexType* lhs, const _MyVertexType* rhs) const 
	{
        return (*distFromCenter)[*lhs] > (*distFromCenter)[*rhs];
    }
};


/** This function is used to calculate the minimum distances between one point (v) and all the others
  * in the mesh. We use the Dijkstra algorithm with one change: only arcs with a cost less or equal
  * of maxHopDist will be taken into account.
  * In the first call of this method, when neither DistParam or KNNGraph are present, we create these
  * attributes and we construct the KNNGraph.
  * The notReachableVect is returned in order to calculate the border in other methods.
  **/
template <typename _MyMeshType, typename _MyVertexType>
void ComponentFinder<_MyMeshType, _MyVertexType>::Dijkstra(_MyMeshType& m, _MyVertexType& v, int numOfNeighbours, float maxHopDist, vector<_MyVertexType*> &notReachableVect) {
    bool hasKNNGraph = vcg::tri::HasPerVertexAttribute(m, "KNNGraph");
    bool hasDistParam = vcg::tri::HasPerVertexAttribute(m, "DistParam");

    notReachableVect.clear();

    typename _MyMeshType::template PerVertexAttributeHandle<float> distFromCenter;
    if (!hasDistParam) {
        distFromCenter = vcg::tri::Allocator<_MyMeshType>::template AddPerVertexAttribute<float>(m, std::string("DistParam"));
    }
    else distFromCenter = vcg::tri::Allocator<_MyMeshType>::template GetPerVertexAttribute<float>(m, std::string("DistParam"));

    if (!hasKNNGraph) {
        KNNTree<_MyMeshType, _MyVertexType>::MakeKNNTree(m, numOfNeighbours);
    }

    typename _MyMeshType::template PerVertexAttributeHandle<vector<_MyVertexType*>* > neighboursVect = vcg::tri::Allocator<_MyMeshType>::template GetPerVertexAttribute<vector<_MyVertexType*>* >(m,"KNNGraph");

    typename vector<_MyVertexType*>::iterator it;

    // For Dijkstra algorithm we use a Priority Queue
    typedef priority_queue<_MyVertexType*, vector<_MyVertexType*>, Compare<_MyMeshType, _MyVertexType> > VertPriorityQueue;
    Compare<_MyMeshType, _MyVertexType> Comparator(&distFromCenter);
    VertPriorityQueue prQueue (Comparator);

    for (typename _MyMeshType::VertexIterator vi = m.vert.begin(); vi != m.vert.end(); vi++)
        distFromCenter[vi] = numeric_limits<float>::max();

    distFromCenter[v] = 0.f;

    tri::UpdateFlags<_MyMeshType>::VertexClearV(m);

    prQueue.push(&v);
    v.SetV();

    float distance;
    _MyVertexType* element;

    while (!prQueue.empty()) {
        element = prQueue.top();
        prQueue.pop();

        for (it = neighboursVect[element]->begin(); it != neighboursVect[element]->end(); it++) 
		{
			//I have not to compute the arches connecting vertices already visited.
			if (!(*it)->IsV())
			{
				distance = vcg::Distance((*it)->P(), element->P());

				// we take into account only the arcs with a distance less or equal to maxHopDist
				if (distance <= maxHopDist) 
				{
					if ((distFromCenter[*element] + distance) < distFromCenter[*it])
						distFromCenter[*it] = distFromCenter[*element] + distance;

					if (!(*it)->IsV()) {
						prQueue.push(*it);
						(*it)->SetV();
					}
				}
				// all the other are the notReachable arcs
				else if (distance > maxHopDist) notReachableVect.push_back(element);
			}
        }
    }
}


/**
  * Used to free memory
  **/
template <typename _MyMeshType, typename _MyVertexType>
void ComponentFinder<_MyMeshType, _MyVertexType>::DeletePerVertexAttribute(_MyMeshType& m) {
    KNNTree<_MyMeshType, _MyVertexType>::DeleteKNNTree(m);

    bool hasDistParam = vcg::tri::HasPerVertexAttribute(m, "DistParam");
    if (hasDistParam) {
        vcg::tri::Allocator<_MyMeshType>::DeletePerVertexAttribute(m, "DistParam");
    }

    return;
}



#endif // CONNECTEDCOMPONENT_H
