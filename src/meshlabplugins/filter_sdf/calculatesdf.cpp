#include <qdatetime.h>
#include <QApplication>
#include <QProgressDialog>

#include "calculatesdf.h"
#include "vcg/math/matrix33.h"
#include "wrap/qt/to_string.h"

Ray3f get_normal_opposite(const Point3f& p, const Point3f& n){
  //direction opposite the normal
  Point3f direction = n * -1;
  //construct segment from vertex pointing towards direction
  Ray3f ray;
  ray.Set( p + direction * 1e-5, direction );
  return ray;
}

// TODO: replace this function with one that actually works
std::list<Ray3f> get_cone_rays( const Point3f& p, const Point3f& n, const float degree, const int spread){
  std::list<Ray3f> list;

  Ray3f ray = get_normal_opposite(p, n);
  Point3f rayv = ray.Direction();
  rayv.normalized();

  qDebug() << "degree: " << degree << "spread: " << spread;
  //now check a cone
  Plane3f plane; plane.Init(ray.Origin(), ray.Direction());
  Point3f base = plane.Direction();

  // DEBUG: this might cause problems... depending how they define the matrix
  // Enriched_kernel::Aff_transformation_3 aff = rotationMatrixAroundVertex(base, degree);
  Matrix33<float> aff = RotationMatrix(base, degree);
  // Enriched_kernel::Aff_transformation_3 aroundRay = rotationMatrixAroundVertex(rayv, 2*M_PI/spread);
  Matrix33<float> aroundRay = RotationMatrix(rayv, 2*M_PI/spread);

  Point3f spinMe = rayv;
  spinMe = aff * spinMe;
  for (int i=0;i<spread; i++){
    Ray3f coneRay( p + spinMe * 1e-5, spinMe);
    list.push_back(coneRay);
    spinMe = aroundRay*spinMe;
  }
  return list;
}

void CalculateSDF::makeFacesNVolume(bool /*smoothing*/, bool /*smoothingAnisotropic*/, int /*smoothingIterations*/){
  // TODO
}

// basically shoot rays opposite the normal and do something with the distances we receive
float CalculateSDF::traceSdfConeAt( const Point3f& p, const Point3f& n ){
  //--- Prepare accelleration metadata
  // This stuff is needed from VCG, I am not too sure what the
  // empty class is for, but the functor has the operator()
  // overloaded so that backfaces ARE intersected as well.
  float rayT, maxDist = std::numeric_limits<float>::max();
  vcg::RayTriangleIntersectionFunctor<true> rayIntersector;
  vcg::EmptyClass a;

  //--- Allocate memory to store the values of rays in the cone
  std::vector<float> values;  // Distance value for a ray
  std::vector<float> weights; // Weight value for a ray
  // +1 is because we always test antinormal as well
  values.reserve( raysInCone + 1 );
  weights.reserve( raysInCone + 1 );

  //--- First check straight opposite to normal
  std::list<Ray3f> list;
  list.push_back( get_normal_opposite(p, n) );

  //--- Generates a cone with a certain aperture angle
  // DEBUG: re-enable this function
#ifndef DEBUG_SDF_SINGLERAY
    list = get_cone_rays(p, n, coneAperture, raysInCone);
#endif

  //--- Trace every ray in the cone computing their weights
  for (std::list<Ray3f>::iterator it = list.begin(); it != list.end(); it++){
      CFaceO* isectFace = sIndex.DoRay(rayIntersector, a, (*it), maxDist, rayT);
      if( isectFace != NULL ){
        values.push_back(rayT);
        weights.push_back(1);
      }
#ifdef DEBUG_SDF
      qDebug() << " Ray: " << toString((*it).Origin()) << ";" << toString((*it).Direction())
               << " intersected face: " << tri::Index(*mesh, isectFace)
               << " at distance: " << rayT;
#endif
  }

#ifdef DEBUG_SDF_SINGLERAY
    return rayT>100 ? 100 : rayT;
#endif

// Original Shapira's code for statistics
#if false
  //calculate the average
  for(unsigned int i=0; i<values.size();i++) {
    distanceCounter += values[i] * weights[i];
    intersectionCounter += weights[i];
  }

  if(intersectionCounter) {
    result = distanceCounter / intersectionCounter;

    //fix really insane values
    std::nth_element(values.begin(), values.begin()+(values.size()/2), values.end());
    float median = values[values.size()/2];

    float std_dev = 0.0;
    for(unsigned int i=0; i<values.size();i++) {
      std_dev += pow(values[i]-result, 2);
    }
    std_dev = sqrt(std_dev/values.size());
    distanceCounter = intersectionCounter = 0.0;

    for(unsigned int i=0; i<values.size(); i++) {
      if (fabs(values[i]-/*result*/median) <= 0.5 * std_dev) {
        distanceCounter += values[i] * weights[i];
        intersectionCounter += weights[i];
      }
    }

    if (intersectionCounter)
      result = distanceCounter / intersectionCounter;
    else
      result = 0.0;
  } else {
    result = 0.0;
  }
  return result;
#endif
}

void CalculateSDF::init(SDFMODE mode, vector<float>& results){
  //--- Prepare the accelleration data structures
  // m_rayIntersect->Init(*mesh, gridsize); // OBSOLETE
  sIndex.Set(mesh->face.begin(), mesh->face.end());

  if(mode == FACES){
    //--- Allocate the memory
    origins = vector<Ray3f>( mesh->fn );
    results.resize( mesh->fn,0 );

    //--- Initialize the query (face center/normal)
    for(int i=0; i<mesh->fn; i++)
      origins[i].Set( Barycenter(mesh->face[i]), mesh->face[i].N() );
  }
  else
    assert(0);
}
void CalculateSDF::compute(vector<float>& results, vcg::CallBackPos* pb){
  qDebug() << "Expected completion: " << origins.size();

  //--- Compute the queries
  for(unsigned int i=0; i<origins.size(); i++){
    if(pb) pb((i+1)/origins.size(), "Tracing rays in the volume");
    results[i] = traceSdfConeAt( origins[i].Origin(), origins[i].Direction() );
  }
  //--- Only apply smoothing if on vertices
  // if (!postprocess(mesh, onVertices, results, normalize, smoothing && onVertices))
  //      return false;

  //--- perform smoothing on the mesh itself
  // makeFacesNVolume(smoothing, smoothingAnisotropic, smoothingIterations );
}
