/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "objWriter.h"


namespace PatateCommon
{


template < typename _Mesh >
OBJWriter<_Mesh>::OBJWriter()
{
}

template < typename _Mesh >
void
OBJWriter<_Mesh>::write(std::ostream& out, const Mesh& mesh)
{
    out.imbue(std::locale::classic());

    //vertices
    for (typename Mesh::VertexIterator vit = mesh.verticesBegin();
         vit != mesh.verticesEnd(); ++vit)
    {
        out << "v " << mesh.position(*vit).transpose() << "\n";
    }

    //faces
    for (typename Mesh::FaceIterator fit = mesh.facesBegin();
         fit != mesh.facesEnd(); ++fit)
    {
        out << "f";
        typename Mesh::VertexAroundFaceCirculator
                fvit  = mesh.vertices(*fit),
                fvend = fvit;
        do
        {
            out << " " << (*fvit).idx()+1;
        }
        while (++fvit != fvend);
        out << "\n";
    }
}


}
