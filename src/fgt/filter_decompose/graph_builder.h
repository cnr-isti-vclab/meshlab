#include "../../external/maxflow-v2.21.src/adjacency_list/graph.h"
#include <stdlib.h>

#ifndef GRAPHBUILDER_H
#define GRAPHBUILDER_H

namespace vcg {
namespace tri {

//the class is templated also on the FunctorType, that is used to compute edge weights
//so the dual graph can be built assigning other weight values.
template<class MeshType, class FunctorType>
class GraphBuilder
{
public:
    typedef typename MeshType::VertexType VertexType;
    typedef typename MeshType::VertexPointer VertexPointer;
    typedef typename MeshType::FaceType FaceType;
    typedef typename MeshType::FacePointer FacePointer;
    typedef typename MeshType::EdgePointer EdgePointer;
    typedef typename MeshType::EdgeType EdgeType;
    typedef typename MeshType::VertexIterator VertexIterator;
    typedef typename MeshType::FaceIterator FaceIterator;
    typedef typename MeshType::CoordType Point3f;
    typedef typename MeshType::ScalarType ScalarType;
    typedef typename face::Pos<FaceType>    PosType;

    /*main function to be called from outside: builds the graph and fires the maxflow/mincut alogrith*/
    static void Compute(MeshType &m, FunctorType &wf, FacePointer startFace, FacePointer endFace)
    {
        assert(startFace!=NULL && endFace!=NULL && !startFace->IsD() && !endFace->IsD());
        Graph *g = GraphBuilder<MeshType, FunctorType>::BuildGraph(m, wf, startFace, endFace);
        double flowVal = g->maxflow();
        qDebug("Flow %f",flowVal);
        GraphBuilder<MeshType, FunctorType>::ColorFaces(m, *g, startFace, endFace);
        assert(vcg::tri::HasPerFaceAttribute(m,"NodeID"));
        vcg::tri::Allocator<MeshType>::DeletePerFaceAttribute(m,"NodeID");
        delete g;
    }

    /* Builds the dual graph.
     * the startFace will be connected to the source
     * the endFace will be connected to the sink
     * wf is used to compute edge weights
    */
   static Graph * BuildGraph(MeshType &m, FunctorType &wf, FacePointer startFace, FacePointer endFace)
    {
        vcg::tri::UpdateFlags<MeshType>::FaceClearV(m);
        tri::Allocator<MeshType>::CompactEveryVector(m);
        Graph *g = new Graph();
        std::vector<FacePointer> faceVec;

        //use a per-face attribute to keep the face-node mapping
        //and initialize to NULL
        typename  MeshType::template PerFaceAttributeHandle <Graph::node_id>  nfH
                = tri::Allocator<MeshType>::template AddPerFaceAttribute<Graph::node_id>(m, std::string("NodeID"));
        for(FaceIterator fit = m.face.begin(); fit!=m.face.end(); fit++)
            nfH[fit] = NULL;
        /*
        * Surf the mesh face by face and build the graph
        * using FF adiacency and marking a face when visiting it
        * Start adding nodes to the graph:
        * link upper face to the source
        * link lower face to the sink
        */
        nfH[startFace] = g->add_node();
        g->set_tweights(nfH[startFace], std::numeric_limits<ScalarType>::max(), 0);
        //link lower face to the sink
        nfH[endFace] = g->add_node();
        g->set_tweights(nfH[endFace], 0, std::numeric_limits<ScalarType>::max());
        faceVec.push_back(endFace);
        //push startFace in the vector: begin visiting the mesh from there
        faceVec.push_back(startFace);
        startFace->SetV();

        for(FaceIterator fi=m.face.begin();fi!=m.face.end();++fi)
        {
          if(nfH[fi] == NULL)
            nfH[fi] = g->add_node();
        }

        for(FaceIterator fi=m.face.begin();fi!=m.face.end();++fi)
        {
          for (int i=0; i<3; i++)
          {
            if(!face::IsBorder(*fi,i))
            {
              PosType pf(&*fi, i);
              //using the functor operator () to compute edge capability
              double cap = wf(pf);
              assert(cap>=0);
              g->add_edge(nfH[fi], nfH[fi->FFp(i)], cap, 0);
            }
          }
        }

        // continue adding nodes to the graph
//        while(!faceVec.empty()){
//          FacePointer curFace = faceVec.back();
//          faceVec.pop_back();

//          curFace->SetV();
//          for (int i=0; i<3; i++)
//          {
//            if(!face::IsBorder(*curFace,i))
//            {
//              if(!curFace->FFp(i)->IsV())
//                faceVec.push_back(curFace->FFp(i));

//              //if the attribute is still NULL then i haven't considered the face yet
//              //so i can add a node for it and add it to the vector
//              if(nfH[curFace->FFp(i)] == NULL)
//                nfH[curFace->FFp(i)] = g->add_node();

//              // add edges (in the dual graph) between adjacent faces
//              // an edge is added in both directions with "cap" and "rev_cap"=0 flow capacities
//              // the reverse edge will be added when curFace->FFp(i) will be popped from the stack
//              PosType pf(curFace, i);
//              //using the functor operator () to compute edge capability
//              double cap = wf(pf);
//              assert(cap>=0);
//              g->add_edge(nfH[curFace], nfH[curFace->FFp(i)], cap, 0);
//            }
//          }
//        }

        return g;
    }

    /*
     Red-coloring Source faces and
     Green-coloring Sink faces
    */
   static void ColorFaces(MeshType &m, Graph &g, FacePointer startFace, FacePointer endFace){

       assert(vcg::tri::HasPerFaceAttribute(m,"NodeID"));

       typename MeshType:: template PerFaceAttributeHandle<Graph::node_id> nfH
               = vcg::tri::Allocator<MeshType>::template FindPerFaceAttribute<Graph::node_id>(m,"NodeID");

       for(FaceIterator fit=m.face.begin(); fit!=m.face.end(); fit++)
       {
           assert(nfH[fit]!=NULL);
           if(!fit->IsD()){
               if(g.what_segment(nfH[fit]) == Graph::SOURCE){
                   fit->C()=Color4b::Red;
                   //fit->SetS(); ///could be used to get the new mesh after the mincut
               }
               else if(g.what_segment(nfH[fit]) == Graph::SINK)
                   fit->C()=Color4b::Green;
           }
           if(&*fit == startFace) fit->C()= Color4b::White;
           if(&*fit == endFace) fit->C()= Color4b::Black;
       }
    }
};


} //end namespace
} //end namespace


#endif // GRAPHBUILDER_H
