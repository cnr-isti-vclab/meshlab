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

bool MLRenderingToolbar::updateVisibility(MeshModel* mm)
{
	bool isvis = false;
    foreach(MLRenderingAction* ract,_acts)
    {
		bool actvis = ract->isVisibleConditionValid(mm);
		isvis = isvis || actvis;
        ract->setVisible(actvis);
		ract->setCheckable(actvis);
        QWidget* wid = widgetForAction(ract);
		if (wid != NULL)
		{
			wid->setVisible(actvis);
		}	
    }
	return isvis;
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
		emit updateRenderingDataAccordingToActions(_meshid, ract, _acts);
		if (ract->isChecked())
			emit activatedAction(ract);
    }
}

void MLRenderingToolbar::setAccordingToRenderingData(const MLRenderingData& dt)
{
	foreach(MLRenderingAction* rendact, _acts)
	{
		rendact->setChecked(rendact->isRenderingDataEnabled(dt));
	}
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

QList<QAction*> MLRenderingToolbar::getTopLevelActions()
{
	QList<QAction*> act;
	for (int ii = 0; ii < _acts.size(); ++ii)
	{
		if (_acts[ii] != NULL)
		{
			MLRenderingUserDefinedColorAction* udcact = qobject_cast<MLRenderingUserDefinedColorAction*>(_acts[ii]);
			MLRenderingBBoxUserDefinedColorAction* bbcact = qobject_cast<MLRenderingBBoxUserDefinedColorAction*>(_acts[ii]);
			if ((udcact == NULL) && (bbcact == NULL))
				act.push_back(_acts[ii]);
			else
			{
				QList<QAction*> curracts = actions();
				if (curracts.size() == _acts.size())
				{
					QWidget* tmp = widgetForAction(curracts[ii]);
					if ((tmp != NULL) && (tmp->actions().size() > 0))
						act.push_back(tmp->actions()[0]);
				}
			}
		}
	}
	return act;
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
	addRenderingAction(new MLRenderingWireAction(_meshid, this));
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

MLRenderingThreeStateSideToolbar::MLRenderingThreeStateSideToolbar(QWidget* parent /*= NULL*/)
	:MLRenderingToolbar(parent)
{

	initGui();
}

void MLRenderingThreeStateSideToolbar::initGui()
{
	setStyleSheet("QToolBar{spacing:0px;padding:0px;} QToolButton{padding:0px;}");

	MLRenderingThreeStateButton* bboxbut = new MLRenderingThreeStateButton(_meshid, this);
	MLRenderingBBoxAction* bboxact =  new MLRenderingBBoxAction(_meshid, this);
	_acts.push_back(bboxact);
	bboxbut->setRenderingAction(bboxact);
	addWidget(bboxbut);

	MLRenderingThreeStateButton* pointbut = new MLRenderingThreeStateButton(_meshid, this);
	MLRenderingPointsAction* pointact = new MLRenderingPointsAction(_meshid, this);
	_acts.push_back(pointact);
	pointbut->setRenderingAction(pointact);
	addWidget(pointbut);

	MLRenderingThreeStateButton* wirebut = new MLRenderingThreeStateButton(_meshid, this);
	MLRenderingWireAction* wireact = new MLRenderingWireAction(_meshid, this);
	_acts.push_back(wireact);
	wirebut->setRenderingAction(wireact);
	addWidget(wirebut);

	MLRenderingThreeStateButton* solidbut = new MLRenderingThreeStateButton(_meshid, this);
	MLRenderingSolidAction* solidact =  new MLRenderingSolidAction(_meshid, this);
	_acts.push_back(solidact);
	solidbut->setRenderingAction(solidact);
	addWidget(solidbut);

	MLRenderingThreeStateButton* selbut = new MLRenderingThreeStateButton(_meshid, this);
	MLRenderingSelectionAction* selact = new MLRenderingSelectionAction(_meshid, this);
	_acts.push_back(selact);
	selbut->setRenderingAction(selact);
	addWidget(selbut);

	MLRenderingThreeStateButton* edgebut = new MLRenderingThreeStateButton(_meshid, this);
	MLRenderingEdgeDecoratorAction* edgeact = new MLRenderingEdgeDecoratorAction(_meshid, this);
	_acts.push_back(edgeact);
	edgebut->setRenderingAction(edgeact);
	addWidget(edgebut);
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
    _shadelab = new QLabel("Shading",this);
    QFont boldfont;
    boldfont.setBold(true);
    _shadelab->setFont(boldfont);
    layout->addWidget(_shadelab,0,0,Qt::AlignLeft);
    _shadingtool->addRenderingAction(new MLRenderingPerVertexNormalAction(MLRenderingData::PR_SOLID,_meshid,_shadingtool));
    _shadingtool->addRenderingAction(new MLRenderingPerFaceNormalAction(_meshid,_shadingtool));
    _shadingtool->addRenderingAction(new MLRenderingNoShadingAction(MLRenderingData::PR_SOLID,_meshid,_shadingtool));
    layout->addWidget(_shadingtool,0,1,Qt::AlignLeft);
    connect(_shadingtool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));
	connect(_shadingtool, SIGNAL(updateRenderingDataAccordingToActions(int,MLRenderingAction*, QList<MLRenderingAction*>&)), this, SIGNAL(updateRenderingDataAccordingToActions(int, MLRenderingAction*, QList<MLRenderingAction*>&)));

    _colorlab = new QLabel("Color",this);
    _colorlab->setFont(boldfont);
    layout->addWidget(_colorlab,1,0,Qt::AlignLeft);
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
	connect(_colortool, SIGNAL(updateRenderingDataAccordingToActions(int, MLRenderingAction*, QList<MLRenderingAction*>&)), this, SIGNAL(updateRenderingDataAccordingToActions(int, MLRenderingAction*, QList<MLRenderingAction*>&)));
    
	_textlab = new QLabel("Texture Coord",this);
    _textlab->setFont(boldfont);
    layout->addWidget(_textlab,2,0,Qt::AlignLeft);
    _texttool = new MLRenderingOnOffToolbar(_meshid,this);
    _texttool->setRenderingAction(new MLRenderingPerWedgeTextCoordAction(_meshid,_texttool));
    layout->addWidget(_texttool,2,1,Qt::AlignLeft);
    connect(_texttool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));
	connect(_texttool, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)), this, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)));

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
	_shadelab->setVisible(_shadingtool->updateVisibility(mm));
    _colorlab->setVisible(_colortool->updateVisibility(mm));
    _textlab->setVisible(_texttool->updateVisibility(mm));
}

void MLRenderingSolidParametersFrame::actionsList(QList<MLRenderingAction*>& actions)
{
	actions.append(_shadingtool->getRenderingActions());
	actions.append(_colortool->getRenderingActions());
	actions.append(_texttool->getRenderingAction());
}

void MLRenderingSolidParametersFrame::allTopLevelGuiActions(QList<QAction*>& tplevelactions)
{
	tplevelactions.append(_shadingtool->getTopLevelActions());
	tplevelactions.append(_colortool->getTopLevelActions());
	tplevelactions.append(_texttool->actions());
}

void MLRenderingSolidParametersFrame::setTextureAction(MLRenderingData::ATT_NAMES textattname)
{
	if (textattname == MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE)
	{
		MLRenderingPerWedgeTextCoordAction* wedact = new MLRenderingPerWedgeTextCoordAction(_meshid, this);
		_texttool->setRenderingAction(wedact);
	}
	else
	{
		if (textattname == MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE)
		{
			MLRenderingPerVertTextCoordAction* vertact = new MLRenderingPerVertTextCoordAction(MLRenderingData::PR_SOLID, this);
			_texttool->setRenderingAction(vertact);
		}
	}	
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
    _shadelab = new QLabel("Shading",this);
    QFont boldfont;
    boldfont.setBold(true);
    _shadelab->setFont(boldfont);
    layout->addWidget(_shadelab,0,0,Qt::AlignLeft);
    _shadingtool->addRenderingAction(new MLRenderingPerVertexNormalAction(MLRenderingData::PR_WIREFRAME_TRIANGLES,_meshid,this));
    _shadingtool->addRenderingAction(new MLRenderingNoShadingAction(MLRenderingData::PR_WIREFRAME_TRIANGLES,_meshid,this));
    layout->addWidget(_shadingtool,0,1,Qt::AlignLeft);
    connect(_shadingtool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));
	connect(_shadingtool, SIGNAL(updateRenderingDataAccordingToActions(int, MLRenderingAction*, QList<MLRenderingAction*>&)), this, SIGNAL(updateRenderingDataAccordingToActions(int, MLRenderingAction*, QList<MLRenderingAction*>&)));

    _colorlab = new QLabel("Color",this);
    _colorlab->setFont(boldfont);
    layout->addWidget(_colorlab,1,0,Qt::AlignLeft);
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
	connect(_colortool, SIGNAL(updateRenderingDataAccordingToActions(int, MLRenderingAction*, QList<MLRenderingAction*>&)), this, SIGNAL(updateRenderingDataAccordingToActions(int, MLRenderingAction*, QList<MLRenderingAction*>&)));

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
	connect(_dimension, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)), this, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)));

    _wirelab = new QLabel("Polygonal Modality",this);
    _wirelab->setFont(boldfont);
    layout->addWidget(_wirelab,3,0,Qt::AlignLeft);
    _edgetool = new MLRenderingOnOffToolbar(_meshid,this);
    _edgetool->setRenderingAction(new MLRenderingFauxEdgeWireAction(_meshid,this));
    layout->addWidget(_edgetool,3,1,Qt::AlignLeft);
    connect(_edgetool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));
	connect(_edgetool, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*,bool)), this, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*,bool)));
	connect(_edgetool, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*)), this, SLOT(switchWireModality(int, MLRenderingAction*)));
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
    _shadelab->setVisible(_shadingtool->updateVisibility(mm));
    _colorlab->setVisible(_colortool->updateVisibility(mm));
    _wirelab->setVisible(_edgetool->updateVisibility(mm));
}


void MLRenderingWireParametersFrame::actionsList(QList<MLRenderingAction*>& actions)
{
	actions.append(_shadingtool->getRenderingActions());
	actions.append(_colortool->getRenderingActions());
	actions.append(_edgetool->getRenderingAction());
	actions.append(_dimension->getRenderingAction());
}

void MLRenderingWireParametersFrame::allTopLevelGuiActions(QList<QAction*>& tplevelactions)
{
	tplevelactions.append(_shadingtool->getTopLevelActions());
	tplevelactions.append(_colortool->getTopLevelActions());
	tplevelactions.append(_edgetool->actions());
	tplevelactions.append(_dimension->actions());
}

void MLRenderingWireParametersFrame::switchWireModality(int meshid,MLRenderingAction* act)
{
	MLRenderingFauxEdgeWireAction* fauxact = qobject_cast<MLRenderingFauxEdgeWireAction*>(act);
	if (fauxact == NULL)
		return;
	MLRenderingData::PRIMITIVE_MODALITY pm;
	if (fauxact->isChecked())
		pm = MLRenderingData::PR_WIREFRAME_EDGES;
	else
		pm = MLRenderingData::PR_WIREFRAME_TRIANGLES;

	foreach(MLRenderingAction* shact, _shadingtool->getRenderingActions())
		shact->switchPrimitive(pm);
	
	foreach(MLRenderingAction* colact, _colortool->getRenderingActions())
		colact->switchPrimitive(pm);

	emit updateRenderingDataAccordingToActions(meshid, _shadingtool->getRenderingActions());
	emit updateRenderingDataAccordingToActions(meshid, _colortool->getRenderingActions());
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
    _shadelab = new QLabel("Shading",this);
    QFont boldfont;
    boldfont.setBold(true);
    _shadelab->setFont(boldfont);
    layout->addWidget(_shadelab,0,0,Qt::AlignLeft);
    _shadingtool->addRenderingAction(new MLRenderingPerVertexNormalAction(MLRenderingData::PR_POINTS,_meshid,this));
    _shadingtool->addRenderingAction(new MLRenderingDotAction(_meshid,this));
    _shadingtool->addRenderingAction(new MLRenderingNoShadingAction(MLRenderingData::PR_POINTS,_meshid,this));
    layout->addWidget(_shadingtool,0,1,Qt::AlignLeft);
    connect(_shadingtool,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));
	connect(_shadingtool, SIGNAL(updateRenderingDataAccordingToActions(int, MLRenderingAction*, QList<MLRenderingAction*>&)), this, SIGNAL(updateRenderingDataAccordingToActions(int, MLRenderingAction*, QList<MLRenderingAction*>&)));

    _colorlab = new QLabel("Color",this);
    _colorlab->setFont(boldfont);
    layout->addWidget(_colorlab,1,0,Qt::AlignLeft);
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
	connect(_colortool, SIGNAL(updateRenderingDataAccordingToActions(int, MLRenderingAction*,QList<MLRenderingAction*>&)), this, SIGNAL(updateRenderingDataAccordingToActions(int, MLRenderingAction*,QList<MLRenderingAction*>&)));


    _textlab = new QLabel("Texture Coord",this);
    _textlab->setFont(boldfont);
    layout->addWidget(_textlab,2,0,Qt::AlignLeft);
    _texttool = new MLRenderingOnOffToolbar(_meshid,this);
    _texttool->setRenderingAction(new MLRenderingPerVertTextCoordAction(MLRenderingData::PR_POINTS,_meshid,_texttool));
    layout->addWidget(_texttool,2,1,Qt::AlignLeft);
    connect(_texttool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));
	connect(_texttool, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)), this, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)));

    QLabel* dimelab = new QLabel("Point Size",this);
    dimelab->setFont(boldfont);
    layout->addWidget(dimelab,3,0,Qt::AlignLeft);
    _dimension = new MLRenderingFloatSlider(_meshid,this);
    _dimension->setRenderingFloatAction(new MLRenderingPointsSizeAction(_meshid,this));
    _dimension->setOrientation(Qt::Horizontal);
    _dimension->setMinimum(1);
    _dimension->setMaximum(5);
    connect(_dimension,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));
	connect(_dimension, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*,bool)), this, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*,bool)));
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
    _shadelab->setVisible(_shadingtool->updateVisibility(mm));
    _colorlab->setVisible(_colortool->updateVisibility(mm));
    _textlab->setVisible(_texttool->updateVisibility(mm));
}

void MLRenderingPointsParametersFrame::actionsList(QList<MLRenderingAction*>& actions)
{
	actions.append(_shadingtool->getRenderingActions());
	actions.append(_colortool->getRenderingActions());
	actions.append(_texttool->getRenderingAction());
	actions.append(_dimension->getRenderingAction());
}

void MLRenderingPointsParametersFrame::allTopLevelGuiActions(QList<QAction*>& tplevelactions)
{
	tplevelactions.append(_shadingtool->getTopLevelActions());
	tplevelactions.append(_colortool->getTopLevelActions());
	tplevelactions.append(_texttool->actions());
	tplevelactions.append(_dimension->actions());
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


void MLRenderingBBoxParametersFrame::actionsList(QList<MLRenderingAction*>& actions)
{
	actions.append(_colortool->getRenderingActions());
	actions.append(_quotedinfotool->getRenderingAction());
}

void MLRenderingBBoxParametersFrame::allTopLevelGuiActions(QList<QAction*>& tplevelactions)
{
	tplevelactions.append(_colortool->getTopLevelActions());
	tplevelactions.append(_quotedinfotool->actions());
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
    _colorlab = new QLabel("Color",this);
    QFont boldfont;
    boldfont.setBold(true);
    _colorlab->setFont(boldfont);
    layout->addWidget(_colorlab,0,0,Qt::AlignLeft);
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
	connect(_colortool, SIGNAL(updateRenderingDataAccordingToActions(int, MLRenderingAction*,QList<MLRenderingAction*>&)), this, SIGNAL(updateRenderingDataAccordingToActions(int, MLRenderingAction*,QList<MLRenderingAction*>&)));

    _quotedinfolab = new QLabel("Measure Info",this);
    _quotedinfolab->setFont(boldfont);
    layout->addWidget(_quotedinfolab,1,0,Qt::AlignLeft);
    _quotedinfotool = new MLRenderingOnOffToolbar(_meshid,this);
    _quotedinfotool->setRenderingAction(new MLRenderingBBoxQuotedInfoAction(_meshid,this));
    layout->addWidget(_quotedinfotool,1,1,Qt::AlignLeft);
    connect(_quotedinfotool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));
	connect(_quotedinfotool, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*,bool)), this, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*,bool)));

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


void MLRenderingDefaultDecoratorParametersFrame::actionsList(QList<MLRenderingAction*>& actions)
{
	actions.append(_boundearyfacetool->getRenderingAction());
	actions.append(_edgemanifoldtool->getRenderingAction());
	actions.append(_boundearyedgetool->getRenderingAction());
	actions.append(_texturebordertool->getRenderingAction());
	actions.append(_vertmanifoldtool->getRenderingAction());
}

void MLRenderingDefaultDecoratorParametersFrame::allTopLevelGuiActions(QList<QAction*>& tplevelactions)
{
	tplevelactions.append(_boundearyfacetool->actions());
	tplevelactions.append(_edgemanifoldtool->actions());
	tplevelactions.append(_boundearyedgetool->actions());
	tplevelactions.append(_texturebordertool->actions());
	tplevelactions.append(_vertmanifoldtool->actions());
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
	connect(_boundearyedgetool, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)), this, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)));

    QLabel* boundaryfacelab = new QLabel("Boundary Faces",this);
    boundaryfacelab->setFont(boldfont);
    layout->addWidget(boundaryfacelab,1,0,Qt::AlignLeft);
    _boundearyfacetool = new MLRenderingOnOffToolbar(_meshid,this);
    _boundearyfacetool->setRenderingAction(new MLRenderingFaceBoundaryAction(_meshid,_boundearyfacetool));
    layout->addWidget(_boundearyfacetool,1,1,Qt::AlignLeft);
    connect(_boundearyfacetool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));
	connect(_boundearyfacetool, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)), this, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)));

    QLabel* vertmanifoldlab = new QLabel("No-Manif Verts",this);
    vertmanifoldlab->setFont(boldfont);
    layout->addWidget(vertmanifoldlab,2,0,Qt::AlignLeft);
    _vertmanifoldtool = new MLRenderingOnOffToolbar(_meshid,this);
    _vertmanifoldtool->setRenderingAction(new MLRenderingVertManifoldAction(_meshid,_vertmanifoldtool));
    layout->addWidget(_vertmanifoldtool,2,1,Qt::AlignLeft);
    connect(_vertmanifoldtool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));
	connect(_vertmanifoldtool, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)), this, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)));

    QLabel* edgemanifoldlab = new QLabel("No-Manif Edges",this);
    edgemanifoldlab->setFont(boldfont);
    layout->addWidget(edgemanifoldlab,3,0,Qt::AlignLeft);
    _edgemanifoldtool = new MLRenderingOnOffToolbar(_meshid,this);
    _edgemanifoldtool->setRenderingAction(new MLRenderingEdgeManifoldAction(_meshid,_edgemanifoldtool));
    layout->addWidget(_edgemanifoldtool,3,1,Qt::AlignLeft);
    connect(_edgemanifoldtool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));
	connect(_edgemanifoldtool, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)), this, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)));

    QLabel* textureborderlab = new QLabel("Texture Border",this);
    textureborderlab->setFont(boldfont);
    layout->addWidget(textureborderlab,4,0,Qt::AlignLeft);
    _texturebordertool = new MLRenderingOnOffToolbar(_meshid,this);
    _texturebordertool->setRenderingAction(new MLRenderingTexBorderAction(_meshid,_texturebordertool));
    layout->addWidget(_texturebordertool,4,1,Qt::AlignLeft);
    connect(_texturebordertool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));
	connect(_texturebordertool, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)), this, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)));

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


void MLRenderingSelectionParametersFrame::actionsList(QList<MLRenderingAction*>& actions)
{
	actions.append(_faceseltool->getRenderingAction());
	actions.append(_vertexseltool->getRenderingAction());
}

void MLRenderingSelectionParametersFrame::allTopLevelGuiActions(QList<QAction*>& tplevelactions)
{
	tplevelactions.append(_faceseltool->actions());
	tplevelactions.append(_vertexseltool->actions());
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
	connect(_vertexseltool, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)), this, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)));

    QLabel* facelab = new QLabel("Selected Face",this);
    facelab->setFont(boldfont);
    layout->addWidget(facelab,1,0,Qt::AlignLeft);
    _faceseltool = new MLRenderingOnOffToolbar(_meshid,this);
    _faceseltool->setRenderingAction(new MLRenderingFaceSelectionAction(_meshid,_faceseltool));
    layout->addWidget(_faceseltool,1,1,Qt::AlignLeft);
    connect(_faceseltool,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));
	connect(_faceseltool, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)), this, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*, bool)));

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

void MLRenderingParametersTab::actionsList(QList<MLRenderingAction*>& actions)
{
	for (QMap<QString, MLRenderingParametersFrame*>::iterator it = _parframe.begin(); it != _parframe.end(); ++it)
	{
		if (it.value() != NULL)
			it.value()->actionsList(actions);
	}
}

void MLRenderingParametersTab::setTextureAction(MLRenderingData::ATT_NAMES textattname)
{
	for (QMap<QString, MLRenderingParametersFrame*>::iterator it = _parframe.begin(); it != _parframe.end(); ++it)
	{
		if (it.value() != NULL)
			it.value()->setTextureAction(textattname);
	}
}

void MLRenderingParametersTab::switchWireModality(MLRenderingFauxEdgeWireAction* act)
{
	for (QMap<QString, MLRenderingParametersFrame*>::iterator it = _parframe.begin(); it != _parframe.end(); ++it)
	{
		MLRenderingWireParametersFrame* wpm = qobject_cast<MLRenderingWireParametersFrame*>(it.value());
		if (wpm != NULL)
			wpm->switchWireModality(act->meshId(), act);
	}
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
		if (_meshid != -1)
		{
			_act->setChecked(_onact->isChecked());
			emit updateRenderingDataAccordingToAction(_meshid,_act);
		}
		else
		{
			_act->setChecked(clickedact->text() == QString("On"));
			emit updateRenderingDataAccordingToAction(_meshid, _act, (clickedact->text() == QString("On")));
		}
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

MLRenderingAction* MLRenderingOnOffToolbar::getRenderingAction()
{
	return _act;
}

void MLRenderingOnOffToolbar::getRenderingDataAccordingToGUI( MLRenderingData& dt ) const
{
    if (_act != NULL)
        _act->updateRenderingData(dt);
}

bool MLRenderingOnOffToolbar::updateVisibility( MeshModel* mm )
{
    if (_act != NULL)    
    {
		bool isvis = _act->isVisibleConditionValid(mm);
        _offact->setVisible(isvis);
		_offact->setCheckable(isvis);
		_onact->setVisible(isvis);
		_onact->setCheckable(isvis);
		return isvis;
    }
	return false;
}

MLRenderingThreeStateButton::MLRenderingThreeStateButton(int meshid, QWidget* parent /*= NULL*/)
	:QWidget(parent),_meshid(meshid)
{
	_onofftool = new MLRenderingOnOffToolbar(_meshid, parent);
	initGui();
}

MLRenderingThreeStateButton::~MLRenderingThreeStateButton()
{
}

void MLRenderingThreeStateButton::setRenderingAction(MLRenderingAction* act)
{
	if (act == NULL)
		return;
	_onofftool->setRenderingAction(act);
	//_onofftool->setMaximumSize(QSize(40, 15));
	QFont ff;
	ff.setPointSize(4);
	_onofftool->setFont(ff);
	if (_layout != NULL)
	{
		QLabel* iconlabel = new QLabel();
		iconlabel->setPixmap(act->icon().pixmap(QSize(20, 20)));
		_layout->addWidget(iconlabel, 0, 0,1,1,Qt::AlignCenter);
	}
}

void MLRenderingThreeStateButton::initGui()
{
	_layout = new QGridLayout();
	if (_onofftool != NULL)
		_layout->addWidget(_onofftool, 1, 0, 1, 1);
	setLayout(_layout);
	//connect(this, SIGNAL(actionTriggered(QAction*)), _onofftool, SLOT(toggle(QAction*)));
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

MLRenderingAction* MLRenderingFloatSlider::getRenderingAction()
{
	return _act;
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
		emit updateRenderingDataAccordingToAction(_meshid, _act,true);
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

MLRenderingGlobalToolbar::MLRenderingGlobalToolbar(QWidget* parent /*= NULL*/)
	:QToolBar(parent)
{
	initGui();
}

void MLRenderingGlobalToolbar::initGui()
{
	MyToolButton* bboxbut = new MyToolButton(0, this);
	MLRenderingBBoxAction* bboxact = new MLRenderingBBoxAction(this);
	//bboxbut->setDefaultAction(bboxact);
	MLRenderingOnOffToolbar* bboxonoff = new MLRenderingOnOffToolbar(-1, this);
	bboxonoff->setRenderingAction(bboxact);
	MLRenderingBBoxParametersFrame* bboxframe = new MLRenderingBBoxParametersFrame(-1, NULL);
	initToolButtonSubMenu(bboxbut,bboxonoff,bboxframe);
	addWidget(bboxbut);

	MyToolButton* pointbut = new MyToolButton(0, this);
	MLRenderingPointsAction* pointact = new MLRenderingPointsAction(this);
	//pointbut->setDefaultAction(pointact);
	MLRenderingOnOffToolbar* pointonoff = new MLRenderingOnOffToolbar(-1, this);
	pointonoff->setRenderingAction(pointact);
	MLRenderingPointsParametersFrame* pointframe = new MLRenderingPointsParametersFrame(-1, this);
	initToolButtonSubMenu(pointbut, pointonoff,pointframe);
	addWidget(pointbut);

	MyToolButton* wirebut = new MyToolButton(0, this);
	MLRenderingWireAction* wireact = new MLRenderingWireAction(false,this);
	//wirebut->setDefaultAction(wireact);
	MLRenderingOnOffToolbar* wireonoff = new MLRenderingOnOffToolbar(-1, this);
	wireonoff->setRenderingAction(wireact);
	MLRenderingWireParametersFrame* wirepar = new MLRenderingWireParametersFrame(-1, this);
	initToolButtonSubMenu(wirebut,wireonoff,wirepar);
	addWidget(wirebut);

	MyToolButton* solidbut = new MyToolButton(0, this);
	MLRenderingSolidAction* solidact = new MLRenderingSolidAction(this);
	//solidbut->setDefaultAction(solidact);
	MLRenderingOnOffToolbar* solidonoff = new MLRenderingOnOffToolbar(-1, this);
	solidonoff->setRenderingAction(solidact);
	MLRenderingSolidParametersFrame* solidframe = new MLRenderingSolidParametersFrame(-1, NULL);
	initToolButtonSubMenu(solidbut, solidonoff,solidframe);
	addWidget(solidbut);

	MyToolButton* selbut = new MyToolButton(0, this);
	MLRenderingSelectionAction* selact = new MLRenderingSelectionAction(this);
	//selbut->setDefaultAction(selact);
	MLRenderingOnOffToolbar* selonoff = new MLRenderingOnOffToolbar(-1, this);
	selonoff->setRenderingAction(selact);
	MLRenderingSelectionParametersFrame* selframe = new MLRenderingSelectionParametersFrame(-1, this);
	initToolButtonSubMenu(selbut, selonoff,selframe);
	addWidget(selbut);

	MyToolButton* edgebut = new MyToolButton(0, this);
	MLRenderingEdgeDecoratorAction* edgeact = new MLRenderingEdgeDecoratorAction(this);
	//edgebut->setDefaultAction(edgeact);
	MLRenderingOnOffToolbar* edgeonoff = new MLRenderingOnOffToolbar(-1, this);
	edgeonoff->setRenderingAction(edgeact);
	MLRenderingDefaultDecoratorParametersFrame* defdecframe = new MLRenderingDefaultDecoratorParametersFrame(-1, this);
	initToolButtonSubMenu(edgebut,edgeonoff, defdecframe);
	addWidget(edgebut);
}

void MLRenderingGlobalToolbar::initToolButtonSubMenu(MyToolButton* button, MLRenderingOnOffToolbar* onoff,MLRenderingParametersFrame* frame)
{
	if ((button == NULL) || (onoff == NULL) || (frame == NULL))
		return;

	QAction* defact = new QAction(this);
	if (onoff->getRenderingAction() != NULL)
		defact->setIcon(onoff->getRenderingAction()->icon());
	button->setDefaultAction(defact);

	foreach(QAction* rendact, onoff->actions())
	{
		rendact->setCheckable(false);
	}
	connect(onoff, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*,bool)), this, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*,bool)));


	connect(frame, SIGNAL(updateRenderingDataAccordingToActions(int, MLRenderingAction*,QList<MLRenderingAction*>&)), this, SIGNAL(updateRenderingDataAccordingToActions(int, MLRenderingAction*,QList<MLRenderingAction*>&)));
	connect(frame, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*,bool)), this, SIGNAL(updateRenderingDataAccordingToAction(int, MLRenderingAction*,bool)));


	/*STATELESS INTERFACE....WARNING!!! BE CAREFUL NOT ALL THE TOP LEVEL ACTIONS ARE MLRENDERINGACTION (for instance on/off actions...)*/
	QList<QAction*> toplevelactions;
	frame->allTopLevelGuiActions(toplevelactions);
	foreach(QAction* tpl, toplevelactions)
		tpl->setCheckable(false);

	QList<MLRenderingAction*> renderingactions; 
	frame->actionsList(renderingactions);
	
	QWidgetAction* bboxwidact = new QWidgetAction(this);
	bboxwidact->setDefaultWidget(onoff);
	QWidgetAction* paramwidact = new QWidgetAction(this);
	paramwidact->setDefaultWidget(frame);
	if (button->menu() == NULL)
	{
		QMenu* mn = new QMenu(this);
		mn->addAction(bboxwidact);
		mn->addAction(paramwidact);
		button->setMenu(mn);
	}
	button->setCheckable(true);
}
