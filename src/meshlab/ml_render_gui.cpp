#include "ml_render_gui.h"
#include <QPushButton>

MLRenderToolbar::MLRenderToolbar(vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK defaultrend,QWidget* parent )
    :QToolBar(parent),_meshid(-1),_mask(defaultrend)
{
    initGui();
}

MLRenderToolbar::MLRenderToolbar( unsigned int meshid,vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK defaultrend,QWidget* parent )
    :QToolBar(parent),_meshid(meshid),_mask(defaultrend)
{
    initGui();
}

MLRenderToolbar::~MLRenderToolbar()
{
    //qDebug("Distruggo");
}

void MLRenderToolbar::toggleBBox()
{
    toggle(_bboxact,(unsigned int) vcg::GLMeshAttributesInfo::PR_BBOX);
}

void MLRenderToolbar::togglePoints()
{
    toggle(_pointsact,(unsigned int) vcg::GLMeshAttributesInfo::PR_POINTS);
}

void MLRenderToolbar::toggleEdges()
{
    toggle(_edgeact,(unsigned int)(/*vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES | */vcg::GLMeshAttributesInfo::PR_WIREFRAME_TRIANGLES));
}

void MLRenderToolbar::toggleSolid()
{
   toggle(_solidact,(unsigned int) vcg::GLMeshAttributesInfo::PR_SOLID);
}

void MLRenderToolbar::initGui()
{
    _bboxact = addAction(QIcon(":/images/bbox.png"),QString("Bounding Box"),this,SLOT(toggleBBox()));
    _bboxact->setCheckable(true);
    _pointsact = addAction(QIcon(":/images/points.png"),QString("Points"),this,SLOT(togglePoints()));
    _pointsact->setCheckable(true);
    _edgeact = addAction(QIcon(":/images/wire.png"),QString("Edges"),this,SLOT(toggleEdges()));
    _edgeact->setCheckable(true);
    _solidact = addAction(QIcon(":/images/smooth.png"),QString("Solid"),this,SLOT(toggleSolid()));
    _solidact->setCheckable(true);
    checkPrimitiveButtonsAccordingToMask();
}

void MLRenderToolbar::toggle( QAction* act,const vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK relatedprimit)
{
    bool actcheck = act->isChecked();
    if (actcheck)
        _mask = _mask | relatedprimit;
    else
        _mask = _mask & ~(relatedprimit);

    if (_meshid != -1)
        emit primitiveModalityUpdateRequested(_meshid,_mask);
    /*else
        emit primitiveModalityUpdateRequested(_mask);*/
}

void MLRenderToolbar::setPrimitiveModality( vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK mask )
{
    _mask = mask;
    checkPrimitiveButtonsAccordingToMask();
}

void MLRenderToolbar::checkPrimitiveButtonsAccordingToMask()
{
    bool m(_mask & vcg::GLMeshAttributesInfo::PR_BBOX);
    bool c(_mask & vcg::GLMeshAttributesInfo::PR_POINTS);
    bool d(_mask & vcg::GLMeshAttributesInfo::PR_WIREFRAME_TRIANGLES);
    bool e(_mask & vcg::GLMeshAttributesInfo::PR_SOLID);
    _bboxact->setChecked(bool(_mask & vcg::GLMeshAttributesInfo::PR_BBOX));
    _pointsact->setChecked(bool(_mask & vcg::GLMeshAttributesInfo::PR_POINTS));
    _edgeact->setChecked(bool(_mask & (/*vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES | */vcg::GLMeshAttributesInfo::PR_WIREFRAME_TRIANGLES)));
    _solidact->setChecked(bool(_mask & vcg::GLMeshAttributesInfo::PR_SOLID));
}

MLRenderParametersFrame::MLRenderParametersFrame(QWidget* parent)
    :QFrame(parent)
{

}
