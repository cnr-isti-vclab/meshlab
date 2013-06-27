#include "../../external/maxflow-v2.21.src/adjacency_list/graph.h"
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <math.h>
#include <vcg/space/point3.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/import_ply.h>
#include <iostream>

#ifndef GRAPHBUILDER_H
#define GRAPHBUILDER_H

namespace vcg {
namespace tri {

//the class is templated also on the FunctType, that is used to compute edge weights
//so the dual graph can be built assigning other weight values.
template<class MeshType, class FunctType>
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
    typedef typename face::Pos<FaceType>    PosType;

    /*main function to be called from outside: builds the graph and fires the maxflow/mincut alogrith*/
    static void Compute(MeshType &m, FunctType &wf, FacePointer startFace, FacePointer endFace)
    {
        Graph *g = GraphBuilder<MeshType, FunctType>::BuildGraph(m, wf, startFace, endFace);
        g->maxflow();
        GraphBuilder<MeshType, FunctType>::ColorFaces(m, *g);
        delete(g);
    }

    /* Builds the dual graph.
     * the startFace will be connected to the source
     * the endFace will be connected to the sink
     * wf is used to compute edge weights
    */
   static Graph * BuildGraph(MeshType &m, FunctType &wf, FacePointer startFace, FacePointer endFace)
    {
        vcg::tri::UpdateFlags<MeshType>::Clear(m);
        Graph *g = new Graph();
        int count=0;
        std::vector<FacePointer> faceVec;

        //allocate a per-face attribute to keep the face-node mapping
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
        //the weight of the edge connecting the startFace with the source is set as float_max
        g->set_tweights(nfH[startFace], std::numeric_limits<float>::max(), 0);
        //link lower face to the sink
        nfH[endFace] = g->add_node();
        g->set_tweights(nfH[endFace], 0, std::numeric_limits<float>::max());
        count++;
        faceVec.push_back(endFace);
        //push startFace in the vector: begin visiting the mesh from there
        faceVec.push_back(startFace);
        startFace->SetV();

        FacePointer curFace;
        // continue adding nodes to the graph
        while(!faceVec.empty()){
            curFace = faceVec.back();
            faceVec.pop_back();
            curFace->SetV();
            for (int i=0; i<3; i++)
            {
                if(curFace->FFp(i)!=curFace)
                {
                    if(!curFace->FFp(i)->IsV())
                        faceVec.push_back(curFace->FFp(i));

                    //if the attribute is still NULL then i haven't considered the face yet
                    //so i can add a node for it and add it to the vector
                    if(nfH[curFace->FFp(i)] == NULL)
                        nfH[curFace->FFp(i)] = g->add_node();

                    // add edges (in the dual graph) between adjacent faces
                    // an edge is added in both directions with "cap" and "rev_cap"=0 flow capacities
                    // the reverse edge will be added when curFace->FFp(i) will be popped from the stack
                    PosType pf(curFace, i);
                    //using the functor operator () to compute edge capability
                    double cap = wf(pf);
                    assert(cap>=0);
                    g->add_edge(nfH[curFace], nfH[curFace->FFp(i)], cap, 0);
                }
            }
        }

        return g;
    }

    /*
     Red-coloring Source faces and
     Green-coloring Sink faces
    */
    static void ColorFaces(MeshType &m, Graph &g){

        typename MeshType:: template PerFaceAttributeHandle<Graph::node_id> nfH
                = vcg::tri::Allocator<MeshType>::template FindPerFaceAttribute<Graph::node_id>(m,"NodeID");

        for(FaceIterator fit=m.face.begin(); fit!=m.face.end(); fit++)
        {
            assert(nfH[fit]!=NULL);
            if(g.what_segment(nfH[fit]) == Graph::SOURCE)
                fit->C()=Color4b::Red;
            else if(g.what_segment(nfH[fit]) == Graph::SINK)
                fit->C()=Color4b::Green;
        }
    }
};


} //end namespace
} //end namespace


#endif // GRAPHBUILDER_H
