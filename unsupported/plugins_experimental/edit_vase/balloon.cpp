#include "balloon.h"
#include "float.h"
#include "math.h"
#include "vcg/complex/allocate.h"           // AddPerVertexAttribute
#include "vcg/complex/algorithms/update/selection.h"
#include "vcg/complex/algorithms/update/color.h"
#include "vcg/complex/algorithms/update/curvature.h"
#include "vcg/complex/algorithms/update/curvature_fitting.h" // Quadric based curvature computation
using namespace vcg;

//---------------------------------------------------------------------------------------//
//
//                                   LOGIC
//
//---------------------------------------------------------------------------------------//
void Balloon::init( int gridsize, int gridpad ){
    //--- Reset the iteration counter
    numiterscompleted = 0;
    isCurvatureUpdated = false;
    isDistanceFieldInit = false;
    isDistanceFieldUpdated = false;
    isWeightFieldUpdated = false;

    //--- Instantiate a properly sized wrapping volume
    vol.init( gridsize, gridpad, cloud.bbox );
    qDebug() << "Created a volume of sizes: " << vol.size(0) << " " << vol.size(1) << " " << vol.size(2);

    //--- Compute hashing of ray intersections (using similar space structure of volume)
    gridAccell.init( vol, cloud );
    qDebug() << "Finished hashing rays into the volume";

    //--- Construct EDF of initial wrapping volume (BBOX)
    // Instead of constructing isosurface exactly on the bounding box, stay a bit large,
    // so that ray-isosurface intersections will not fail for that region.
    // Remember that rays will take a step JUST in their direction, so if they lie exactly
    // on the bbox, they would go outside. The code below corrects this from happening.
    Box3f enlargedbb = cloud.bbox;
    // float del = .99*vol.getDelta(); // almost +1 voxel in each direction
    float del = 1.99*vol.getDelta(); // USED FOR DEBUG
    
    // float del = .50*vol.getDelta(); // ADEBUG: almost to debug correspondences
    Point3f offset( del,del,del );
    enlargedbb.Offset( offset );
    vol.initField( enlargedbb );  // init volumetric field with the bounding box
    
    //--- Extract initial zero level set surface
    vol.isosurface( surf, 0 ); // qDebug() << "Extracted balloon isosurface (" << surf.vn << " " << surf.fn << ")";
    //--- Clear band for next isosurface, clearing the corresponding computation field
    for(unsigned int i=0; i<vol.band.size(); i++){
        Point3i& voxi = vol.band[i];
        MyVoxel& v = vol.Voxel(voxi);
        v.status = 0;
        v.face = 0;
        v.index = 0;
        v.field = NAN;
    }

    //--- Create extra space to store temporary variables
    if( !vcg::tri::HasPerVertexAttribute (surf,std::string("Viewpoint distance field")) )
        surf_df = vcg::tri::Allocator<CMeshO>::AddPerVertexAttribute<float>(surf, "Viewpoint distance field");
    if( !vcg::tri::HasPerVertexAttribute (surf,std::string("Data support weight field")) )
        surf_wf = vcg::tri::Allocator<CMeshO>::AddPerVertexAttribute<float>(surf, "Data support weight field");

    //--- Update correspondences & band
    vol.band.clear();
    vol.band.reserve(5*surf.fn);
    vol.updateSurfaceCorrespondence( surf, gridAccell, 2*vol.getDelta() );
}

bool Balloon::init_fields(){
    //-----------------------------------------------------------------------------------------------------------//
    //                                     SETUP THE INTERPOLATOR SYSTEM
    //
    //-----------------------------------------------------------------------------------------------------------//
    surf.face.EnableQuality();
    tri::UpdateQuality<CMeshO>::FaceConstant(surf, 0);
    tri::UpdateSelection<CMeshO>::AllFace(surf);
    bool op_succeed = true;
    op_succeed &= dinterp.Init( &surf, 1, COTANGENT );
    op_succeed &= winterp.Init( &surf, 1, COTANGENT );
    if( !op_succeed ){
        dinterp.ColorizeIllConditioned( COTANGENT );
        return false;
    }

    //-----------------------------------------------------------------------------------------------------------//
    //                                     INIT PSEUDO-HEAT WEIGHT SYSTEM
    //
    // Assign a value of 0 to all vertices of the mesh. A value of 0 will be associated to parts of the mesh
    // which are considered "close" to data. Interpolating the field in a smooth way will end up generating
    // an heat-like scalar field distribution on the surface.
    //-----------------------------------------------------------------------------------------------------------//
    {
        for(CMeshO::VertexIterator vi=surf.vert.begin();vi!=surf.vert.end();vi++)
            winterp.AddConstraint( tri::Index(surf,*vi), OMEGA_WEIGHT_FIELD, 1 );
    }
    //-----------------------------------------------------------------------------------------------------------//
    //                                   ASSIGN A "MINIMAL" FACE TO EACH RAY
    //
    // In this first phase, we scan through faces and we update the information contained in the rays. We try to
    // have a many-1 correspondence in between rays and faces: each face can have more than one ray, but one ray
    // can only have one face associated with it. This face can either be behind or in front of the ray
    // startpoint. In between all the choices available, we choose the face which is closest to the startpoint
    //-----------------------------------------------------------------------------------------------------------//
    {
        // Ray-Triangle intersection parameters
        float t,u,v;
        // Clear the meta-data for correspondence
        for( unsigned int i=0; i<gridAccell.rays.size(); i++ ){
            gridAccell.rays[i].f = NULL;
            gridAccell.rays[i].t = +FLT_MAX;
        }
        // Assign one face to every ray
        for(CMeshO::FaceIterator fi=surf.face.begin();fi!=surf.face.end();fi++){
            CFaceO& f = *(fi);
            //--- Clear quality and selection flags
            f.ClearS();
            f.Q() = 0;
            //--- Store closest face to ray
            for(gridAccell.iter.first(f); !gridAccell.iter.isDone(); gridAccell.iter.next()){
                PokingRay& pray = gridAccell.iter.currentItem();
                Line3<float> line(pray.ray.Origin(), pray.ray.Direction());
                if( IntersectionLineTriangle(line, f.P(0), f.P(1), f.P(2), t, u, v) ){
                    if( pray.f==NULL || fabs(t)<fabs(pray.t) ){
                        pray.f=&f;
                        pray.t=t;
                    }
                }
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------//
    //                                  ASSIGN WEIGHT FROM EACH RAY TO FACES
    //
    // 1) visit the face and we set the field constraint imposed by the poking ray
    // 2) every intersection which is very close to the data generates a confidence contribution.
    //
    // NOTE: The use of vcg::Simplify to remove degenerate triangles and any bug in the DDR iterator could cause a ray
    // to fail to detect an intersection. If this takes place, a warning message is displayed and the insertion
    // of the constraint gets skipped.
    //-----------------------------------------------------------------------------------------------------------//
    {
        // Ray-Triangle intersection parameters
        float t,u,v;
        // Debug: total weight to take average of dist on face and display it
        std::vector<float> tot_w( surf.fn, 0 );
        for(unsigned int i=0; i<gridAccell.rays.size(); i++){
            //--- Retrieve the corresponding face and signed distance
            Ray3f& ray = gridAccell.rays[i].ray;
            if( gridAccell.rays[i].f == NULL){
                qDebug() << "warning: ray #" << i << "has provided NULL intersection"; // << toString(ray.Origin()) << " " << toString(ray.Direction());
                continue;
            }
            CFaceO& f = *(gridAccell.rays[i].f);
            t = gridAccell.rays[i].t;
            assert( !math::IsNAN(t) );

            // Color the faces, if more than one, take average
            tot_w[ tri::Index(surf,f) ]++;
            f.Q() += t; // normalize with tot_w after
            f.SetS(); // enable the face for coloring

            // I was lazy and didn't store the u,v... we need to recompute them
            vcg::IntersectionRayTriangle<float>(ray, f.P(0), f.P(1), f.P(2), t, u, v);
            assert( u>=0 && u<=1 && v>=0 && v<=1 );

            //--- Add the barycenter-weighted constraints to the vertices of the face
            dinterp.AddConstraint( tri::Index(surf,f.V(0)), OMEGA_VIEW_FIELD*(1-u-v), t );
            dinterp.AddConstraint( tri::Index(surf,f.V(1)), OMEGA_VIEW_FIELD*(u), t );
            dinterp.AddConstraint( tri::Index(surf,f.V(2)), OMEGA_VIEW_FIELD*(v), t );

            //--- Whenever we have an intersection, we have data
            winterp.AddConstraint( tri::Index(surf,f.V(0)), OMEGA_WEIGHT_FIELD*(1-u-v), 0 );
            winterp.AddConstraint( tri::Index(surf,f.V(1)), OMEGA_WEIGHT_FIELD*(u),     0 );
            winterp.AddConstraint( tri::Index(surf,f.V(2)), OMEGA_WEIGHT_FIELD*(v),     0 );
        }
        //--- Normalize in case there is more than 1 ray per-face
        for(CMeshO::FaceIterator fi=surf.face.begin();fi!=surf.face.end();fi++){
            if( tot_w[ tri::Index(surf,*fi) ] > 0 )
                fi->Q() /= tot_w[ tri::Index(surf,*fi) ];
        }
    }

    isDistanceFieldInit = true;
    return true;
}

bool Balloon::interp_fields(){
    // Cannot interpolate if computation has failed
    if( isDistanceFieldInit == false )
        return false;

    isDistanceFieldUpdated = true;
    dinterp.SolveInAttribute( surf_df );

    isWeightFieldUpdated = true;
    winterp.SolveInAttribute( surf_wf );
}

bool Balloon::compute_curvature(){
    isCurvatureUpdated = true;
    tri::UpdateCurvatureFitting<CMeshO>::computeCurvature( surf );
    for(CMeshO::VertexIterator vi = surf.vert.begin(); vi != surf.vert.end(); ++vi){
        (*vi).Kh() = ( (*vi).K1() + (*vi).K2() ) / 2;
    }
    return true;
}

bool Balloon::evolve(){
    numiterscompleted++;
    std::vector<float> updates_view(vol.band.size(),0);
    std::vector<float> updates_whgt(vol.band.size(),0);
    std::vector<float> updates_curv(vol.band.size(),0);
    float view_max_absdst = -FLT_MAX;
    float view_max_dst    = -FLT_MAX;
    float view_min_dst    = +FLT_MAX;
    float view_min_weight = +FLT_MAX;
    float view_max_weight = -FLT_MAX;
    float curv_maxval     = -FLT_MAX;

    // Only meaningful if it has been computed..
    qDebug("Delta: %f", vol.getDelta());

    //-----------------------------------------------------------------------------------------------------------//
    //                          1) TRANSFER PROPERTIES FROM VERTICES TO SURROUNDING GRID BAND
    //
    // We have a band of voxels around the current surface. In this first step we transfer the information stored
    // on the surface to the surrounding voxels. First, we project the voxel center onto the explicit surface.
    // Once there, we estimate where within the corresponding face we are located, and we interpolate the field
    // values accordingly. We also keep track of the ranges so that we can determine which elements
    //-----------------------------------------------------------------------------------------------------------//
    {
        Point3f c;      // barycentric coefficients
        Point3f voxp;
        for(unsigned int i=0; i<vol.band.size(); i++){
            Point3i& voxi = vol.band[i];
            MyVoxel& v = vol.Voxel(voxi);
            CFaceO& f = *v.face;

            // extract projected points on surface and obtain barycentric coordinates
            // TODO: double work, it was already computed during correspodence, write a new function?
            Point3f proj;
            vol.off2pos(voxi, voxp);
            vcg::SignedFacePointDistance(f, voxp, proj);
            Triangle3<float> triFace( f.P(0), f.P(1), f.P(2) );

            // Paolo, is this really necessary?
            int axis;
            if     (f.Flags() & CFaceO::NORMX )   axis = 0;
            else if(f.Flags() & CFaceO::NORMY )   axis = 1;
            else                                  axis = 2;
            vcg::InterpolationParameters(triFace, axis, proj, c);

            if( isDistanceFieldUpdated ){
                updates_view[i] = c[0]*surf_df[f.V(0)] + c[1]*surf_df[f.V(1)] + c[2]*surf_df[f.V(2)];
                view_max_absdst = (fabs(updates_view[i])>view_max_absdst) ? fabs(updates_view[i]) : view_max_absdst;
                view_max_dst = updates_view[i]>view_max_dst ? updates_view[i] : view_max_dst;
                view_min_dst = updates_view[i]<view_min_dst ? updates_view[i] : view_min_dst;

            }
            if( isWeightFieldUpdated ){
                updates_whgt[i] = c[0]*surf_wf[f.V(0)] + c[1]*surf_wf[f.V(1)] + c[2]*surf_wf[f.V(2)];
                view_min_weight = updates_whgt[i]<view_min_weight ? updates_whgt[i] : view_min_weight;
                view_max_weight = updates_whgt[i]>view_max_weight ? updates_whgt[i] : view_max_weight;
            }
            if( isCurvatureUpdated ){
                updates_curv[i] = c[0]*f.V(0)->Kh() + c[1]*f.V(1)->Kh() + c[2]*f.V(2)->Kh();
                curv_maxval = (fabs(updates_curv[i])>curv_maxval) ? fabs(updates_curv[i]) : curv_maxval;
            }
        }

        if( isWeightFieldUpdated )
            qDebug("weight   field: min %.3f max %.3f", view_min_dst, view_max_dst);
        if( isDistanceFieldUpdated )
            qDebug("distance field: min %.3f max %.3f", view_min_weight, view_max_weight);
        if( isCurvatureUpdated )
            qDebug("curvat   field:          max %.3f", curv_maxval);
    }


    //-----------------------------------------------------------------------------------------------------------//
    //                          2) COMBINE THE VARIOUS UPDATES MEANINGFULLY
    // TODO
    //
    //-----------------------------------------------------------------------------------------------------------//
    {
        // Max evolution speed is proportional to grid size
        float max_speed = vol.getDelta()/2;
        // bound on energy balance: E = E_view + alpha*E_smooth
        float alpha = .5;
        // small (slows down) when worst case scenario is approaching grid size
        float sigma2 = vol.getDelta()*vol.getDelta();
        float k_notconverging = 1 - exp( -powf(view_max_absdst,2) / sigma2 );
        // high  (speed up  ) when vertex is far away from surface
        float k_highdist;
        // high  (speed up  ) when vertex has high local curvature
        float k_highcurv;

        // For every element of the active band
        for(unsigned int i=0; i<vol.band.size(); i++){
            Point3i& voxi = vol.band[i];
            MyVoxel& v = vol.Voxel(voxi);

            //--- If I know current distance avoid over-shooting, as maximum speed is bound to dist from surface
            if( isDistanceFieldUpdated  ){
                max_speed = std::min( vol.getDelta()/2, std::abs(updates_view[i]) );
                k_highdist = exp( -powf(fabs(updates_view[i])-view_max_absdst,2) / sigma2 );
                v.sfield += vcg::sign( max_speed*k_highdist*k_notconverging, updates_view[i] );
            }
            //--- Curvature weight (faster if spiky)
            if( isCurvatureUpdated ){
                k_highcurv = sign(1.0f,updates_curv[i])*exp(-powf(fabs(updates_curv[i])-curv_maxval,2)/curv_maxval);

                // If a "support" field is specified
                if( isWeightFieldUpdated ){
                    v.sfield += updates_whgt[i]*alpha*max_speed*k_highcurv;
                } else {
                    v.sfield += max_speed*k_highcurv;
                }
            }
        }

        //--- show what's being updated
        if( isDistanceFieldUpdated && isCurvatureUpdated && isWeightFieldUpdated )
            qDebug() << "update: E_vd + alpha*E_kh";
        else if( isDistanceFieldUpdated && isCurvatureUpdated )
            qDebug() << "update: E_vd + kost*E_kh";
        else if( isDistanceFieldUpdated )
            qDebug() << "update: E_vd";
        else if( isCurvatureUpdated )
            qDebug() << "update: E_kh";
    }



    //-----------------------------------------------------------------------------------------------------------//
    //                                     3) EXTRACT THE NEW EXPLICIT SURFACE
    // TODO
    //
    //-----------------------------------------------------------------------------------------------------------//
    {
        //--- Extract isosurface
        vol.isosurface( surf, 0 );

        //--- Clear band for next isosurface, clearing the corresponding computation field
        for(unsigned int i=0; i<vol.band.size(); i++){
            Point3i& voxi = vol.band[i];
            MyVoxel& v = vol.Voxel(voxi);
            v.status = 0;
            v.face = 0;
            v.index = 0;
            v.field = NAN;
        }
        vol.band.clear();
        //--- Update correspondences & band
        vol.band.reserve(5*surf.fn);
        vol.updateSurfaceCorrespondence( surf, gridAccell, 2*vol.getDelta() );
        //--- Nothing is updated anymore!!
        isWeightFieldUpdated   = false;
        isDistanceFieldUpdated = false;
        isCurvatureUpdated     = false;
        isDistanceFieldInit    = false;
    }

    return true;
}

//---------------------------------------------------------------------------------------//
//
//                                   RENDERING
//
//---------------------------------------------------------------------------------------//
/// Transfer the average distance stored in face quality to a color for triangles belonging to a certain selection
void Balloon::selectedFacesQualityToColor(){
    this->rm ^= SURF_VCOLOR; // disable vertex color
    this->rm |= SURF_FCOLOR; // enable face color
    surf.face.EnableColor();
    tri::UpdateColor<CMeshO>::FaceConstant(surf, Color4b::White);
    tri::UpdateColor<CMeshO>::FaceQualityRamp(surf, true);
}


void Balloon::wfieldToVertexColor(){
    // Enable vertex coloring
    rm &= ~SURF_FCOLOR;
    rm |=  SURF_VCOLOR;

    // Transfer from wfield to vertex quality
    for( CMeshO::VertexIterator vi=surf.vert.begin(); vi!=surf.vert.end(); vi++ )
        (*vi).Q() = surf_wf[vi];

    // Build histogram and map range to colors
    Histogram<float> H;
    tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(surf,H);
    tri::UpdateColor<CMeshO>::VertexQualityRamp(surf,H.Percentile(0.0f),H.Percentile(1.0f));
}
void Balloon::dfieldToVertexColor(){
    // Enable vertex coloring
    rm &= ~SURF_FCOLOR;
    rm |=  SURF_VCOLOR;

    // Transfer from dfield to vertex quality
    for( CMeshO::VertexIterator vi=surf.vert.begin(); vi!=surf.vert.end(); vi++ )
        (*vi).Q() = surf_df[vi];

    // Build histogram and map range to colors
    Histogram<float> H;
    tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(surf,H);
    tri::UpdateColor<CMeshO>::VertexQualityRamp(surf,H.Percentile(0.0f),H.Percentile(1.0f));
}

void Balloon::KhToVertexColor(){
    if( !surf.vert.CurvatureEnabled ) return;

    // Disable face coloring and enable vertex
    this->rm &= ~SURF_FCOLOR;
    this->rm |=  SURF_VCOLOR;

    // Compute curvature bounds
    float absmax = -FLT_MAX;
    for(CMeshO::VertexIterator vi = surf.vert.begin(); vi != surf.vert.end(); ++vi){
        float cabs = fabs((*vi).Kh());
        absmax = (cabs>absmax) ? cabs : absmax;
    }

    //--- Map curvature to two color ranges:
    //    - Blue => Yellow: negative values
    //    - Yellow => Red:  positive values
    for(CMeshO::VertexIterator vi = surf.vert.begin(); vi != surf.vert.end(); ++vi){
        if( (*vi).Kh() < 0 )
            (*vi).C().lerp(Color4b::Yellow, Color4b::Blue, fabs((*vi).Kh())/absmax );
        else
            (*vi).C().lerp(Color4b::Yellow, Color4b::Red, (*vi).Kh()/absmax);
    }
}

void Balloon::render_cloud(){
    // Draw the ray/rays from their origin up to some distance away
    glDisable(GL_LIGHTING);
    glColor3f(.5, .5, .5);
    for(CMeshO::VertexIterator vi=cloud.vert.begin(); vi!=cloud.vert.end(); ++vi){
        Point3f p1 = (*vi).P();
        Point3f n = (*vi).N();
        // n[0] *= .1; n[1] *= .1; n[2] *= .1; // Scale the viewdir
        Point3f p2 = (*vi).P() + n;
        glBegin(GL_LINES);
            glVertex3f(p1[0],p1[1],p1[2]);
            glVertex3f(p2[0],p2[1],p2[2]);
        glEnd();
    }
    glEnable(GL_LIGHTING);
}
void Balloon::render_isosurface(GLArea* gla){
    GLW::DrawMode       dm = GLW::DMFlatWire;
    GLW::ColorMode      cm = GLW::CMPerVert;
    GLW::TextureMode	tm = GLW::TMNone;

    // By default vertColor is defined, so let's check if we need/want to
    // draw the face colors first.
    if( (rm & SURF_FCOLOR) && tri::HasPerFaceColor(surf) ){
        gla->rm.colorMode = vcg::GLW::CMPerFace; // Corrects MESHLAB BUG
        cm = GLW::CMPerFace;
    }
    else if( (rm & SURF_VCOLOR) && tri::HasPerVertexColor(surf) ){
        gla->rm.colorMode = vcg::GLW::CMPerVert; // Corrects MESHLAB BUG
        cm = GLW::CMPerVert;
    }
    GlTrimesh<CMeshO> surf_renderer;
    surf_renderer.m = &surf;
    surf_renderer.Draw(dm, cm, tm);
}
void Balloon::render_surf_to_acc(){
    gridAccell.render();

#if 0
    glDisable( GL_LIGHTING );
    const float ONETHIRD = 1.0f/3.0f;
    Point3f fcenter;
    Point3i off, o;
    glColor3f(1.0, 0.0, 0.0);
    for(unsigned int fi =0; fi<surf.face.size(); fi++){
        if( fi!= 5 )
            continue;
        CFaceO& f = surf.face[fi];
        fcenter = f.P(0) + f.P(1) + f.P(2);
        fcenter = myscale( fcenter, ONETHIRD );
        gridAccell.pos2off( fcenter, off );
        vol.pos2off( fcenter, o );
        gridAccell.off2pos( off, fcenter );
        vcg::drawBox( fcenter, vol.getDelta()*.95, true );


        // SHO BLOCK
        qDebug() << "full volume: " << endl << vol.toString(2, o[2]);
        // DEBUG: examine field values around current coordinate
        QString q;       
        qDebug();
        q.sprintf("%2.2f %2.2f %2.2f \n%2.2f %2.2f %2.2f \n%2.2f %2.2f %2.2f",
                  vol.Val(o[0]-1,o[1]+1,o[2]), vol.Val(o[0],o[1]+1,o[2]), vol.Val(o[0]+1,o[1]+1,o[2]),
                  vol.Val(o[0]-1,o[1],o[2]),   vol.Val(o[0],o[1],o[2]), vol.Val(o[0]+1,o[1]+0,o[2]),
                  vol.Val(o[0]-1,o[1]-1,o[2]), vol.Val(o[0],o[1]-1,o[2]), vol.Val(o[0]+1,o[1]-1,o[2]) );
        qDebug() << q;
        qDebug();
        o = off; // use the gridaccell data
        q.sprintf("%2.2f %2.2f %2.2f \n%2.2f %2.2f %2.2f \n%2.2f %2.2f %2.2f",
                  vol.Val(o[0]-1,o[1]+1,o[2]), vol.Val(o[0],o[1]+1,o[2]), vol.Val(o[0]+1,o[1]+1,o[2]),
                  vol.Val(o[0]-1,o[1],o[2]),   vol.Val(o[0],o[1],o[2]), vol.Val(o[0]+1,o[1]+0,o[2]),
                  vol.Val(o[0]-1,o[1]-1,o[2]), vol.Val(o[0],o[1]-1,o[2]), vol.Val(o[0]+1,o[1]-1,o[2]) );
        qDebug() << q;
    }
#endif

    glEnable( GL_LIGHTING );
}
void Balloon::render_surf_to_vol(){
    if( !vol.isInit() ) return;
    Point3f p, proj;
    glDisable(GL_LIGHTING);
    for(int i=0;i<vol.size(0);i++)
        for(int j=0;j<vol.size(1);j++)
            for(int k=0;k<vol.size(2);k++){
                // if( i!=3 || j!=6 || k!=5 ) continue;

                MyVoxel& v = vol.Voxel(i,j,k);
                vol.off2pos(i,j,k,p);
                // Only ones belonging to active band
                if( v.status ==  2 ){ // && ( tri::Index(surf, v.face)==35||tri::Index(surf, v.face)==34) ){
                    assert( v.face != 0 );
                    vcg::drawBox(p, .05*vol.getDelta());
                    Point3f proj;
                    // float dist = vcg::SignedFacePointDistance(*v.face, p, proj);
                    vcg::SignedFacePointDistance(*v.face, p, proj);
                    //proj = vcg::Barycenter(*v.face);
                    vcg::drawSegment( p, proj );
                }
            }
    glEnable(GL_LIGHTING);
}
void Balloon::render(GLArea* gla){
    if( rm & SHOW_CLOUD )
        render_cloud();
    if( rm & SHOW_VOLUME )
        vol.render();
    if( rm & SHOW_SURF )
        render_isosurface(gla);
    //if( rm & SHOW_ACCEL )
    //    gridAccell.render();
    if( rm & SHOW_3DDDR )
        render_surf_to_acc();
    if( rm & SHOW_SURF_TO_VOL )
        render_surf_to_vol();
}
