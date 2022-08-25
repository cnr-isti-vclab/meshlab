#include    "edit_vs_widget.h"

#include    <math.h>

EditVsWidget::EditVsWidget( QWidget* parent, Qt::WindowFlags flags )
    : QWidget( parent, flags )
{
    this->resize( 300, 200 );

    createWidgets();
    prepareWidgets();
    makeEventConnections();
}

EditVsWidget::~EditVsWidget( void )
{
    destroyWidgets();
}

/* widgets management */
void EditVsWidget::createWidgets( void )
{
    mainLayout  = new QGridLayout( this );

    /* povs group box */
    povsBox     = new QGroupBox( tr( "Povs" ), this );
    povsBoxLayout       = new QGridLayout( povsBox );
    genPovsRadioBtn     = new QRadioButton( tr( "Generate povs within a cone of directions" ), povsBox );
    savedPovsRadioBtn   = new QRadioButton( tr( "Use povs saved in the current mesh" ), povsBox );
    povsGroup           = new QButtonGroup( 0 );
    povsNumberLabel     = new QLabel( tr( "Povs number: " ), povsBox );
    povsNumberLine      = new QLineEdit( povsBox );

    /* cone axis group box */
    coneAxisBox         = new QGroupBox( tr( "Cone Axis" ), povsBox );
    coneAxisBoxLayout   = new QGridLayout( povsBox );
    xCompLabel          = new QLabel( tr( "X component: " ), coneAxisBox );
    xCompLine           = new QLineEdit( coneAxisBox );
    yCompLabel          = new QLabel( tr( "Y component: " ), coneAxisBox );
    yCompLine           = new QLineEdit( coneAxisBox );
    zCompLabel          = new QLabel( tr( "Z component: " ), coneAxisBox );
    zCompLine           = new QLineEdit( coneAxisBox );

    coneAxisBoxLayout->addWidget( xCompLabel, 0, 0, 1, 1 );
    coneAxisBoxLayout->addWidget( xCompLine, 0, 1, 1, 1 );
    coneAxisBoxLayout->addWidget( yCompLabel, 1, 0, 1, 1 );
    coneAxisBoxLayout->addWidget( yCompLine, 1, 1, 1, 1 );
    coneAxisBoxLayout->addWidget( zCompLabel, 2, 0, 1, 1 );
    coneAxisBoxLayout->addWidget( zCompLine, 2, 1, 1, 1 );

    coneAxisBox->setLayout( coneAxisBoxLayout );
    /* ------------------- */

    gapLabel            = new QLabel( tr( "Cone gap: " ), povsBox );
    gapSlider           = new QSlider( Qt::Horizontal, povsBox );
    gapValue            = new QLabel( tr( "180°" ), povsBox );

    povsBoxLayout->addWidget( genPovsRadioBtn, 0, 0, 1, 4 );
    povsBoxLayout->addWidget( povsNumberLabel, 1, 1, 1, 1 );
    povsBoxLayout->addWidget( povsNumberLine, 1, 2, 1, 2 );
    povsBoxLayout->addWidget( coneAxisBox, 2, 1, 1, 3 );
    povsBoxLayout->addWidget( gapLabel, 3, 1, 1, 1 );
    povsBoxLayout->addWidget( gapSlider, 3, 2, 1, 1 );
    povsBoxLayout->addWidget( gapValue, 3, 3, 1, 1 );
    povsBoxLayout->addWidget( savedPovsRadioBtn, 4, 0, 1, 4 );

    povsGroup->addButton( genPovsRadioBtn );
    povsGroup->addButton( savedPovsRadioBtn );

    povsBox->setLayout( povsBoxLayout );
    /* ------------------- */

    /* uniform samples group box */
    uniformBox          = new QGroupBox( tr( "Uniform samples" ), this );
    uniformBoxLayout    = new QGridLayout( this );
    uResLabel           = new QLabel( tr( "Viewport side: " ), uniformBox );
    uResSlider          = new QSlider( Qt::Horizontal, uniformBox );
    uResValue           = new QLabel( tr( "64" ), uniformBox );
    uConeLabel          = new QLabel( tr( "Front-facing cone gap: " ), uniformBox );
    uConeSlider         = new QSlider( Qt::Horizontal, uniformBox );
    uConeValue          = new QLabel( tr( "90°" ), uniformBox );

    uniformBoxLayout->addWidget( uResLabel, 0, 0, 1, 1 );
    uniformBoxLayout->addWidget( uResSlider, 0, 1, 1, 2 );
    uniformBoxLayout->addWidget( uResValue, 0, 3, 1, 1 );
    uniformBoxLayout->addWidget( uConeLabel, 1, 0, 1, 1 );
    uniformBoxLayout->addWidget( uConeSlider, 1, 1, 1, 2 );
    uniformBoxLayout->addWidget( uConeValue, 1, 3, 1, 1 );

    uniformBox->setLayout( uniformBoxLayout );
    /* ------------------------- */

    /* feature samples group box */
    featureBox          = new QGroupBox( tr( "Feature samples" ), this );
    featureBoxLayout    = new QGridLayout( this );
    fResLabel           = new QLabel( tr( "Viewport side: " ), featureBox );
    fResSlider          = new QSlider( Qt::Horizontal, featureBox );
    fResValue           = new QLabel( tr( "64" ), featureBox );
    borderJumpLabel     = new QLabel( tr( "Border depth jump: " ), featureBox );
    borderJumpSlider    = new QSlider( Qt::Horizontal, featureBox );
    borderJumpValue     = new QLabel( tr( "20%" ), featureBox );
    borderFacingLabel   = new QLabel( tr( "Facing-border cone gap: " ), featureBox );
    borderFacingSlider  = new QSlider( Qt::Horizontal, featureBox );
    borderFacingValue   = new QLabel( tr( "40°" ), featureBox );
    patchJumpLabel      = new QLabel( tr( "Patch depth jump: " ), featureBox );
    patchJumpSlider     = new QSlider( Qt::Horizontal, featureBox );
    patchJumpValue      = new QLabel( tr( "1%" ), featureBox );
    normalsAngleLabel   = new QLabel( tr( "Normals angle: " ), featureBox );
    normalsAngleSlider  = new QSlider( Qt::Horizontal, featureBox );
    normalsAngleValue   = new QLabel( tr( "40°" ), featureBox );

    featureBoxLayout->addWidget( fResLabel, 0, 0, 1, 1 );
    featureBoxLayout->addWidget( fResSlider, 0, 1, 1, 2 );
    featureBoxLayout->addWidget( fResValue, 0, 3, 1, 1 );
    featureBoxLayout->addWidget( borderJumpLabel, 1, 0, 1, 1 );
    featureBoxLayout->addWidget( borderJumpSlider, 1, 1, 1, 2 );
    featureBoxLayout->addWidget( borderJumpValue, 1, 3, 1, 1 );
    featureBoxLayout->addWidget( borderFacingLabel, 2, 0, 1, 1 );
    featureBoxLayout->addWidget( borderFacingSlider, 2, 1, 1, 2 );
    featureBoxLayout->addWidget( borderFacingValue, 2, 3, 1, 1 );
    featureBoxLayout->addWidget( patchJumpLabel, 3, 0, 1, 1 );
    featureBoxLayout->addWidget( patchJumpSlider, 3, 1, 1, 2 );
    featureBoxLayout->addWidget( patchJumpValue, 3, 3, 1, 1 );
    featureBoxLayout->addWidget( normalsAngleLabel, 4, 0, 1, 1 );
    featureBoxLayout->addWidget( normalsAngleSlider, 4, 1, 1, 2 );
    featureBoxLayout->addWidget( normalsAngleValue, 4, 3, 1, 1 );

    featureBox->setLayout( featureBoxLayout );
    /* ------------------------- */

    unifyCheck          = new QCheckBox( tr( "Unify feature and uniform samples" ), this );
    goBtn               = new QPushButton( tr( "Generate samples" ), this );

    mainLayout->addWidget( povsBox, 0, 0, 6, 1 );
    mainLayout->addWidget( uniformBox, 6, 0, 3, 1 );
    mainLayout->addWidget( featureBox, 9, 0, 5, 1 );
    mainLayout->addWidget( unifyCheck, 14, 0, 1, 1 );
    mainLayout->addWidget( goBtn, 15, 0, 1, 1 );

    this->setLayout( mainLayout );
}

void EditVsWidget::prepareWidgets( void )
{
    povsValidator = new QIntValidator( 1, 9999, this );
    povsNumberLine->setMaxLength( 4 );
    povsNumberLine->setValidator( povsValidator );
    povsNumberLine->setAlignment( Qt::AlignRight );

    axisValidator = new QDoubleValidator( this );
    QLineEdit*  axis[3] = { xCompLine, yCompLine, zCompLine };
    for( int i=0; i<3; i++ )
    {
        axis[i]->setAlignment( Qt::AlignRight );
        axis[i]->setValidator( axisValidator );
    }

    gapSlider->setMinimum( 0 );
    gapSlider->setMaximum( 360 );

    uResSlider->setMinimum( 0 );
    uResSlider->setMaximum( 13 );

    uConeSlider->setMinimum( 0 );
    uConeSlider->setMaximum( 200 );

    fResSlider->setMinimum( 0 );
    fResSlider->setMaximum( 13 );

    borderJumpSlider->setMinimum( 0 );
    borderJumpSlider->setMaximum( 100 );

    borderFacingSlider->setMinimum( 0 );
    borderFacingSlider->setMaximum( 200 );

    patchJumpSlider->setMinimum( 0 );
    patchJumpSlider->setMaximum( 100 );

    normalsAngleSlider->setMinimum( 0 );
    normalsAngleSlider->setMaximum( 200 );
}

void EditVsWidget::makeEventConnections( void )
{
    QObject::connect( savedPovsRadioBtn,    SIGNAL(toggled(bool)),          this, SLOT(useSavedPovsRadioToggled(bool)) );
    QObject::connect( povsNumberLine,       SIGNAL(textChanged(QString)),   this, SLOT(povsNumberTextChanged(QString)) );
    QObject::connect( xCompLine,            SIGNAL(textChanged(QString)),   this, SLOT(xCompTextChanged(QString)) );
    QObject::connect( yCompLine,            SIGNAL(textChanged(QString)),   this, SLOT(yCompTextChanged(QString)) );
    QObject::connect( zCompLine,            SIGNAL(textChanged(QString)),   this, SLOT(zCompTextChanged(QString)) );
    QObject::connect( gapSlider,            SIGNAL(valueChanged(int)),      this, SLOT(gapSliderChanged(int)) );
    QObject::connect( uResSlider,           SIGNAL(valueChanged(int)),      this, SLOT(uResSliderChanged(int)) );
    QObject::connect( uConeSlider,          SIGNAL(valueChanged(int)),      this, SLOT(uGapSliderChanged(int)) );
    QObject::connect( fResSlider,           SIGNAL(valueChanged(int)),      this, SLOT(fResSliderChanged(int)) );
    QObject::connect( borderJumpSlider,     SIGNAL(valueChanged(int)),      this, SLOT(borderJumpSliderChanged(int)) );
    QObject::connect( borderFacingSlider,   SIGNAL(valueChanged(int)),      this, SLOT(borderFacingSliderChanged(int)) );
    QObject::connect( patchJumpSlider,      SIGNAL(valueChanged(int)),      this, SLOT(patchJumpSliderChanged(int)) );
    QObject::connect( normalsAngleSlider,   SIGNAL(valueChanged(int)),      this, SLOT(normalsSliderChanged(int)) );
    QObject::connect( unifyCheck,           SIGNAL(toggled(bool)),          this, SLOT(unifyCheckToggled(bool)) );
    QObject::connect( goBtn,                SIGNAL(clicked()),              this, SLOT(goBtnClicked()) );
}

void EditVsWidget::initializeWidgets( bool useCustomPovs, int povsNumber,
                                      float xAxis, float yAxis, float zAxis, int gap,
                                      int uRes, int uGap,
                                      int fRes, int bigJump, int borderConeGap, int smallJump, int normalsAngle,
                                      bool unifyClouds )
{
    savedPovsRadioBtn->setChecked( true );  // little workaround
    savedPovsRadioBtn->setChecked( useCustomPovs );
    genPovsRadioBtn->setChecked( !useCustomPovs );
    povsNumberLine->setText( QString::number( povsNumber ) );
    xCompLine->setText( QString::number( xAxis ) );
    yCompLine->setText( QString::number( yAxis ) );
    zCompLine->setText( QString::number( zAxis ) );
    gapSlider->setValue( gap );
    uResSlider->setValue( uRes );
    uConeSlider->setValue( uGap );
    fResSlider->setValue( fRes );
    borderJumpSlider->setValue( bigJump );
    borderFacingSlider->setValue( borderConeGap );
    patchJumpSlider->setValue( smallJump );
    normalsAngleSlider->setValue( normalsAngle );
    unifyCheck->setChecked( unifyClouds );
}

void EditVsWidget::destroyWidgets( void )
{
    delete genPovsRadioBtn;
    delete savedPovsRadioBtn;
    delete povsGroup;
    delete povsBoxLayout;
    delete povsBox;
    delete mainLayout;
}

/* private slots */
void EditVsWidget::useSavedPovsRadioToggled( bool toggled )
{
    coneAxisBox->setEnabled( !toggled );
    povsNumberLabel->setEnabled( !toggled );
    povsNumberLine->setEnabled( !toggled );
    gapLabel->setEnabled( !toggled );
    gapSlider->setEnabled( !toggled );
    gapValue->setEnabled( !toggled );
    emit customPovsMode( toggled );
}

void EditVsWidget::povsNumberTextChanged( QString newText )
{
    int val = newText.toInt();
    emit ( povsNumberChanged( val ) );
}

void EditVsWidget::xCompTextChanged( QString newText )
{
    float comp = newText.toFloat();
    emit ( xAxisChanged( comp ) );
}

void EditVsWidget::yCompTextChanged( QString newText )
{
    float comp = newText.toFloat();
    emit ( yAxisChanged( comp ) );
}

void EditVsWidget::zCompTextChanged( QString newText )
{
    float comp = newText.toFloat();
    emit ( zAxisChanged( comp ) );
}

void EditVsWidget::gapSliderChanged( int val )
{
    gapValue->setText( QString::number( val ) + tr( "°" ) );
    emit ( povConeGapChanged( val ) );
}

void EditVsWidget::uResSliderChanged( int val )
{
    int resolution = (int)( pow( 2.0, (double)val ) );
    uResValue->setText( QString::number( resolution ) );
    emit ( uResChanged( resolution ) );
}

void EditVsWidget::uGapSliderChanged( int val )
{
    uConeValue->setText( QString::number( val ) + tr( "°" ) );
    emit ( uConeGapChanged( val ) );
}

void EditVsWidget::fResSliderChanged( int val )
{
    int resolution = (int)( pow( 2.0, (double)val ) );
    fResValue->setText( QString::number( resolution ) );
    emit ( fResChanged( resolution ) );
}

void EditVsWidget::borderJumpSliderChanged( int val )
{
    borderJumpValue->setText( QString::number( val ) + tr( "%" ) );
    float jump = val / 100.0f;
    emit ( bigJumpChanged( jump ) );
}

void EditVsWidget::borderFacingSliderChanged( int val )
{
    borderFacingValue->setText( QString::number( val ) + tr( "°" ) );
    emit ( borderFGapChanged( val ) );
}

void EditVsWidget::patchJumpSliderChanged( int val )
{
    float jump = ( val / 100.0f ) * 5.0f;
    patchJumpValue->setText( QString::number( jump ) + tr( "%" ) );
    float toEmit = jump / 100.0f;
    emit ( smallJumpChanged( toEmit ) );
}

void EditVsWidget::normalsSliderChanged( int val )
{
    normalsAngleValue->setText( QString::number( val ) + tr( "°" ) );
    emit ( normalsAngleChanged( val ) );
}

void EditVsWidget::unifyCheckToggled( bool toggled )
{
    emit ( unifyPointClouds( toggled ) );
}

void EditVsWidget::goBtnClicked( void )
{
    emit ( go() );
}
