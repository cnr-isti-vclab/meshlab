#include "rendermodeactions.h"
#include <QObject>

RenderModeAction::RenderModeAction( const QIcon& icn,const QString& title,QObject* parent )
    :QAction(icn,title,parent)
{
    commonInit(-1);
}

RenderModeAction::RenderModeAction( const QString& title,QObject* parent )
    :QAction(title,parent)
{
    commonInit(-1);
}

RenderModeAction::RenderModeAction( const unsigned int meshid,const QString& title,QObject* parent )
    :QAction(title,parent)
{
    commonInit(meshid);
}

RenderModeAction::RenderModeAction( const unsigned int meshid,const QIcon& icn,const QString& title,QObject* parent )
    :QAction(icn,title,parent)
{
    commonInit(meshid);
}

void RenderModeAction::commonInit(const unsigned int meshid)
{
    setData(QVariant(meshid));
}


void RenderModeAction::updateRenderMode( QList<RenderMode>& rmlist )
{
    for(int ii = 0; ii < rmlist.size();++ii)
        updateRenderMode(rmlist[ii]);
}

RenderModeBBoxAction::RenderModeBBoxAction( QObject* parent)
    :RenderModeAction(QIcon(":/images/bbox.png"),QString("&Bounding box"), parent)
{
}

RenderModeBBoxAction::RenderModeBBoxAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QIcon(":/images/bbox.png"),QString("&Bounding box"), parent)
{
}

void RenderModeBBoxAction::updateRenderMode( RenderMode& rm )
{
    rm.setDrawMode(vcg::GLW::DMBox);
}

bool RenderModeBBoxAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return (rm.drawMode == vcg::GLW::DMBox);
}

RenderModePointsAction::RenderModePointsAction( QObject* parent)
    :RenderModeAction(QIcon(":/images/points.png"),QString("&Points"), parent)
{

}

RenderModePointsAction::RenderModePointsAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QIcon(":/images/points.png"),QString("&Points"), parent)
{

}

void RenderModePointsAction::updateRenderMode( RenderMode& rm )
{
    rm.setDrawMode(vcg::GLW::DMPoints);
}

bool RenderModePointsAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return (rm.drawMode == vcg::GLW::DMPoints);
}


RenderModeWireAction::RenderModeWireAction( QObject* parent)
    :RenderModeAction(QIcon(":/images/wire.png"),QString("&Wireframe"), parent)
{

}

RenderModeWireAction::RenderModeWireAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QIcon(":/images/wire.png"),QString("&Wireframe"), parent)
{

}

void RenderModeWireAction::updateRenderMode( RenderMode& rm )
{
    rm.setDrawMode(vcg::GLW::DMWire);
}

bool RenderModeWireAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return (rm.drawMode == vcg::GLW::DMWire);
}

RenderModeHiddenLinesAction::RenderModeHiddenLinesAction( QObject* parent)
    :RenderModeAction(QIcon(":/images/backlines.png"),QString("&Hidden Lines"),parent)
{

}

RenderModeHiddenLinesAction::RenderModeHiddenLinesAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QIcon(":/images/backlines.png"),QString("&Hidden Lines"),parent)
{

}

void RenderModeHiddenLinesAction::updateRenderMode( RenderMode& rm )
{
    rm.setDrawMode(vcg::GLW::DMHidden);
}

bool RenderModeHiddenLinesAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return (rm.drawMode == vcg::GLW::DMHidden);
}

RenderModeFlatLinesAction::RenderModeFlatLinesAction( QObject* parent)
    :RenderModeAction(QIcon(":/images/flatlines.png"),QString("Flat &Lines"), parent)
{

}

RenderModeFlatLinesAction::RenderModeFlatLinesAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QIcon(":/images/flatlines.png"),QString("Flat &Lines"), parent)
{

}

void RenderModeFlatLinesAction::updateRenderMode( RenderMode& rm )
{
    rm.setDrawMode(vcg::GLW::DMFlatWire);
}

bool RenderModeFlatLinesAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return (rm.drawMode == vcg::GLW::DMFlatWire);
}


RenderModeFlatAction::RenderModeFlatAction( QObject* parent)
    :RenderModeAction(QIcon(":/images/flat.png"),QString("&Flat"), parent)
{

}

RenderModeFlatAction::RenderModeFlatAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QIcon(":/images/flat.png"),QString("&Flat"), parent)
{

}

void RenderModeFlatAction::updateRenderMode( RenderMode& rm )
{
    rm.setDrawMode(vcg::GLW::DMFlat);
}

bool RenderModeFlatAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return (rm.drawMode == vcg::GLW::DMFlat);
}


RenderModeSmoothAction::RenderModeSmoothAction( QObject* parent)
    :RenderModeAction(QIcon(":/images/smooth.png"),QString("&Smooth"), parent)
{

}

RenderModeSmoothAction::RenderModeSmoothAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QIcon(":/images/smooth.png"),QString("&Smooth"), parent)
{

}

void RenderModeSmoothAction::updateRenderMode( RenderMode& rm )
{
    rm.setDrawMode(vcg::GLW::DMSmooth);
}

bool RenderModeSmoothAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return (rm.drawMode == vcg::GLW::DMSmooth);
}


RenderModeTexturePerVertAction::RenderModeTexturePerVertAction( QObject* parent)
    :RenderModeAction(QIcon(":/images/textures.png"),QString("&Texture"),parent)
{

}

RenderModeTexturePerVertAction::RenderModeTexturePerVertAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QIcon(":/images/textures.png"),QString("&Texture"),parent)
{

}

void RenderModeTexturePerVertAction::updateRenderMode( RenderMode& rm )
{
    rm.setTextureMode(!isChecked() ? vcg::GLW::TMNone : vcg::GLW::TMPerVert);
}

bool RenderModeTexturePerVertAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return (rm.textureMode == vcg::GLW::TMPerVert);
}

RenderModeTexturePerWedgeAction::RenderModeTexturePerWedgeAction( QObject* parent)
    :RenderModeAction(QIcon(":/images/textures.png"),QString("&Texture"),parent)
{

}

RenderModeTexturePerWedgeAction::RenderModeTexturePerWedgeAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QIcon(":/images/textures.png"),QString("&Texture"),parent)
{

}

void RenderModeTexturePerWedgeAction::updateRenderMode( RenderMode& rm )
{
    rm.setTextureMode((!isChecked() ? vcg::GLW::TMNone : vcg::GLW::TMPerWedgeMulti));
}

bool RenderModeTexturePerWedgeAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return ((rm.textureMode == vcg::GLW::TMPerWedgeMulti) || (rm.textureMode == vcg::GLW::TMPerWedge));
}


RenderModeDoubleLightingAction::RenderModeDoubleLightingAction( QObject* parent)
    :RenderModeAction(QString("&Double side lighting"),parent)
{

}

RenderModeDoubleLightingAction::RenderModeDoubleLightingAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QString("&Double side lighting"),parent)
{

}

void RenderModeDoubleLightingAction::updateRenderMode( RenderMode& rm )
{
    rm.setDoubleFaceLighting(isChecked());
}

bool RenderModeDoubleLightingAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return rm.doubleSideLighting;
}

RenderModeFancyLightingAction::RenderModeFancyLightingAction( QObject* parent)
    :RenderModeAction(QString("&Fancy Lighting"),parent)
{

}

RenderModeFancyLightingAction::RenderModeFancyLightingAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QString("&Fancy Lighting"),parent)
{

}

void RenderModeFancyLightingAction::updateRenderMode( RenderMode& rm )
{
    rm.setFancyLighting(isChecked());
}

bool RenderModeFancyLightingAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return rm.fancyLighting;
}


RenderModeLightOnOffAction::RenderModeLightOnOffAction( QObject* parent)
    :RenderModeAction(QIcon(":/images/lighton.png"),QString("&Light on/off"),parent)
{

}

RenderModeLightOnOffAction::RenderModeLightOnOffAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QIcon(":/images/lighton.png"),QString("&Light on/off"),parent)
{

}

void RenderModeLightOnOffAction::updateRenderMode( RenderMode& rm )
{
    rm.setLighting(isChecked());
}

bool RenderModeLightOnOffAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return rm.lighting;
}

RenderModeFaceCullAction::RenderModeFaceCullAction( QObject* parent)
    :RenderModeAction(QString("BackFace &Culling"),parent)
{

}

RenderModeFaceCullAction::RenderModeFaceCullAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QString("BackFace &Culling"),parent)
{

}

void RenderModeFaceCullAction::updateRenderMode( RenderMode& rm )
{
    rm.setBackFaceCull(isChecked());
}

bool RenderModeFaceCullAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return rm.backFaceCull;
}

RenderModeColorModeNoneAction::RenderModeColorModeNoneAction( QObject* parent)
    :RenderModeAction(QString("&None"),parent)
{

}

RenderModeColorModeNoneAction::RenderModeColorModeNoneAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QString("&None"),parent)
{

}

void RenderModeColorModeNoneAction::updateRenderMode( RenderMode& rm )
{
    rm.setColorMode(vcg::GLW::CMNone);
}

bool RenderModeColorModeNoneAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return (rm.colorMode == vcg::GLW::CMNone);
}


RenderModeColorModePerMeshAction::RenderModeColorModePerMeshAction( QObject* parent)
    :RenderModeAction(QString("Per &Mesh"),parent)
{

}

RenderModeColorModePerMeshAction::RenderModeColorModePerMeshAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QString("Per &Mesh"),parent)
{

}

void RenderModeColorModePerMeshAction::updateRenderMode( RenderMode& rm )
{
    rm.setColorMode(vcg::GLW::CMPerMesh);
}

bool RenderModeColorModePerMeshAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return (rm.colorMode == vcg::GLW::CMPerMesh);
}

RenderModeColorModePerVertexAction::RenderModeColorModePerVertexAction( QObject* parent)
    :RenderModeAction(QString("Per &Vertex"),parent)
{

}

RenderModeColorModePerVertexAction::RenderModeColorModePerVertexAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QString("Per &Vertex"),parent)
{

}

void RenderModeColorModePerVertexAction::updateRenderMode( RenderMode& rm )
{
    rm.setColorMode(vcg::GLW::CMPerVert);
}

bool RenderModeColorModePerVertexAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return (rm.colorMode == vcg::GLW::CMPerVert);
}

RenderModeColorModePerFaceAction::RenderModeColorModePerFaceAction( QObject* parent)
    :RenderModeAction(QString("Per &Face"),parent)
{

}

RenderModeColorModePerFaceAction::RenderModeColorModePerFaceAction( const unsigned int meshid,QObject* parent)
    :RenderModeAction(meshid,QString("Per &Face"),parent)
{

}

void RenderModeColorModePerFaceAction::updateRenderMode( RenderMode& rm )
{
    rm.colorMode = vcg::GLW::CMPerFace;
}

bool RenderModeColorModePerFaceAction::isRenderModeEnabled( const RenderMode& rm ) const
{
    return (rm.colorMode == vcg::GLW::CMPerFace);
}
