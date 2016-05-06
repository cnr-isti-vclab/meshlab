#include "rendermodeactions.h"
#include <QObject>

RenderModeAction::RenderModeAction( QObject* parent )
    :QAction(parent)
{
    setCheckable(true);
    setChecked(false);
    commonInit(-1);
}

RenderModeAction::RenderModeAction( const unsigned int meshid,QObject* parent )
    :QAction(parent)
{
    setCheckable(true);
    setChecked(false);
    commonInit(meshid);
}



unsigned int RenderModeAction::meshId() const
{
    bool isvalidid = false;
    return data().toUInt(&isvalidid);
}


void RenderModeAction::commonInit(const unsigned int meshid)
{
    setData(QVariant(meshid));
}

bool RenderModeAction::isBufferObjectUpdateRequired() const
{
    return false;
}

RenderModeBBoxAction::RenderModeBBoxAction( QObject* parent)
    :RenderModeAction(parent)
{
    setIcon(QIcon(":/images/bbox.png"));
    setText(QString("Bounding Box"));
}

RenderModeBBoxAction::RenderModeBBoxAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,parent)
{
    setIcon(QIcon(":/images/bbox.png"));
    setText(QString("Bounding Box"));
}

void RenderModeBBoxAction::updateRenderMode(RenderMode& rm )
{
    if (!isRenderModeEnabled(rm))
        rm.addPrimitiveModality(vcg::GLMeshAttributesInfo::PR_BBOX);
    else
        rm.removePrimitiveModality(vcg::GLMeshAttributesInfo::PR_BBOX);
}

bool RenderModeBBoxAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return (rm.pmmask & vcg::GLMeshAttributesInfo::PR_BBOX);
}

bool RenderModeBBoxAction::isBufferObjectUpdateRequired() const
{
    return true;
}

RenderModePointsAction::RenderModePointsAction( QObject* parent)
    :RenderModeAction(parent)
{
    setIcon(QIcon(":/images/points.png"));
    setText(QString("Points"));
}

RenderModePointsAction::RenderModePointsAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid, parent)
{
    setIcon(QIcon(":/images/points.png"));
    setText(QString("Points"));
}

void RenderModePointsAction::updateRenderMode(RenderMode& rm )
{
    if (!isRenderModeEnabled(rm))
    {
        rm.addPrimitiveModality(vcg::GLMeshAttributesInfo::PR_POINTS);
        rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = true;
    }
    else
    {
        rm.removePrimitiveModality(vcg::GLMeshAttributesInfo::PR_POINTS);
        //BBOX doesn't require ATT_VERTPOS. If it is the last remaining PRIMITIVE_MODALITY i do not need to request the ATT_VERTPOSITION bo
        if ((rm.pmmask == vcg::GLMeshAttributesInfo::PR_BBOX) || (rm.pmmask == vcg::GLMeshAttributesInfo::PR_NONE))
            rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = false;
    }
}

bool RenderModePointsAction::isRenderModeEnabled( const RenderMode& rm ) const
{    
    return ((rm.pmmask & vcg::GLMeshAttributesInfo::PR_POINTS) && (rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION]));
}

bool RenderModePointsAction::isBufferObjectUpdateRequired() const
{
    return true;
}


RenderModeWireAction::RenderModeWireAction( QObject* parent)
    :RenderModeAction(parent)
{
    setIcon(QIcon(":/images/wire.png"));
    setText(QString("Wireframe"));
}

RenderModeWireAction::RenderModeWireAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,parent)
{

}

void RenderModeWireAction::updateRenderMode(RenderMode& rm )
{
    if (!isRenderModeEnabled(rm))
    {
        rm.addPrimitiveModality(vcg::GLMeshAttributesInfo::PR_WIREFRAME_TRIANGLES);
        rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = true;
    }
    else
    {
        rm.removePrimitiveModality(vcg::GLMeshAttributesInfo::PR_WIREFRAME_TRIANGLES);
        //BBOX doesn't require ATT_VERTPOS. If it is the last remaining PRIMITIVE_MODALITY i do not need to request the ATT_VERTPOSITION bo
        if ((rm.pmmask == vcg::GLMeshAttributesInfo::PR_BBOX) || (rm.pmmask == vcg::GLMeshAttributesInfo::PR_NONE))
            rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = false;
    }
}

bool RenderModeWireAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return (((rm.pmmask & vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES) || (rm.pmmask & vcg::GLMeshAttributesInfo::PR_WIREFRAME_TRIANGLES)) && (rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION]));
}

bool RenderModeWireAction::isBufferObjectUpdateRequired() const
{
    return true;
}

RenderModeQuadWireAction::RenderModeQuadWireAction( QObject* parent )
    :RenderModeAction(parent)
{
    setIcon(QIcon(":/images/poly_wire.png"));
    setText(QString("Quads Wireframe"));
}

RenderModeQuadWireAction::RenderModeQuadWireAction( const unsigned int meshid,QObject* parent )
    :RenderModeAction(meshid,parent)
{
    setIcon(QIcon(":/images/poly_wire.png"));
    setText(QString("Quads Wireframe"));
}

void RenderModeQuadWireAction::updateRenderMode( RenderMode& rm )
{
    if (!isRenderModeEnabled(rm))
    {
        rm.addPrimitiveModality(vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES);
        rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = true;
    }
    else
    {
        rm.removePrimitiveModality(vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES);
        //BBOX doesn't require ATT_VERTPOS. If it is the last remaining PRIMITIVE_MODALITY i do not need to request the ATT_VERTPOSITION bo
        if ((rm.pmmask == vcg::GLMeshAttributesInfo::PR_BBOX) || (rm.pmmask == vcg::GLMeshAttributesInfo::PR_NONE))
            rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = false;
    }
}

bool RenderModeQuadWireAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return ((rm.pmmask & vcg::GLMeshAttributesInfo::PR_WIREFRAME_EDGES) && (rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION]));
}

bool RenderModeQuadWireAction::isBufferObjectUpdateRequired() const
{
    return true;
}



RenderModeHiddenLinesAction::RenderModeHiddenLinesAction( QObject* parent)
    :RenderModeAction(parent)
{
    setIcon(QIcon(":/images/backlines.png"));
    setText(QString("Back Face Culling"));
}

RenderModeHiddenLinesAction::RenderModeHiddenLinesAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,parent)
{
    setIcon(QIcon(":/images/backlines.png"));
    setText(QString("Back Face Culling"));
}

void RenderModeHiddenLinesAction::updateRenderMode(RenderMode& rm )
{
    //rm.setDrawMode(vcg::GLW::DMHidden);
}

bool RenderModeHiddenLinesAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    //return (rm.drawMode == vcg::GLW::DMHidden);
    return false;
}



RenderModeFlatAction::RenderModeFlatAction( QObject* parent)
    :RenderModeAction(parent)
{
    setIcon(QIcon(":/images/flat.png"));
    setText(QString("Flat"));
}

RenderModeFlatAction::RenderModeFlatAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid, parent)
{
    setIcon(QIcon(":/images/flat.png"));
    setText(QString("Flat"));
}

void RenderModeFlatAction::updateRenderMode(RenderMode& rm )
{
    if (!isRenderModeEnabled(rm))
    {
        rm.addPrimitiveModality(vcg::GLMeshAttributesInfo::PR_SOLID);
        rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = true;
        rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL] = true;
    }
    else
    {
        rm.removePrimitiveModality(vcg::GLMeshAttributesInfo::PR_SOLID);
        rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL] = false;
        if ((rm.pmmask == vcg::GLMeshAttributesInfo::PR_BBOX) || (rm.pmmask == vcg::GLMeshAttributesInfo::PR_NONE))
            rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = false;
    }
}

bool RenderModeFlatAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return ((rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACENORMAL]) && (rm.pmmask & vcg::GLMeshAttributesInfo::PR_SOLID) && (rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION]));
}

bool RenderModeFlatAction::isBufferObjectUpdateRequired() const
{
    return true;
}


RenderModeSmoothAction::RenderModeSmoothAction( QObject* parent)
    :RenderModeAction(parent)
{
    setIcon(QIcon(":/images/smooth.png"));
    setText(QString("Smooth"));
}

RenderModeSmoothAction::RenderModeSmoothAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,parent)
{
    setIcon(QIcon(":/images/smooth.png"));
    setText(QString("Smooth"));
}

void RenderModeSmoothAction::updateRenderMode(RenderMode& rm )
{
    if (!isRenderModeEnabled(rm))
    {
        rm.addPrimitiveModality(vcg::GLMeshAttributesInfo::PR_SOLID);
        rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = true;
        rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL] = true;
    }
    else
    {
        rm.removePrimitiveModality(vcg::GLMeshAttributesInfo::PR_SOLID);
        rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL] = false;
        if ((rm.pmmask == vcg::GLMeshAttributesInfo::PR_BBOX) || (rm.pmmask == vcg::GLMeshAttributesInfo::PR_NONE))
            rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION] = false;
    }
}

bool RenderModeSmoothAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return ((rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTNORMAL]) && (rm.pmmask & vcg::GLMeshAttributesInfo::PR_SOLID) && (rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTPOSITION]));
}

bool RenderModeSmoothAction::isBufferObjectUpdateRequired() const
{
    return true;
}


RenderModeTexturePerVertAction::RenderModeTexturePerVertAction( QObject* parent)
    :RenderModeAction(/*QIcon(":/images/textures.png"),QString("&Texture"),*/parent)
{
    setIcon(QIcon(":/images/textures.png"));
    setText(QString("Per-Vert Texture"));
}

RenderModeTexturePerVertAction::RenderModeTexturePerVertAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid/*,QIcon(":/images/textures.png"),QString("&Texture")*/,parent)
{
    setIcon(QIcon(":/images/textures.png"));
    setText(QString("Per-Vert Texture"));
}

void RenderModeTexturePerVertAction::updateRenderMode(RenderMode& rm )
{
    //rm.setTextureMode(!isChecked() ? vcg::GLW::TMNone : vcg::GLW::TMPerVert);
    if (!isRenderModeEnabled(rm))
        rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE] = true;
    else
        rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE] = false;
}

bool RenderModeTexturePerVertAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTTEXTURE];
}

bool RenderModeTexturePerVertAction::isBufferObjectUpdateRequired() const
{
    return true;
}

RenderModeTexturePerWedgeAction::RenderModeTexturePerWedgeAction( QObject* parent)
    :RenderModeAction(/*QIcon(":/images/textures.png"),QString("&Texture"),*/parent)
{
    setIcon(QIcon(":/images/textures.png"));
    setText(QString("Per-Wedge Texture"));
}

RenderModeTexturePerWedgeAction::RenderModeTexturePerWedgeAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid/*,QIcon(":/images/textures.png"),QString("&Texture")*/,parent)
{
    setIcon(QIcon(":/images/textures.png"));
    setText(QString("Per-Wedge Texture"));
}

void RenderModeTexturePerWedgeAction::updateRenderMode(RenderMode& rm )
{
    //rm.setTextureMode((!isChecked() ? vcg::GLW::TMNone : vcg::GLW::TMPerWedgeMulti));
    if (!isRenderModeEnabled(rm))
        rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE] = true;
    else
        rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE] = false;
}

bool RenderModeTexturePerWedgeAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    //return ((rm.textureMode == vcg::GLW::TMPerWedgeMulti) || (rm.textureMode == vcg::GLW::TMPerWedge));
    return rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_WEDGETEXTURE];
}

bool RenderModeTexturePerWedgeAction::isBufferObjectUpdateRequired() const
{
    return true;
}


RenderModeDoubleLightingAction::RenderModeDoubleLightingAction( QObject* parent)
    :RenderModeAction(/*QString("&Double side lighting"),*/parent)
{
    setText(QString("Double side lighting"));
}

RenderModeDoubleLightingAction::RenderModeDoubleLightingAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid/*,QString("&Double side lighting")*/,parent)
{
    setText(QString("Double side lighting"));
}

void RenderModeDoubleLightingAction::updateRenderMode(RenderMode& rm )
{
    rm.setDoubleFaceLighting(isChecked());
}

bool RenderModeDoubleLightingAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return rm.doubleSideLighting;
}

RenderModeFancyLightingAction::RenderModeFancyLightingAction( QObject* parent)
    :RenderModeAction(/*QString("&Fancy Lighting"),*/parent)
{
    setText(QString("Fancy Lighting"));
}

RenderModeFancyLightingAction::RenderModeFancyLightingAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,/*QString("&Fancy Lighting"),*/parent)
{
    setText(QString("Fancy Lighting"));
}

void RenderModeFancyLightingAction::updateRenderMode(RenderMode& rm )
{
    rm.setFancyLighting(isChecked());
}

bool RenderModeFancyLightingAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return rm.fancyLighting;
}


RenderModeLightOnOffAction::RenderModeLightOnOffAction( QObject* parent)
    :RenderModeAction(/*QIcon(":/images/lighton.png"),QString("&Light on/off"),*/parent)
{
    setIcon(QIcon(":/images/lighton.png"));
    setText(QString("Light on/off"));  
}

RenderModeLightOnOffAction::RenderModeLightOnOffAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid/*,QIcon(":/images/lighton.png"),QString("&Light on/off")*/,parent)
{
    setIcon(QIcon(":/images/lighton.png"));
    setText(QString("Light on/off"));  
}

void RenderModeLightOnOffAction::updateRenderMode(RenderMode& rm )
{
    rm.setLighting(isChecked());
}

bool RenderModeLightOnOffAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return rm.lighting;
}

RenderModeFaceCullAction::RenderModeFaceCullAction( QObject* parent)
    :RenderModeAction(/*QString("BackFace &Culling"),*/parent)
{
    setText(QString("BackFace Culling"));
}

RenderModeFaceCullAction::RenderModeFaceCullAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid/*,QString("BackFace &Culling")*/,parent)
{
    setText(QString("BackFace Culling"));
}

void RenderModeFaceCullAction::updateRenderMode(RenderMode& rm )
{
    rm.setBackFaceCull(isChecked());
}

bool RenderModeFaceCullAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return rm.backFaceCull;
}

RenderModeColorModeNoneAction::RenderModeColorModeNoneAction( QObject* parent)
    :RenderModeAction(/*QString("&None"),*/parent)
{
    setText(QString("None"));
}

RenderModeColorModeNoneAction::RenderModeColorModeNoneAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid/*,QString("&None")*/,parent)
{
    setText(QString("None"));
}

void RenderModeColorModeNoneAction::updateRenderMode(RenderMode& rm )
{
    rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_MESHCOLOR] = false;
    rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] = false;
    rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR] = false;
}

bool RenderModeColorModeNoneAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return (!rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_MESHCOLOR] && !rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] && !rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR]);
}

bool RenderModeColorModeNoneAction::isBufferObjectUpdateRequired() const
{
    return true;
}


RenderModeColorModePerMeshAction::RenderModeColorModePerMeshAction( QObject* parent)
    :RenderModeAction(/*QString("Per &Mesh"),*/parent)
{
    setIcon(QIcon(":/images/per_mesh.png"));
    setText(QString("Per-Mesh Color"));  
}

RenderModeColorModePerMeshAction::RenderModeColorModePerMeshAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,/*QString("Per &Mesh"),*/parent)
{
    setIcon(QIcon(":/images/per_mesh.png"));
    setText(QString("Per-Mesh Color"));
}

void RenderModeColorModePerMeshAction::updateRenderMode(RenderMode& rm )
{
    rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_MESHCOLOR] = true;
}

bool RenderModeColorModePerMeshAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return (vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_MESHCOLOR);
}

bool RenderModeColorModePerMeshAction::isBufferObjectUpdateRequired() const
{
    return true;
}

RenderModeColorModePerVertexAction::RenderModeColorModePerVertexAction( QObject* parent)
    :RenderModeAction(/*QString("Per &Vertex"),*/parent)
{
    setIcon(QIcon(":/images/per_vertex.png"));
    setText(QString("Per-Vertex Color"));
}

RenderModeColorModePerVertexAction::RenderModeColorModePerVertexAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,/*QString("Per &Vertex"),*/parent)
{
    setIcon(QIcon(":/images/per_vertex.png"));
    setText(QString("Per-Vertex Color"));
}

void RenderModeColorModePerVertexAction::updateRenderMode(RenderMode& rm )
{
    rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR] = true;
}

bool RenderModeColorModePerVertexAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_VERTCOLOR];
}

bool RenderModeColorModePerVertexAction::isBufferObjectUpdateRequired() const
{
    return true;
}

RenderModeColorModePerFaceAction::RenderModeColorModePerFaceAction( QObject* parent)
    :RenderModeAction(/*QString("Per &Face Color"),*/parent)
{
    setIcon(QIcon(":/images/per_face.png"));
    setText(QString("Per-Face Color"));
}

RenderModeColorModePerFaceAction::RenderModeColorModePerFaceAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,/*QString("Per &Face"),*/parent)
{
    setIcon(QIcon(":/images/per_face.png"));
    setText(QString("Per-Face Color"));
}

void RenderModeColorModePerFaceAction::updateRenderMode(RenderMode& rm )
{
    rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR];
}

bool RenderModeColorModePerFaceAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return rm.atts[vcg::GLMeshAttributesInfo::ATT_NAMES::ATT_FACECOLOR];
}

bool RenderModeColorModePerFaceAction::isBufferObjectUpdateRequired() const
{
    return true;
}


