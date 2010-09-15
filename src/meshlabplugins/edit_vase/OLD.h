/**
                         THIS FILE SHOULD NEVER BE COMPILED...
           BUT I KEEP IT IN THE PROJECT SO I CAN DUMP PIECES OF CODE INSIDE IT          */

// Evolution debug
float prev = v.sfield;
if( updates_view[i] == view_min_dst && view_min_dst < 0 ){
    qDebug("UPDATE VALUE %.3f (negative for expansion)", vcg::sign( k1*k2*max_speed, updates_view[i] ) );
    qDebug("d_view: %.2f", updates_view[i]);
    qDebug("k1: %.2f", k1);
    qDebug("k2: %.2f", k2);
    qDebug("max_speed: %.2f", max_speed);
    qDebug("Prev   value %.3f", updates_view[i] );
    qDebug("Curvature update: %.3f", k3*alpha*max_speed );
}

// DEBUG!!! PAOLO ASKED TO CALL IT TWICE
#ifdef REPEAT_SIMPLIFY_PROCESS
        tri::UpdateTopology<CMeshO>::VertexFace( mesh );
        tri::MCSimplify<CMeshO>( mesh, getDelta()/4 );
        tri::Allocator<CMeshO>::CompactVertexVector( mesh );
        tri::Allocator<CMeshO>::CompactFaceVector( mesh );
        mesh.face.EnableFFAdjacency();
        tri::Clean<CMeshO>::RemoveTVertexByFlip(mesh,20,true);
        tri::Clean<CMeshO>::RemoveFaceFoldByFlip(mesh);
    }
#endif

// TESTS INVERTIBILITY OF MARCHING CUBE / DISTANCE
if( false ){
    //--- Test uniform band update
    for(unsigned int i=0; i<vol.band.size(); i++){
        Point3i& voxi = vol.band[i];
        MyVoxel& v = vol.Voxel(voxi);
        v.sfield += .05;
    }
    //--- Estrai isosurface
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
    return true;
}

// SMOOTHING BY SIMPLE ONE-RING
#if FALSE
    // Enable curvature supports, How do I avoid a
    // double computation of topology here?
    surf.vert.EnableCurvature();
    surf.vert.EnableVFAdjacency();
    surf.face.EnableVFAdjacency();
    surf.face.EnableFFAdjacency();
    vcg::tri::UpdateTopology<CMeshO>::VertexFace( surf );
    vcg::tri::UpdateTopology<CMeshO>::FaceFace( surf );

    //--- Compute curvature and its bounds
    tri::UpdateCurvature<CMeshO>::MeanAndGaussian( surf );
#endif

// SMOOTHING BY SOLVING A LAPLACIAN SYSTEM
#if FALSE
    float OMEGA = 1; // interpolation coefficient
    sinterp.Init( &surf, 3, COMBINATORIAL );
    for( CMeshO::VertexIterator vi=surf.vert.begin();vi!=surf.vert.end();vi++ )
        sinterp.AddConstraint3( tri::Index(surf,*vi), OMEGA, (*vi).P()[0], (*vi).P()[1], (*vi).P()[2] );
    FieldInterpolator::XBType* coords[3];
    sinterp.Solve(coords);
    for( CMeshO::VertexIterator vi=surf.vert.begin();vi!=surf.vert.end();vi++ ){
        int vIdx = tri::Index(surf,*vi);
        (*vi).P()[0] = (*coords[0])(vIdx);
        (*vi).P()[1] = (*coords[1])(vIdx);
        (*vi).P()[2] = (*coords[2])(vIdx);
    }
#endif

//--- DEBUG: Create a dummy QUALITY field Q() on mesh vertices and add constraints
#if false
    rm |= SURF_VCOLOR;
    float OMEGA = 100;
    tri::UpdateQuality<CMeshO>::VertexConstant(surf, 0);
    for( int i=0; i<surf.vert.size(); i++ ){
        CVertexO& v = surf.vert[i];
        float f = v.P()[1];
        if( fabs(f)>.9 ){
            v.Q() = f;
            // rfinterp.AddConstraint( i, OMEGA, f );
            //qDebug() << "active constraints at vertex " << i << " val: " << f << endl;
        }
    }
    // Map vertex quality to a color
    Histogramf H;
    tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(surf,H);
    tri::UpdateColor<CMeshO>::VertexQualityRamp(surf,H.Percentile(0.0f),H.Percentile(1.0f));
    qDebug() << H.Percentile(1.0f);
    return;
#endif

// This function used to display the voxels in volume space
void Balloon::render_surf_to_vol(){
    glDisable( GL_LIGHTING );
    const float ONETHIRD = 1.0f/3.0f;
    Point3f fcenter;
    Point3i off;
    glColor3f(0.0, 1.0, 0.0);
    for(CMeshO::FaceIterator fi=surf.face.begin();fi!=surf.face.end();fi++){
        CFaceO& f = *(fi);
        fcenter = f.P(0) + f.P(1) + f.P(2);
        fcenter = myscale( fcenter, ONETHIRD );
        vol.pos2off( fcenter, off );
        vol.off2pos( off, fcenter );
        gl3DBox( fcenter, vol.getDelta()*.95, true );
    }
    glEnable( GL_LIGHTING );
}

//---------------------------------------------------------------------------------------------------
//
//                             POINT 2 SURFACE CORRESPONDENCE
//
//---------------------------------------------------------------------------------------------------
// Init
typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshFaceGrid;
MetroMeshFaceGrid   unifGridFace;
typedef tri::FaceTmark<CMeshO> MarkerFace;
MarkerFace markerFunctor;

tri::UpdateNormals<CMeshO>::PerFaceNormalized(*m);
tri::UpdateFlags<CMeshO>::FaceProjection(*m);
unifGridFace.Set(m->face.begin(),m->face.end());
markerFunctor.SetMesh(m);

// and then when you need to find the closest point on a surface from a starting point
 Point3f       closestPt,      normf, bestq, ip;
float dist = dist_upper_bound;

// compute distance between startPt and the mesh S2
CMeshO::FaceType   *nearestF=0;
vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
nearestF =  unifGridFace.GetClosest(PDistFunct,markerFunctor,startPt,dist_upper_bound,dist,closestPt);



// ------------- CLEANING -------------
if(par.getBool("AutoClean"))
{
    int nullFaces=tri::Clean<CMeshO>::RemoveFaceOutOfRangeArea(m.cm,0);
    if(nullFaces) Log(GLLogStream::FILTER, "PostSimplification Cleaning: Removed %d null faces", nullFaces);
    int deldupvert=tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
        if(deldupvert) Log(GLLogStream::FILTER, "PostSimplification Cleaning: Removed %d duplicated vertices", deldupvert);

    int delvert=tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
    if(delvert) Log(GLLogStream::FILTER, "PostSimplification Cleaning: Removed %d unreferenced vertices",delvert);
    m.clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
    tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
    tri::Allocator<CMeshO>::CompactFaceVector(m.cm);
}

tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);
tri::UpdateBounding<CMeshO>::Box(m.cm);
