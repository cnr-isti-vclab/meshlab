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
        emit updateRenderingDataAccordingToActions(_meshid,_acts);
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

void MLRenderingSolidParametersFrame::getAllRenderingActions( QList<MLRenderingAction*>& acts )
{
    acts.append(_shadingtool->getRenderingActions());
    acts.append(_colortool->getRenderingActions());
    acts.append(_texttool->getRenderingActions());
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
        
    /*if (qobject_cast<MLRenderingPointsAction*>(act) != NULL)
        return new MLRenderingPointsParametersFrame(meshid,parent);

    if (qobject_cast<MLRenderingWireAction*>(act) != NULL)
        return new MLRenderingWireParametersFrame(meshid,parent);

    if (qobject_cast<MLRenderingLightOnOffAction*>(act) != NULL)
        return new MLRenderingLightingParametersFrame(meshid,parent);*/

    /*if (qobject_cast<MLRenderingLightOnOffAction*>(act) != NULL)
        return new MLRenderingSolidParametersFrame(meshid,parent);*/

    return NULL;
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


void MLRenderingParametersTab::switchTab( const QString& name )
{
    QMap<QString,MLRenderingParametersFrame*>::iterator itt = _parframe.find(name);
    if (itt != _parframe.end())
        setCurrentWidget((*itt));
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

