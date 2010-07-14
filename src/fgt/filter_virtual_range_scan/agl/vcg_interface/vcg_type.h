#ifndef VCG_TYPE_H
#define VCG_TYPE_H

/* *************** this section defines the vcg mesh type **************************** */

#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/vertex/component.h>
#include <vcg/simplex/face/topology.h>
#include <vcg/simplex/face/base.h>
#include <vcg/simplex/face/component.h>

#include <vcg/complex/trimesh/base.h>

using namespace vcg;
using namespace std;

namespace agl
{
    namespace vcg_interface
    {

        class vcg_vertex;    // dummy vertex class
        class vcg_edge;      // dummy edge class
        class vcg_face;      // dummy face class

        // mesh used types
        class my_used_types : public vcg::UsedTypes<
                vcg::Use< vcg_vertex >   ::AsVertexType,
                vcg::Use< vcg_edge >     ::AsEdgeType,
                vcg::Use< vcg_face >     ::AsFaceType >
        {};

        // vertex and face type
        class vcg_vertex     : public vcg::Vertex<
                my_used_types,
                vcg::vertex::Coord3f,
                vcg::vertex::BitFlags,
                vcg::vertex::Normal3f >
        {};

        class vcg_face       : public vcg::Face<
                my_used_types,
                vcg::face::VertexRef,
                vcg::face::Normal3f,
                vcg::face::BitFlags,
                vcg::face::FFAdj >
        {};

        // mesh type
        class vcg_mesh       : public vcg::tri::TriMesh<
                vector< vcg_vertex >,
                vector< vcg_face > >
        {};

    }
}

/* *********************************************************************************** */

#endif // VCG_STUB_H
