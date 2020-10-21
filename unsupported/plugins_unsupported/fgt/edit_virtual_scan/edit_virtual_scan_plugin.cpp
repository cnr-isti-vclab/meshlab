#include    "edit_virtual_scan_plugin.h"
#include    "my_gl_widget.h"

#include    <meshlab/glarea.h>

// #include <GL/glew.h>

EditVirtualScanPlugin::EditVirtualScanPlugin( void )
{
    dockWidget      = 0;
    editVsWidget    = 0;
    unifyClouds     = true;
}

EditVirtualScanPlugin::~EditVirtualScanPlugin( void )
{
    ;
}

const QString EditVirtualScanPlugin::Info( void )
{
    return tr( "Virtual Scan Sampling" );
}

bool EditVirtualScanPlugin::StartEdit( MeshModel& m, GLArea* parent )
{
    dockWidget      = new QDockWidget( parent->window() );
    editVsWidget    = new EditVsWidget( dockWidget );
    QPoint p        = parent->mapToGlobal( QPoint( 0, 0 ) );

    connectWidgetEvents();

    // get the current rendering mode
    params.cmDrawMode = parent->rm.drawMode;
    params.cmColorMode = parent->rm.colorMode;
    params.cmTextureMode = parent->rm.textureMode;

    // save some useful informations;
    inputMeshModel = &m;
    glArea = parent;

    // look for pre-computed povs
    CMeshO::PerMeshAttributeHandle< std::vector< Pov > > povs_handle;
    povs_handle = vcg::tri::Allocator< CMeshO >::GetPerMeshAttribute< std::vector< Pov > >( m.cm, "pointofviews" );
    bool customPovsAvailable = vcg::tri::Allocator< CMeshO >::IsValidHandle( m.cm, povs_handle );
    if( customPovsAvailable )
    {
        params.customPovs = povs_handle();
    }

    // initialize widgets and indirectly the virtual scanning parameters
    editVsWidget->initializeWidgets( customPovsAvailable, 25, 0.0f, 1.0f, 0.0f, 360, 6, 200,
                                     9, 20, 40, 20, 60, true );

    dockWidget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    dockWidget->setWidget( editVsWidget );
    dockWidget->setGeometry( 5 + p.x(), p.y() + 5, editVsWidget->width(), parent->height() - 10 );
    dockWidget->setFloating( true );
    dockWidget->setVisible( true );

    return true;
}

void EditVirtualScanPlugin::EndEdit( MeshModel& m, GLArea* parent )
{
    disconnectWidgetEvents();
    delete editVsWidget;
    delete dockWidget;
}

void EditVirtualScanPlugin::Decorate( MeshModel& m, GLArea* parent )
{
    ;
}

void EditVirtualScanPlugin::mousePressEvent( QMouseEvent* event, MeshModel& m, GLArea* parent )
{
    ;
}

void EditVirtualScanPlugin::mouseMoveEvent( QMouseEvent* event, MeshModel& m, GLArea* parent )
{
    ;
}

void EditVirtualScanPlugin::mouseReleaseEvent( QMouseEvent* event, MeshModel& m, GLArea* parent )
{
    ;
}

void EditVirtualScanPlugin::connectWidgetEvents( void )
{
    QObject::connect    ( editVsWidget, SIGNAL(customPovsMode(bool)),       this, SLOT(customPovsMode(bool)) );
    QObject::connect    ( editVsWidget, SIGNAL(povsNumberChanged(int)),     this, SLOT(povsNumberChanged(int)) );
    QObject::connect    ( editVsWidget, SIGNAL(xAxisChanged(float)),        this, SLOT(xAxisChanged(float)) );
    QObject::connect    ( editVsWidget, SIGNAL(yAxisChanged(float)),        this, SLOT(yAxisChanged(float)) );
    QObject::connect    ( editVsWidget, SIGNAL(zAxisChanged(float)),        this, SLOT(zAxisChanged(float)) );
    QObject::connect    ( editVsWidget, SIGNAL(povConeGapChanged(int)),     this, SLOT(povsConeGapChanged(int)) );
    QObject::connect    ( editVsWidget, SIGNAL(uResChanged(int)),           this, SLOT(uResChanged(int)) );
    QObject::connect    ( editVsWidget, SIGNAL(uConeGapChanged(int)),       this, SLOT(uConeGapChanged(int)) );
    QObject::connect    ( editVsWidget, SIGNAL(fResChanged(int)),           this, SLOT(fResChanged(int)) );
    QObject::connect    ( editVsWidget, SIGNAL(bigJumpChanged(float)),      this, SLOT(bigJumpChanged(float)) );
    QObject::connect    ( editVsWidget, SIGNAL(borderFGapChanged(int)),     this, SLOT(borderFGapChanged(int)) );
    QObject::connect    ( editVsWidget, SIGNAL(smallJumpChanged(float)),    this, SLOT(smallJumpChanged(float)) );
    QObject::connect    ( editVsWidget, SIGNAL(normalsAngleChanged(int)),   this, SLOT(normalsAngleChanged(int)) );
    QObject::connect    ( editVsWidget, SIGNAL(unifyPointClouds(bool)),     this, SLOT(unifyPointClouds(bool)) );
    QObject::connect    ( editVsWidget, SIGNAL(go()),                       this, SLOT(go()) );
}

void EditVirtualScanPlugin::disconnectWidgetEvents( void )
{
    QObject::disconnect ( editVsWidget, SIGNAL(customPovsMode(bool)),       this, SLOT(customPovsMode(bool)) );
    QObject::disconnect ( editVsWidget, SIGNAL(povsNumberChanged(int)),     this, SLOT(povsNumberChanged(int)) );
    QObject::disconnect ( editVsWidget, SIGNAL(xAxisChanged(float)),        this, SLOT(xAxisChanged(float)) );
    QObject::disconnect ( editVsWidget, SIGNAL(yAxisChanged(float)),        this, SLOT(yAxisChanged(float)) );
    QObject::disconnect ( editVsWidget, SIGNAL(zAxisChanged(float)),        this, SLOT(zAxisChanged(float)) );
    QObject::disconnect ( editVsWidget, SIGNAL(povConeGapChanged(int)),     this, SLOT(povsConeGapChanged(int)) );
    QObject::disconnect ( editVsWidget, SIGNAL(uResChanged(int)),           this, SLOT(uResChanged(int)) );
    QObject::disconnect ( editVsWidget, SIGNAL(uConeGapChanged(int)),       this, SLOT(uConeGapChanged(int)) );
    QObject::disconnect ( editVsWidget, SIGNAL(fResChanged(int)),           this, SLOT(fResChanged(int)) );
    QObject::disconnect ( editVsWidget, SIGNAL(bigJumpChanged(float)),      this, SLOT(bigJumpChanged(float)) );
    QObject::disconnect ( editVsWidget, SIGNAL(borderFGapChanged(int)),     this, SLOT(borderFGapChanged(int)) );
    QObject::disconnect ( editVsWidget, SIGNAL(smallJumpChanged(float)),    this, SLOT(smallJumpChanged(float)) );
    QObject::disconnect ( editVsWidget, SIGNAL(normalsAngleChanged(int)),   this, SLOT(normalsAngleChanged(int)) );
    QObject::disconnect ( editVsWidget, SIGNAL(unifyPointClouds(bool)),     this, SLOT(unifyPointClouds(bool)) );
    QObject::disconnect ( editVsWidget, SIGNAL(go()),                       this, SLOT(go()) );
}

/* private slots */
void EditVirtualScanPlugin::customPovsMode( bool on )
{
    params.useCustomPovs = on;
}

void EditVirtualScanPlugin::povsNumberChanged( int povsNumber )
{
    params.povs = povsNumber;
}

void EditVirtualScanPlugin::xAxisChanged( float newVal )
{
    params.coneAxis[0] = newVal;
}

void EditVirtualScanPlugin::yAxisChanged( float newVal )
{
    params.coneAxis[1] = newVal;
}

void EditVirtualScanPlugin::zAxisChanged( float newVal )
{
    params.coneAxis[2] = newVal;
}

void EditVirtualScanPlugin::povsConeGapChanged( int newGap )
{
    params.coneGap = (float)newGap;
}

void EditVirtualScanPlugin::uResChanged( int newRes )
{
    params.uniformResolution = newRes;
}

void EditVirtualScanPlugin::uConeGapChanged( int newGap )
{
    params.frontFacingConeU = (float)newGap;
}

void EditVirtualScanPlugin::fResChanged( int newRes )
{
    params.featureResolution = newRes;
}

void EditVirtualScanPlugin::bigJumpChanged( float newJump )
{
    params.bigDepthJump = newJump;
}

void EditVirtualScanPlugin::borderFGapChanged( int newGap )
{
    params.frontFacingConeF = newGap;
}

void EditVirtualScanPlugin::smallJumpChanged( float newJump )
{
    params.smallDepthJump = newJump;
}

void EditVirtualScanPlugin::normalsAngleChanged( int newAngle )
{
    params.angleThreshold = newAngle;
}

void EditVirtualScanPlugin::unifyPointClouds( bool unify )
{
    unifyClouds = unify;
}

void EditVirtualScanPlugin::go( void )
{
    assert( glArea && inputMeshModel );
    CMeshO* firstCloud = 0, *secondCloud = 0;
    MeshDocument* mDoc = glArea->meshDoc;
    MeshModel* tmpModel = 0;

    if( unifyClouds )
    {
        tmpModel = mDoc->addNewMesh( "VS Point Cloud", 0, false );
        firstCloud = &( tmpModel->cm );
        secondCloud = firstCloud;
    }
    else
    {
        tmpModel = mDoc->addNewMesh( "VS Uniform Samples", 0, false );
        firstCloud = &( tmpModel->cm );
        tmpModel = mDoc->addNewMesh( "VS Feature Samples", 0, false );
        secondCloud = &( tmpModel->cm );
    }

    MyGLWidget* tmpWidget = new MyGLWidget
                            ( &params, inputMeshModel, firstCloud, secondCloud, glArea );
    bool ok = tmpWidget->result;
    if( !ok )
    {
        QString errorMessage = tmpWidget->errorString;
        Log( errorMessage.toStdString().c_str() );
    }
    delete tmpWidget;
}
