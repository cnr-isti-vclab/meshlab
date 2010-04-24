#include "edit_scan.h"
#include "wrap/gui/trackball.h"

// Re-definition of the one in #include "wrap/qt/trackball.h"
// Lacks the CTRL modifier so that I can re-use it for my needs, all others are enabled
static vcg::Trackball::Button MyQT2VCG(Qt::MouseButton qtbt, Qt::KeyboardModifiers modifiers){
  int vcgbt = vcg::Trackball::BUTTON_NONE;
  if (qtbt & Qt::LeftButton)	vcgbt |= vcg::Trackball::BUTTON_LEFT;
  if (qtbt & Qt::RightButton)	vcgbt |= vcg::Trackball::BUTTON_LEFT;
  if (qtbt & Qt::MidButton)		vcgbt |= vcg::Trackball::BUTTON_MIDDLE;
  if (modifiers & Qt::ShiftModifier)		vcgbt |= vcg::Trackball::KEY_SHIFT;
  if (modifiers & Qt::AltModifier)			vcgbt |= vcg::Trackball::KEY_ALT;
  return vcg::Trackball::Button (vcgbt);
}

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
QString toString( const Point4f& p ){
    QString s;
    s.sprintf("%f %f %f %f", p[0], p[1], p[2], p[3]);
    return s;
}
QString toString( const Point3f& p ){
    QString s;
    s.sprintf("%f %f %f", p[0], p[1], p[2]);
    return s;
}
QString toString( const Point2f& p ){
    QString s;
    s.sprintf("%f %f", p[0], p[1]);
    return s;
}
QString toString( const Point2i& p ){
    QString s;
    s.sprintf("%d %d", p[0], p[1]);
    return s;
}
QString toString(Matrix44f& m){
    QString mat;
    for(int i=0; i<3; i++){
        mat.append( toString( m.GetRow4(i) ) );
        mat.append("\n");
    }
    return mat;
}
CVertexO dummy; // dummy for vertex insertion
double randn(){
    static double N=100;
    return (rand() % ((int)N)) / N -.5;
}

bool VirtualScan::StartEdit(MeshDocument& md, GLArea* gla){
    // Check a model exists and save it
    assert(md.mm() != NULL);
    // We are scanning... set orthographic
    gla->fov = 5;
    // Create a scanline
    sline = ScanLineGeom(10, 1);
    // Create a new model to store the scan cloud
    cloud = new MeshModel("Scan cloud");
    // Set initial state
    isScanning = false;
    // Create a timer which measures the scanning frequency (time in ms)
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(readytoscan()));
    timer->start(100); // frequency of repetitions

    return true;
}

// This is called only when mouse is pressed at first during a drag or a click is received
void VirtualScan::mousePressEvent(QMouseEvent* e, MeshModel &, GLArea* gla){
    gla->trackball.MouseDown(e->x(),gla->height()-e->y(), MyQT2VCG(e->button(), e->modifiers() ) );
    gla->update();
}
// This is called during the whole drag
void VirtualScan::mouseMoveEvent(QMouseEvent* e, MeshModel &, GLArea* gla){
    isScanning = (e->modifiers() & Qt::MetaModifier);
    gla->trackball.MouseMove(e->x(),gla->height()-e->y());
    gla->update();
}
void VirtualScan::mouseReleaseEvent(QMouseEvent* e, MeshModel &, GLArea* gla){
    isScanning = false;
    gla->trackball.MouseUp(e->x(),gla->height()-e->y(), MyQT2VCG(e->button(), e->modifiers() ) );
    gla->update();
}
void VirtualScan::Decorate(MeshModel& mm, GLArea*){
    // Scan the mesh only if we are triggering a scan and if the scanner is ready to suck in a new sample
    // This needs to be done after the mesh rendering has been done, but before any the scanned cloud is drawn!!
    if( isScanning && sampleReady ){
        sampleReady = false; // We used the sample, jump to next one
        scanpoints();
    }

    //--- Draw the scanned samples stored in the cloud
    glDisable(GL_LIGHTING);
        cloud->cm.C() = Color4b(255,200,200,255);
        glColor4f(.4f,.4f,1.f,.6f);
        cloud->glw.SetHintParamf(GLW::HNPPointSize,5);
        cloud->Render(GLW::DMPoints, GLW::CMPerMesh, GLW::TMNone);
    glEnable(GL_LIGHTING);

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

    //--- Draw the laser beam (just to help interfacing)
    // must be done after all 3D stuff is done as well
    sline.render();
}

ScanLineGeom::ScanLineGeom(int N, float width){
    // Compute start and stop coordinates in image space
    Point2f srt = myGluProject(Point3f(-width/2,0,0));
    Point2f sto = myGluProject(Point3f(+width/2,0,0));
    float delta = width/N;
    // qDebug() << "Scanpoint list: ";
    for( float i=0; i<1; i+=delta ){
        if( N==1 )
            i = .5;
        Point2f curr = srt*(1-i) + sto*i;
        soff.push_back(curr);
        // qDebug() << " - " << toString( curr );
        Point2i currI( curr[0], curr[1] );
        bbox.Add(currI);
        if( N==1 )
            break;
    }
    // Retrieve a block 2 pixel larger from buffer
    bbox.Offset(2);
}
void VirtualScan::scanpoints(){
    // Read the portion of depth buffer we are interested in
    float* buffer = new float[ sline.bbox.Area() ];
    glReadPixels(sline.bbox.min[0],sline.bbox.min[1],
                 sline.bbox.Dim()[0],sline.bbox.Dim()[1],
                 GL_DEPTH_COMPONENT, GL_FLOAT, buffer);

    for( unsigned int i=0; i<sline.soff.size(); i++ ){
        //--- Get curren scan point offset
        Point2f curr = sline.soff[i];
        qDebug() << "scannign sample: " << toString(curr);
        //--- Convert point into the buffer offset
        Point2i currb;
        currb[0] = round( curr[0]-sline.bbox.min[0] );
        currb[1] = round( curr[1]-sline.bbox.min[1] );
        //--- Retrieve Z from depth buffer
        float z = buffer[ currb[0] + sline.bbox.DimX()*currb[1] ];
        // Check against Z-Buffer limit set by glClearDepth
        // http://www.opengl.org/sdk/docs/man/xhtml/glClearDepth.xml
        if( z == 1 )
            continue;
        //--- Retrieve x,y coordinates in object space and another sample
        // with a bit of offset toward the camera
        Point3f sample = myGluUnProject( curr, z );
        Point3f sample2 = myGluUnProject( curr, z+.01);
        Point3f viewdir = (sample-sample2).normalized();
        qDebug() << "correspodning in object space to: " << toString(sample);

        //--- Add scanned sample to the cloud
        cloud->cm.vert.push_back(dummy);
        cloud->cm.vert.back().P() = sample;
        cloud->cm.vert.back().N() = viewdir;
    }
    delete [] buffer;
}
void ScanLineGeom::render(){
    // Attempts to render the scanline
    #if 0
        glDisable(GL_DEPTH_TEST);
        // Now draw some 2D stuff
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
            glLoadIdentity();
            glOrtho(0, gla->width(), gla->height(), 0, -1, 1);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
            glRectf(100,100, 200, 200);
        glPopMatrix();
    #endif
}

// Must be at the end of everything in CPP file or get segfault at plugin load
Q_EXPORT_PLUGIN(VirtualScan)

