#include "ml_render_gui.h"
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QHeaderView>
#include <QColorDialog>
#include <QToolButton>
#include <QMenu>
#include <QWidgetAction>
#include <QStylePainter>
#include <QApplication>
#include <wrap/qt/col_qt_convert.h>

MLRenderingToolbar::MLRenderingToolbar(QWidget* parent )
    :QToolBar(parent),_meshid(-1),_previoussel(NULL),_actgroup(NULL)
{
    _actgroup = new QActionGroup(this);
    connect(this,SIGNAL(actionTriggered(QAction*)),this,SLOT(toggle(QAction*)));
}

MLRenderingToolbar::MLRenderingToolbar(int meshid,QWidget* parent )
    :QToolBar(parent),_meshid(meshid),_previoussel(NULL),_actgroup(NULL)
{
    _actgroup = new QActionGroup(this);
    connect(this,SIGNAL(actionTriggered(QAction*)),this,SLOT(toggle(QAction*)));
}

MLRenderingToolbar::~MLRenderingToolbar()
{
}

void MLRenderingToolbar::updateVisibility(MeshModel* mm)
{
    foreach(MLRenderingAction* ract,_acts)
    {
        ract->setCheckable(ract->isCheckableConditionValid(mm));
        QWidget* wid = widgetForAction(ract);
        if (wid != NULL)
            wid->setVisible(ract->isCheckable());
    }
}

void MLRenderingToolbar::addRenderingAction( MLRenderingAction* act )
{
    if (act == NULL)
        return;
    _actgroup->addAction(act);  
    _acts.push_back(act);
    addAction(act);
    act->setCheckable(true);
    act->setVisible(true);
}


void MLRenderingToolbar::toggle( QAction* act)
{
    MLRenderingAction* ract = qobject_cast<MLRenderingAction*>(act);
    if (ract != NULL)
    {
        emit updateRenderingDataAccordingToActions(_meshid,_acts);
        if (ract->isChecked())
            emit activatedAction(ract);
    }
}

void MLRenderingToolbar::setAccordingToRenderingData(const MLRenderingData& dt)
{
    foreach(MLRenderingAction* rendact,_acts)
        rendact->setChecked(rendact->isRenderingDataEnabled(dt));
}

void MLRenderingToolbar::setAssociatedMeshId( int meshid )
{
    _meshid = meshid;
    foreach(MLRenderingAction* act,_acts)
        act->setMeshId(_meshid);
}

QList<MLRenderingAction*>& MLRenderingToolbar::getRenderingActions()
{
    return _acts;
}

void MLRenderingToolbar::addColorPicker( MLRenderingColorPicker* pick )
{
    MLRenderingUserDefinedColorAction* colact = qobject_cast<MLRenderingUserDefinedColorAction*>(pick->defaultAction());
    if (colact != NULL)
    {
        _actgroup->addAction(colact);
        _acts.push_back(colact);
        colact->setCheckable(true);
        colact->setVisible(true);
    }
    addWidget(pick);
    connect(pick,SIGNAL(triggered(QAction*)),this,SLOT(toggle(QAction*)));
    connect(pick,SIGNAL(userDefinedColorAction(int,MLRenderingAction*)),this,SLOT(extraUpdateRequired(int,MLRenderingAction*)));
}

void MLRenderingToolbar::addColorPicker( MLRenderingBBoxColorPicker* pick )
{
    MLRenderingBBoxUserDefinedColorAction* colact = qobject_cast<MLRenderingBBoxUserDefinedColorAction*>(pick->defaultAction());
    if (colact != NULL)
    {
        _actgroup->addAction(colact);
        _acts.push_back(colact);
        colact->setCheckable(true);
        colact->setVisible(true);
    }
    addWidget(pick);
    connect(pick,SIGNAL(triggered(QAction*)),this,SLOT(toggle(QAction*)));
    connect(pick,SIGNAL(userDefinedColorAction(int,MLRenderingAction*)),this,SLOT(extraUpdateRequired(int,MLRenderingAction*)));
}

void MLRenderingToolbar::extraUpdateRequired( int,MLRenderingAction* act )
{
    toggle(act);
}

void MLRenderingToolbar::getCurrentRenderingDataAccordingToGUI( MLRenderingData& dt ) const
{
    for(int ii = 0;ii < _acts.size();++ii)
    {
        if (_acts[ii] != NULL)
            _acts[ii]->updateRenderingData(dt);
    }
}

MLRenderingSideToolbar::MLRenderingSideToolbar(QWidget* parent /*= NULL*/ )
    :MLRenderingToolbar(parent)
{
    initGui();
}

MLRenderingSideToolbar::MLRenderingSideToolbar(int meshid,QWidget* parent /*= NULL*/ )
        :MLRenderingToolbar(meshid,parent)
{
    initGui();
}

void MLRenderingSideToolbar::initGui()
{
    _actgroup->setExclusive(false);
    addRenderingAction(new MLRenderingBBoxAction(_meshid,this));
    addRenderingAction(new MLRenderingPointsAction(_meshid,this));
    addRenderingAction(new MLRenderingWireAction(_meshid,this));
    addRenderingAction(new MLRenderingSolidAction(_meshid,this));
    addRenderingAction(new MLRenderingSelectionAction(_meshid,this));
    addRenderingAction(new MLRenderingEdgeDecoratorAction(_meshid,this));
}

void MLRenderingSideToolbar::toggle( QAction* clickedact )
{
    if ((clickedact != NULL) && (_actgroup != NULL))
    {
        Qt::KeyboardModifiers mod = QApplication::keyboardModifiers();
        if (_actgroup != NULL)
        {
            if (mod == Qt::ControlModifier)
            {
                foreach(MLRenderingAction* act,_acts)
                {
                    if (act != NULL)
                        act->setChecked(clickedact == act);
                }
            }
        }
    }
    MLRenderingToolbar::toggle(clickedact);
}

MLRenderingParametersFrame::MLRenderingParametersFrame( int meshid,QWidget* parent )
    :QFrame(parent),_meshid(meshid)
{

}

MLRenderingParametersFrame::~MLRenderingParametersFrame()
{

}

MLRenderingParametersFrame* MLRenderingParametersFrame::factory( MLRenderingAction* act,int meshid,QWidget* parent)
{
    if (qobject_cast<MLRenderingSolidAction*>(act) != NULL)
        return new MLRenderingSolidParametersFrame(meshid,parent);
        
    if (qobject_cast<MLRenderingPointsAction*>(act) != NULL)
        return new MLRenderingPointsParametersFrame(meshid,parent);

    if (qobject_cast<MLRenderingWireAction*>(act) != NULL)
        return new MLRenderingWireParametersFrame(meshid,parent);

    if (qobject_cast<MLRenderingSelectionAction*>(act) != NULL)
        return new MLRenderingSelectionParametersFrame(meshid,parent);

    if (qobject_cast<MLRenderingBBoxAction*>(act) != NULL)
        return new MLRenderingBBoxParametersFrame(meshid,parent);

    if (qobject_cast<MLRenderingEdgeDecoratorAction*>(act) != NULL)
        return new MLRenderingDefaultDecoratorParametersFrame(meshid,parent);

    return NULL;
}

MLRenderingSolidParametersFrame::MLRenderingSolidParametersFrame(QWidget* parent )
    :MLRenderingParametersFrame(-1,parent)
{
    initGui();
}

MLRenderingSolidParametersFrame::MLRenderingSolidParametersFrame( int meshid,QWidget* parent )
    :MLRenderingParametersFrame(meshid,parent)
{
    initGui();
}

void MLRenderingSolidParametersFrame::initGui()
{   
    setAutoFillBackground(true);
    QGridLayout* layout = new QGridLayout();
    _shadingtool = new MLRenderingToolbar(_meshid,this);
    QLabel* shadelab = new QLabel("Shading",this);
    QFont boldfont;
    boldfont.setBold(true);
    shadelab->setFont(boldfont);
    layout->addWidget(shadelab,0,0,Qt::AlignLeft);
    _shadingtool->addRenderingAction(new MLRenderingPerVertexNormalAction(MLRenderingData::PR_SOLID,_meshid,_shadingtool));
    _shadingtool->addRenderingAction(new MLRenderingPerFaceNormalAction(_meshid,_shadingtool));
    _shadingtool->addRenderingAction(new MLRenderingNoShadingAction(MLRenderingData::PR_SOLID,_meshid,_shadingtool));
    layout->addWidget(_shadingtool,0,1,Qt::AlignLeft);
    connect(_shadingtool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));
    

    QLabel* colorlab = new QLabel("Color",this);
    colorlab->setFont(boldfont);
    layout->addWidget(colorlab,1,0,Qt::AlignLeft);
    _colortool = new MLRenderingToolbar(_meshid,this);
    _colortool->addRenderingAction(new MLRenderingPerVertexColorAction(MLRenderingData::PR_SOLID,_meshid,_colortool));
    _colortool->addRenderingAction(new MLRenderingPerFaceColorAction(_meshid,_colortool));
    _colortool->addRenderingAction(new MLRenderingPerMeshColorAction(MLRenderingData::PR_SOLID,_meshid,_colortool));
    MLRenderingColorPicker* colbut = new MLRenderingColorPicker(_meshid,MLRenderingData::PR_SOLID,_colortool);
    MLPerViewGLOptions tmp;
    MLPoliciesStandAloneFunctions::suggestedDefaultPerViewGLOptions(tmp);
    colbut->setColor(vcg::ColorConverter::ToQColor(tmp._persolid_fixed_color));
    _colortool->addColorPicker(colbut);
    layout->addWidget(_colortool,1,1,Qt::AlignLeft);
    connect(_colortool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));

    QLabel* textlab = new QLabel("Texture Coord",this);
    textlab->setFont(boldfont);
    layout->addWidget(textlab,2,0,Qt::AlignLeft);
    _texttool = new MLRenderingOnOffToolbar(_meshid,this);
    _texttool->setRenderingAction(new MLRenderingPerWedgeTextCoordAction(_meshid,_texttool));
    layout->addWidget(_texttool,2,1,Qt::AlignLeft);
    connect(_texttool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));

    setMinimumSize(layout->sizeHint());
    setLayout(layout);
    showNormal();
    adjustSize();   
}

void MLRenderingSolidParametersFrame::setPrimitiveButtonStatesAccordingToRenderingData( const MLRenderingData& dt )
{
    _shadingtool->setAccordingToRenderingData(dt);
    _colortool->setAccordingToRenderingData(dt);
    _texttool->setAccordingToRenderingData(dt);
}

void MLRenderingSolidParametersFrame::setAssociatedMeshId( int meshid )
{
    _meshid = meshid;
    _shadingtool->setAssociatedMeshId(meshid);
    _colortool->setAssociatedMeshId(meshid);
    _texttool->setAssociatedMeshId(meshid);
}

MLRenderingSolidParametersFrame::~MLRenderingSolidParametersFrame()
{
    delete _shadingtool;
    delete _colortool;
    delete _texttool;
}


void MLRenderingSolidParametersFrame::getCurrentRenderingDataAccordingToGUI( MLRenderingData& dt ) const
{
    _shadingtool->getCurrentRenderingDataAccordingToGUI(dt);
    _colortool->getCurrentRenderingDataAccordingToGUI(dt);
    _texttool->getRenderingDataAccordingToGUI(dt);
}

void MLRenderingSolidParametersFrame::updateVisibility( MeshModel* mm )
{
    _shadingtool->updateVisibility(mm);
    _colortool->updateVisibility(mm);
    _texttool->updateVisibility(mm);
}

MLRenderingWireParametersFrame::MLRenderingWireParametersFrame( QWidget* parent )
    :MLRenderingParametersFrame(-1,parent)
{
    initGui();
}

MLRenderingWireParametersFrame::MLRenderingWireParametersFrame( int meshid,QWidget* parent )
    :MLRenderingParametersFrame(meshid,parent)
{
    initGui();
}

MLRenderingWireParametersFrame::~MLRenderingWireParametersFrame()
{
    delete _edgetool;
    delete _shadingtool;
    delete _colortool;
    delete _dimension;
}

void MLRenderingWireParametersFrame::setPrimitiveButtonStatesAccordingToRenderingData( const MLRenderingData& dt )
{
    _shadingtool->setAccordingToRenderingData(dt);
    _colortool->setAccordingToRenderingData(dt);
    _edgetool->setAccordingToRenderingData(dt);
    _dimension->setAccordingToRenderingData(dt);
}

void MLRenderingWireParametersFrame::setAssociatedMeshId( int meshid )
{
    _meshid = meshid;
    _shadingtool->setAssociatedMeshId(meshid);
    _colortool->setAssociatedMeshId(meshid);
    _edgetool->setAssociatedMeshId(meshid);
    _dimension->setAssociatedMeshId(meshid);
}

void MLRenderingWireParametersFrame::initGui()
{
    setAutoFillBackground(true);
    QGridLayout* layout = new QGridLayout();
    _shadingtool = new MLRenderingToolbar(_meshid,this);
    QLabel* shadelab = new QLabel("Shading",this);
    QFont boldfont;
    boldfont.setBold(true);
    shadelab->setFont(boldfont);
    layout->addWidget(shadelab,0,0,Qt::AlignLeft);
    _shadingtool->addRenderingAction(new MLRenderingPerVertexNormalAction(MLRenderingData::PR_WIREFRAME_TRIANGLES,_meshid,this));
    _shadingtool->addRenderingAction(new MLRenderingNoShadingAction(MLRenderingData::PR_WIREFRAME_TRIANGLES,_meshid,this));
    layout->addWidget(_shadingtool,0,1,Qt::AlignLeft);
    connect(_shadingtool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));

    QLabel* colorlab = new QLabel("Color",this);
    colorlab->setFont(boldfont);
    layout->addWidget(colorlab,1,0,Qt::AlignLeft);
    _colortool = new MLRenderingToolbar(_meshid,this);
    _colortool->addRenderingAction(new MLRenderingPerVertexColorAction(MLRenderingData::PR_WIREFRAME_TRIANGLES,_meshid,this));
    _colortool->addRenderingAction(new MLRenderingPerMeshColorAction(MLRenderingData::PR_WIREFRAME_TRIANGLES,_meshid,this));
    MLRenderingColorPicker* colbut = new MLRenderingColorPicker(_meshid,MLRenderingData::PR_WIREFRAME_TRIANGLES,_colortool);
    MLPerViewGLOptions tmp;
    MLPoliciesStandAloneFunctions::suggestedDefaultPerViewGLOptions(tmp);
    colbut->setColor(vcg::ColorConverter::ToQColor(tmp._perwire_fixed_color));
    _colortool->addColorPicker(colbut);
    layout->addWidget(_colortool,1,1,Qt::AlignLeft);
    connect(_colortool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));
    
    QLabel* dimelab = new QLabel("Edge Width",this);
    dimelab->setFont(boldfont);
    layout->addWidget(dimelab,2,0,Qt::AlignLeft);
    _dimension = new MLRenderingFloatSlider(this);
    _dimension->setRenderingFloatAction(new MLRenderingWireWidthAction(_meshid,this));
    _dimension->setOrientation(Qt::Horizontal);
    _dimension->setMinimum(1);
    _dimension->setMaximum(5);
    layout->addWidget(_dimension,2,1,Qt::AlignCenter);
    connect(_dimension,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));

    QLabel* wirelab = new QLabel("Wire Modality",this);
    wirelab->setFont(boldfont);
    layout->addWidget(wirelab,3,0,Qt::AlignLeft);
    _edgetool = new MLRenderingOnOffToolbar(_meshid,this);
    _edgetool->setRenderingAction(new MLRenderingFauxEdgeWireAction(_meshid,this));
    layout->addWidget(_edgetool,3,1,Qt::AlignLeft);
    connect(_edgetool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));

    setMinimumSize(layout->sizeHint());
    setLayout(layout);
    showNormal();
    adjustSize();   
}

void MLRenderingWireParametersFrame::getCurrentRenderingDataAccordingToGUI( MLRenderingData& dt ) const
{
    _shadingtool->getCurrentRenderingDataAccordingToGUI(dt);
    _colortool->getCurrentRenderingDataAccordingToGUI(dt);
    _dimension->getRenderingDataAccordingToGUI(dt);
    _edgetool->getRenderingDataAccordingToGUI(dt);
    
}

void MLRenderingWireParametersFrame::updateVisibility( MeshModel* mm )
{
    _shadingtool->updateVisibility(mm);
    _colortool->updateVisibility(mm);
    _edgetool->updateVisibility(mm);
}


MLRenderingPointsParametersFrame::MLRenderingPointsParametersFrame( QWidget* parent )
    :MLRenderingParametersFrame(-1,parent)
{
    initGui();
}


MLRenderingPointsParametersFrame::MLRenderingPointsParametersFrame( int meshid,QWidget* parent )
    :MLRenderingParametersFrame(meshid,parent)
{
    initGui();
}

MLRenderingPointsParametersFrame::~MLRenderingPointsParametersFrame()
{
    delete _shadingtool;
    delete _colortool;
    delete _texttool;
    delete _dimension;
}

void MLRenderingPointsParametersFrame::getCurrentRenderingDataAccordingToGUI( MLRenderingData& dt ) const
{
    _shadingtool->getCurrentRenderingDataAccordingToGUI(dt);
    _colortool->getCurrentRenderingDataAccordingToGUI(dt);
    _dimension->getRenderingDataAccordingToGUI(dt);
    _texttool->getRenderingDataAccordingToGUI(dt);
}


void MLRenderingPointsParametersFrame::setPrimitiveButtonStatesAccordingToRenderingData( const MLRenderingData& dt )
{
    _shadingtool->setAccordingToRenderingData(dt);
    _colortool->setAccordingToRenderingData(dt);
    _texttool->setAccordingToRenderingData(dt);
    _dimension->setAccordingToRenderingData(dt);
}

void MLRenderingPointsParametersFrame::setAssociatedMeshId( int meshid )
{
    _meshid = meshid;
    _shadingtool->setAssociatedMeshId(meshid);
    _colortool->setAssociatedMeshId(meshid);
    _texttool->setAssociatedMeshId(meshid);
    _dimension->setAssociatedMeshId(meshid);
}

void MLRenderingPointsParametersFrame::initGui()
{
    setAutoFillBackground(true);
    QGridLayout* layout = new QGridLayout();
    _shadingtool = new MLRenderingToolbar(_meshid,this);
    QLabel* shadelab = new QLabel("Shading",this);
    QFont boldfont;
    boldfont.setBold(true);
    shadelab->setFont(boldfont);
    layout->addWidget(shadelab,0,0,Qt::AlignLeft);
    _shadingtool->addRenderingAction(new MLRenderingPerVertexNormalAction(MLRenderingData::PR_POINTS,_meshid,this));
    _shadingtool->addRenderingAction(new MLRenderingDotAction(_meshid,this));
    _shadingtool->addRenderingAction(new MLRenderingNoShadingAction(MLRenderingData::PR_POINTS,_meshid,this));
    layout->addWidget(_shadingtool,0,1,Qt::AlignLeft);
    connect(_shadingtool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));

    QLabel* colorlab = new QLabel("Color",this);
    colorlab->setFont(boldfont);
    layout->addWidget(colorlab,1,0,Qt::AlignLeft);
    _colortool = new MLRenderingToolbar(_meshid,this);
    _colortool->addRenderingAction(new MLRenderingPerVertexColorAction(MLRenderingData::PR_POINTS,_meshid,this));
    _colortool->addRenderingAction(new MLRenderingPerMeshColorAction(MLRenderingData::PR_POINTS,_meshid,this));
    MLRenderingColorPicker* colbut = new MLRenderingColorPicker(_meshid,MLRenderingData::PR_POINTS,_colortool);
    MLPerViewGLOptions tmp;
    MLPoliciesStandAloneFunctions::suggestedDefaultPerViewGLOptions(tmp);
    colbut->setColor(vcg::ColorConverter::ToQColor(tmp._perpoint_fixed_color));
    _colortool->addColorPicker(colbut);
    layout->addWidget(_colortool,1,1,Qt::AlignLeft);
    connect(_colortool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));


    QLabel* textlab = new QLabel("Texture Coord",this);
    textlab->setFont(boldfont);
    layout->addWidget(textlab,2,0,Qt::AlignLeft);
    _texttool = new MLRenderingOnOffToolbar(_meshid,this);
    _texttool->setRenderingAction(new MLRenderingPerVertTextCoordAction(MLRenderingData::PR_POINTS,_meshid,_texttool));
    layout->addWidget(_texttool,2,1,Qt::AlignLeft);
    connect(_texttool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));

    QLabel* dimelab = new QLabel("Point Size",this);
    dimelab->setFont(boldfont);
    layout->addWidget(dimelab,3,0,Qt::AlignLeft);
    _dimension = new MLRenderingFloatSlider(_meshid,this);
    _dimension->setRenderingFloatAction(new MLRenderingPointsSizeAction(_meshid,this));
    _dimension->setOrientation(Qt::Horizontal);
    _dimension->setMinimum(1);
    _dimension->setMaximum(5);
    connect(_dimension,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));
    /*_dimension->setDecimals(1);
    _dimension->setAlignment(Qt::AlignRight);*/
    layout->addWidget(_dimension,3,1,Qt::AlignCenter);
    setMinimumSize(layout->sizeHint());
    setLayout(layout);
    showNormal();
    adjustSize();   
}

void MLRenderingPointsParametersFrame::updateVisibility( MeshModel* mm )
{
    _shadingtool->updateVisibility(mm);
    _colortool->updateVisibility(mm);
    _texttool->updateVisibility(mm);
}

MLRenderingBBoxParametersFrame::MLRenderingBBoxParametersFrame(QWidget* parent )
    :MLRenderingParametersFrame(-1,parent)
{
    initGui();
}

MLRenderingBBoxParametersFrame::MLRenderingBBoxParametersFrame( int meshid,QWidget* parent )
    :MLRenderingParametersFrame(meshid,parent)
{
    initGui();
}

MLRenderingBBoxParametersFrame::~MLRenderingBBoxParametersFrame()
{
    delete _colortool;
    delete _quotedinfotool;
}

void MLRenderingBBoxParametersFrame::getCurrentRenderingDataAccordingToGUI( MLRenderingData& dt ) const
{
    _colortool->getCurrentRenderingDataAccordingToGUI(dt);
    _quotedinfotool->getRenderingDataAccordingToGUI(dt);
}


void MLRenderingBBoxParametersFrame::setPrimitiveButtonStatesAccordingToRenderingData( const MLRenderingData& dt )
{
    _colortool->setAccordingToRenderingData(dt);
    _quotedinfotool->setAccordingToRenderingData(dt);
}

void MLRenderingBBoxParametersFrame::setAssociatedMeshId( int meshid )
{
    _meshid = meshid;
    _colortool->setAssociatedMeshId(meshid);
    _quotedinfotool->setAssociatedMeshId(meshid);
}

void MLRenderingBBoxParametersFrame::initGui()
{
    setAutoFillBackground(true);
    QGridLayout* layout = new QGridLayout();
    QLabel* colorlab = new QLabel("Color",this);
    QFont boldfont;
    boldfont.setBold(true);
    colorlab->setFont(boldfont);
    layout->addWidget(colorlab,0,0,Qt::AlignLeft);
    _colortool = new MLRenderingToolbar(_meshid,this);
    _colortool->addRenderingAction(new MLRenderingBBoxPerMeshColorAction(_meshid,this));
    MLRenderingBBoxColorPicker* colbut = new MLRenderingBBoxColorPicker(_meshid,_colortool);
    MLPerViewGLOptions tmp; 
    MLPoliciesStandAloneFunctions::suggestedDefaultPerViewGLOptions(tmp);
    //tmp._perbbox_fixed_color = vcg::Color4b(255,85,0,255);
    colbut->setColor(vcg::ColorConverter::ToQColor(tmp._perbbox_fixed_color));
    _colortool->addColorPicker(colbut);
    layout->addWidget(_colortool,0,1,Qt::AlignLeft);
    connect(_colortool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));

    QLabel* quotedinfolab = new QLabel("Measure Info",this);
    quotedinfolab->setFont(boldfont);
    layout->addWidget(quotedinfolab,1,0,Qt::AlignLeft);
    _quotedinfotool = new MLRenderingOnOffToolbar(_meshid,this);
    _quotedinfotool->setRenderingAction(new MLRenderingBBoxQuotedInfoAction(_meshid,this));
    layout->addWidget(_quotedinfotool,1,1,Qt::AlignLeft);
    connect(_quotedinfotool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));

    setMinimumSize(layout->sizeHint());
    setLayout(layout);
    showNormal();
    adjustSize();   
}

MLRenderingDefaultDecoratorParametersFrame::MLRenderingDefaultDecoratorParametersFrame( QWidget* parent )
    :MLRenderingParametersFrame(-1,parent)
{
    initGui();
}

MLRenderingDefaultDecoratorParametersFrame::MLRenderingDefaultDecoratorParametersFrame( int meshid,QWidget* parent )
    :MLRenderingParametersFrame(meshid,parent)
{
    initGui();
}

MLRenderingDefaultDecoratorParametersFrame::~MLRenderingDefaultDecoratorParametersFrame()
{
    delete _boundearyedgetool;
    delete _boundearyfacetool;
    delete _edgemanifoldtool;
    delete _vertmanifoldtool;
    delete _texturebordertool;
}

void MLRenderingDefaultDecoratorParametersFrame::getCurrentRenderingDataAccordingToGUI( MLRenderingData& dt ) const
{
    _boundearyedgetool->getRenderingDataAccordingToGUI(dt);
    _boundearyfacetool->getRenderingDataAccordingToGUI(dt);
    _edgemanifoldtool->getRenderingDataAccordingToGUI(dt);
    _vertmanifoldtool->getRenderingDataAccordingToGUI(dt);
    _texturebordertool->getRenderingDataAccordingToGUI(dt);
}


void MLRenderingDefaultDecoratorParametersFrame::setPrimitiveButtonStatesAccordingToRenderingData( const MLRenderingData& dt )
{
    _boundearyedgetool->setAccordingToRenderingData(dt);
    _boundearyfacetool->setAccordingToRenderingData(dt);
    _edgemanifoldtool->setAccordingToRenderingData(dt);
    _vertmanifoldtool->setAccordingToRenderingData(dt);
    _texturebordertool->setAccordingToRenderingData(dt);
}

void MLRenderingDefaultDecoratorParametersFrame::setAssociatedMeshId( int meshid )
{
    _boundearyedgetool->setAssociatedMeshId(meshid);
    _boundearyfacetool->setAssociatedMeshId(meshid);
    _edgemanifoldtool->setAssociatedMeshId(meshid);
    _vertmanifoldtool->setAssociatedMeshId(meshid);
    _texturebordertool->setAssociatedMeshId(meshid);
}

void MLRenderingDefaultDecoratorParametersFrame::initGui()
{
    setAutoFillBackground(true);
    QGridLayout* layout = new QGridLayout();
    QLabel* boundarylab = new QLabel("Boundary Edges",this);
    QFont boldfont;
    boldfont.setBold(true);
    boundarylab->setFont(boldfont);
    layout->addWidget(boundarylab,0,0,Qt::AlignLeft);
    _boundearyedgetool = new MLRenderingOnOffToolbar(_meshid,this);
    _boundearyedgetool->setRenderingAction(new MLRenderingEdgeBoundaryAction(_meshid,_boundearyedgetool));
    layout->addWidget(_boundearyedgetool,0,1,Qt::AlignLeft);
    connect(_boundearyedgetool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));

    QLabel* boundaryfacelab = new QLabel("Boundary Faces",this);
    boundaryfacelab->setFont(boldfont);
    layout->addWidget(boundaryfacelab,1,0,Qt::AlignLeft);
    _boundearyfacetool = new MLRenderingOnOffToolbar(_meshid,this);
    _boundearyfacetool->setRenderingAction(new MLRenderingFaceBoundaryAction(_meshid,_boundearyfacetool));
    layout->addWidget(_boundearyfacetool,1,1,Qt::AlignLeft);
    connect(_boundearyfacetool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));

    QLabel* vertmanifoldlab = new QLabel("No-Manif Verts",this);
    vertmanifoldlab->setFont(boldfont);
    layout->addWidget(vertmanifoldlab,2,0,Qt::AlignLeft);
    _vertmanifoldtool = new MLRenderingOnOffToolbar(_meshid,this);
    _vertmanifoldtool->setRenderingAction(new MLRenderingVertManifoldAction(_meshid,_vertmanifoldtool));
    layout->addWidget(_vertmanifoldtool,2,1,Qt::AlignLeft);
    connect(_vertmanifoldtool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));

    QLabel* edgemanifoldlab = new QLabel("No-Manif Edges",this);
    edgemanifoldlab->setFont(boldfont);
    layout->addWidget(edgemanifoldlab,3,0,Qt::AlignLeft);
    _edgemanifoldtool = new MLRenderingOnOffToolbar(_meshid,this);
    _edgemanifoldtool->setRenderingAction(new MLRenderingEdgeManifoldAction(_meshid,_edgemanifoldtool));
    layout->addWidget(_edgemanifoldtool,3,1,Qt::AlignLeft);
    connect(_edgemanifoldtool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));

    QLabel* textureborderlab = new QLabel("Texture Border",this);
    textureborderlab->setFont(boldfont);
    layout->addWidget(textureborderlab,4,0,Qt::AlignLeft);
    _texturebordertool = new MLRenderingOnOffToolbar(_meshid,this);
    _texturebordertool->setRenderingAction(new MLRenderingTexBorderAction(_meshid,_texturebordertool));
    layout->addWidget(_texturebordertool,4,1,Qt::AlignLeft);
    connect(_texturebordertool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));

    setMinimumSize(layout->sizeHint());
    setLayout(layout);
    showNormal();
    adjustSize();   
}

MLRenderingSelectionParametersFrame::MLRenderingSelectionParametersFrame( QWidget* parent )
    :MLRenderingParametersFrame(-1,parent)
{
    initGui();
}

MLRenderingSelectionParametersFrame::MLRenderingSelectionParametersFrame( int meshid,QWidget* parent )
    :MLRenderingParametersFrame(meshid,parent)
{
    initGui();
}

MLRenderingSelectionParametersFrame::~MLRenderingSelectionParametersFrame()
{
    delete _vertexseltool;
    delete _faceseltool;
}

void MLRenderingSelectionParametersFrame::getCurrentRenderingDataAccordingToGUI( MLRenderingData& dt ) const
{
    _vertexseltool->getRenderingDataAccordingToGUI(dt);
    _faceseltool->getRenderingDataAccordingToGUI(dt);
}


void MLRenderingSelectionParametersFrame::setPrimitiveButtonStatesAccordingToRenderingData( const MLRenderingData& dt )
{
    _vertexseltool->setAccordingToRenderingData(dt);
    _faceseltool->setAccordingToRenderingData(dt);
}

void MLRenderingSelectionParametersFrame::setAssociatedMeshId( int meshid )
{
    _vertexseltool->setAssociatedMeshId(meshid);
    _faceseltool->setAssociatedMeshId(meshid);
}

void MLRenderingSelectionParametersFrame::initGui()
{
    setAutoFillBackground(true);
    QGridLayout* layout = new QGridLayout();
    QLabel* vertlab = new QLabel("Selected Vertex",this);
    QFont boldfont;
    boldfont.setBold(true);
    vertlab->setFont(boldfont);
    layout->addWidget(vertlab,0,0,Qt::AlignLeft);
    _vertexseltool = new MLRenderingOnOffToolbar(_meshid,this);
    _vertexseltool->setRenderingAction(new MLRenderingVertSelectionAction(_meshid,_vertexseltool));
    layout->addWidget(_vertexseltool,0,1,Qt::AlignLeft);
    connect(_vertexseltool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));

    QLabel* facelab = new QLabel("Selected Face",this);
    facelab->setFont(boldfont);
    layout->addWidget(facelab,1,0,Qt::AlignLeft);
    _faceseltool = new MLRenderingOnOffToolbar(_meshid,this);
    _faceseltool->setRenderingAction(new MLRenderingFaceSelectionAction(_meshid,_faceseltool));
    layout->addWidget(_faceseltool,1,1,Qt::AlignLeft);
    connect(_faceseltool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));

    setMinimumSize(layout->sizeHint());
    setLayout(layout);
    showNormal();
    adjustSize();   
}



MLRenderingParametersTab::MLRenderingParametersTab( int meshid,const QList<MLRenderingAction*>& tab, QWidget* parent )
    :QTabWidget(parent),_meshid(meshid)
{
    initGui(tab);
}

void MLRenderingParametersTab::setAssociatedMeshId( int meshid )
{
    _meshid = meshid;
    for(QMap<QString,MLRenderingParametersFrame*>::iterator itt = _parframe.begin();itt != _parframe.end();++itt)
        (*itt)->setAssociatedMeshId(meshid);
}

void MLRenderingParametersTab::activateRenderingMode(int index)
{
	if ((index >= 0) && (index < _paract.size()))
	{
		MLRenderingAction* act = _paract[index];
		if (act != NULL)
		{
			act->setChecked(!act->isChecked());
			emit updateRenderingDataAccordingToAction(_meshid, act);

			/*WARNING!!! ADDED just to avoid usual mac strange behavior*/
			emit updateLayerTableRequested();
		}
	}
}

void MLRenderingParametersTab::switchTab(int meshid,const QString& tabname)
{
    QMap<QString,MLRenderingParametersFrame*>::iterator itt = _parframe.find(tabname);
    if (itt != _parframe.end())
        setCurrentWidget((*itt));
    //setAssociatedMeshIdAndRenderingData(meshid,dt);
}

void MLRenderingParametersTab::updateGUIAccordingToRenderingData(const MLRenderingData& dt )
{
    for(QMap<QString,MLRenderingParametersFrame*>::iterator itt = _parframe.begin();itt != _parframe.end();++itt)
        (*itt)->setPrimitiveButtonStatesAccordingToRenderingData(dt);
}

void MLRenderingParametersTab::initGui(const QList<MLRenderingAction*>& tab)
{
	_paract.resize(tab.size());
	int ii = 0;
    foreach(MLRenderingAction* ract,tab)
    {
        if (ract != NULL)
        {
            MLRenderingParametersFrame* fr = MLRenderingParametersFrame::factory(ract,_meshid,this);
            if (fr != NULL)
            {
                _parframe[ract->text()] = fr;
				MLRenderingAction* sister = NULL;
				ract->createSisterAction(sister, this);
				if (sister != NULL)
					_paract[ii] = sister;
                addTab(fr,ract->icon(),"");
                connect(fr,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));
                connect(fr,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));
            }
        }
		++ii;
    }
	connect(this, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(activateRenderingMode(int)));
}

void MLRenderingParametersTab::updateVisibility(MeshModel* mm)
{
    for(QMap<QString,MLRenderingParametersFrame*>::iterator itt = _parframe.begin();itt != _parframe.end();++itt)
        itt.value()->updateVisibility(mm);
}

void MLRenderingParametersTab::updatePerMeshRenderingAction(QList<MLRenderingAction*>& acts)
{
	_paract.clear();
	for(int ii = 0;ii < acts.size();++ii)
		_paract.push_back(acts[ii]);
}

MLRenderingParametersTab::~MLRenderingParametersTab()
{
    for(QMap<QString,MLRenderingParametersFrame*>::iterator it = _parframe.begin();it != _parframe.end();++it)
        delete it.value();
}

void MLRenderingParametersTab::setAssociatedMeshIdAndRenderingData( int meshid,const MLRenderingData& dt )
{
    _meshid = meshid;
    for(QMap<QString,MLRenderingParametersFrame*>::iterator itt = _parframe.begin();itt != _parframe.end();++itt)
    {
        (*itt)->setAssociatedMeshId(meshid);
        (*itt)->setPrimitiveButtonStatesAccordingToRenderingData(dt);
    }
}

void MLRenderingParametersTab::getCurrentRenderingDataAccordingToGUI( MLRenderingData& dt ) const
{
    for(QMap<QString,MLRenderingParametersFrame*>::const_iterator it = _parframe.begin();it != _parframe.end();++it)
        if (it.value() != NULL)
            it.value()->getCurrentRenderingDataAccordingToGUI(dt);
}

MLRenderingColorPicker::MLRenderingColorPicker( int meshid,MLRenderingData::PRIMITIVE_MODALITY pr,QWidget *p )
    :QToolButton(p),_act(NULL)
{
    _act = new MLRenderingUserDefinedColorAction(pr,meshid,this);
    //_act->setColor(vcg::ColorConverter::ToColor4b(def));
    initGui();
}

MLRenderingColorPicker::MLRenderingColorPicker( MLRenderingData::PRIMITIVE_MODALITY pr,QWidget *p )
    :QToolButton(p),_act(NULL)
{
    _act = new MLRenderingUserDefinedColorAction(pr,-1,this);
    //_act->setColor(vcg::ColorConverter::ToColor4b(def));
    initGui();
}

MLRenderingColorPicker::~MLRenderingColorPicker()
{
    delete _cbutton;
    delete _act;
}
//
void MLRenderingColorPicker::updateColorInfo()
{
    if (_act == NULL)
        return;
    const QColor cc = vcg::ColorConverter::ToQColor(_act->getColor());
    QPalette pal(cc);
    _cbutton->setPalette(pal);
}

void MLRenderingColorPicker::pickColor()
{
    if (_act == NULL)
        return;
    const QColor cc = vcg::ColorConverter::ToQColor(_act->getColor());
    QColor ret = QColorDialog::getColor(cc,this);
    if (ret.isValid())
    {
        _act->setColor(vcg::ColorConverter::ToColor4b(ret));
        updateColorInfo();
        emit userDefinedColorAction(_act->meshId(),_act);
    }
}

void MLRenderingColorPicker::initGui()
{
    if (_act == NULL)
        return;
    setDefaultAction(_act);
    //setText(_act->text());
    QMenu* colmenu = new QMenu();
    QWidgetAction* wa = new QWidgetAction(colmenu);
    _cbutton = new QPushButton(colmenu);
    _cbutton->setAutoFillBackground(true);
    _cbutton->setFlat(true);
    _cbutton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    wa->setDefaultWidget(_cbutton);
    colmenu->addAction(wa);
    setMenu(colmenu);
    updateColorInfo();
    connect(_cbutton,SIGNAL(clicked()),this,SLOT(pickColor()));
}

void MLRenderingColorPicker::setColor( QColor def )
{
    _act->setColor(def);
    updateColorInfo();
}

MLRenderingBBoxColorPicker::MLRenderingBBoxColorPicker(QWidget *p )
    :QToolButton(p),_act(NULL)
{
    _act = new MLRenderingBBoxUserDefinedColorAction(-1,this);
    //_act->setColor(vcg::ColorConverter::ToColor4b(def));
    initGui();
}

MLRenderingBBoxColorPicker::MLRenderingBBoxColorPicker( int meshid,QWidget *p )
    :QToolButton(p),_act(NULL)
{
    _act = new MLRenderingBBoxUserDefinedColorAction(meshid,this);
    initGui();
}

MLRenderingBBoxColorPicker::~MLRenderingBBoxColorPicker()
{
    delete _cbutton;
    delete _act;
}
//
void MLRenderingBBoxColorPicker::updateColorInfo()
{
    if (_act == NULL)
        return;
    const QColor cc = vcg::ColorConverter::ToQColor(_act->getColor());
    QPalette pal(cc);
    _cbutton->setPalette(pal);
}

void MLRenderingBBoxColorPicker::pickColor()
{
    if (_act == NULL)
        return;
    const QColor cc = vcg::ColorConverter::ToQColor(_act->getColor());
    QColor ret = QColorDialog::getColor(cc,this);
    if (ret.isValid())
    {
        _act->setColor(vcg::ColorConverter::ToColor4b(ret));
        updateColorInfo();
        emit userDefinedColorAction(_act->meshId(),_act);
    }
}

void MLRenderingBBoxColorPicker::initGui()
{
    if (_act == NULL)
        return;
    setDefaultAction(_act);
    //setText(_act->text());
    QMenu* colmenu = new QMenu();
    QWidgetAction* wa = new QWidgetAction(colmenu);
    _cbutton = new QPushButton(colmenu);
    _cbutton->setAutoFillBackground(true);
    _cbutton->setFlat(true);
    _cbutton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    wa->setDefaultWidget(_cbutton);
    colmenu->addAction(wa);
    setMenu(colmenu);
    updateColorInfo();
    connect(_cbutton,SIGNAL(clicked()),this,SLOT(pickColor()));
}

void MLRenderingBBoxColorPicker::setColor( QColor def )
{
    _act->setColor(def);
    updateColorInfo();
}

//void MLRenderingColorPicker::paintEvent( QPaintEvent * )
//{
//    QStylePainter p(this);
//    QStyleOptionToolButton opt;
//    initStyleOption( & opt ); 
//    QColor cc = vcg::ColorConverter::ToQColor(_act->getColor());
//    opt.palette = QPalette(QColor(255,0,0),QColor(0,255,0),QColor(0,0,255),QColor(255,255,0),QColor(255,0,255),QColor(0,125,125),QColor(255,255,255));
//    //opt.features &= (~ QStyleOptionToolButton::HasMenu);
//    p.drawComplexControl( QStyle::CC_ToolButton, opt );
//}


MLRenderingOnOffToolbar::MLRenderingOnOffToolbar( int meshid,QWidget* parent /*= NULL*/ )
    :QToolBar(parent),_meshid(meshid),_act(NULL)
{
    initGui();
}

MLRenderingOnOffToolbar::~MLRenderingOnOffToolbar()
{

}

void MLRenderingOnOffToolbar::initGui()
{
    QActionGroup* actgroup = new QActionGroup(this);
    _onact = addAction(QString("On"));
    _offact = addAction(QString("Off"));
    _onact->setCheckable(true);
    _onact->setVisible(true);
    _offact->setCheckable(true);
    _offact->setVisible(true);
    _onact->setActionGroup(actgroup);
    _offact->setActionGroup(actgroup);

    connect(this,SIGNAL(actionTriggered(QAction*)),this,SLOT(toggle(QAction*)));
}

void MLRenderingOnOffToolbar::toggle(QAction* clickedact)
{
    if ((_act != NULL) && (clickedact != NULL))
    {
        //clickedact->setChecked(true);
        _act->setChecked(_onact->isChecked());
        emit updateRenderingDataAccordingToAction(_meshid,_act);
    }
}

void MLRenderingOnOffToolbar::setAccordingToRenderingData( const MLRenderingData& dt )
{
    _onact->setChecked(_act->isRenderingDataEnabled(dt));
    _offact->setChecked(!(_act->isRenderingDataEnabled(dt)));
}

void MLRenderingOnOffToolbar::setAssociatedMeshId( int meshid )
{
    _meshid = meshid;
    if (_act != NULL)
        _act->setMeshId(meshid);
}

void MLRenderingOnOffToolbar::setRenderingAction( MLRenderingAction* act )
{
    _act = act;
    _act->setVisible(false);
    _act->setCheckable(true);
}

void MLRenderingOnOffToolbar::getRenderingDataAccordingToGUI( MLRenderingData& dt ) const
{
    if (_act != NULL)
        _act->updateRenderingData(dt);
}

void MLRenderingOnOffToolbar::updateVisibility( MeshModel* mm )
{
    if (_act != NULL)    
    {
        _offact->setCheckable(_act->isCheckableConditionValid(mm));
        _onact->setCheckable(_act->isCheckableConditionValid(mm));
    }
}

MLRenderingFloatSlider::MLRenderingFloatSlider( int meshid,QWidget *p )
    :MLFloatSlider(p),_meshid(meshid),_act(NULL)
{
    connect(this,SIGNAL(floatValueChanged(float)),this,SLOT(valueChanged(float)));
}

MLRenderingFloatSlider::MLRenderingFloatSlider(QWidget *p )
    :MLFloatSlider(p),_meshid(-1),_act(NULL)
{
    connect(this,SIGNAL(floatValueChanged(float)),this,SLOT(valueChanged(float)));
}

MLRenderingFloatSlider::~MLRenderingFloatSlider()
{

}

void MLRenderingFloatSlider::setRenderingFloatAction( MLRenderingFloatAction* act )
{
    _act = act;
}

void MLRenderingFloatSlider::setAccordingToRenderingData( const MLRenderingData& dt )
{
    if (_act != NULL)
    {
        float dtval = _act->getValueFromRenderingData(dt);
        setValue(dtval);
        _act->setValue(dtval);
    }
}

void MLRenderingFloatSlider::valueChanged( float val)
{
    if (_act != NULL)
    {
        _act->setValue(val);
        emit updateRenderingDataAccordingToAction(_meshid,_act);
    }
}

void MLRenderingFloatSlider::setAssociatedMeshId( int meshid )
{
    _meshid = meshid;
    if (_act != NULL)
        _act->setMeshId(_meshid);
}

void MLRenderingFloatSlider::getRenderingDataAccordingToGUI( MLRenderingData& dt ) const
{
    if (_act != NULL)
        _act->updateRenderingData(dt);
}

