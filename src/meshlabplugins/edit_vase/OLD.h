THIS FILE SHOULD NEVER BE COMPILED...
BUT KEEP IT IN THE PROJECT SO I CAN DUMP PIECES OF CODE INSIDE IT

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


