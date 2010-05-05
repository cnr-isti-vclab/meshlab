#include <vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/complex/trimesh/update/edges.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/quality.h>
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/intersection.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/space/index/spatial_hashing.h>
#include <vcg/math/matrix33.h>

#include <vcg/space/index/grid_static_ptr.h> // vcg::GridStaticPtr
#include <vcg/space/index/spatial_hashing.h> // vcg::SpatialHashTable

#include <wrap/qt/to_string.h>
#include "filter_sdf.h"

#include "myrandom.h"


//--- Uncomment only one of the two following lines to test different data structures
typedef vcg::GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType> TriMeshGrid;
/* typedef vcg::SpatialHashTable<CMeshO::FaceType, CMeshO::float> TriMeshGrid; */

using namespace std;
using namespace vcg;


void generateRayCone( Ray3f& dir, float aperture, int raysPerCone, vector<Ray3f>& cone, vector<float>& coneSdf ){
  // Reset the vales
  coneSdf.resize(raysPerCone, 0);  
  cone.resize(raysPerCone, Ray3f());  
  
  // TODO
#ifdef SINGLERAY
  cone[0] = dir;
  return
#endif
  
// #define DEBUG_CONERAY
#ifdef DEBUG_CONERAY
  // Tested setting the direction along cardinal axis
  dir.SetDirection(Point3f(1,1,0));
  dir.SetOrigin(Point3f(0,0,0));
  raysPerCone = 1000;
  aperture = math::ToRad(10.0);
  QFile fp("/Users/ata2/Desktop/rays.txt");
  fp.open(QFile::WriteOnly);
  QTextStream out(&fp);
#endif
      
  // Matrix that vector around +x: (phi=0,theta=0) to vectors around current direction
  dir.Normalize();
  Matrix33<float> relToAbs = RotationMatrix( Point3f(1,0,0), dir.Direction() );
  for( int i=0; i<raysPerCone; i++ ){
    // Construct an isotropic bi-variate distribution
    // of "giggling" around +x axis
    float theta_giggle = ranfn(0, aperture);
    float phi_giggle   = ranfn(0, aperture);
    Point3f giggle_v;
    giggle_v.FromPolarRad(1, theta_giggle, phi_giggle);
    
    // Transform the ray back in object's coordinate system
    // and add it to the list 
    Ray3f currRay;
    currRay.SetDirection( relToAbs*giggle_v );
    currRay.SetOrigin( dir.Origin() );
    cone[i] = currRay;
    
#ifdef DEBUG_CONERAY
    out << toString( currRay.Direction() ) << "\n";
#endif 
  }  
  
#ifdef DEBUG_CONERAY
  fp.close();
  exit(0);
#endif  
}

SdfPlugin::SdfPlugin() : SingleMeshFilterInterface("Compute SDF"){}

void SdfPlugin::initParameterSet(MeshDocument&, RichParameterSet& par){
  qDebug() << "called here!";
  QStringList onPrimitive; onPrimitive.push_back("On vertices"); onPrimitive.push_back("On Faces");
  par.addParam( new RichEnum("onPrimitive", 0, onPrimitive, "Metric:", 
                             "Choose whether to trace rays from faces or from vertices. "
                             "Recall that tracing from vertices will use vertex normal "
                             "estimation."));
  par.addParam(new RichFloat("coneWidth", 20, "Cone width: ",
                             "The standard deviation of the rays that will be casted around "
                             "the anti-normals. Remember that most sampled directions are "
                             "expected to fall within 3x this value."));
  par.addParam(  new RichInt("numberRays", 10, "Number of rays: ",
                             "The standard deviation of the rays that will be casted around "
                             "the anti-normals. Remember that most sampled directions are "
                             "expected to fall within 3x this value."));
  par.addParam(new RichFloat("lowQuantile", .1, "Bottom quantile",
                             "We will throw away the set of ray distances for each cone which distance "
                             "value falls under this quantile. Value in between [0,1]. 0 Implies all "
                             "values are kept"));
  par.addParam(new RichFloat("hiQuantile", .9, "Top quantile",
                             "We will throw away the set of ray distances for each cone which distance "
                             "value falls under this quantile. Value in between [0,1]. 1 Implies all "
                             "values are kept"));
}


bool SdfPlugin::applyFilter(MeshDocument& md, RichParameterSet& pars, vcg::CallBackPos* cb){
  enum ONPRIMITIVE{ON_VERTICES, ON_FACES} onPrimitive;
  MeshModel* mm = md.mm();
  CMeshO& m = mm->cm;

  //--- Retrieve parameters
  float widenessRad = math::ToRad(pars.getFloat("coneWidth"));
  int raysPerCone = pars.getInt("numberRays");
  onPrimitive = (ONPRIMITIVE) pars.getInt("onPrimitive");
  float lo01pec = pars.getFloat("lowQuantile");
  float hi01pec = pars.getFloat("hiQuantile");
  assert( onPrimitive==ON_VERTICES && "Face mode not supported yet" );
  
  //--- If on vertices, do some cleaning first
  if( onPrimitive == ON_VERTICES ){
    int dup = tri::Clean<CMeshO>::RemoveDuplicateVertex(m);
    int unref =  tri::Clean<CMeshO>::RemoveUnreferencedVertex(m);
    if (dup > 0 || unref > 0) Log("Removed %i duplicate and %i unreferenced vertices\n",dup,unref);
  }

  //--- Updating mesh metadata
  tri::UpdateBounding<CMeshO>::Box(m);
  tri::UpdateNormals<CMeshO>::PerFaceNormalized(m);
  tri::UpdateNormals<CMeshO>::PerVertexAngleWeighted(m);
  tri::UpdateNormals<CMeshO>::NormalizeVertex(m);
  tri::UpdateFlags<CMeshO>::FaceProjection(m);
    
  //--- Enable & Reset the necessary attributes
  switch(onPrimitive){
    case ON_VERTICES:   
      mm->updateDataMask(MeshModel::MM_VERTQUALITY); 
      tri::UpdateQuality<CMeshO>::VertexConstant(m,0);
      break;
    case ON_FACES:   
      mm->updateDataMask(MeshModel::MM_FACEQUALITY); 
      tri::UpdateQuality<CMeshO>::FaceConstant(m,0);
      break; 
  }
  
  //--- Add the mesh to an indexing structure (fast ray intersection)
  Log("Initializing spatial accelleration...");
  mm->updateDataMask(MeshModel::MM_FACEMARK);
  TriMeshGrid static_grid; //TODO: rename spatial index
  static_grid.Set(m.face.begin(), m.face.end());
  Log("Initializing spatial accelleration... DONE!");
   
  // since we are measuring the interior of the shape
  // A ray should never go beyond this value 
  float maxDist=m.bbox.Diag();
  // This is a small number to avoid self-intersection during ray 
  // casting. It's a very common trick
  float epsilon = maxDist / 10000.0; 

  //--- Ray casting
  vector<Ray3f> cone;
  vector<float> coneSdf;
  Ray3f ray; 
  float t;
  if( onPrimitive == ON_VERTICES ){
    for(int i=0; i<m.vert.size(); i++){
      CVertexO& v = m.vert[i];
      //--- Update progressbar
      cb( i/m.vert.size(), "Casting rays into volume...");
      
      //--- Generate the set of cones
      ray.Set( v.P(), -v.N() );
      ray.SetOrigin( ray.P(epsilon) );
      generateRayCone( ray, widenessRad, raysPerCone, cone, coneSdf );
      
      //--- Trace rays in cone
      float mind = +numeric_limits<float>::max();
      float maxd = -numeric_limits<float>::max();
      for(unsigned int i=0; i<cone.size(); i++){
        bool hasInt = tri::DoRay<CMeshO,TriMeshGrid>(m,static_grid,cone[i],maxDist,t);
        coneSdf[i] = (hasInt==true) ? t : numeric_limits<float>::quiet_NaN();
        mind = (hasInt && (t<mind)) ? t : mind;
        maxd = (hasInt && (t>maxd)) ? t : maxd;
      }
      
      //--- Compute per-cone statistics
      Histogram<float> H;
      H.Clear();
      H.SetRange( mind,maxd, 100);
      for(unsigned int i=0; i<cone.size(); i++)
        if(!math::IsNAN(coneSdf[i]))
          H.Add(coneSdf[i]);
      float loperc = H.Percentile(lo01pec);
      float hiperc = H.Percentile(hi01pec);
      
      //--- Compute average of samples, throwing away outliers
      float totVal = 0, totCnt = 0;
      for(unsigned int i=0; i<coneSdf.size(); i++)
        if( !math::IsNAN(coneSdf[i]) && coneSdf[i]>loperc && coneSdf[i]<hiperc ){
          totVal += coneSdf[i];
          totCnt += 1;
        } 
           
      //--- Save in mesh
      v.Q() = totCnt>0 ? (totVal/totCnt) : 0;
    }
  }
  return true;
}
Q_EXPORT_PLUGIN(SdfPlugin)
