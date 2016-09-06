/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include "ml_rendering_actions.h"
#include <wrap/qt/col_qt_convert.h>
#include <QObject>
#include <QString>
#include <common/meshmodel.h>

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

bool MLRenderingAction::isSameType(const MLRenderingAction& act) const
{
	return (metaObject()->className() == act.metaObject()->className());
}

bool MLRenderingAction::isRenderingDataEnabled( MLRenderingData::PRIMITIVE_MODALITY pm,MLRenderingData::ATT_NAMES att,const MLRenderingData& rd ) const
{
    MLRenderingData::RendAtts atts;
    rd.get(pm,atts);
    return atts[att];
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

void MLRenderingBBoxAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingBBoxAction(meshId(), par);
}

void MLRenderingBBoxAction::updateRenderingData(MLRenderingData& rd)
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._perbbox_enabled = isChecked();
        rd.set(opts);
    }
}

bool MLRenderingBBoxAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    return (valid && opts._perbbox_enabled);
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

void MLRenderingPointsAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingPointsAction(meshId(), par);
}

void MLRenderingPointsAction::updateRenderingData(MLRenderingData& rd )
{
    rd.set(MLRenderingData::PR_POINTS,isChecked());
}

bool MLRenderingPointsAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{    
    return rd.isPrimitiveActive(MLRenderingData::PR_POINTS);
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

void MLRenderingWireAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingWireAction(meshId(), par);
}

void MLRenderingWireAction::updateRenderingData(MLRenderingData& rd )
{
    rd.set(MLRenderingData::PR_WIREFRAME_TRIANGLES,isChecked());
}

bool MLRenderingWireAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return rd.isPrimitiveActive(MLRenderingData::PR_WIREFRAME_TRIANGLES);
}

MLRenderingSolidAction::MLRenderingSolidAction( QObject* parent )
    :MLRenderingAction(parent)
{
    setIcon(QIcon(":/images/smooth.png"));
    setText(QString("Fill"));
}

MLRenderingSolidAction::MLRenderingSolidAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
    setIcon(QIcon(":/images/smooth.png"));
    setText(QString("Fill"));
}

void MLRenderingSolidAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingSolidAction(meshId(), par);
}

void MLRenderingSolidAction::updateRenderingData( MLRenderingData& rd )
{
    rd.set(MLRenderingData::PR_SOLID,isChecked());
}

bool MLRenderingSolidAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return rd.isPrimitiveActive(MLRenderingData::PR_SOLID);
}

MLRenderingFauxEdgeWireAction::MLRenderingFauxEdgeWireAction( QObject* parent )
    :MLRenderingAction(parent)
{
    setText(QString("Edges Wireframe"));
}

MLRenderingFauxEdgeWireAction::MLRenderingFauxEdgeWireAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
    setText(QString("Edges Wireframe"));
}

void MLRenderingFauxEdgeWireAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingFauxEdgeWireAction(meshId(), par);
}

void MLRenderingFauxEdgeWireAction::updateRenderingData( MLRenderingData& rd )
{
    rd.set(MLRenderingData::PR_WIREFRAME_EDGES,isChecked());
}

bool MLRenderingFauxEdgeWireAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return rd.isPrimitiveActive(MLRenderingData::PR_WIREFRAME_EDGES);
}

bool MLRenderingFauxEdgeWireAction::isCheckableConditionValid( MeshModel* mm) const
{
    return mm->hasDataMask(MeshModel::MM_POLYGONAL);
}

MLRenderingPerFaceNormalAction::MLRenderingPerFaceNormalAction( QObject* parent)
    :MLRenderingAction(parent)
{
    setText(QString("Face"));
}

MLRenderingPerFaceNormalAction::MLRenderingPerFaceNormalAction( int meshid,QObject* parent)
    :MLRenderingAction(meshid, parent)
{
    setText(QString("Face"));
}

void MLRenderingPerFaceNormalAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingPerFaceNormalAction(meshId(), par);
}

void MLRenderingPerFaceNormalAction::updateRenderingData(MLRenderingData& rd )
{
    rd.set(MLRenderingData::PR_SOLID,MLRenderingData::ATT_NAMES::ATT_FACENORMAL,isChecked()); 
}

bool MLRenderingPerFaceNormalAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return MLRenderingAction::isRenderingDataEnabled(MLRenderingData::PR_SOLID,MLRenderingData::ATT_NAMES::ATT_FACENORMAL,rd);
}

MLRenderingPerVertexNormalAction::MLRenderingPerVertexNormalAction(MLRenderingData::PRIMITIVE_MODALITY pm,QObject* parent)
    :MLRenderingAction(parent),_pm(pm)
{
    setText(QString("Vert"));
}

MLRenderingPerVertexNormalAction::MLRenderingPerVertexNormalAction(MLRenderingData::PRIMITIVE_MODALITY pm,int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent),_pm(pm)
{
    setText(QString("Vert"));
}

void MLRenderingPerVertexNormalAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingPerVertexNormalAction(_pm, meshId(), par);
}

void MLRenderingPerVertexNormalAction::updateRenderingData(MLRenderingData& rd )
{
    rd.set(_pm,MLRenderingData::ATT_NAMES::ATT_VERTNORMAL,isChecked()); 
}

bool MLRenderingPerVertexNormalAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return MLRenderingAction::isRenderingDataEnabled(_pm,MLRenderingData::ATT_NAMES::ATT_VERTNORMAL,rd);
}

MLRenderingPerVertTextCoordAction::MLRenderingPerVertTextCoordAction(MLRenderingData::PRIMITIVE_MODALITY pm,QObject* parent)
    :MLRenderingAction(-1,parent),_pm(pm)
{
    setText(QString("Enabled"));
}

MLRenderingPerVertTextCoordAction::MLRenderingPerVertTextCoordAction(MLRenderingData::PRIMITIVE_MODALITY pm,int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent),_pm(pm)
{
    setText(QString("Enabled"));
}

void MLRenderingPerVertTextCoordAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingPerVertTextCoordAction(_pm,meshId(), par);
}

void MLRenderingPerVertTextCoordAction::updateRenderingData(MLRenderingData& rd )
{
    rd.set(_pm,MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE,isChecked()); 
}

bool MLRenderingPerVertTextCoordAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return MLRenderingAction::isRenderingDataEnabled(_pm,MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE,rd);
}

bool MLRenderingPerVertTextCoordAction::isCheckableConditionValid( MeshModel* mm) const
{
    return mm->hasDataMask(MeshModel::MM_VERTTEXCOORD);
}

MLRenderingPerWedgeTextCoordAction::MLRenderingPerWedgeTextCoordAction(QObject* parent)
    :MLRenderingAction(parent)
{
}

MLRenderingPerWedgeTextCoordAction::MLRenderingPerWedgeTextCoordAction(int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent)
{
}

void MLRenderingPerWedgeTextCoordAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingPerWedgeTextCoordAction(meshId(), par);
}

void MLRenderingPerWedgeTextCoordAction::updateRenderingData(MLRenderingData& rd )
{
    rd.set(MLRenderingData::PR_SOLID,MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE,isChecked()); 
}

bool MLRenderingPerWedgeTextCoordAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return MLRenderingAction::isRenderingDataEnabled(MLRenderingData::PR_SOLID,MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE,rd);
}

bool MLRenderingPerWedgeTextCoordAction::isCheckableConditionValid( MeshModel* mm) const
{
    return mm->hasDataMask(MeshModel::MM_WEDGTEXCOORD);
}

MLRenderingDoubleLightingAction::MLRenderingDoubleLightingAction( QObject* parent)
    :MLRenderingAction(parent)
{
    setText(QString("Enabled"));
}

MLRenderingDoubleLightingAction::MLRenderingDoubleLightingAction( int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent)
{
    setText(QString("Enabled"));
}

void MLRenderingDoubleLightingAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingDoubleLightingAction(meshId(), par);
}

void MLRenderingDoubleLightingAction::updateRenderingData(MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._double_side_lighting = isChecked();
        rd.set(opts);
    }
}

bool MLRenderingDoubleLightingAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    return (valid && opts._double_side_lighting);
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

void MLRenderingFancyLightingAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingFancyLightingAction(meshId(), par);
}

void MLRenderingFancyLightingAction::updateRenderingData(MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._fancy_lighting = isChecked();
        rd.set(opts);
    }
}

bool MLRenderingFancyLightingAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    return (valid && opts._fancy_lighting);
}

MLRenderingNoShadingAction::MLRenderingNoShadingAction(MLRenderingData::PRIMITIVE_MODALITY pm,QObject* parent)
    :MLRenderingAction(parent),_pm(pm)
{
    //setIcon(QIcon(":/images/lighton.png"));
    setText(QString("None")); 
    setToolTip(QString("Light on/off"));
}

MLRenderingNoShadingAction::MLRenderingNoShadingAction(MLRenderingData::PRIMITIVE_MODALITY pm,int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent),_pm(pm)
{
    //setIcon(QIcon(":/images/lighton.png"));
    setText(QString("None")); 
    setToolTip(QString("Light on/off"));
}

void MLRenderingNoShadingAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingNoShadingAction(_pm,meshId(), par);
}

void MLRenderingNoShadingAction::updateRenderingData(MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
 
    if (valid)
    {
        switch (_pm)
        {
        case (MLRenderingData::PR_POINTS):
            {
                opts._perpoint_noshading = isChecked();
                break;
            }
        case (MLRenderingData::PR_WIREFRAME_TRIANGLES):
        case (MLRenderingData::PR_WIREFRAME_EDGES):
            {
                opts._perwire_noshading = isChecked();
                break;
            }
        case (MLRenderingData::PR_SOLID):
            {
                opts._persolid_noshading = isChecked();
                break;
            }
        }
        rd.set(opts);
    }
}

bool MLRenderingNoShadingAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        switch (_pm)
        {
        case (MLRenderingData::PR_POINTS):
            {
                return opts._perpoint_noshading;
            }
        case (MLRenderingData::PR_WIREFRAME_TRIANGLES):
        case (MLRenderingData::PR_WIREFRAME_EDGES):
            {
                return opts._perwire_noshading;
            }
        case (MLRenderingData::PR_SOLID):
            {
                return opts._persolid_noshading;
            }
        }
    }
    return false;
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

void MLRenderingFaceCullAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingFaceCullAction(meshId(), par);
}

void MLRenderingFaceCullAction::updateRenderingData(MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._back_face_cull = isChecked();
        rd.set(opts);
    }
}

bool MLRenderingFaceCullAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    return (valid && opts._back_face_cull);
}

MLRenderingPerMeshColorAction::MLRenderingPerMeshColorAction(MLRenderingData::PRIMITIVE_MODALITY pm,QObject* parent)
    :MLRenderingAction(-1,parent),_pm(pm)
{
    setText(QString("Mesh"));  
}

MLRenderingPerMeshColorAction::MLRenderingPerMeshColorAction(MLRenderingData::PRIMITIVE_MODALITY pm,int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent),_pm(pm)
{
    setText(QString("Mesh"));
}

void MLRenderingPerMeshColorAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingPerMeshColorAction(_pm,meshId(), par);
}

void MLRenderingPerMeshColorAction::updateRenderingData(MLRenderingData& rd)
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        switch (_pm)
        {
        case (MLRenderingData::PR_POINTS):
            {
                opts._perpoint_mesh_color_enabled = isChecked();
                break;
            }
        case (MLRenderingData::PR_WIREFRAME_TRIANGLES):
        case (MLRenderingData::PR_WIREFRAME_EDGES):
            {
                opts._perwire_mesh_color_enabled = isChecked();
                break;
            }
        case (MLRenderingData::PR_SOLID):
            {
                opts._persolid_mesh_color_enabled = isChecked();
                break;
            }
        }
        rd.set(opts);
    }
}

bool MLRenderingPerMeshColorAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        switch (_pm)
        {
        case (MLRenderingData::PR_POINTS):
            {
                return opts._perpoint_mesh_color_enabled;

            }
        case (MLRenderingData::PR_WIREFRAME_TRIANGLES):
        case (MLRenderingData::PR_WIREFRAME_EDGES):
            {
                return opts._perwire_mesh_color_enabled;

            }
        case (MLRenderingData::PR_SOLID):
            {
                return opts._persolid_mesh_color_enabled;
            }
        }
    }
    return false;
}

void MLRenderingPerMeshColorAction::setColor( const QColor& col )
{
    _col = vcg::ColorConverter::ToColor4b(col);
}

void MLRenderingPerMeshColorAction::setColor( const vcg::Color4b& col )
{
    _col = col;
}

vcg::Color4b& MLRenderingPerMeshColorAction::getColor()
{
    return _col;
}

MLRenderingPerVertexColorAction::MLRenderingPerVertexColorAction(MLRenderingData::PRIMITIVE_MODALITY pm,QObject* parent)
    :MLRenderingAction(-1,parent),_pm(pm)
{
    setText(QString("Vert"));
}

MLRenderingPerVertexColorAction::MLRenderingPerVertexColorAction(MLRenderingData::PRIMITIVE_MODALITY pm,int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent),_pm(pm)
{
    setText(QString("Vert"));
}

void MLRenderingPerVertexColorAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingPerVertexColorAction(_pm, meshId(), par);
}

void MLRenderingPerVertexColorAction::updateRenderingData(MLRenderingData& rd )
{
    rd.set(_pm,MLRenderingData::ATT_NAMES::ATT_VERTCOLOR,isChecked());
}

bool MLRenderingPerVertexColorAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return MLRenderingAction::isRenderingDataEnabled(_pm,MLRenderingData::ATT_NAMES::ATT_VERTCOLOR,rd);
}

MLRenderingPerFaceColorAction::MLRenderingPerFaceColorAction(QObject* parent)
    :MLRenderingAction(parent)
{
    setText(QString("Face"));
}

MLRenderingPerFaceColorAction::MLRenderingPerFaceColorAction(int meshid,QObject* parent)
    :MLRenderingAction(meshid,parent)
{
    setText(QString("Face"));
}

void MLRenderingPerFaceColorAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingPerFaceColorAction(meshId(), par);
}

void MLRenderingPerFaceColorAction::updateRenderingData(MLRenderingData& rd )
{
   rd.set(MLRenderingData::PR_SOLID,MLRenderingData::ATT_NAMES::ATT_FACECOLOR,isChecked());
}

bool MLRenderingPerFaceColorAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    return MLRenderingAction::isRenderingDataEnabled(MLRenderingData::PR_SOLID,MLRenderingData::ATT_NAMES::ATT_FACECOLOR,rd);
}

bool MLRenderingPerFaceColorAction::isCheckableConditionValid( MeshModel* mm) const
{
    return mm->hasDataMask(MeshModel::MM_FACECOLOR);
}

MLRenderingUserDefinedColorAction::MLRenderingUserDefinedColorAction( MLRenderingData::PRIMITIVE_MODALITY pm,QObject* parent )
    :MLRenderingAction(-1,parent),_pm(pm),_coluser(vcg::Color4b::DarkGray)
{
    setText(QString("User-Def"));
}

MLRenderingUserDefinedColorAction::MLRenderingUserDefinedColorAction( MLRenderingData::PRIMITIVE_MODALITY pm,int meshid, QObject* parent )
    :MLRenderingAction(meshid,parent),_pm(pm)
{
    setText(QString("User-Def"));
}

MLRenderingUserDefinedColorAction::MLRenderingUserDefinedColorAction(MLRenderingUserDefinedColorAction* origin, QObject * par)
	: MLRenderingAction(origin->meshId(),par)
{
	setText(origin->text());
	setColor(origin->getColor());
	_pm = origin->_pm;
}

void MLRenderingUserDefinedColorAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingUserDefinedColorAction(this,par);
}

void MLRenderingUserDefinedColorAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        switch (_pm)
        {
        case (MLRenderingData::PR_POINTS):
            {
                opts._perpoint_fixed_color_enabled = isChecked();
                opts._perpoint_fixed_color = _coluser;
                break;
            }
        case (MLRenderingData::PR_WIREFRAME_TRIANGLES):
        case (MLRenderingData::PR_WIREFRAME_EDGES):
            {
                opts._perwire_fixed_color_enabled = isChecked();
                opts._perwire_fixed_color = _coluser;
                break;
            }
        case (MLRenderingData::PR_SOLID):
            {
                opts._persolid_fixed_color_enabled = isChecked();
                opts._persolid_fixed_color = _coluser;
                break;
            }
        }
        rd.set(opts);
    }
}

bool MLRenderingUserDefinedColorAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        switch (_pm)
        {
        case (MLRenderingData::PR_POINTS):
            {
                return opts._perpoint_fixed_color_enabled;
                
            }
        case (MLRenderingData::PR_WIREFRAME_TRIANGLES):
        case (MLRenderingData::PR_WIREFRAME_EDGES):
            {
                return opts._perwire_fixed_color_enabled;
                
            }
        case (MLRenderingData::PR_SOLID):
            {
                return opts._persolid_fixed_color_enabled;
            }
        }
    }
    return false;
}

void MLRenderingUserDefinedColorAction::setColor( const vcg::Color4b& col )
{
    _coluser = col;
}

void MLRenderingUserDefinedColorAction::setColor( const QColor& col )
{
    _coluser = vcg::ColorConverter::ToColor4b(col);
}

vcg::Color4b& MLRenderingUserDefinedColorAction::getColor()
{
    return _coluser;
}


MLRenderingSelectionAction::MLRenderingSelectionAction( QObject* parent )
    :MLRenderingAction(-1,parent)
{
    setIcon(QIcon(":/images/selected.png"));
    setText(QString("Selection"));
}

MLRenderingSelectionAction::MLRenderingSelectionAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
    setIcon(QIcon(":/images/selected.png"));
    setText(QString("Selection"));
}

void MLRenderingSelectionAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingSelectionAction(meshId(), par);
}

void MLRenderingSelectionAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._sel_enabled = isChecked();
        rd.set(opts);
    }
}

bool MLRenderingSelectionAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    return (valid && opts._sel_enabled);
}

MLRenderingPointsDotAction::MLRenderingPointsDotAction( QObject* parent )
    :MLRenderingAction(-1,parent)
{
    setText("Dot");
}

MLRenderingPointsDotAction::MLRenderingPointsDotAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
    setText("Dot");
}

void MLRenderingPointsDotAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingPointsDotAction(meshId(), par);
}

void MLRenderingPointsDotAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._perpoint_dot_enabled = isChecked();
        rd.set(opts);
    }
}

bool MLRenderingPointsDotAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    return (valid && opts._perpoint_dot_enabled);
}

MLRenderingVertSelectionAction::MLRenderingVertSelectionAction( QObject* parent )
    :MLRenderingAction(-1,parent)
{
    setText("Vert Selection");

}

MLRenderingVertSelectionAction::MLRenderingVertSelectionAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
    setText("Vert Selection");
}

void MLRenderingVertSelectionAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingVertSelectionAction(meshId(), par);
}

void MLRenderingVertSelectionAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._vertex_sel = isChecked();
        rd.set(opts);
    }
}

bool MLRenderingVertSelectionAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    return (valid && opts._vertex_sel);
}

MLRenderingFaceSelectionAction::MLRenderingFaceSelectionAction( QObject* parent )
    :MLRenderingAction(-1,parent)
{
    setText("Face Selection");

}

MLRenderingFaceSelectionAction::MLRenderingFaceSelectionAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
    setText("Face Selection");
}

void MLRenderingFaceSelectionAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingFaceSelectionAction(meshId(), par);
}

void MLRenderingFaceSelectionAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._face_sel = isChecked();
        rd.set(opts);
    }
}

bool MLRenderingFaceSelectionAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    return (valid && opts._face_sel);
}

MLRenderingBBoxPerMeshColorAction::MLRenderingBBoxPerMeshColorAction( QObject* parent )
    :MLRenderingAction(-1,parent)
{
    setText("Mesh");
}

MLRenderingBBoxPerMeshColorAction::MLRenderingBBoxPerMeshColorAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
    setText("Mesh");
}

void MLRenderingBBoxPerMeshColorAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingBBoxPerMeshColorAction(meshId(), par);
}

void MLRenderingBBoxPerMeshColorAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._perbbox_mesh_color_enabled = isChecked();
        rd.set(opts);
    }
}

bool MLRenderingBBoxPerMeshColorAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
        return opts._perbbox_mesh_color_enabled;
    return false;
}

void MLRenderingBBoxPerMeshColorAction::setColor( const QColor& col )
{
    _col = vcg::ColorConverter::ToColor4b(col);
}

void MLRenderingBBoxPerMeshColorAction::setColor( const vcg::Color4b& col )
{
    _col = col;
}

MLRenderingBBoxUserDefinedColorAction::MLRenderingBBoxUserDefinedColorAction( QObject* parent )
    :MLRenderingAction(-1,parent)
{
    setText("User-Def");
}

MLRenderingBBoxUserDefinedColorAction::MLRenderingBBoxUserDefinedColorAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
    setText("User-Def");
}

MLRenderingBBoxUserDefinedColorAction::MLRenderingBBoxUserDefinedColorAction(MLRenderingBBoxUserDefinedColorAction* origin, QObject * par)
	: MLRenderingAction(origin->meshId(), par)
{
	setText(origin->text());
	_col = origin->_col;
}

void MLRenderingBBoxUserDefinedColorAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingBBoxUserDefinedColorAction(this, par);
}

void MLRenderingBBoxUserDefinedColorAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._perbbox_fixed_color_enabled = isChecked();
        opts._perbbox_fixed_color = _col;
        rd.set(opts);
    }
}

bool MLRenderingBBoxUserDefinedColorAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
        return opts._perbbox_fixed_color_enabled;
    return false;
}

void MLRenderingBBoxUserDefinedColorAction::setColor( const QColor& col )
{
    _col = vcg::ColorConverter::ToColor4b(col);
}

void MLRenderingBBoxUserDefinedColorAction::setColor( const vcg::Color4b& col )
{
    _col = col;
}

vcg::Color4b& MLRenderingBBoxUserDefinedColorAction::getColor()
{
    return _col;
}


MLRenderingEdgeDecoratorAction::MLRenderingEdgeDecoratorAction( QObject* parent )
    :MLRenderingAction(-1,parent)
{
    setIcon(QIcon(":/images/border.png"));
    setText("Edge Decorators");
}

MLRenderingEdgeDecoratorAction::MLRenderingEdgeDecoratorAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
    setIcon(QIcon(":/images/border.png"));
    setText("Edge Decorators");
}


void MLRenderingEdgeDecoratorAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingEdgeDecoratorAction(meshId(), par);
}

void MLRenderingEdgeDecoratorAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._peredge_extra_enabled = isChecked();
        rd.set(opts);
    }  
}

bool MLRenderingEdgeDecoratorAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
        return opts._peredge_extra_enabled;
    return false;
}

MLRenderingEdgeBoundaryAction::MLRenderingEdgeBoundaryAction( QObject* parent )
    :MLRenderingAction(-1,parent)
{
     setText("Edges Boundary");
}

MLRenderingEdgeBoundaryAction::MLRenderingEdgeBoundaryAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
     setText("Edges Boundary");
}

void MLRenderingEdgeBoundaryAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingEdgeBoundaryAction(meshId(), par);
}

void MLRenderingEdgeBoundaryAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._peredge_edgeboundary_enabled = isChecked();
        rd.set(opts);
    }
}

bool MLRenderingEdgeBoundaryAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
        return opts._peredge_edgeboundary_enabled;
    return false;
}

MLRenderingFaceBoundaryAction::MLRenderingFaceBoundaryAction( QObject* parent )
    :MLRenderingAction(-1,parent)
{
    setText("Faces Boundary");
}

MLRenderingFaceBoundaryAction::MLRenderingFaceBoundaryAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
    setText("Faces Boundary");
}

void MLRenderingFaceBoundaryAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingFaceBoundaryAction(meshId(), par);
}

void MLRenderingFaceBoundaryAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._peredge_faceboundary_enabled = isChecked();
        rd.set(opts);
    }
}

bool MLRenderingFaceBoundaryAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
        return opts._peredge_faceboundary_enabled;
    return false;
}

MLRenderingEdgeManifoldAction::MLRenderingEdgeManifoldAction( QObject* parent )
    :MLRenderingAction(-1,parent)
{
    setText("No 2-Manifold Edges Decorator");
}

MLRenderingEdgeManifoldAction::MLRenderingEdgeManifoldAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
    setText("No 2-Manifold Edges Decorator");
}

void MLRenderingEdgeManifoldAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingEdgeManifoldAction(meshId(), par);
}

void MLRenderingEdgeManifoldAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._peredge_edgemanifold_enabled = isChecked();
        rd.set(opts);
    }
}

bool MLRenderingEdgeManifoldAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
        return opts._peredge_edgemanifold_enabled;
    return false;
}

MLRenderingVertManifoldAction::MLRenderingVertManifoldAction( QObject* parent )
    :MLRenderingAction(-1,parent)
{
    setText("No 2-Manifold Vertices Decorator");
}

MLRenderingVertManifoldAction::MLRenderingVertManifoldAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
    setText("No 2-Manifold Vertices Decorator");
}

void MLRenderingVertManifoldAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingVertManifoldAction(meshId(), par);
}

void MLRenderingVertManifoldAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._peredge_vertmanifold_enabled = isChecked();
        rd.set(opts);
    }
}

bool MLRenderingVertManifoldAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
        return opts._peredge_vertmanifold_enabled;
    return false;
}

MLRenderingTexBorderAction::MLRenderingTexBorderAction( QObject* parent )
    :MLRenderingAction(-1,parent)
{
    setText("Texture Borders Decorator");
}

MLRenderingTexBorderAction::MLRenderingTexBorderAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
    setText("Texture Borders Decorator");
}

void MLRenderingTexBorderAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingTexBorderAction(meshId(), par);
}

void MLRenderingTexBorderAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._peredge_text_boundary_enabled = isChecked();
        rd.set(opts);
    }
}

bool MLRenderingTexBorderAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
        return opts._peredge_text_boundary_enabled;
    return false;
}

MLRenderingDotAction::MLRenderingDotAction( QObject* parent )
    :MLRenderingAction(-1,parent)
{
    setText("Dot Decorator");
}

MLRenderingDotAction::MLRenderingDotAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{
    setText("Dot Decorator");
}

void MLRenderingDotAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingDotAction(meshId(), par);
}

void MLRenderingDotAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._perpoint_dot_enabled = isChecked();
        rd.set(opts);
    }
}

bool MLRenderingDotAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
        return opts._perpoint_dot_enabled;
    return false;
}

MLRenderingPointsSizeAction::MLRenderingPointsSizeAction( QObject* parent )
    :MLRenderingFloatAction(-1,parent)
{
    setText("Point Size");
}

MLRenderingPointsSizeAction::MLRenderingPointsSizeAction( int meshid,QObject* parent )
    :MLRenderingFloatAction(meshid,parent)
{
    setText("Point Size");
}

MLRenderingPointsSizeAction::MLRenderingPointsSizeAction(MLRenderingPointsSizeAction* origin, QObject* parent)
	:MLRenderingFloatAction(origin->meshId(),parent)
{
	setText(origin->text());
	_value = origin->_value;
}

void MLRenderingPointsSizeAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingPointsSizeAction(this, par);
}

void MLRenderingPointsSizeAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._perpoint_pointsize = _value;
        rd.set(opts);
    }
}

bool MLRenderingPointsSizeAction::isRenderingDataEnabled( const MLRenderingData& /*rd*/ ) const
{
    return true;
}

float MLRenderingPointsSizeAction::getValueFromRenderingData( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        return opts._perpoint_pointsize;
    }
    return 0.0f;
}

MLRenderingWireWidthAction::MLRenderingWireWidthAction( QObject* parent )
    :MLRenderingFloatAction(-1,parent)
{
    setText("Edge Width");
}

MLRenderingWireWidthAction::MLRenderingWireWidthAction( int meshid,QObject* parent )
    :MLRenderingFloatAction(meshid,parent)
{
    setText("Edge Width");
}

MLRenderingWireWidthAction::MLRenderingWireWidthAction(MLRenderingWireWidthAction* origin, QObject* parent)
:MLRenderingFloatAction(origin->meshId(), parent)
{
	setText(origin->text());
	_value = origin->_value;
}

void MLRenderingWireWidthAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingWireWidthAction(this, par);
}

void MLRenderingWireWidthAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._perwire_wirewidth = _value;
        rd.set(opts);
    }
}

bool MLRenderingWireWidthAction::isRenderingDataEnabled( const MLRenderingData& /*rd*/ ) const
{
    return true;
}

float MLRenderingWireWidthAction::getValueFromRenderingData( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        return opts._perwire_wirewidth;
    }
    return 0.0f;
}

MLRenderingFloatAction::MLRenderingFloatAction( QObject* parent )
    :MLRenderingAction(-1,parent)
{


}

MLRenderingFloatAction::MLRenderingFloatAction(int meshid, QObject* parent )
    :MLRenderingAction(meshid,parent)
{


}

MLRenderingBBoxQuotedInfoAction::MLRenderingBBoxQuotedInfoAction( QObject* parent )
    :MLRenderingAction(-1,parent)
{

}

MLRenderingBBoxQuotedInfoAction::MLRenderingBBoxQuotedInfoAction( int meshid,QObject* parent )
    :MLRenderingAction(meshid,parent)
{

}

void MLRenderingBBoxQuotedInfoAction::createSisterAction(MLRenderingAction *& sisteract, QObject * par)
{
	sisteract = new MLRenderingBBoxQuotedInfoAction(meshId(), par);
}

void MLRenderingBBoxQuotedInfoAction::updateRenderingData( MLRenderingData& rd )
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
    {
        opts._perbbox_quoted_info_enabled = isChecked();
        rd.set(opts);
    }
}

bool MLRenderingBBoxQuotedInfoAction::isRenderingDataEnabled( const MLRenderingData& rd ) const
{
    MLPerViewGLOptions opts;
    bool valid = rd.get(opts);
    if (valid)
        return opts._perbbox_quoted_info_enabled;
    return false;
}
