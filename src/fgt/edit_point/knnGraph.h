#ifndef KNNGRAPH_H
#define KNNGRAPH_H

#include <Vector>

#include <vcg/complex/complex.h>
#include <vcg/complex/allocate.h>

#include <vcg/space/index/kdtree/kdtree.h>

using namespace std;
using namespace vcg;

template <typename _MyMeshType, typename _MyVertexType>
class KNNTree {
public:
    static void MakeKNNTree(_MyMeshType& m, int numOfNeighbours);
    static void DeleteKNNTree(_MyMeshType& m);
};

/**
 * Static function to create and fill the PerVertexAttribute KNNGraph, which stores the k-nearest
 * neighbours via vertex pointers
 */
template <typename _MyMeshType, typename _MyVertexType>
void KNNTree<_MyMeshType, _MyVertexType>::MakeKNNTree(_MyMeshType& m, int numOfNeighbours) {
    //we search k+1 neighbours in order to exclude the queryPoint from the returned heap
    int neighboursVectSize = numOfNeighbours + 1;

    int neighbours; //number of neighbours found (no more than neighboursVectSize)

    //we have to use the indices of the vertices, and they MUST be continuous
    tri::Allocator<_MyMeshType>::CompactVertexVector(m);

    //the PerVertexAttribute handles is create and each of the vector capacity set to the maximum possible
    typename _MyMeshType::template PerVertexAttributeHandle<vector<_MyVertexType*>* > kNeighboursVect;
    kNeighboursVect = vcg::tri::Allocator<_MyMeshType>::template AddPerVertexAttribute<vector<_MyVertexType*>* >(m, std::string("KNNGraph"));
    for (typename _MyMeshType::VertexIterator vi = m.vert.begin(); vi != m.vert.end(); vi++) {
        kNeighboursVect[vi] = new vector<_MyVertexType*>();
        kNeighboursVect[vi]->reserve(neighboursVectSize);
    }

    //we create and fill the DataWrapper we need to pass the points to the KdTree
    vector<typename _MyMeshType::CoordType> input(m.vn);
    int i = 0;
    for (typename _MyMeshType::VertexIterator vi = m.vert.begin(); vi != m.vert.end(); vi++, ++i) {
        input[i] = vi->cP();
    }
    ConstDataWrapper<typename _MyMeshType::CoordType> DW(&(input[0]), input.size());

    KdTree<float> tree(DW);

    tree.setMaxNofNeighbors(neighboursVectSize);

    //For each vertex we insert the k-nearest neighbours in the associated vector.
    //If the number of the found vertices is less than the required we exclude the point with
    //the highest value (inserted by the doQueryK function of the vcg library). Moreover
    //we have to exclude the queryPoint!
    int neightId = -1;
    for (int j = 0; j < m.vn; j++) {
        tree.doQueryK(m.vert[j].cP());

        neighbours = tree.getNofFoundNeighbors();
        for (int i = 0; i < neighbours; i++) {
            neightId = tree.getNeighborId(i);
            if (neightId < m.vn && (neightId != j))
                kNeighboursVect[m.vert[j]]->push_back(&(m.vert[neightId]));
        }
    }

    return;
}

/**
 * Static function which removes the KNNGraph PerVertex attribute
 */
template <typename _MyMeshType, typename _MyVertexType>
void KNNTree<_MyMeshType, _MyVertexType>::DeleteKNNTree(_MyMeshType& m) {
    bool hasKNNGraph = tri::HasPerVertexAttribute(m, "KNNGraph");

    if (hasKNNGraph) {
        vcg::tri::Allocator<_MyMeshType>::DeletePerVertexAttribute(m, "KNNGraph");
    }

    return;
}

#endif // KNNGRAPH_H
