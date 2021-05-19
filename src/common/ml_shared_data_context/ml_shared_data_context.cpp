/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2021                                           \/)\/    *
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

#include "ml_shared_data_context.h"
#include "../GLExtensionsManager.h"
#include "../mlexception.h"
#include <vector>
#include <QThread>

#include "../ml_document/mesh_document.h"

void MLPoliciesStandAloneFunctions::computeRequestedRenderingDataCompatibleWithMeshSameGLOpts(MeshModel* meshmodel, const MLRenderingData& inputdt, MLRenderingData& outputdt)
{
	MLPoliciesStandAloneFunctions::computeRequestedRenderingDataCompatibleWithMeshCommonCode(meshmodel, inputdt, outputdt);
	MLPerViewGLOptions opts;
	inputdt.get(opts);
	outputdt.set(opts);	
}

void MLPoliciesStandAloneFunctions::computeRequestedRenderingDataCompatibleWithMesh( MeshModel* meshmodel,const MLRenderingData& inputdt,MLRenderingData& outputdt)                                                                                    
{
	MLPoliciesStandAloneFunctions::computeRequestedRenderingDataCompatibleWithMeshCommonCode(meshmodel, inputdt, outputdt);
	MLPoliciesStandAloneFunctions::setPerViewGLOptionsAccordindToWireModality(meshmodel, outputdt);
	MLPoliciesStandAloneFunctions::setPerViewGLOptionsPriorities(outputdt);
	MLPerViewGLOptions opts;
	outputdt.get(opts);
	MLPoliciesStandAloneFunctions::suggestedDefaultPerViewGLOptions(opts);
	outputdt.set(opts);
}

void MLPoliciesStandAloneFunctions::computeRequestedRenderingDataCompatibleWithMeshCommonCode(MeshModel* meshmodel, const MLRenderingData& inputdt, MLRenderingData& outputdt)
{
	if (meshmodel == NULL)
		return;
	CMeshO& mesh = meshmodel->cm;
	if (mesh.VN() == 0)
	{
		outputdt.reset(false);
		return;
	}
	bool validfaces = (mesh.FN() > 0);
	
//	MLRenderingData::PRIMITIVE_MODALITY_MASK tmpoutputpm = inputdt.getPrimitiveModalityMask();
	for (size_t pmind = 0; pmind < size_t(MLRenderingData::PR_ARITY); ++pmind)
	{
		MLRenderingData::PRIMITIVE_MODALITY pmc = MLRenderingData::PRIMITIVE_MODALITY(pmind);


		MLRenderingData::RendAtts tmpoutputatts;
		if (inputdt.get(MLRenderingData::PRIMITIVE_MODALITY(pmind), tmpoutputatts))
		{
			tmpoutputatts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] &= meshmodel->hasDataMask(MeshModel::MM_VERTCOORD);
			tmpoutputatts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] &= meshmodel->hasDataMask(MeshModel::MM_VERTNORMAL);
			tmpoutputatts[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] &= validfaces && meshmodel->hasDataMask(MeshModel::MM_FACENORMAL);
			tmpoutputatts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] &= meshmodel->hasDataMask(MeshModel::MM_VERTCOLOR);
			tmpoutputatts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] &= validfaces && meshmodel->hasDataMask(MeshModel::MM_FACECOLOR);

			//horrible trick caused by MeshLab GUI. In MeshLab exists just a button turning on/off the texture visualization.
			//Unfortunately the RenderMode::textureMode member field is not just a boolean value but and enum one.
			//The enum-value depends from the enabled attributes of input mesh.
			bool wedgetexture = meshmodel->hasDataMask(MeshModel::MM_WEDGTEXCOORD);
			tmpoutputatts[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] &= (meshmodel->hasDataMask(MeshModel::MM_VERTTEXCOORD) && (!wedgetexture));
			tmpoutputatts[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE] &= validfaces && wedgetexture;
			if (MLPoliciesStandAloneFunctions::isPrimitiveModalityCompatibleWithMesh(meshmodel, pmc))
				outputdt.set(pmc, tmpoutputatts);
		}
		else
			throw MLException(QString("MLPoliciesStandAloneFunctions: trying to access to a non defined PRIMITIVE_MODALITY!"));
	}
}

void MLPoliciesStandAloneFunctions::fromMeshModelMaskToMLRenderingAtts( int meshmodelmask,MLRenderingData::RendAtts& atts)
{
    atts.reset();
    //connectivitychanged = bool(meshmodelmask | MeshModel::MM_FACEFACETOPO) || bool(meshmodelmask | MeshModel::MM_VERTFACETOPO) || bool(meshmodelmask | MeshModel::MM_VERTNUMBER) || bool(meshmodelmask | MeshModel::MM_FACENUMBER);
    atts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = bool(meshmodelmask & MeshModel::MM_VERTCOORD);
    atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = bool(meshmodelmask & MeshModel::MM_VERTNORMAL);
    atts[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] = bool(meshmodelmask & MeshModel::MM_FACENORMAL);
    atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] = bool(meshmodelmask & MeshModel::MM_VERTCOLOR);
    atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] = bool(meshmodelmask & MeshModel::MM_FACECOLOR);
    //atts[MLRenderingData::ATT_NAMES::ATT_FIXEDCOLOR] = bool(meshmodelmask & MeshModel::MM_COLOR);
    atts[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] = bool(meshmodelmask & MeshModel::MM_VERTTEXCOORD);
    atts[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE] = bool(meshmodelmask & MeshModel::MM_WEDGTEXCOORD);
}

void MLPoliciesStandAloneFunctions::maskMeaninglessAttributesPerPrimitiveModality( MLRenderingData::PRIMITIVE_MODALITY pm,MLRenderingData::RendAtts& atts )
{
    switch(pm)
    {
    case (MLRenderingData::PR_POINTS):
        {
            atts[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] = false;
            atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] = false;
            atts[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE] = false;
            return;
        }
    case (MLRenderingData::PR_WIREFRAME_EDGES):
    case (MLRenderingData::PR_WIREFRAME_TRIANGLES):
        {
            atts[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] = false;
            atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] = false;
            atts[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] = false;
            atts[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE] = false;  
            return;
        }
    case (MLRenderingData::PR_SOLID):
        {
            return;
        }
    case (MLRenderingData::PR_ARITY):
        {
            throw MLException("PR_ARITY passed as parameter!");
        }
    }
}

void MLPoliciesStandAloneFunctions::updatedRendAttsAccordingToPriorities(
		const MLRenderingData::PRIMITIVE_MODALITY pm,
		const MLRenderingData::RendAtts& updated, //from the result of the filter
		const MLRenderingData::RendAtts& current, //from the current model
		MLRenderingData::RendAtts& result) //returned final result
{
	MLRenderingData::RendAtts filteredupdated = updated;
	MLRenderingData::RendAtts tmp = current; // tmp will be then saved in returned
	tmp[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] |= filteredupdated[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION];
	if ((pm == MLRenderingData::PR_WIREFRAME_TRIANGLES) || (pm == MLRenderingData::PR_WIREFRAME_EDGES))
	{
		tmp[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = false;
		tmp[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] = false;
	}
	else
	{
		//vert normal shading if in current or in updated
		tmp[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] |= filteredupdated[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL];
//		tmp[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] = (tmp[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] || filteredupdated[MLRenderingData::ATT_NAMES::ATT_FACENORMAL]) && !(filteredupdated[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL]);
		//face normal shading if in current and in updated
		tmp[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] = (tmp[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] && filteredupdated[MLRenderingData::ATT_NAMES::ATT_FACENORMAL]);
	}

	tmp[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] |= filteredupdated[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR];
	tmp[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] = (tmp[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] || filteredupdated[MLRenderingData::ATT_NAMES::ATT_FACECOLOR]) && !(filteredupdated[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR]);
	tmp[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE] |= filteredupdated[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE];
	tmp[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] = (tmp[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] || filteredupdated[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE]) && !(filteredupdated[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE]);
	result = tmp;
}

void MLPoliciesStandAloneFunctions::suggestedDefaultPerViewRenderingData(MeshModel* meshmodel,MLRenderingData& dtout, size_t minpolnumpersmoothshading)
{
    MLRenderingData dt; 
    if (meshmodel == NULL)
        return;
    if (meshmodel->cm.VN() == 0)
    {
        dt.reset();
    }
    else
    {
        MLRenderingData::RendAtts tmpatts;
        tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
        tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;
        tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] = true;
        tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] = true;

        dt.set(MLRenderingData::PR_POINTS,tmpatts);
       
        if (meshmodel->cm.FN() > 0)
        {
            dt.set(MLRenderingData::PR_POINTS,false);
            tmpatts.reset();
            tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
            tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = (size_t(meshmodel->cm.FN()) > minpolnumpersmoothshading);
			tmpatts[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] = !(tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL]);
            tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] = true;
			if (meshmodel != NULL)
				tmpatts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] = (meshmodel->hasDataMask(MeshModel::MM_FACECOLOR) && !meshmodel->hasDataMask(MeshModel::MM_VERTCOLOR));
            tmpatts[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE] = true;
			tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] = true;
            dt.set(MLRenderingData::PR_SOLID,tmpatts);
        }
    
		if (((meshmodel->cm.FN() == 0) && (meshmodel->cm.EN() > 0)) || (meshmodel->hasDataMask(MeshModel::MM_POLYGONAL)))
		{
			//dt.reset();
			tmpatts.reset();
			tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
			tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;
			tmpatts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] = true;
			MLPerViewGLOptions opts;
			dt.get(opts);
			opts._peredge_wire_enabled = true;
			opts._peredge_fauxwire_enabled = true;
			dt.set(opts);
			//dt.set(MLRenderingData::PR_SOLID, false);
		}
        
    }
	MLPoliciesStandAloneFunctions::setBestWireModality(meshmodel, dt);
    MLPoliciesStandAloneFunctions::computeRequestedRenderingDataCompatibleWithMesh(meshmodel,dt,dtout);
}


void MLPoliciesStandAloneFunctions::disableRedundatRenderingDataAccordingToPriorities(MLRenderingData& dt)
{
	for (MLRenderingData::PRIMITIVE_MODALITY pr = MLRenderingData::PRIMITIVE_MODALITY(0); pr < MLRenderingData::PR_ARITY; pr = MLRenderingData::next(pr))
	{
		MLRenderingData::RendAtts atts;
		dt.get(pr, atts);
		MLPoliciesStandAloneFunctions::setAttributePriorities(atts);
		dt.set(pr, atts);
	}

	//setPerViewGLOptionsPriorities(dt);
}


bool MLPoliciesStandAloneFunctions::isPrimitiveModalityCompatibleWithMesh(MeshModel* m,const MLRenderingData::PRIMITIVE_MODALITY pm)
{
    bool validvert = (m->cm.VN() > 0);
    bool validfaces = (m->cm.FN() > 0);
    bool validedges = (m->cm.EN() > 0);
    return MLPoliciesStandAloneFunctions::isPrimitiveModalityCompatibleWithMeshInfo(validvert,validfaces,validedges,m->dataMask(),pm);
}

bool MLPoliciesStandAloneFunctions::isPrimitiveModalityCompatibleWithMeshInfo(bool validvert,bool validfaces,bool validedges,int meshmask,const MLRenderingData::PRIMITIVE_MODALITY pm)
{
    switch(pm)
    {
    case(MLRenderingData::PR_POINTS):
        {
            return validvert;
        }
    case(MLRenderingData::PR_WIREFRAME_EDGES):
        {
            return (validvert) && ((meshmask & MeshModel::MM_POLYGONAL) || ((!validfaces) && (validedges)));
        }
    case(MLRenderingData::PR_WIREFRAME_TRIANGLES):
    case(MLRenderingData::PR_SOLID):
        {
            return validvert && validfaces;
        }
    case (MLRenderingData::PR_ARITY):
        {
            throw MLException("PR_ARITY passed as parameter!");
        }      
    }
    return false;
}

bool MLPoliciesStandAloneFunctions::isPrimitiveModalityWorthToBeActivated(MLRenderingData::PRIMITIVE_MODALITY pm,bool wasvisualized,bool wasmeanigful,bool ismeaningful)
{
    (void) pm;
    if ((!wasmeanigful) && (ismeaningful))
        return true;
    if (wasmeanigful && ismeaningful && wasvisualized)
        return true;
    return false;
}

void MLPoliciesStandAloneFunctions::setAttributePriorities(MLRenderingData::RendAtts& atts )
{
    atts[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] &= !(atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL]);
    atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] &= !(atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR]);
    atts[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] &= !(atts[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE]);
}

//void MLPoliciesStandAloneFunctions::setPerViewRenderingDataPriorities( MeshModel* mm,MLRenderingData& dt )
//{
//    if (mm == NULL)
//        return;
//    
//}

void MLPoliciesStandAloneFunctions::setPerViewGLOptionsPriorities(MLRenderingData& dt )
{
    //if (mm == NULL)
    //    return;
    //bool permeshcolor = mm->hasDataMask(MeshModel::MM_COLOR);
    MLPerViewGLOptions glopts;
    if (!dt.get(glopts))
        return;
    /*if (permeshcolor)
        glopts._perpoint_mesh_color_enabled = true;*/

    for(MLRenderingData::PRIMITIVE_MODALITY pm = MLRenderingData::PRIMITIVE_MODALITY(0);pm < MLRenderingData::PR_ARITY;pm = MLRenderingData::next(pm))
    {
        MLRenderingData::RendAtts atts;
        if (dt.get(pm,atts))
        {
            switch(pm)
            {
            case (MLRenderingData::PR_POINTS):
                {
                    glopts._perpoint_noshading = !atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL];
                    glopts._perpoint_mesh_color_enabled &= !atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR];
                    glopts._perpoint_fixed_color_enabled = !atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] && !glopts._perpoint_mesh_color_enabled;

                    break;
                }
            case (MLRenderingData::PR_WIREFRAME_EDGES):
            case (MLRenderingData::PR_WIREFRAME_TRIANGLES):
                {
                    glopts._perwire_noshading = !atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL];
                    glopts._perwire_mesh_color_enabled &= !atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR];
                    glopts._perwire_fixed_color_enabled =  !atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] && !atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] && !glopts._perwire_mesh_color_enabled;
                    break;
                }
            case (MLRenderingData::PR_SOLID):
                {
                    glopts._persolid_noshading = (!atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL]) && (!atts[MLRenderingData::ATT_NAMES::ATT_FACENORMAL]);
                    glopts._persolid_mesh_color_enabled &= !atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] && !atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR];
                    glopts._persolid_fixed_color_enabled = !atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] && !atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] && !glopts._persolid_mesh_color_enabled;
                    break;
                }
            case (MLRenderingData::PR_ARITY):
                {
                    throw MLException("PR_ARITY passed as parameter!");
                }
              
            }
        }
    }
    dt.set(glopts);
}

void MLPoliciesStandAloneFunctions::filterUselessUdpateAccordingToMeshMask( MeshModel* m,MLRenderingData::RendAtts& atts )
{
    if (m == NULL)
        return;
    atts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] &= m->hasDataMask(MeshModel::MM_VERTCOORD);
    atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] &= m->hasDataMask(MeshModel::MM_VERTNORMAL);
    atts[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] &= m->hasDataMask(MeshModel::MM_FACENORMAL);
    atts[MLRenderingData::ATT_NAMES::ATT_VERTCOLOR] &= m->hasDataMask(MeshModel::MM_VERTCOLOR);
    atts[MLRenderingData::ATT_NAMES::ATT_FACECOLOR] &= m->hasDataMask(MeshModel::MM_FACECOLOR);
    //atts[MLRenderingData::ATT_NAMES::ATT_FIXEDCOLOR]
    atts[MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE] &= m->hasDataMask(MeshModel::MM_WEDGTEXCOORD);
    atts[MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE] &= m->hasDataMask(MeshModel::MM_VERTTEXCOORD);
}

void MLPoliciesStandAloneFunctions::setBestWireModality(MeshModel* mm, MLRenderingData& dt)
{
	if (mm == NULL)
		return;
	MLPerViewGLOptions opts;
	dt.get(opts);
	
	if ((mm->cm.VN() > 0) && (mm->cm.FN() == 0) && (mm->cm.EN() > 0))
	{
		dt.set(MLRenderingData::PR_WIREFRAME_EDGES,true && opts._peredge_wire_enabled);
		dt.set(MLRenderingData::PR_WIREFRAME_TRIANGLES, false);
		return;
	}

	if (opts._peredge_wire_enabled)
	{
		if (opts._peredge_fauxwire_enabled)
		{
			dt.set(MLRenderingData::PR_WIREFRAME_EDGES, true);
			dt.set(MLRenderingData::PR_WIREFRAME_TRIANGLES, false);
		}
		else
		{
			dt.set(MLRenderingData::PR_WIREFRAME_TRIANGLES, true);
			dt.set(MLRenderingData::PR_WIREFRAME_EDGES, false);
		}
	}
	else
	{
		dt.set(MLRenderingData::PR_WIREFRAME_TRIANGLES, false);
		dt.set(MLRenderingData::PR_WIREFRAME_EDGES, false);
	}
}


void MLPoliciesStandAloneFunctions::setPerViewGLOptionsAccordindToWireModality(MeshModel* mm, MLRenderingData& dt)
{
	if (mm == NULL)
		return;
	MLPerViewGLOptions glopts;
	dt.get(glopts);
	glopts._peredge_fauxwire_enabled = dt.isPrimitiveActive(MLRenderingData::PR_WIREFRAME_EDGES);
	glopts._peredge_wire_enabled = (dt.isPrimitiveActive(MLRenderingData::PR_WIREFRAME_EDGES) || dt.isPrimitiveActive(MLRenderingData::PR_WIREFRAME_TRIANGLES) || (glopts._peredge_wire_enabled && (mm->cm.VN() > 0) && (mm->cm.FN() > 0)));
	dt.set(glopts);
}

MLRenderingData::PRIMITIVE_MODALITY MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh( MeshModel* m )
{
    if (m != NULL)
    {
        if (m->cm.FN() > 0)
            return MLRenderingData::PR_SOLID;
        else
            if ((m->cm.VN() > 0) && (m->cm.EN() > 0))
                return MLRenderingData::PR_WIREFRAME_EDGES;
            else
                if (m->cm.VN() > 0)
                    return MLRenderingData::PR_POINTS;
    }
    return MLRenderingData::PR_ARITY;
}

void MLPoliciesStandAloneFunctions::suggestedDefaultPerViewGLOptions( MLPerViewGLOptions& tmp )
{
	tmp._perbbox_fixed_color = vcg::Color4b(0xffeaeaea);
	tmp._perpoint_fixed_color = vcg::Color4b(0xff459583);
    tmp._perwire_fixed_color = vcg::Color4b(vcg::Color4b::DarkGray);
    tmp._persolid_fixed_color = vcg::Color4b(vcg::Color4b::LightGray);
	tmp._sel_enabled = true;
	tmp._peredge_extra_enabled = true;
}

//void MLPoliciesStandAloneFunctions::bestPrimitiveModalityMaskAfterUpdate( MeshModel* meshmodel,int meshmodelmask,const MLRenderingData::PRIMITIVE_MODALITY_MASK& inputpm,MLRenderingData::PRIMITIVE_MODALITY_MASK& outputpm )
//{
//    MLRenderingData::PRIMITIVE_MODALITY_MASK tmpmask = 0;
//    if (meshmodel->cm.VN() == 0)
//    {
//        outputpm = MLRenderingData::PR_NONE;
//        return;
//    }
//
//    tmpmask = (inputpm
//}

MLRenderingData::MLRenderingData()
    :PerViewData<MLPerViewGLOptions>()
{
    _glopts = new MLPerViewGLOptions();
}

//MLRenderingData::MLRenderingData( const MLRenderingData& dt )
//    :PerViewData<MLPerViewGLOptions>(dt)
//{
//}

bool MLRenderingData::set( MLRenderingData::PRIMITIVE_MODALITY pm,const MLRenderingData::RendAtts& atts )
{
    MLRenderingData::RendAtts tmp(atts);
    MLPoliciesStandAloneFunctions::maskMeaninglessAttributesPerPrimitiveModality(pm,tmp);
    //MLPoliciesStandAloneFunctions::setAttributePriorities(tmp);
    return PerViewData<MLPerViewGLOptions>::set(pm,tmp);
}

bool MLRenderingData::set( MLRenderingData::PRIMITIVE_MODALITY pm,MLRenderingData::ATT_NAMES att,bool onoff )
{
    MLRenderingData::RendAtts tmp;
    bool valid = this->get(pm,tmp);
    if (valid)
    {
        tmp[att] = onoff;
        return set(pm,tmp);
    }
    return false;
}

bool MLRenderingData::set( MLRenderingData::PRIMITIVE_MODALITY pm,bool onoff )
{
    return vcg::PerViewData<MLPerViewGLOptions>::set(pm,onoff);
}

void MLRenderingData::set( const MLPerViewGLOptions& opts )
{
    vcg::PerViewData<MLPerViewGLOptions>::set(opts);
}

