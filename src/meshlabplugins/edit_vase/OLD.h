THIS FILE SHOULD NEVER BE COMPILED...

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
