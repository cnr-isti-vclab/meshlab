// ATA2 - MESHLAB STUFF
#include "common/interfaces.h"

#include "RayIntersect.h"
#include <qfile>
// #include <q3textstream>
#include <qdatetime>

void RayIntersectCell::initGeometry(
    const Point3f& p1,
    const double xsize,
    const double ysize,
    const double zsize)
{
	m_points[0] = p1;
  m_points[1] = Point3f(p1[0], p1[1], p1[2] + zsize);
  m_points[2] = Point3f(p1[0], p1[1] + ysize, p1[2]);
  m_points[3] = Point3f(p1[0], p1[1] + ysize, p1[2] + zsize);
  m_points[4] = Point3f(p1[0] + xsize, p1[1], p1[2]);
  m_points[5] = Point3f(p1[0] + xsize, p1[1], p1[2] + zsize);
  m_points[6] = Point3f(p1[0] + xsize, p1[1] + ysize, p1[2]);
  m_points[7] = Point3f(p1[0] + xsize, p1[1] + ysize, p1[2] + zsize);

  // Faces of a cube
  m_planes[0].Init( m_points[7],m_points[3],m_points[1] );
  m_planes[1].Init( m_points[1],m_points[3],m_points[2]);
  m_planes[2].Init( m_points[5],m_points[1],m_points[0]);
  m_planes[3].Init( m_points[7],m_points[5],m_points[4]);
  m_planes[4].Init( m_points[4],m_points[0],m_points[2]);
  m_planes[5].Init( m_points[6],m_points[2],m_points[3]);
}


/**
 *	Searches in the triangles references of the current cell, which triangle
 *  intersects with the ray(closest) and returns its Pointer (or NULL)
 */
CFaceO* RayIntersectCell::intersects(const Ray3f& ray){
  float t,u,v;
  qDebug("This cell contains %d triangles", triangles.size());
  for (unsigned int i=0; i<triangles.size(); i++ ){
    if( vcg::IntersectionRayTriangle<float>(ray, triangles[i]->P(0), triangles[i]->P(1), triangles[i]->P(2), t, u, v) ){
      qDebug("intersect!!");
      return triangles[i];
    }
    else{
      qDebug("  %f %f %f", t,u,v);
    }
  }
  return NULL;
}

RayIntersect::~RayIntersect(){
	//delete m_grid
  for (int x=0;x<m_grid.dim1();x++)
    for (int y=0;y<m_grid.dim2();y++)
      for (int z=0;z<m_grid.dim3();z++)
				delete m_grid[x][y][z];
}

void RayIntersect::Init(CMeshO& mesh, const int gridSize){
  qDebug() << "Initializing the grid accellerator";
	//step 0 - make sure all data structures are empty
  for (int x=0;x<m_grid.dim1();x++)
    for (int y=0;y<m_grid.dim2();y++)
      for (int z=0;z<m_grid.dim3();z++)
        delete m_grid[x][y][z];

  // Bounding geometry
  m_xmin = mesh.bbox.min[0]; m_xmax = mesh.bbox.max[0];
  m_ymin = mesh.bbox.min[1]; m_ymax = mesh.bbox.max[1];
  m_zmin = mesh.bbox.min[2]; m_zmax = mesh.bbox.max[2];
	m_xspread = 1 / (m_xmax - m_xmin + 1e-5);
	m_yspread = 1 / (m_ymax - m_ymin + 1e-5);
	m_zspread = 1 / (m_zmax - m_zmin + 1e-5);
	m_diagonal = sqrt(pow(m_xmax-m_xmin,2)+pow(m_ymax-m_ymin,2)+pow(m_zmax-m_zmin,2));
	double xsize = (double) (m_xmax - m_xmin) / gridSize;
	double ysize = (double) (m_ymax - m_ymin) / gridSize;
	double zsize = (double) (m_zmax - m_zmin) / gridSize;


  // Allocate memory for the grid
  qDebug() << "Allocating grid memory";
  m_grid = TNT::Array3D<RayIntersectCell*>(gridSize, gridSize, gridSize);
  for (int x=0;x<gridSize;x++)
    for (int y=0;y<gridSize;y++)
			for (int z=0;z<gridSize;z++) {
        m_grid[x][y][z] = new RayIntersectCell();
				m_grid[x][y][z]->initGeometry(index2coordinates(x,y,z), xsize, ysize, zsize);
			}


  // Run over triangles and add their triangles to the grid
  qDebug() << "Adding triangles to the grid";
  int xindex, yindex, zindex;
  for(int i=0; i<mesh.fn; i++){
    Point3f pb = Barycenter( mesh.face[i] );
    coordinates2index(pb, xindex, yindex, zindex);
    m_grid[xindex][yindex][zindex]->addTriangle( mesh.face[i] );
  }
}

void RayIntersect::coordinates2index(Point3f& p, int& x, int& y, int& z){
  x = ((double) (p[0] - m_xmin) * m_xspread * m_grid.dim1());
	x = qMin(x,m_grid.dim1()-1);
  y = ((double) (p[1] - m_ymin) * m_yspread * m_grid.dim2());
	y = qMin(y,m_grid.dim2()-1);
  z = ((double) (p[2] - m_zmin) * m_zspread * m_grid.dim3());
	z = qMin(z,m_grid.dim3()-1);
}

float RayIntersect::Query(Ray3f& ray){
	int xindex,yindex,zindex;
  coordinates2index(ray.Origin(), xindex, yindex, zindex);
	
  //--- DESCRIPTION:
  // At the end of this loop, either we found a face and it's stored in
  // "face" or we have cellLegal=false;
  CFaceO* face;
  bool cellLegal = true;
	do {
    qDebug("visiting cell %d %d %d", xindex, yindex, zindex);
    face = m_grid[xindex][yindex][zindex]->intersects(ray);
    if (face == NULL) {
			//what's the new cell, each cell is neighbor with 26 other cells
      if (!findNextCell(ray, xindex, yindex, zindex))
				cellLegal = false;
      else
        if( xindex<0 || yindex<0 || zindex<0 || xindex>m_grid.dim1() || yindex>m_grid.dim2() || zindex>m_grid.dim3() )
          cellLegal = false;
		}
  } while (face==NULL && cellLegal);

  //--- Triangle was found
  // now find distance to triangle then control whether the normal of the
  // triangle and the face normal agree. If not, we might have hit an inner
  // component of the mesh.
  if( face!= NULL ) {
    float t,u,v;
    vcg::IntersectionRayTriangle<float>(ray, face->P(0), face->P(1), face->P(2), t, u, v);
    if( ray.Direction() * face->N() < 0 )
      return FLT_MAX;
    else
      return t;
	}

	return FLT_MAX;
}

bool RayIntersect::findNextCell(Ray3f& ray, int& x, int& y, int& z){
	RayIntersectCell* cell = m_grid[x][y][z];
  Point3f rayVector = ray.Direction();
  bool use_pl1 = (cell->planes()[0].Direction() * rayVector > 0 ? true : false); //otherwise pl5
  bool use_pl3 = (cell->planes()[2].Direction() * rayVector > 0 ? true : false); //o/w pl6
  bool use_pl2 = (cell->planes()[1].Direction() * rayVector > 0 ? true : false);// o/w pl4
  Plane3f test_plane1 = (use_pl1 ? cell->planes()[0] : cell->planes()[4]);
  Plane3f test_plane2 = (use_pl2 ? cell->planes()[1] : cell->planes()[3]);
  Plane3f test_plane3 = (use_pl3 ? cell->planes()[2] : cell->planes()[5]);
  double id2Plane1 = ray2planeDistance(test_plane1, ray);
  double id2Plane2 = ray2planeDistance(test_plane2, ray);
  double id2Plane3 = ray2planeDistance(test_plane3, ray);
	if (id2Plane1 < id2Plane2 && id2Plane1 < id2Plane3) {
		if (use_pl1) z++; else z--;
	} else if (id2Plane2 < id2Plane1 && id2Plane2 < id2Plane3) {
		if (use_pl2) x--; else x++;
	} else {
		if (use_pl3) y--; else y++;
	}
  if (x<0||y<0||z<0|| x>=m_grid.dim1()||y>=m_grid.dim2()||z>=m_grid.dim3())
		return false;

	return true;
}

// A ray is an oriented line, but the VCG only gives an intersection
// point from which I recover the parameters
double RayIntersect::ray2planeDistance(Plane3f& plane, Ray3f& ray){
	double dist = FLT_MAX;
  Point3f interP;
  Line3f line; line.Set( ray.Origin(), ray.Direction() );
  vcg::IntersectionLinePlane<float>( plane, line, interP );
  float t = (interP - ray.Origin()).Norm();
  t = (interP - ray.Origin()).dot( ray.Direction() )>0 ? +t : -t;
  if( t>0 ) dist = t;
  return dist;
}

Point3f RayIntersect::index2coordinates(const int x, const int y, const int z){
  Point3f p(
      m_xmin + ((double) x / m_grid.dim1() * (m_xmax-m_xmin)),
      m_ymin + ((double) y / m_grid.dim2() * (m_ymax-m_ymin)),
      m_zmin + ((double) z / m_grid.dim3() * (m_zmax-m_zmin)));
	return p;
}

void RayIntersect::renderCell(const std::vector<int>& cellIndexes){
	renderCell(cellIndexes[0], cellIndexes[1], cellIndexes[2]);
}

void RayIntersect::renderSquare(const Point3f& p1, const Point3f& p2, const Point3f& p3, const Point3f& p4){
	//render a cell nicely
	glBegin(GL_QUADS);
  glVertex3f(p1[0], p1[1], p1[2]);
  glVertex3f(p2[0], p2[1], p2[2]);
  glVertex3f(p3[0], p3[1], p3[2]);
  glVertex3f(p4[0], p4[1], p4[2]);
	glEnd();
}

void RayIntersect::renderCell(const int x, const int y, const int z){
	RayIntersectCell* cell = m_grid[x][y][z];

	renderSquare(cell->points()[7],cell->points()[3],cell->points()[1],cell->points()[5]);
	renderSquare(cell->points()[1],cell->points()[3],cell->points()[2],cell->points()[0]);
	renderSquare(cell->points()[5],cell->points()[1],cell->points()[0],cell->points()[4]);
	renderSquare(cell->points()[7],cell->points()[5],cell->points()[4],cell->points()[6]);
	renderSquare(cell->points()[4],cell->points()[0],cell->points()[2],cell->points()[6]);
	renderSquare(cell->points()[6],cell->points()[2],cell->points()[3],cell->points()[7]);
}

void RayIntersect::renderAllCells(){
	for (int x=0;x<m_grid.dim1();x++)
		for (int y=0;y<m_grid.dim2();y++)
			for (int z=0;z<m_grid.dim3();z++) 
			{
    renderCell(x,y,z);
  }
}
