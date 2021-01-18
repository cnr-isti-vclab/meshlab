#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/edges.h>
#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/quality.h>
#include <vcg/complex/algorithms/update/color.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/intersection.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/space/index/spatial_hashing.h>
#include <vcg/math/matrix33.h>

#include <vcg/space/index/grid_static_ptr.h> // vcg::GridStaticPtr
#include <vcg/space/index/grid_closest.h> // Closest queries
#include <vcg/space/index/spatial_hashing.h> // vcg::SpatialHashTable

#include <wrap/qt/to_string.h>
#include "filter_sdf.h"

#include "mysampling.h"


//--- Uncomment only one of the two following lines to test different data structures
typedef vcg::GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType> TriMeshGrid;
/* typedef vcg::SpatialHashTable<CMeshO::FaceType, CMeshO::float> TriMeshGrid; */

using namespace std;
using namespace vcg;

SdfPlugin::SdfPlugin() : SingleMeshFilterInterface("Compute SDF"){}

void SdfPlugin::initParameterSet(MeshDocument&, RichParameterSet& par){
  QStringList onPrimitive; onPrimitive.push_back("On vertices"); onPrimitive.push_back("On Faces");
  par.addParam( new RichEnum("onPrimitive", 0, onPrimitive, "Metric:", 
                             "Choose whether to trace rays from faces or from vertices. "
                             "Recall that tracing from vertices will use vertex normal "
                             "estimation."));
  par.addParam(new RichFloat("coneWidth", 20, "Cone width: ",
                             "The standard deviation of the rays that will be casted around "
                             "the anti-normals. Remember that most sampled directions are "
                             "expected to fall within 3x this value."));
  par.addParam(  new RichInt("numberRays", 50, "Number of rays: ",
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
  onPrimitive = (ONPRIMITIVE) pars.getEnum("onPrimitive");
  qDebug() << "which primitive?" << onPrimitive;
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
      // qDebug() << "initializing vert quality";
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
  for(unsigned int i=0; i<m.vert.size(); i++){
    CVertexO& v = m.vert[i];
    //--- Update progressbar
    cb( i/m.vert.size(), "Casting rays into volume...");
    
    //--- Generate the set of cones
    ray.Set( v.P(), -v.N() );
    ray.SetOrigin( ray.P(epsilon) );
    generateRayCone( ray, widenessRad, raysPerCone, cone, coneSdf, (i==266) );
        
    //--- Trace rays in cone
    float mind = +numeric_limits<float>::max();
    float maxd = -numeric_limits<float>::max();
    for(unsigned int j=0; j<cone.size(); j++){
      bool hasInt = tri::DoRay<CMeshO,TriMeshGrid>(m,static_grid,cone[j],maxDist,t);
      coneSdf[j] = (hasInt==true) ? t : numeric_limits<float>::quiet_NaN();
      mind = (hasInt && (t<mind)) ? t : mind;
      maxd = (hasInt && (t>maxd)) ? t : maxd;

      if( i==266 ){
        qDebug() << " sampled: " << coneSdf[j] 
                 << " dir: " << toString(cone[j].Direction())
                 << " hasInt: " << hasInt;
      }
    }
    
    //--- Compute per-cone statistics
    Histogram<float> H;
    H.Clear();
    H.SetRange( mind, maxd, 100);
    for(unsigned int j=0; j<cone.size(); j++)
      if(!math::IsNAN(coneSdf[j]))
        H.Add(coneSdf[j]);
    float loperc = H.Percentile(lo01pec);
    float hiperc = H.Percentile(hi01pec);

    if( i == 266){
      qDebug() << "percentiles: " << loperc << " " << hiperc;
    }
    
    
    //--- Compute average of samples, throwing away outliers
    if( i == 266) 
      qDebug() << "averaging samples";
    float totVal = 0, totCnt = 0;
    for(unsigned int j=0; j<coneSdf.size(); j++)
      if( !math::IsNAN(coneSdf[j]) ){
        // Histogram statistics valid only for dense sets (more 3 members..)
        if( coneSdf[j]<loperc || coneSdf[j]>hiperc && coneSdf.size()>3) 
          continue;
        
        // Weight giving more importance to aligned samples
        float weight = cone[j].Direction().dot( ray.Direction() );
        // Even more importance
        weight = powf( weight, 10 );
        
        if( i==266 ){
          qDebug() << "sampled: " << coneSdf[j] << "weight " << weight 
                   << " dir:" << toString(cone[j].Direction());
        }
        
        totVal += weight*coneSdf[j];
        totCnt += weight;
      } 
         
    //--- Save in mesh
    v.Q() = totCnt>0 ? (totVal/totCnt) : 0;
  }

  //----------------------------------------------------------------------------//
  // 
  //                          STEROIDS STARTS HERE 
  //
  //----------------------------------------------------------------------------//
  //--- Create the medial cloud by offsetting the samples of the medial amount
  MeshModel* medCloud = md.addNewMesh("medial cloud.obj", NULL, false);
  for(unsigned int i=0; i<m.vert.size(); i++){
    Ray3f r;
    r.Set(m.vert[i].P(), -m.vert[i].N());
    tri::Allocator<CMeshO>::AddVertices(medCloud->cm,1);
    medCloud->cm.vert.back().P() = r.P(m.vert[i].Q() / 2 );
  }
  
  //--- Data for distance queries
  vcg::tri::FaceTmark<CMeshO> mf; 
  mf.SetMesh( &m );
  vcg::face::PointDistanceBaseFunctor<float> PDistFunct;
    
  Log("querying real distances");
  
  // Query the location of closest point on the mesh, then measure the difference
  float allowedDiff = .02;
  vector<float> realdistances(m.vn, 0);
  for(unsigned int i=0; i<m.vert.size(); i++){
    Point3f currp = medCloud->cm.vert[i].P();
    float minDist = maxDist;
    Point3f closest;  
    GridClosest(static_grid, PDistFunct, mf, currp, maxDist, minDist, closest);  
    float difference = m.vert[i].Q()/2.0 - minDist;
    m.vert[i].Q() = exp( -powf(difference/allowedDiff,2) );
  }
 
  // Correct the viewmodel so that after this is done the original mesh
  // is shown in wireframe and the medial as a cloud.
  // mm->glw.cdm = vcg::GLW::DMWire; // show original mesh in wireframe
  medCloud->glw.cdm = vcg::GLW::DMPoints; // show cloud
  return true;
}
Q_EXPORT_PLUGIN(SdfPlugin)
