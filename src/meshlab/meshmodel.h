/*
model

i dati relativi ad un singolo oggetto.
Ogni oggetto si sa caricare e contiene una mesh
*/

#include <stdio.h>
#include <time.h>

#include<vcg/simplex/vertexplus/base.h>
#include<vcg/simplex/faceplus/base.h>
#include<vcg/simplex/face/topology.h>

#include<vcg/simplex/vertexplus/component_ocf.h>
#include<vcg/simplex/faceplus/component_ocf.h>
#include<vcg/complex/trimesh/base.h>
#include<vcg/complex/trimesh/create/platonic.h>
#include<vcg/complex/trimesh/update/topology.h>
#include<vcg/complex/trimesh/update/flag.h>
#include<vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/refine.h>

#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/gl/trimesh.h>

using namespace vcg;
using namespace std;

class CEdge;    // dummy prototype never used
class CFaceO;
class CVertexO;

// Opt stuff

class CVertexO  : public VertexSimp2< CVertexO, CEdge, CFaceO, vert::Coord3f, vert::Normal3f, vert::BitFlags >{};
class CFaceO    : public FaceSimp2< CVertexO, CEdge, CFaceO, face::InfoOcf, face::FFAdjOcf, face::WedgeTexture2f, face::VertexRef, face::BitFlags, face::Normal3f > {};
class CMeshO    : public vcg::tri::TriMesh< vector<CVertexO>, face::vector_ocf<CFaceO> > {};


class MeshModel
{
public:

  CMeshO cm;
  GlTrimesh<CMeshO> glw;
  MeshModel() {glw.m=&cm;}
  bool Open(const char* filename);
  bool Render(GLW::DrawMode dm, GLW::ColorMode cm);
};