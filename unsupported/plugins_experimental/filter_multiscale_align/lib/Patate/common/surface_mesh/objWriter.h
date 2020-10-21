/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _PATATE_COMMON_SURFACE_MESH_OBJ_WRITER_
#define _PATATE_COMMON_SURFACE_MESH_OBJ_WRITER_


#include <stdexcept>
#include <ostream>
#include <fstream>

#include "surfaceMesh.h"


namespace PatateCommon
{


template < typename _Mesh >
class OBJWriter
{
public:
    typedef _Mesh Mesh;

    typedef typename Mesh::Vector Vector;

public:
    inline OBJWriter();

    void write(std::ostream& out, const Mesh& mesh);
};


}  // namespace Patate

#include "objWriter.hpp"


#endif  // _OBJWRITER_H_
