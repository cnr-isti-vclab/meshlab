/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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
/****************************************************************************
  History

 filtergeodesic.cpp,v
 Revision 1.1  2007/12/13 00:33:55  cignoni
 New small samples

 Revision 1.1  2007/12/02 07:57:48  cignoni
 Added the new sample filter plugin that removes border faces


*****************************************************************************/
#include <Qt>

#include "filter_geodesic.h"

using namespace std;
using namespace vcg;

FilterGeodesic::FilterGeodesic()
{
	typeList = {
		FP_QUALITY_BORDER_GEODESIC,
		FP_QUALITY_POINT_GEODESIC,
        FP_QUALITY_SELECTED_GEODESIC,
        FP_QUALITY_SELECTED_GEODESIC_HEAT
	};

	for(ActionIDType tt : types())
		actionList.push_back(new QAction(filterName(tt), this));
}

FilterGeodesic::~FilterGeodesic() {
}

QString FilterGeodesic::pluginName() const
{
	return "FilterGeodesic";
}

QString FilterGeodesic::filterName(ActionIDType filter) const
{
	switch (filter) {
	case FP_QUALITY_BORDER_GEODESIC: return QString("Colorize by border distance");
	case FP_QUALITY_POINT_GEODESIC:
		return QString("Colorize by geodesic distance from a given point");
	case FP_QUALITY_SELECTED_GEODESIC:
		return QString("Colorize by geodesic distance from the selected points");
    case FP_QUALITY_SELECTED_GEODESIC_HEAT:
        return QString("Colorize by approximated geodesic distance from the selected points");
	default: assert(0); return QString();
	}
}

QString FilterGeodesic::pythonFilterName(ActionIDType f) const
{
	switch (f) {
	case FP_QUALITY_BORDER_GEODESIC: return QString("compute_scalar_by_border_distance_per_vertex");
	case FP_QUALITY_POINT_GEODESIC:
		return QString("compute_scalar_by_geodesic_distance_from_given_point_per_vertex");
	case FP_QUALITY_SELECTED_GEODESIC:
		return QString("compute_scalar_by_geodesic_distance_from_selection_per_vertex");
    case FP_QUALITY_SELECTED_GEODESIC_HEAT:
        return QString("compute_scalar_by_heat_geodesic_distance_from_selection_per_vertex");
	default: assert(0); return QString();
	}
}

QString FilterGeodesic::filterInfo(ActionIDType filterId) const
{
	switch(filterId)
	{
    case FP_QUALITY_BORDER_GEODESIC        : return tr("Store in the quality field the geodesic distance from borders and color the mesh accordingly.");
    case FP_QUALITY_POINT_GEODESIC         : return tr("Store in the quality field the geodesic distance from a given point on the mesh surface and color the mesh accordingly.");
    case FP_QUALITY_SELECTED_GEODESIC      : return tr("Store in the quality field the geodesic distance from the selected points on the mesh surface and color the mesh accordingly.");
    case FP_QUALITY_SELECTED_GEODESIC_HEAT : return tr("Store in the quality field the approximated geodesic distance, computed via heat method (Crane et al.), from the selected points on the mesh surface and color the mesh accordingly. As this implementation does not use intrinsic triangulation it is very sensitive to trinagulation. First run takes longer as factorization has to be build. ");
	default                             : assert(0);
	}
	return QString("error!");
}

FilterGeodesic::FilterClass FilterGeodesic::getClass(const QAction *a) const
{
	switch(ID(a))
	{
    case FP_QUALITY_BORDER_GEODESIC        :
    case FP_QUALITY_SELECTED_GEODESIC      :
    case FP_QUALITY_POINT_GEODESIC         :
    case FP_QUALITY_SELECTED_GEODESIC_HEAT : return FilterGeodesic::FilterClass(FilterPlugin::VertexColoring + FilterPlugin::Quality);
	default                          : assert(0);
	}
	return FilterPlugin::Generic;
}

int FilterGeodesic::getRequirements(const QAction *action)
{
	switch(ID(action))
	{
    case FP_QUALITY_BORDER_GEODESIC          :
    case FP_QUALITY_SELECTED_GEODESIC        :
    case FP_QUALITY_POINT_GEODESIC           : return MeshModel::MM_VERTFACETOPO;
    case FP_QUALITY_SELECTED_GEODESIC_HEAT   : return MeshModel::MM_VERTFACETOPO + MeshModel::MM_FACEFACETOPO;
	default: assert(0);
	}
	return 0;
}

std::map<std::string, QVariant> FilterGeodesic::applyFilter(const QAction *filter, const RichParameterList & par, MeshDocument &md, unsigned int& /*postConditionMask*/, vcg::CallBackPos *cb)
{
	MeshModel &m=*(md.mm());
	CMeshO::VertexIterator vi;
	switch (ID(filter)) {
	case FP_QUALITY_POINT_GEODESIC:
	{
		m.updateDataMask(MeshModel::MM_VERTFACETOPO);
		m.updateDataMask(MeshModel::MM_VERTMARK);
		m.updateDataMask(MeshModel::MM_VERTQUALITY);
		m.updateDataMask(MeshModel::MM_VERTCOLOR);
		tri::UpdateFlags<CMeshO>::FaceBorderFromVF(m.cm);
		tri::UpdateFlags<CMeshO>::VertexBorderFromFaceBorder(m.cm);
		Point3m startPoint = par.getPoint3m("startPoint");
		// first search the closest point on the surface;
		CMeshO::VertexPointer startVertex=0;
		Scalarm minDist= std::numeric_limits<Scalarm>::max();

		for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi) if(!(*vi).IsD())
			if(SquaredDistance(startPoint,(*vi).P()) < minDist) {
				startVertex=&*vi;
				minDist=SquaredDistance(startPoint,(*vi).P());
			}


		log("Input point is %f %f %f Closest on surf is %f %f %f",startPoint[0],startPoint[1],startPoint[2],startVertex->P()[0],startVertex->P()[1],startVertex->P()[2]);

		// Now actually compute the geodesic distance from the closest point
		Scalarm dist_thr = par.getAbsPerc("maxDistance");
		tri::EuclideanDistance<CMeshO> dd;
		tri::Geodesic<CMeshO>::Compute(m.cm, vector<CVertexO*>(1,startVertex),dd,dist_thr);

		// Cleaning Quality value of the unreferenced vertices
		// Unreached vertices has a quality that is maxfloat
		int unreachedCnt=0;
		Scalarm unreached  = std::numeric_limits<Scalarm>::max();
		for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi) if(!(*vi).IsD())
			if((*vi).Q() == unreached) {
				unreachedCnt++;
				(*vi).Q()=0;
			}
		if(unreachedCnt >0 )
			log("Warning: %i vertices were unreachable from the borders, probably your mesh has unreferenced vertices",unreachedCnt);

		tri::UpdateColor<CMeshO>::PerVertexQualityRamp(m.cm);

	}
		break;
	case FP_QUALITY_BORDER_GEODESIC:
	{
		m.updateDataMask(MeshModel::MM_VERTFACETOPO);
		m.updateDataMask(MeshModel::MM_VERTMARK);
		m.updateDataMask(MeshModel::MM_VERTQUALITY);
		m.updateDataMask(MeshModel::MM_VERTCOLOR);
		tri::UpdateFlags<CMeshO>::FaceBorderFromVF(m.cm);
		tri::UpdateFlags<CMeshO>::VertexBorderFromFaceBorder(m.cm);

		bool ret = tri::Geodesic<CMeshO>::DistanceFromBorder(m.cm);

		// Cleaning Quality value of the unreferenced vertices
		// Unreached vertices has a quality that is maxfloat
		int unreachedCnt=0;
		Scalarm unreached  = std::numeric_limits<Scalarm>::max();
		for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi) if(!(*vi).IsD())
			if((*vi).Q() == unreached) {
				unreachedCnt++;
				(*vi).Q()=0;
			}
		if(unreachedCnt >0 )
			log("Warning: %i vertices were unreachable from the borders, probably your mesh has unreferenced vertices",unreachedCnt);

		if(!ret) log("Mesh Has no borders. No geodesic distance computed");
		else tri::UpdateColor<CMeshO>::PerVertexQualityRamp(m.cm);
	}

		break;
	case FP_QUALITY_SELECTED_GEODESIC:
	{
		m.updateDataMask(MeshModel::MM_VERTFACETOPO);
		m.updateDataMask(MeshModel::MM_VERTMARK);
		m.updateDataMask(MeshModel::MM_VERTQUALITY);
		m.updateDataMask(MeshModel::MM_VERTCOLOR);
		tri::UpdateFlags<CMeshO>::FaceBorderFromVF(m.cm);
		tri::UpdateFlags<CMeshO>::VertexBorderFromFaceBorder(m.cm);

		std::vector<CMeshO::VertexPointer> seedVec;
		ForEachVertex(m.cm, [&seedVec] (CMeshO::VertexType & v) {
			if (v.IsS())
				seedVec.push_back(&v);
		});

		if (seedVec.size() > 0)
		{
			Scalarm dist_thr = par.getAbsPerc("maxDistance");
			tri::EuclideanDistance<CMeshO> dd;
			tri::Geodesic<CMeshO>::Compute(m.cm, seedVec, dd, dist_thr);

			// Cleaning Quality value of the unreferenced vertices
			// Unreached vertices has a quality that is maxfloat
			int unreachedCnt=0;
			Scalarm unreached  = std::numeric_limits<Scalarm>::max();
			ForEachVertex(m.cm, [&] (CMeshO::VertexType & v){
				if (v.Q() == unreached)
				{
					++unreachedCnt;
					v.Q() = 0;
				}
			});

			if(unreachedCnt >0 )
				log("Warning: %i vertices were unreachable from the seeds, probably your mesh has unreferenced vertices",unreachedCnt);

			tri::UpdateColor<CMeshO>::PerVertexQualityRamp(m.cm);
		}
		else
			log("Warning: no vertices are selected! aborting geodesic computation.");
	}
        break;
    case FP_QUALITY_SELECTED_GEODESIC_HEAT:
    {
        m.updateDataMask(MeshModel::MM_FACEFACETOPO);
        m.updateDataMask(MeshModel::MM_VERTFACETOPO);
        m.updateDataMask(MeshModel::MM_FACEQUALITY);
        m.updateDataMask(MeshModel::MM_FACENORMAL);
        m.updateDataMask(MeshModel::MM_VERTQUALITY);
        m.updateDataMask(MeshModel::MM_VERTCOLOR);

        std::vector<CMeshO::VertexPointer> seedVec;
        ForEachVertex(m.cm, [&seedVec] (CMeshO::VertexType & v) {
            if (v.IsS()) seedVec.push_back(&v);
        });

        float param_m = par.getFloat("m");

        if (seedVec.size() > 0){
            // cache factorizations to reduce runtime on successive computations
            std::pair<float, tri::GeodesicHeat<CMeshO>::GeodesicHeatCache> cache;
            if (!vcg::tri::HasPerMeshAttribute(m.cm, "GeodesicHeatCache")){
                cb(20, "Building Cache: Computing Factorizations...");
                cache = std::make_pair(param_m, tri::GeodesicHeat<CMeshO>::BuildCache(m.cm, param_m));
                // save cache for next compute
                auto GeodesicHeatCacheHandle = tri::Allocator<CMeshO>::GetPerMeshAttribute<std::pair<float, tri::GeodesicHeat<CMeshO>::GeodesicHeatCache>>(m.cm, std::string("GeodesicHeatCache"));
                GeodesicHeatCacheHandle() = cache;
            }
            else {
                cb(10, "Recovering Cache...");
                // recover cache
                auto GeodesicHeatCacheHandle = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::pair<float, tri::GeodesicHeat<CMeshO>::GeodesicHeatCache>>(m.cm, std::string("GeodesicHeatCache"));
                // if m has changed rebuild everything
                if (std::get<0>(GeodesicHeatCacheHandle()) != param_m){
                    cb(20, "Parameter Changed: Rebuilding Factorizations...");
                    GeodesicHeatCacheHandle() = std::make_pair(param_m, tri::GeodesicHeat<CMeshO>::BuildCache(m.cm, param_m));
                }
                cache = GeodesicHeatCacheHandle();
            }
            cb(80, "Computing Geodesic Distance...");

            if (tri::GeodesicHeat<CMeshO>::ComputeFromCache(m.cm, seedVec, std::get<1>(cache))){
                tri::UpdateColor<CMeshO>::PerVertexQualityRamp(m.cm);
            }
            else{
                log("Warning: heat method has failed. The mesh is most likely badly conditioned (e.g. angles ~ 0deg) or has disconnected components");
                // delete cache as its most likely useless after failure
                auto GeodesicHeatCacheHandle = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::pair<float, tri::GeodesicHeat<CMeshO>::GeodesicHeatCache>>(m.cm, std::string("GeodesicHeatCache"));
                tri::Allocator<CMeshO>::DeletePerMeshAttribute<std::pair<float, tri::GeodesicHeat<CMeshO>::GeodesicHeatCache>>(m.cm, GeodesicHeatCacheHandle);
            }
        }
        else
            log("Warning: no vertices are selected! aborting geodesic computation.");
    }
    break;
	default:
		wrongActionCalled(filter);
		break;
	}
	return std::map<std::string, QVariant>();
}

RichParameterList FilterGeodesic::initParameterList(const QAction *action, const MeshModel &m)
{
	RichParameterList parlst;
	switch(ID(action))
	{
	case FP_QUALITY_POINT_GEODESIC :
		parlst.addParam(RichPosition("startPoint",m.cm.bbox.min,"Starting point","The starting point from which geodesic distance has to be computed. If it is not a surface vertex, the closest vertex to the specified point is used as starting seed point."));
		parlst.addParam(RichPercentage("maxDistance",m.cm.bbox.Diag(),0,m.cm.bbox.Diag()*2,"Max Distance","If not zero it indicates a cut off value to be used during geodesic distance computation."));
		break;
	case FP_QUALITY_SELECTED_GEODESIC :
		parlst.addParam(RichPercentage("maxDistance",m.cm.bbox.Diag(),0,m.cm.bbox.Diag()*2,"Max Distance","If not zero it indicates a cut off value to be used during geodesic distance computation."));
        break;
    case FP_QUALITY_SELECTED_GEODESIC_HEAT :
        parlst.addParam(RichFloat("m", 1.0, tr("Euler Step"), tr("Multiplier used in backward Euler timestep. Changing this value will reset the cache.")));
        break;
	default: break; // do not add any parameter for the other filters
	}
	return parlst;
}

int FilterGeodesic::postCondition(const QAction * filter) const
{
	switch (ID(filter))
	{
    case FP_QUALITY_BORDER_GEODESIC        :
    case FP_QUALITY_SELECTED_GEODESIC      :
    case FP_QUALITY_POINT_GEODESIC         : return MeshModel::MM_VERTCOLOR + MeshModel::MM_VERTQUALITY;
    case FP_QUALITY_SELECTED_GEODESIC_HEAT : return MeshModel::MM_VERTCOLOR + MeshModel::MM_VERTQUALITY + MeshModel::MM_FACEQUALITY;
    default                                : return MeshModel::MM_ALL;
	}
}
MESHLAB_PLUGIN_NAME_EXPORTER(FilterGeodesic)
