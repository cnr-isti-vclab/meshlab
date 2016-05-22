#include "ml_rendering_actions.h"
#include <wrap/qt/col_qt_convert.h>
#include <QObject>

MLRenderingAction::MLRenderingAction( QObject* parent )
    :QAction(parent)
{
    setCheckable(true);
    setChecked(false);
    setMeshId(-1);
}

MLRenderingAction::MLRenderingAction(int meshid,QObject* parent )
    :QAction(parent)
{
    setCheckable(true);
    setChecked(false);
    setMeshId(meshid);
}

int MLRenderingAction::meshId() const
{
    bool isvalidid = false;
    return data().toInt(&isvalidid);
}


void MLRenderingAction::setMeshId(int meshid)
{
    setData(QVariant(meshid));
}

MLRenderingAction* MLRenderingAction::copyAction( const MLRenderingAction* tocopy,QObject* newactionparent )
{
    MLRenderingAction* res = tocopy->copyCtr(tocopy,newactionparent);
    res->setMeshId(tocopy->meshId());
    res->setChecked(tocopy->isChecked());
    return res;
}

MLRenderingBBoxAction::MLRenderingBBoxAction( QObject* parent)
    :MLRenderingAction(parent)
{
    setIcon(QIcon(":/images/bbox.png"));
    setText(QString("Bounding Box"));
}

MLRenderingBBoxAction::MLRenderingBBoxAction( int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent)
{
    setIcon(QIcon(":/images/bbox.png"));
    setText(QString("Bounding Box"));
}

void MLRenderingBBoxAction::updateRenderingData(MLRenderingData& rd)
{
    if (isChecked())
        rd._mask = vcg::GLMeshAttributesInfo::addPrimitiveModality(rd._mask,vcg::GLMeshAttributesInfo::PR_BBOX);
    else
        rd._mask = vcg::GLMeshAttributesInfo::removePrimitiveModality(rd._mask,vcg::GLMeshAttributesInfo::PR_BBOX);
}

bool MLRenderingBBoxAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return (bool)(rd._mask & vcg::GLMeshAttributesInfo::PR_BBOX);
}

MLRenderingAction* MLRenderingBBoxAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    (void*) tocopy;
    return new MLRenderingBBoxAction(newactionparent);
}

MLRenderingPointsAction::MLRenderingPointsAction( QObject* parent)
    :MLRenderingAction(parent)
{
    setIcon(QIcon(":/images/points.png"));
    setText(QString("Points"));
}

MLRenderingPointsAction::MLRenderingPointsAction( int meshid,QObject* parent)
    :MLRenderingAction(meshid, parent)
{
    setIcon(QIcon(":/images/points.png"));
    setText(QString("Points"));
}

void MLRenderingPointsAction::updateRenderingData(MLRenderingData& rd )
{
    if (isChecked())
        rd._mask = vcg::GLMeshAttributesInfo::addPrimitiveModality(rd._mask,vcg::GLMeshAttributesInfo::PR_POINTS);
    else
        rd._mask = vcg::GLMeshAttributesInfo::removePrimitiveModality(rd._mask,vcg::GLMeshAttributesInfo::PR_POINTS);
}

bool MLRenderingPointsAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{    
    return (bool)(rd._mask & vcg::GLMeshAttributesInfo::PR_POINTS);
}

MLRenderingAction* MLRenderingPointsAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    (void*) tocopy;
    return new MLRenderingPointsAction(newactionparent);
}

MLRenderingWireAction::MLRenderingWireAction( QObject* parent)
    :MLRenderingAction(parent)
{
    setIcon(QIcon(":/images/wire.png"));
    setText(QString("Wireframe"));
}

MLRenderingWireAction::MLRenderingWireAction( int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent)
{
    setIcon(QIcon(":/images/wire.png"));
    setText(QString("Wireframe"));
}

void MLRenderingWireAction::updateRenderingData(MLRenderingData& rd )
{
    if (isChecked())
        rd._mask = vcg::GLMeshAttributesInfo::addPrimitiveModality(rd._mask,vcg::GLMeshAttributesInfo::PR_WIREFRAME_TRIANGLES);
    else
        rd._mask = vcg::GLMeshAttributesInfo::removePrimitiveModality(rd._mask,vcg::GLMeshAttributesInfo::PR_WIREFRAME_TRIANGLES);
}

bool MLRenderingWireAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return (bool)(rd._mask & vcg::GLMeshAttributesInfo::PR_WIREFRAME_TRIANGLES);
}

MLRenderingAction* MLRenderingWireAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    (void*) tocopy;
    return new MLRenderingWireAction(newactionparent);
}

MLRenderingSolidAction::MLRenderingSolidAction( QObject* parent )
    :MLRenderingAction(parent)
{
    setIcon(QIcon(":/images/smooth.png"));
    setText(QString("Solid"));
}

MLRenderingSolidAction::MLRenderingSolidAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
    setIcon(QIcon(":/images/smooth.png"));
    setText(QString("Solid"));
}

void MLRenderingSolidAction::updateRenderingData( MLRenderingData& rd )
{
    if (isChecked())
        rd._mask = vcg::GLMeshAttributesInfo::addPrimitiveModality(rd._mask,vcg::GLMeshAttributesInfo::PR_SOLID);
    else
        rd._mask = vcg::GLMeshAttributesInfo::removePrimitiveModality(rd._mask,vcg::GLMeshAttributesInfo::PR_SOLID);
}

bool MLRenderingSolidAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return (bool)(rd._mask & vcg::GLMeshAttributesInfo::PR_SOLID);
}

MLRenderingAction* MLRenderingSolidAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    (void*) tocopy;
    return new MLRenderingSolidAction(newactionparent);
}

MLRenderingEdgeWireAction::MLRenderingEdgeWireAction( QObject* parent )
    :MLRenderingAction(parent)
{
    setText(QString("Edges Wireframe"));
}

MLRenderingEdgeWireAction::MLRenderingEdgeWireAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
    setText(QString("Edges Wireframe"));
}

void MLRenderingEdgeWireAction::updateRenderingData( MLRenderingData& rd )
{
    if (isChecked())
        rd._mask = vcg::GLMeshAttributesInfo::addPrimitiveModality(rd._mask,vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES);
    else
        rd._mask = vcg::GLMeshAttributesInfo::removePrimitiveModality(rd._mask,vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES);
}

bool MLRenderingEdgeWireAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return (bool)(rd._mask & vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES);
}

MLRenderingAction* MLRenderingEdgeWireAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    (void*) tocopy;
    return new MLRenderingEdgeWireAction(newactionparent);
}

MLRenderingHiddenLinesAction::MLRenderingHiddenLinesAction( QObject* parent)
    :MLRenderingAction(parent)
{
    setText(QString("Back Face Culling"));
}

MLRenderingHiddenLinesAction::MLRenderingHiddenLinesAction( int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent)
{
    setText(QString("Back Face Culling"));
}

void MLRenderingHiddenLinesAction::updateRenderingData(MLRenderingData& rd )
{
     rd._opts._backfacecull = isChecked();
}

bool MLRenderingHiddenLinesAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return (rd._opts._backfacecull);
}

MLRenderingAction* MLRenderingHiddenLinesAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    (void*) tocopy;
    return new MLRenderingHiddenLinesAction(newactionparent);
}

MLRenderingFlatAction::MLRenderingFlatAction( QObject* parent)
    :MLRenderingAction(parent)
{
    setText(QString("Flat"));
}

MLRenderingFlatAction::MLRenderingFlatAction( int meshid,QObject* parent)
    :MLRenderingAction(meshid, parent)
{
    setText(QString("Flat"));
}

void MLRenderingFlatAction::updateRenderingData(MLRenderingData& rd )
{
    rd._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL] = isChecked(); 
}

bool MLRenderingFlatAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return rd._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL];
}

MLRenderingAction* MLRenderingFlatAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    (void*) tocopy;
    return new MLRenderingFlatAction(newactionparent);
}

MLRenderingSmoothAction::MLRenderingSmoothAction( QObject* parent)
    :MLRenderingAction(parent)
{
    setText(QString("Smooth"));
}

MLRenderingSmoothAction::MLRenderingSmoothAction( int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent)
{
    setText(QString("Smooth"));
}

void MLRenderingSmoothAction::updateRenderingData(MLRenderingData& rd )
{
    rd._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL] = isChecked();
}

bool MLRenderingSmoothAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return rd._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL];
}

MLRenderingAction* MLRenderingSmoothAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    (void*) tocopy;
    return new MLRenderingSmoothAction(newactionparent);
}

MLRenderingPerVertTextCoordAction::MLRenderingPerVertTextCoordAction( QObject* parent)
    :MLRenderingAction(parent)
{
    setText(QString("Per-Vert"));
}

MLRenderingPerVertTextCoordAction::MLRenderingPerVertTextCoordAction( int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent)
{
    setText(QString("Per-Vert"));
}

void MLRenderingPerVertTextCoordAction::updateRenderingData(MLRenderingData& rd )
{
    rd._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE] = isChecked();
}

bool MLRenderingPerVertTextCoordAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return rd._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE];
}

MLRenderingAction* MLRenderingPerVertTextCoordAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    (void*) tocopy;
    return new MLRenderingPerVertTextCoordAction(newactionparent);
}

MLRenderingPerWedgeTextCoordAction::MLRenderingPerWedgeTextCoordAction( QObject* parent)
    :MLRenderingAction(parent)
{
    setText(QString("Per-Wedge"));
}

MLRenderingPerWedgeTextCoordAction::MLRenderingPerWedgeTextCoordAction( int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent)
{
    setText(QString("Per-Wedge"));
}

void MLRenderingPerWedgeTextCoordAction::updateRenderingData(MLRenderingData& rd )
{
    rd._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE] = isChecked();
}

bool MLRenderingPerWedgeTextCoordAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return rd._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE];
}

MLRenderingAction* MLRenderingPerWedgeTextCoordAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    (void*) tocopy;
    return new MLRenderingPerWedgeTextCoordAction(newactionparent);
}

MLRenderingDoubleLightingAction::MLRenderingDoubleLightingAction( QObject* parent)
    :MLRenderingAction(parent)
{
    setText(QString("Double side lighting"));
}

MLRenderingDoubleLightingAction::MLRenderingDoubleLightingAction( int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent)
{
    setText(QString("Double side lighting"));
}

void MLRenderingDoubleLightingAction::updateRenderingData(MLRenderingData& rd )
{
    rd._opts._doublesidelighting = !rd._opts._doublesidelighting;
}

bool MLRenderingDoubleLightingAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return rd._opts._doublesidelighting;
}

MLRenderingAction* MLRenderingDoubleLightingAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    (void*) tocopy;
    return new MLRenderingDoubleLightingAction(newactionparent);
}

MLRenderingFancyLightingAction::MLRenderingFancyLightingAction( QObject* parent)
    :MLRenderingAction(parent)
{
    setText(QString("Fancy Lighting"));
}

MLRenderingFancyLightingAction::MLRenderingFancyLightingAction( int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent)
{
    setText(QString("Fancy Lighting"));
}

void MLRenderingFancyLightingAction::updateRenderingData(MLRenderingData& rd )
{
    rd._opts._fancylighting = isChecked();
}

bool MLRenderingFancyLightingAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return rd._opts._fancylighting;
}

MLRenderingAction* MLRenderingFancyLightingAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    (void*) tocopy;
    return new MLRenderingFancyLightingAction(newactionparent);
}


MLRenderingLightOnOffAction::MLRenderingLightOnOffAction( QObject* parent)
    :MLRenderingAction(parent)
{
    setIcon(QIcon(":/images/lighton.png"));
    setText(QString("Light on/off"));  
}

MLRenderingLightOnOffAction::MLRenderingLightOnOffAction( int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent)
{
    setIcon(QIcon(":/images/lighton.png"));
    setText(QString("Light on/off"));  
}

void MLRenderingLightOnOffAction::updateRenderingData(MLRenderingData& rd )
{
    rd._opts._lighting = isChecked();
}

bool MLRenderingLightOnOffAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return rd._opts._lighting;
}

MLRenderingAction* MLRenderingLightOnOffAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    (void*) tocopy;
    return new MLRenderingLightOnOffAction(newactionparent);
}

MLRenderingFaceCullAction::MLRenderingFaceCullAction( QObject* parent)
    :MLRenderingAction(parent)
{
    setText(QString("BackFace Culling"));
}

MLRenderingFaceCullAction::MLRenderingFaceCullAction( int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent)
{
    setText(QString("BackFace Culling"));
}

void MLRenderingFaceCullAction::updateRenderingData(MLRenderingData& rd )
{
    rd._opts._backfacecull = isChecked();
}

bool MLRenderingFaceCullAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return rd._opts._backfacecull;
}

MLRenderingAction* MLRenderingFaceCullAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    (void*) tocopy;
    return new MLRenderingFaceCullAction(newactionparent);
}


MLRenderingPerMeshColorAction::MLRenderingPerMeshColorAction( QObject* parent)
    :MLRenderingAction(parent)
{
    setText(QString("Per-Mesh"));  
}

MLRenderingPerMeshColorAction::MLRenderingPerMeshColorAction(int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent)
{
    setText(QString("Per-Mesh"));
}

void MLRenderingPerMeshColorAction::updateRenderingData(MLRenderingData& rd)
{
    rd._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_MESHCOLOR] = isChecked();
    rd._opts._permesh_color = _col;
}

bool MLRenderingPerMeshColorAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return rd._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_MESHCOLOR];
}

MLRenderingAction* MLRenderingPerMeshColorAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    MLRenderingPerMeshColorAction* tmp = new MLRenderingPerMeshColorAction(newactionparent);
    const MLRenderingPerMeshColorAction* tmpcopy = qobject_cast<const MLRenderingPerMeshColorAction*>(tocopy);
    if (tmpcopy != NULL)
        tmp->_col = tmpcopy->_col;
    return tmp;
}

MLRenderingPerVertexColorAction::MLRenderingPerVertexColorAction(QObject* parent)
    :MLRenderingAction(parent)
{
    setText(QString("Per-Vertex"));
}

MLRenderingPerVertexColorAction::MLRenderingPerVertexColorAction(int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent)
{
    setText(QString("Per-Vertex"));
}

void MLRenderingPerVertexColorAction::updateRenderingData(MLRenderingData& rd )
{
    rd._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] = isChecked();
}

bool MLRenderingPerVertexColorAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return rd._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR];
}

MLRenderingAction* MLRenderingPerVertexColorAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    (void*) tocopy;
    return new MLRenderingPerVertexColorAction(newactionparent);
}

MLRenderingPerFaceColorAction::MLRenderingPerFaceColorAction(QObject* parent)
    :MLRenderingAction(parent)
{
    setText(QString("Per-Face"));
}

MLRenderingPerFaceColorAction::MLRenderingPerFaceColorAction(int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent)
{
    setText(QString("Per-Face"));
}

void MLRenderingPerFaceColorAction::updateRenderingData(MLRenderingData& rd )
{
    rd._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] = isChecked();
}

bool MLRenderingPerFaceColorAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return rd._atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR];
}

MLRenderingAction* MLRenderingPerFaceColorAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    (void*) tocopy;
    return new MLRenderingPerFaceColorAction(newactionparent);
}



MLRenderingUserDefinedColorAction::MLRenderingUserDefinedColorAction( vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY pm,QObject* parent )
    :MLRenderingAction(-1,parent),_pm(pm),_col(vcg::Color4b::DarkGray)
{
    setText(QString("User-Defined"));
}

MLRenderingUserDefinedColorAction::MLRenderingUserDefinedColorAction( vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY pm,int meshid, QObject* parent )
    :MLRenderingAction(meshid,parent),_pm(pm)
{
    setText(QString("User-Defined"));
}

void MLRenderingUserDefinedColorAction::updateRenderingData( MLRenderingData& rd )
{
    switch(_pm) 
    {
        case (vcg::GLMeshAttributesInfo::PR_POINTS):
        {
            rd._opts._use_perpoint_fixedcolor = !(rd._opts._use_perpoint_fixedcolor);
            if (rd._opts._use_perpoint_fixedcolor)
                rd._opts._perpoint_fixedcolor = _col;
            break;
        }
        case (vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES):
        case (vcg::GLMeshAttributesInfo::PR_WIREFRAME_TRIANGLES):
        {
            rd._opts._use_peredge_fixedcolor = !(rd._opts._use_peredge_fixedcolor);
            if (rd._opts._use_peredge_fixedcolor)
                rd._opts._peredge_fixedcolor = _col;
            break;
        }
        case (vcg::GLMeshAttributesInfo::PR_SOLID):
        {
            rd._opts._use_perface_fixedcolor = !(rd._opts._use_perface_fixedcolor);
            if (rd._opts._use_perface_fixedcolor)
                rd._opts._perface_fixedcolor = _col;
            break;
        }
        default:
            break;
    }
}

bool MLRenderingUserDefinedColorAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    switch(_pm) 
    {
        case (vcg::GLMeshAttributesInfo::PR_POINTS):
        {
            
            return (rd._opts._use_perpoint_fixedcolor);
            break;
        }
        case (vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES):
        case (vcg::GLMeshAttributesInfo::PR_WIREFRAME_TRIANGLES):
        {
            
            return (rd._opts._use_peredge_fixedcolor);
            break;
        }
        case (vcg::GLMeshAttributesInfo::PR_SOLID):
        {
            return (rd._opts._use_perface_fixedcolor);
            break;
        }
        default:
            break;
    }
    return false;
}

void MLRenderingUserDefinedColorAction::setColor( const QColor& col )
{
    _col = vcg::ColorConverter::ToColor4b(col);
}

void MLRenderingUserDefinedColorAction::setColor( const vcg::Color4b& col )
{
    _col = col;
}

MLRenderingAction* MLRenderingUserDefinedColorAction::copyCtr( const MLRenderingAction* tocopy,QObject* newactionparent ) const
{
    MLRenderingUserDefinedColorAction* tmp = new MLRenderingUserDefinedColorAction(_pm,newactionparent);
    const MLRenderingUserDefinedColorAction* tmpcopy = qobject_cast<const MLRenderingUserDefinedColorAction*>(tocopy);
    if (tmpcopy != NULL)
        tmp->_col = tmpcopy->_col;
    return tmp;
}
