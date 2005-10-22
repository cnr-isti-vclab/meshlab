#include "meshmodel.h"
#include<vcg/complex/trimesh/update/bounding.h>

bool MeshModel::Open(const char *filename)
{
  int mask;
  vcg::tri::io::ImporterPLY<CMeshO>::LoadMask(filename, mask);
  if(mask&ply::PLYMask::PM_VERTQUALITY) printf("Has Vertex Quality\n");
  if(mask&ply::PLYMask::PM_FACEQUALITY) printf("Has Face Quality\n");
  if(mask&ply::PLYMask::PM_FACECOLOR) printf("Has Face Color\n");
  if(mask&ply::PLYMask::PM_VERTCOLOR) printf("Has Vertex Color\n");

  int ret=vcg::tri::io::ImporterPLY<CMeshO>::Open(cm,filename);
  
	vcg::tri::UpdateBounding<CMeshO>::Box(cm);
  

  return ret==::vcg::ply::E_NOERROR;
}


bool MeshModel::Render(GLW::DrawMode dm, GLW::ColorMode cm)
{
  glw.Draw(dm,cm,GLW::TMNone);
  return true;
}
