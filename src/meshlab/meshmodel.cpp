#include "meshmodel.h"
#include<vcg/complex/trimesh/update/bounding.h>
#include <QtGlobal>
bool MeshModel::Open(const char *filename)
{
  int mask;
  vcg::tri::io::ImporterPLY<CMeshO>::LoadMask(filename, mask);
  if(mask&ply::PLYMask::PM_VERTQUALITY) printf("Has Vertex Quality\n");
  if(mask&ply::PLYMask::PM_FACEQUALITY) printf("Has Face Quality\n");
  if(mask&ply::PLYMask::PM_FACECOLOR) qDebug("Has Face Color\n");
  if(mask&ply::PLYMask::PM_VERTCOLOR) qDebug("Has Vertex Color\n");
  if(mask&ply::PLYMask::PM_WEDGTEXCOORD) 
  {
    qDebug("Has Wedge Text Coords\n");
    cm.face.EnableWedgeTex();
  }
  int ret=vcg::tri::io::ImporterPLY<CMeshO>::Open(cm,filename);
  qDebug("FAce 0 %f %f \n",cm.face[0].WT(0).u(),cm.face[0].WT(0).v());
	vcg::tri::UpdateBounding<CMeshO>::Box(cm);
	vcg::tri::UpdateNormals<CMeshO>::PerVertex(cm);
  

  return ret==::vcg::ply::E_NOERROR;
}


bool MeshModel::Render(GLW::DrawMode dm, GLW::ColorMode cm)
{
  glw.Draw(dm,cm,GLW::TMNone);
  return true;
}
