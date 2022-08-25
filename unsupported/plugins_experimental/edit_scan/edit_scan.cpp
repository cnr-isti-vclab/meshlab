#include "edit_scan.h"
#include "wrap/gui/trackball.h"
#include "wrap/qt/trackball.h" //QT2VCG trackball function
#include "wrap/qt/to_string.h" //QT2VCG trackball function

#include <wrap/gl/pick.h>

#define EDITSCANHACK
#ifdef EDITSCANHACK
    #include <stdio.h>   
    
    class Sample{
    public:
        Point3f p;  // object space coordinate
        Point3f n;  // normal
        Point3f v;  // view direction
        bool    bg; // volume carving ray? (part of background)
    };

    vector<Sample> temp_samples;
    
    void write_to_ply(){
        FILE* fid = fopen( "/tmp/output.ply", "w" );
        if( fid == NULL ){
            qDebug("The output cannot be opened!");
            return;
        }
        
        fprintf(fid, "ply\n");
        fprintf(fid, "format ascii 1.0\n");
        fprintf(fid, "element vertex %d\n", (int) temp_samples.size() );
        fprintf(fid, "property float x\n");
        fprintf(fid, "property float y\n");
        fprintf(fid, "property float z\n");
        fprintf(fid, "property float nx\n");
        fprintf(fid, "property float ny\n");
        fprintf(fid, "property float nz\n");
        fprintf(fid, "property float vx\n");
        fprintf(fid, "property float vy\n");
        fprintf(fid, "property float vz\n");
        fprintf(fid, "end_header\n");
        for(unsigned int i=0; i<temp_samples.size(); i++){
            Point3f& p = temp_samples[i].p;
            Point3f& n = temp_samples[i].n;
            Point3f& v = temp_samples[i].v;
            fprintf(fid, "%f %f %f %f %f %f %f %f %f\n", p[0],p[1],p[2],n[0],n[1],n[2],v[0],v[1],v[2]);            
        }
        fclose(fid);       
    }
#endif 


Point2f myGluProject( Point3f p ){
    // retrieve matrixes from the pipeline
    GLdouble mvMatrix_f[16];  glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix_f);
    GLdouble prMatrix_f[16];  glGetDoublev(GL_PROJECTION_MATRIX, prMatrix_f);
    GLint viewpSize[4];       glGetIntegerv(GL_VIEWPORT, viewpSize);
    // project the point p on viewport obtaining point q
    Point2d q; double discard;
    gluProject(p[0], p[1], p[2],
               (const GLdouble *) mvMatrix_f,
               (const GLdouble *) prMatrix_f,
               (const GLint *)    viewpSize,
               &q[0], &q[1], &discard );
    Point2f retf( q[0], q[1] );
    return retf;
}
Point3f myGluUnProject( Point2f p, float z ){
    // retrieve matrixes from the pipeline
    GLdouble mvMatrix_f[16];  glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix_f);
    GLdouble prMatrix_f[16];  glGetDoublev(GL_PROJECTION_MATRIX, prMatrix_f);
    GLint viewpSize[4];       glGetIntegerv(GL_VIEWPORT, viewpSize);
    // project the point p on viewport obtaining point q
    Point3d q;
    gluUnProject(p[0], p[1], z,
                (const GLdouble *) mvMatrix_f,
                (const GLdouble *) prMatrix_f,
                (const GLint *)    viewpSize,
                 &q[0], &q[1], &q[2] );
    Point3f retf( q[0], q[1], q[2] );
    return retf;
}

double randn(){
    static double N=100;
    return (rand() % ((int)N)) / N -.5;
}

bool VirtualScan::StartEdit(MeshDocument& md, GLArea* gla){
    assert(md.mm() != NULL); // Model exists
    this->md = &md;
    this->gla = gla;
    gla->fov = 5; // Orthographic
    isScanning = false; // Trigger is off initially
    timer = 0;

    //--- Create a new model to store the scan cloud
    cloud = md.addNewMesh("Scan cloud",false);
   
    //--- Instantiate the UI, and connect events
    widget = new Widget(gla->window());
    connect(widget, SIGNAL(laser_parameter_updated()),
            this, SLOT(laser_parameter_updated()));
    connect(widget, SIGNAL(scan_requested()),
            this, SLOT(scan_requested()));
    connect(widget, SIGNAL(save_requested()),
            this, SLOT(save_requested()));

    //--- Compute initial beam
    laser_parameter_updated();

    return true;
}
// We need to refresh the laser scan completely!
void VirtualScan::laser_parameter_updated(){
    //--- Retrieve values from GUI
    int   period = 1000 / widget->getSampfreq();
    int   numsamp = widget->getNumsample();

    // Compute start and stop coordinates in image space
    Point2f delta( widget->getScanwidth()*gla->height()/(3.0*100.0), 0 );
    Point2f str( gla->width()/2, gla->height()/2 ); str-=delta;
    Point2f sto( gla->width()/2, gla->height()/2 ); sto+=delta;
    // float width = *md->mm()->cm.bbox.MaxDim()/100.0;
    qDebug("period: %d, samples: %d", period, numsamp);
    // qDebug() << toString(str) << " " << toString(sto);

    //--- Create the geometry of the scanline
    gla->update(); // since we use gluproject
    sline = ScanLine( numsamp, str, sto);
    // sline = ScanLineGeom( 10, .1 ); // hardcoded parameters (GUI independent)

    //--- Create a timer which enables scanning periodically
    if( timer!=0 ) delete timer;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(readytoscan()));
    timer->start(period); // period of repetition in ms

    //--- Update the laser rendering
    gla->update();
}
void VirtualScan::EndEdit(MeshModel&, GLArea* ){
    delete cloud;
    delete widget;
}

void VirtualScan::save_requested(){
#ifdef EDITSCANHACK
    //--- Output model of "samples" to Ply file
    qDebug("Writing to ply...");
    write_to_ply();
#else
    //--- Create a new model to store the scan cloud
    cloud = md->addNewMesh("Scan cloud",false);
#endif
}

// This is called only when mouse is pressed at first during a drag or a click is received
void VirtualScan::mousePressEvent(QMouseEvent* e, MeshModel &, GLArea* gla){
    this->laser_parameter_updated();
    gla->trackball.MouseDown(e->x(),gla->height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
    gla->update();
}
// This is called during the whole drag
void VirtualScan::mouseMoveEvent(QMouseEvent* e, MeshModel &, GLArea* gla){
    gla->trackball.MouseMove(e->x(),gla->height()-e->y());
    gla->update();
}
void VirtualScan::mouseReleaseEvent(QMouseEvent* e, MeshModel &, GLArea* gla){
    isScanning = false;
    gla->trackball.MouseUp(e->x(),gla->height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
    gla->update();
}
void VirtualScan::Decorate(MeshModel& mm, GLArea* gla){
    // Scan the mesh only if we are triggering a scan and if the scanner is ready to suck in a new sample
    // This needs to be done after the mesh rendering has been done, but before any the scanned cloud is drawn!!
    if( isScanning && sampleReady ){
        sampleReady = false;
        isScanning = false;
        scanpoints();
    }

    //--- Draw the scanned samples stored in the cloud
    glDisable(GL_LIGHTING);
        cloud->cm.C() = Color4b(255,200,200,255);
        glColor4f(.4f,.4f,1.f,.6f);
        cloud->glw.cdm = GLW::DMPoints;
        cloud->glw.SetHintParamf(GLW::HNPPointSize,SCANPOINTSIZE);
        cloud->Render(GLW::DMPoints, GLW::CMPerMesh, GLW::TMNone);
    glEnable(GL_LIGHTING);
    if(widget->getDrawLineFlag()){
        //--- Shows the view directions of the scanned samples
        // The "decorate plugin does nothing inside GLW, but does it directly
        // put these before ->Render
        //    cloud->updateDataMask(MeshModel::MM_VERTNORMAL);
        //    cloud->glw.cnm = GLW::NMPerVert;
        float LineLen = mm.cm.bbox.Diag()/20.0;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
            glColor4f(.4f,.4f,1.f,.6f);
            for(CMeshO::VertexIterator vi=cloud->cm.vert.begin(); vi!=cloud->cm.vert.end(); ++vi){
                glVertex((*vi).P());
                glVertex((*vi).P()+(*vi).N()*LineLen);
            }
        glEnd();
    }
    //--- Draw the laser beam (just to help interfacing)
    sline.render(gla);
}

ScanLine::ScanLine(int N, Point2f& srt, Point2f& end){
    
    
    // qDebug() << "Scanpoint list: ";

#define RANGE
#ifdef LASER
    float alpha=0;
    double delta_01 = 1.0 / (N-1);
    for( int i=0; i<N; i++, alpha+=delta_01 ){
        Point2f curr = srt*(1-alpha) + end*alpha;
        soff.push_back(curr);
        // qDebug() << " - " << toString( curr );
        Point2i currI( curr[0], curr[1] );
        bbox.Add(currI);
    }
#endif
#ifdef RANGE
    double delta_01 = 1.0 / (N-1.0); 
    double halfedgel = ((srt-end).Norm()/2.0);
    srt[1] = srt[1] - halfedgel;
    end[1] = end[1] + halfedgel;

    Point2f srt_x=srt, end_x=end;
    Point2f srt_y=srt, end_y=end;
    srt_x[1]=0; end_x[1]=0;
    srt_y[0]=0; end_y[0]=0;
    float alpha=0;
    for( int i=0; i<N; i++, alpha+=delta_01 ){
        float beta = 0;
        for( int j=0; j<N; j++, beta+=delta_01 ){
            Point2f curr = srt_x*(1-alpha) + end_x*alpha + srt_y*(1-beta)  + end_y*beta;
            soff.push_back(curr);
            Point2i currI( curr[0], curr[1] );
            bbox.Add(currI);
        }
    }
#endif

    // Retrieve a block 2 pixel larger from buffer
    bbox.Offset(2);
}

void VirtualScan::scanpoints(){
    //--- Create samples
    int X = sqrt( sline.soff.size() ); // TODO: move    
    int Y = sqrt( sline.soff.size() ); // TODO: move    
    vector<Sample> curr_samples( sline.soff.size() );
        
    //--- Read the portion of depth buffer we are interested in
    float* buffer = new float[ sline.bbox.Area() ];
    glReadPixels(sline.bbox.min[0],sline.bbox.min[1],sline.bbox.Dim()[0],
                 sline.bbox.Dim()[1],GL_DEPTH_COMPONENT, GL_FLOAT, buffer);
    
    //--- Extract samples
    for( int x=0,I=0; x<X; x++ ){
        for( int y=0; y<Y; y++,I=x+X*y ){
            //--- Get current scan point offset
            Point2f laser_xy = sline.soff[I];
            
            //--- Convert xy location into the buffer offset
            Point2i buf_off;
            buf_off[0] = round( laser_xy[0]-sline.bbox.min[0] );
            buf_off[1] = round( laser_xy[1]-sline.bbox.min[1] );
    
            //--- Retrieve Z from depth buffer
            double z = buffer[ buf_off[0] + sline.bbox.DimX()*buf_off[1] ];
    
            //--- Initialize sample
            Point3f temp = myGluUnProject( laser_xy, z+0.01 );
            curr_samples[I].p  = myGluUnProject( laser_xy, z );         // sample
            curr_samples[I].v  = (curr_samples[I].p-temp).normalized(); // view direction
            curr_samples[I].n  = Point3f(0,0,0);                        // normal
            // Check against Z-Buffer limit set by glClearDepth
            // http://www.opengl.org/sdk/docs/man/xhtml/glClearDepth.xml
            // Out of Z-buffer implies rays at infinity!!
            curr_samples[I].bg = (z==1);                                 // isbackground?                       
        }
    }
   
    //--- Compute gradients and discard invalid
    //    samples on outer layer are invalid by default
    for( int x=1,I=0; x<X-1; x++ ){
        for( int y=1; y<Y-1; y++,I=x+X*y){
            Sample&  curr    = curr_samples[  x   + X* (y) ];
            
            //--- Extract samples for center differences
            Point3f& px_next = curr_samples[(x+1) + X* (y) ].p;
            Point3f& px_prev = curr_samples[(x-1) + X* (y) ].p;
            Point3f& py_next = curr_samples[ (x)  + X*(y+1)].p;
            Point3f& py_prev = curr_samples[ (x)  + X*(y-1)].p;
            
            //--- Extract tangent vectors & normal
            Point3f px_tan = px_next-px_prev;
            Point3f py_tan = py_next-py_prev;
            curr.n = -( px_tan ^ py_tan );
            curr.n.Normalize();
            
            //--- Regardless if BG or not
            if( curr.n.dot(curr.v) < cos( 60/180.0*3.14159 ) )
                continue;
 
            //--- Restore NULL normal for BG samples
            if( curr.bg )
                curr.n = Point3f(0,0,0);
            
            //--- Add scanned sample to the "viz" cloud
            tri::Allocator<CMeshO>::AddVertices(cloud->cm,1);
            cloud->cm.vert.back().P() = curr.p;
            cloud->cm.vert.back().N() = curr.v;
            
            //--- Add scanned sample to the "data" cloud
            temp_samples.push_back( curr );
        }
    }
    
    delete [] buffer;
}

void ScanLine::render(GLArea* gla){
#if 0
    //--- DEBUG! why mv[4,3] = -1000?
    GLdouble mv[16];  glGetDoublev(GL_MODELVIEW_MATRIX, mv);
    for(int i=0; i<4; i++)
        qDebug() << mv[4*i+0] << " " << mv[4*i+1] << " " << mv[4*i+2] << " " << mv[4*i+3];
    qDebug();
#endif

    // This draws directly in 2D image space
    glDisable(GL_DEPTH_TEST);
    glPointSize(SCANPOINTSIZE);
    glColor(Color4f(255,0,0,255));
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        glOrtho(0, gla->width(), 0, gla->height(), -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
            glColor3f(1.0,0.0,0.0);
            glBegin(GL_POINTS);
                for(unsigned int i=0; i<soff.size(); i++)
                    glVertex(soff[i]);
                // glVertex2f( gla->width()/2, gla->height()/2 );
            glEnd();
            // glRectf(100,100, 200, 200);
        glPopMatrix(); // restore modelview
    // The pop must be done in the projection mode
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

// Must be at the end of everything in CPP file or get segfault at plugin load
Q_EXPORT_PLUGIN(VirtualScan)

