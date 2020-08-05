#ifndef CONNECTEDCOMPONENT_H
#define CONNECTEDCOMPONENT_H

#include <QTime>

#include "knnGraph.h"

#include <vector>
#include <stack>

#include <vcg/complex/complex.h>

#include <vcg/space/fitting3.h>

namespace vcg {
namespace tri {

template <typename _MyMeshType>
class ComponentFinder {
  typedef typename _MyMeshType::VertexType VertexType;
  typedef typename _MyMeshType::VertexIterator VertexIterator;
  typedef typename _MyMeshType::CoordType CoordType;
public:

  /** This function returns a vector which stores pointers to the vertices of the connected component with
  * max distance 'dim' from the center. It assumes that the vertices have the DistParam attribute, so
  * we don't have to know where is the center.
  * The border is also computed, using the notReachableVect parameter, which stores the vertices we
  * couldn't reach because of the hop distance.
  *
  * We can specify to fit some points: in this case first we compute the fitting plane (vcg library method),
  * then we calculate the connected component and the border.
  **/
static std::vector<VertexType*> &FindComponent(_MyMeshType& m, float dim,
                                                  std::vector<VertexType*> &borderVect,
                                                  std::vector<VertexType*> &notReachableVect,
                                                  bool fitting=false, float planeDim=0, float distanceFromPlane=0,
                                                  Plane3<typename _MyMeshType::ScalarType> *fittingPlane=NULL)
{
    std::vector<VertexType*> *resultVect = new std::vector<VertexType*>();
    std::vector<CoordType> pointToFit = std::vector<CoordType>();

    tri::UpdateFlags<_MyMeshType>::VertexClearV(m);

    bool hasDistParam = tri::HasPerVertexAttribute(m, "DistParam");

    typename _MyMeshType::template PerVertexAttributeHandle<float> distFromCenter;

    if (hasDistParam) distFromCenter = tri::Allocator<_MyMeshType>::template GetPerVertexAttribute<float>(m, std::string("DistParam"));
    else return *resultVect;

    for (VertexIterator vi = m.vert.begin(); vi != m.vert.end(); vi++) {
        if (fitting) {
            if (distFromCenter[vi] < planeDim) {
                pointToFit.push_back(vi->cP());
            }
        }
        else if (distFromCenter[vi] < dim) resultVect->push_back(&*vi);
    }

    typename std::vector<VertexType*>::iterator it;
    if (fitting) {
        vcg::FitPlaneToPointSet(pointToFit, *fittingPlane);

        for (VertexIterator vi = m.vert.begin(); vi != m.vert.end(); vi++) {
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
class Compare {
private:
    typename _MyMeshType::template PerVertexAttributeHandle<float> *distFromCenter;

public:
    Compare(typename _MyMeshType::template PerVertexAttributeHandle<float> *distFromCenter) {
        this->distFromCenter = distFromCenter;
    }

	bool operator() (const VertexType* lhs, const VertexType* rhs) const
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

static void Dijkstra(_MyMeshType& m, VertexType& v, int numOfNeighbours, float maxHopDist, std::vector<VertexType*> &notReachableVect)
{
    bool hasKNNGraph = tri::HasPerVertexAttribute(m, "KNNGraph");
//    bool hasDistParam = tri::HasPerVertexAttribute(m, "DistParam");

    notReachableVect.clear();

    typename _MyMeshType::template PerVertexAttributeHandle<float> distFromCenter = vcg::tri::Allocator<_MyMeshType>::template GetPerVertexAttribute<float>(m, std::string("DistParam"));

    if (!hasKNNGraph) {
        KNNGraph<_MyMeshType>::MakeKNNTree(m, numOfNeighbours);
    }

    typename _MyMeshType::template PerVertexAttributeHandle<std::vector<VertexType*>* > neighboursVect = tri::Allocator<_MyMeshType>::template GetPerVertexAttribute<std::vector<VertexType*>* >(m,"KNNGraph");

    typename std::vector<VertexType*>::iterator it;

    // For Dijkstra algorithm we use a Priority Queue
    typedef std::priority_queue<VertexType*, std::vector<VertexType*>, Compare > VertPriorityQueue;
    Compare Comparator(&distFromCenter);
    VertPriorityQueue prQueue (Comparator);

    for (typename _MyMeshType::VertexIterator vi = m.vert.begin(); vi != m.vert.end(); vi++)
        distFromCenter[vi] = std::numeric_limits<float>::max();

    distFromCenter[v] = 0.f;

    tri::UpdateFlags<_MyMeshType>::VertexClearV(m);

    prQueue.push(&v);
    v.SetV();

    while (!prQueue.empty()) {
         VertexType* element = prQueue.top();
        prQueue.pop();

        for (it = neighboursVect[element]->begin(); it != neighboursVect[element]->end(); it++) 
		{
			//I have not to compute the arches connecting vertices already visited.
			if (!(*it)->IsV())
			{
				float distance = vcg::Distance((*it)->P(), element->P());

				// we take into account only the arcs with a distance less or equal to maxHopDist
				if (distance <= maxHopDist) 
				{
					if ((distFromCenter[*element] + distance) < distFromCenter[*it])
					{
						distFromCenter[*it] = distFromCenter[*element] + distance;
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

static void DeletePerVertexAttribute(_MyMeshType& m)
{
    KNNGraph<_MyMeshType>::DeleteKNNTree(m);

    bool hasDistParam = tri::HasPerVertexAttribute(m, "DistParam");
    //The Virtual Goniometer Plugin uses the DistParam to determine the 
    //Point the user clicked on. Please leave the deletion code below commented out.
    /*if (hasDistParam) {
        Allocator<_MyMeshType>::DeletePerVertexAttribute(m, "DistParam");
    }*/
    return;
}

}; // end ComponentFinder Class
} //end namespace tri
} // end namespace vcg;

#endif // CONNECTEDCOMPONENT_H
