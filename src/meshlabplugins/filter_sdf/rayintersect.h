#ifndef __RAY_INTERSECT_H_
#define __RAY_INTERSECT_H_

#include "vcg/space/point3.h"
#include <list>
#include <set>
#include "tnt/tnt_array3d.h" // 3D-Grid
#include "common/interfaces.h"

using namespace vcg;
using namespace std;

// USE THE FACE LIST INSTEAD
typedef std::vector<std::vector<int> > cellVector_t;
typedef std::set<int> triangleIndexSet_t;

//forward declaration of mesh class
class CMeshO;


/// A cell holding mesh triangles, searches for intersections of triangle with a given ray
class RayIntersectCell{
private:
    /// hold a vector of triangles contained within this cell
    std::vector<CFaceO*> triangles;
    /// Geometry of the cell used for intersection accelleration
    Point3f m_points[8];
    Plane3f m_planes[6];

public:
    RayIntersectCell(){ }
    ~RayIntersectCell(){ }
    void initGeometry(const Point3f& p1, const double xsize, const double ysize, const double zsize);
    CFaceO* intersects(const Ray3f& ray);
    const Point3f* points() const { return m_points; }
    const Plane3f* planes() const { return m_planes; }
    int size() const{ return triangles.size(); }
};

class RayIntersect {
private:
    /// has the structures been initialized */
    bool m_initialized;
    /// reference to the original mesh (face <==> index)
    CMeshO* m;
    /// the grid holding the divided triangles*/
	TNT::Array3D<RayIntersectCell*> m_grid; 
    /// Geometry of grid accellerator
	double m_xmin,m_xmax,m_ymin,m_ymax,m_zmin,m_zmax;
	double m_xspread,m_yspread,m_zspread;
	double m_diagonal;

private:
    bool findNextCell(Ray3f& ray, int& x, int& y, int& z);
    double ray2planeDistance(Plane3f& plane, Ray3f& ray);
    Point3f index2coordinates(const int x, const int y, const int z);
    void coordinates2index(Point3f& p, int& x, int& y, int& z);
    std::vector<int> makeCellIndexVector(const int x, const int y, const int z){
		std::vector<int> cell;
		cell.push_back(x);
		cell.push_back(y);
		cell.push_back(z);
		return cell;
	}
    void renderSquare(const Point3f& p1, const Point3f& p2, const Point3f& p3, const Point3f& p4);

public:
    RayIntersect( CMeshO* mesh ){
        m_initialized = false;
        this->m = mesh;
    }
	~RayIntersect();

    /// Read all triangles from mesh and put them in search structure
    void Init( CMeshO& mesh, const int gridSize = 20);

    bool isInitialized(){ return m_initialized; }

    /// Queries closest triangle which the ray crosses
    float Query(Ray3f& ray);
    bool Query(Ray3f& ray, float distance){
        distance = Query(ray);
        return distance != FLT_MAX;
    }

	/////////////////////////////////////////////////
	// Debug rendering procedures
	/////////////////////////////////////////////////
    void renderCell(const std::vector<int>& cellIndexes);
    void renderCell(const int x, const int y, const int z);
    void renderAllCells();
};

#endif
