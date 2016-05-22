#include "ml_render_gui.h"
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QHeaderView>

MLRenderingToolbar::MLRenderingToolbar(bool exclusive,QWidget* parent )
    :QToolBar(parent),_meshid(-1),_exclusive(exclusive),_previoussel(NULL)
{
    connect(this,SIGNAL(actionTriggered(QAction*)),this,SLOT(toggle(QAction*)));
}

MLRenderingToolbar::MLRenderingToolbar(int meshid,bool exclusive,QWidget* parent )
    :QToolBar(parent),_meshid(meshid),_exclusive(exclusive),_previoussel(NULL)
{
    connect(this,SIGNAL(actionTriggered(QAction*)),this,SLOT(toggle(QAction*)));
}

MLRenderingToolbar::~MLRenderingToolbar()
{
}

void MLRenderingToolbar::addRenderingAction( MLRenderingAction* act )
{
    _acts.push_back(act);
    addAction(act);
    act->setCheckable(true);
    act->setVisible(true);
}

void MLRenderingToolbar::toggle( QAction* act)
{
    MLRenderingAction* ract = qobject_cast<MLRenderingAction*>(act);
    if ((_exclusive) && (ract != NULL)) 
    {
        foreach(MLRenderingAction* curact,_acts)
        {
            if (curact != NULL)
            {
                if (curact != ract)
                    curact->setChecked(false);
                else
                {
                    if (ract == _previoussel)
                    {
                        ract->setChecked(false);
                        _previoussel = NULL;
                    }
                    else
                    {
                        ract->setChecked(true);
                        _previoussel = ract;
                    }
                }
            }
        }
    }
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
    {
        rendact->setChecked(rendact->isRenderingDataEnabled(dt));
        if ((_exclusive) && (rendact->isChecked()))
            _previoussel = rendact;

    }
}

void MLRenderingToolbar::getRenderingActionsCopy(QList<MLRenderingAction*>& acts,QObject* newparent) const
{
    foreach(MLRenderingAction* act,_acts)
        acts.push_back(act->copyAction(act,newparent));
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


//MLRenderingParametersFrame::~MLRenderingParametersFrame()
//{
//
//}
//
//MLRenderingParametersFrame::MLRenderingParametersFrame( int meshid,const MLRenderingData& data,QWidget* parent )
//    :QFrame(parent),_meshid(meshid)
//{
//    initGui(data);
//}
//
//void MLRenderingParametersFrame::initGui( const MLRenderingData& dt )
//{
//    setAutoFillBackground(true);
//    QVBoxLayout* layout = new QVBoxLayout();    
//    _paramgrouptree = new QTreeWidget(this);
//    _paramgrouptree->setAutoFillBackground(true);
//    _paramgrouptree->setFrameStyle(QFrame::NoFrame);
//    _paramgrouptree->setContentsMargins(0,0,0,0);
//    layout->addWidget(_paramgrouptree);
//    _paramgrouptree->header()->hide();
//    QTreeWidgetItem* soliditm = new QTreeWidgetItem();
//    soliditm->setText(0,tr("solid_params"));
//    _paramgrouptree->addTopLevelItem(soliditm);
//    QTreeWidgetItem* widgitm = new QTreeWidgetItem();
//    soliditm->addChild(widgitm);
//    _solid = new MLRenderingSolidParametersFrame(_meshid,dt,this);
//    _paramgrouptree->setItemWidget(widgitm,0,_solid);
//    //QPushButton* cb =new QPushButton("PUPPA",this);
//    _paramgrouptree->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);
//    setContentsMargins(0,0,0,0);
//    setLayout(layout);
//    
//}




MLRenderingSideToolbar::MLRenderingSideToolbar(QWidget* parent /*= NULL*/ )
    :MLRenderingToolbar(false,parent)
{
    initGui();
}

MLRenderingSideToolbar::MLRenderingSideToolbar(int meshid,QWidget* parent /*= NULL*/ )
        :MLRenderingToolbar(meshid,false,parent)
{
    initGui();
}

void MLRenderingSideToolbar::initGui()
{
    addRenderingAction(new MLRenderingBBoxAction(_meshid,this));
    addRenderingAction(new MLRenderingPointsAction(_meshid,this));
    addRenderingAction(new MLRenderingWireAction(_meshid,this));
    addRenderingAction(new MLRenderingSolidAction(_meshid,this));
    addRenderingAction(new MLRenderingLightOnOffAction(_meshid,this));

}




MLRenderingParametersFrame::MLRenderingParametersFrame( int meshid,QWidget* parent )
    :QFrame(parent),_meshid(-1)
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

    if (qobject_cast<MLRenderingLightOnOffAction*>(act) != NULL)
        return new MLRenderingLightingParametersFrame(meshid,parent);

    if (qobject_cast<MLRenderingBBoxAction*>(act) != NULL)
        return new MLRenderingBBoxParametersFrame(meshid,parent);

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
    _shadingtool = new MLRenderingToolbar(_meshid,true,this);
    QLabel* shadelab = new QLabel("Shading",this);
    QFont boldfont;
    boldfont.setBold(true);
    shadelab->setFont(boldfont);
    layout->addWidget(shadelab,0,0,Qt::AlignLeft);
    _shadingtool->addRenderingAction(new MLRenderingSmoothAction(_meshid,this));
    _shadingtool->addRenderingAction(new MLRenderingFlatAction(_meshid,this));
    layout->addWidget(_shadingtool,0,1,Qt::AlignLeft);
    connect(_shadingtool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));
    

    QLabel* colorlab = new QLabel("Color",this);
    colorlab->setFont(boldfont);
    layout->addWidget(colorlab,1,0,Qt::AlignLeft);
    _colortool = new MLRenderingToolbar(_meshid,true,this);
    _colortool->addRenderingAction(new MLRenderingPerVertexColorAction(_meshid,this));
    _colortool->addRenderingAction(new MLRenderingPerFaceColorAction(_meshid,this));
    _colortool->addRenderingAction(new MLRenderingPerMeshColorAction(_meshid,this));
    _colortool->addRenderingAction(new MLRenderingUserDefinedColorAction(vcg::GLMeshAttributesInfo::PR_SOLID,_meshid,this));
    layout->addWidget(_colortool,1,1,Qt::AlignLeft);
    connect(_colortool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));


    QLabel* textlab = new QLabel("Texture Coord",this);
    textlab->setFont(boldfont);
    layout->addWidget(textlab,2,0,Qt::AlignLeft);
    _texttool = new MLRenderingToolbar(this);
    _texttool->addRenderingAction(new MLRenderingPerVertTextCoordAction(_meshid,this));
    _texttool->addRenderingAction(new MLRenderingPerWedgeTextCoordAction(_meshid,this));
    layout->addWidget(_texttool,2,1,Qt::AlignLeft);
    connect(_texttool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));

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
    delete _wiretool;
    delete _shadingtool;
    delete _colortool;
    delete _texttool;
}

void MLRenderingWireParametersFrame::setPrimitiveButtonStatesAccordingToRenderingData( const MLRenderingData& dt )
{
    _shadingtool->setAccordingToRenderingData(dt);
    _colortool->setAccordingToRenderingData(dt);
    _texttool->setAccordingToRenderingData(dt);
    _wiretool->setAccordingToRenderingData(dt);
}

void MLRenderingWireParametersFrame::setAssociatedMeshId( int meshid )
{
    _meshid = meshid;
    _shadingtool->setAssociatedMeshId(meshid);
    _colortool->setAssociatedMeshId(meshid);
    _texttool->setAssociatedMeshId(meshid);
    _wiretool->setAssociatedMeshId(meshid);
}

void MLRenderingWireParametersFrame::initGui()
{
    setAutoFillBackground(true);
    QGridLayout* layout = new QGridLayout();
    _shadingtool = new MLRenderingToolbar(_meshid,true,this);
    QLabel* shadelab = new QLabel("Shading",this);
    QFont boldfont;
    boldfont.setBold(true);
    shadelab->setFont(boldfont);
    layout->addWidget(shadelab,0,0,Qt::AlignLeft);
    _shadingtool->addRenderingAction(new MLRenderingSmoothAction(_meshid,this));
    _shadingtool->addRenderingAction(new MLRenderingFlatAction(_meshid,this));
    layout->addWidget(_shadingtool,0,1,Qt::AlignLeft);
    connect(_shadingtool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));

    QLabel* colorlab = new QLabel("Color",this);
    colorlab->setFont(boldfont);
    layout->addWidget(colorlab,1,0,Qt::AlignLeft);
    _colortool = new MLRenderingToolbar(_meshid,true,this);
    _colortool->addRenderingAction(new MLRenderingPerVertexColorAction(_meshid,this));
    _colortool->addRenderingAction(new MLRenderingPerFaceColorAction(_meshid,this));
    _colortool->addRenderingAction(new MLRenderingPerMeshColorAction(_meshid,this));
    _colortool->addRenderingAction(new MLRenderingUserDefinedColorAction(vcg::GLMeshAttributesInfo::PR_WIREFRAME_TRIANGLES,_meshid,this));
    layout->addWidget(_colortool,1,1,Qt::AlignLeft);
    connect(_colortool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));
    
    QLabel* textlab = new QLabel("Texture Coord",this);
    textlab->setFont(boldfont);
    layout->addWidget(textlab,2,0,Qt::AlignLeft);
    _texttool = new MLRenderingToolbar(this);
    _texttool->addRenderingAction(new MLRenderingPerVertTextCoordAction(_meshid,this));
    _texttool->addRenderingAction(new MLRenderingPerWedgeTextCoordAction(_meshid,this));
    layout->addWidget(_texttool,2,1,Qt::AlignLeft);
    connect(_texttool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));

    QLabel* wirelab = new QLabel("Wire Modality",this);
    wirelab->setFont(boldfont);
    layout->addWidget(wirelab,3,0,Qt::AlignLeft);
    _wiretool = new MLRenderingToolbar(this);
    _wiretool->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _wiretool->addRenderingAction(new MLRenderingWireAction(_meshid,this));
    _wiretool->addRenderingAction(new MLRenderingEdgeWireAction(_meshid,this));
    layout->addWidget(_wiretool,3,1,Qt::AlignLeft);
    connect(_wiretool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));

    setMinimumSize(layout->sizeHint());
    setLayout(layout);
    showNormal();
    adjustSize();   
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
}

void MLRenderingPointsParametersFrame::setPrimitiveButtonStatesAccordingToRenderingData( const MLRenderingData& dt )
{
    _shadingtool->setAccordingToRenderingData(dt);
    _colortool->setAccordingToRenderingData(dt);
    _texttool->setAccordingToRenderingData(dt);
}

void MLRenderingPointsParametersFrame::setAssociatedMeshId( int meshid )
{
    _meshid = meshid;
    _shadingtool->setAssociatedMeshId(meshid);
    _colortool->setAssociatedMeshId(meshid);
    _texttool->setAssociatedMeshId(meshid);
}

void MLRenderingPointsParametersFrame::initGui()
{
    setAutoFillBackground(true);
    QGridLayout* layout = new QGridLayout();
    _shadingtool = new MLRenderingToolbar(_meshid,true,this);
    QLabel* shadelab = new QLabel("Shading",this);
    QFont boldfont;
    boldfont.setBold(true);
    shadelab->setFont(boldfont);
    layout->addWidget(shadelab,0,0,Qt::AlignLeft);
    _shadingtool->addRenderingAction(new MLRenderingSmoothAction(_meshid,this));
    layout->addWidget(_shadingtool,0,1,Qt::AlignLeft);
    connect(_shadingtool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));

    QLabel* colorlab = new QLabel("Color",this);
    colorlab->setFont(boldfont);
    layout->addWidget(colorlab,1,0,Qt::AlignLeft);
    _colortool = new MLRenderingToolbar(_meshid,true,this);
    _colortool->addRenderingAction(new MLRenderingPerVertexColorAction(_meshid,this));
    _colortool->addRenderingAction(new MLRenderingPerMeshColorAction(_meshid,this));
    _colortool->addRenderingAction(new MLRenderingUserDefinedColorAction(vcg::GLMeshAttributesInfo::PR_POINTS,_meshid,this));
    layout->addWidget(_colortool,1,1,Qt::AlignLeft);
    connect(_colortool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));

    QLabel* textlab = new QLabel("Texture Coord",this);
    textlab->setFont(boldfont);
    layout->addWidget(textlab,2,0,Qt::AlignLeft);
    _texttool = new MLRenderingToolbar(this);
    _texttool->addRenderingAction(new MLRenderingPerVertTextCoordAction(_meshid,this));
    layout->addWidget(_texttool,2,1,Qt::AlignLeft);
    connect(_texttool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));

    setMinimumSize(layout->sizeHint());
    setLayout(layout);
    showNormal();
    adjustSize();   
}

MLRenderingLightingParametersFrame::MLRenderingLightingParametersFrame( QWidget* parent )
    :MLRenderingParametersFrame(-1,parent)
{
    initGui();
}

MLRenderingLightingParametersFrame::MLRenderingLightingParametersFrame( int meshid,QWidget* parent )
    :MLRenderingParametersFrame(meshid,parent)
{
    initGui();
}


MLRenderingLightingParametersFrame::~MLRenderingLightingParametersFrame()
{
    delete _lighttool;
}

void MLRenderingLightingParametersFrame::setPrimitiveButtonStatesAccordingToRenderingData( const MLRenderingData& dt )
{
    _lighttool->setAccordingToRenderingData(dt);
}

void MLRenderingLightingParametersFrame::setAssociatedMeshId( int meshid )
{
    _meshid = meshid;
    _lighttool->setAssociatedMeshId(meshid);
}

void MLRenderingLightingParametersFrame::initGui()
{
    setAutoFillBackground(true);
    QGridLayout* layout = new QGridLayout();
    _lighttool = new MLRenderingToolbar(_meshid,true,this);
    QLabel* lightlab = new QLabel("Lighting Options",this);
    QFont boldfont;
    boldfont.setBold(true);
    lightlab->setFont(boldfont);
    layout->addWidget(lightlab,0,0,Qt::AlignLeft);
    _lighttool->addRenderingAction(new MLRenderingFancyLightingAction(_meshid,this));
    _lighttool->addRenderingAction(new MLRenderingDoubleLightingAction(_meshid,this));
    layout->addWidget(_lighttool,0,1,Qt::AlignLeft);
    connect(_lighttool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));

    setMinimumSize(layout->sizeHint());
    setLayout(layout);
    showNormal();
    adjustSize();   
}

MLRenderingBBoxParametersFrame::MLRenderingBBoxParametersFrame( QWidget* parent )
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
}

void MLRenderingBBoxParametersFrame::setPrimitiveButtonStatesAccordingToRenderingData( const MLRenderingData& dt )
{
    _colortool->setAccordingToRenderingData(dt);
}

void MLRenderingBBoxParametersFrame::setAssociatedMeshId( int meshid )
{
    _meshid = meshid;
    _colortool->setAssociatedMeshId(meshid);
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
    _colortool = new MLRenderingToolbar(_meshid,true,this);
    _colortool->addRenderingAction(new MLRenderingPerMeshColorAction(_meshid,this));
    _colortool->addRenderingAction(new MLRenderingUserDefinedColorAction(vcg::GLMeshAttributesInfo::PR_BBOX,_meshid,this));
    layout->addWidget(_colortool,0,1,Qt::AlignLeft);
    connect(_colortool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));

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


void MLRenderingParametersTab::switchTab(int meshid,const QString& tabname,const MLRenderingData& dt)
{
    QMap<QString,MLRenderingParametersFrame*>::iterator itt = _parframe.find(tabname);
    if (itt != _parframe.end())
        setCurrentWidget((*itt));
    setAssociatedMeshIdAndRenderingData(meshid,dt);
}

void MLRenderingParametersTab::setPrimitiveButtonStatesAccordingToRenderingData(const MLRenderingData& dt )
{
    for(QMap<QString,MLRenderingParametersFrame*>::iterator itt = _parframe.begin();itt != _parframe.end();++itt)
        (*itt)->setPrimitiveButtonStatesAccordingToRenderingData(dt);
}

void MLRenderingParametersTab::initGui(const QList<MLRenderingAction*>& tab)
{
    foreach(MLRenderingAction* ract,tab)
    {
        if (ract != NULL)
        {
            MLRenderingParametersFrame* fr = MLRenderingParametersFrame::factory(ract,_meshid,this);
            if (fr != NULL)
            {
                _parframe[ract->text()] = fr;
                addTab(fr,ract->icon(),"");
                connect(fr,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));
            }
        }
    }
}

MLRenderingParametersTab::~MLRenderingParametersTab()
{
    for(QMap<QString,MLRenderingParametersFrame*>::iterator it = _parframe.begin();it != _parframe.end();++it)
        delete it.value();
}

void MLRenderingParametersTab::setAssociatedMeshIdAndRenderingData( int meshid,const MLRenderingData& dt )
{
    for(QMap<QString,MLRenderingParametersFrame*>::iterator itt = _parframe.begin();itt != _parframe.end();++itt)
    {
        (*itt)->setAssociatedMeshId(meshid);
        (*itt)->setPrimitiveButtonStatesAccordingToRenderingData(dt);
    }
}

