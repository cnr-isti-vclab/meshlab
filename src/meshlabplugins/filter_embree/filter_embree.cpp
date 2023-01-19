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

#include "filter_embree.h"
#include <wrap/embree/EmbreeAdaptor.h>
#include <QCoreApplication>
/**
 * @brief Constructor usually performs only two simple tasks of filling the two lists
 *  - typeList: with all the possible id of the filtering actions
 *  - actionList with the corresponding actions. If you want to add icons to
 *  your filtering actions you can do here by construction the QActions accordingly
 */
FilterEmbreePlugin::FilterEmbreePlugin() 
{ 
	typeList = {		 
		FP_OBSCURANCE, 
		FP_AMBIENT_OCCLUSION,
		FP_SDF, 
		FP_SELECT_VISIBLE_FACES, 
		FP_ANALYZE_NORMALS
		};

	for(ActionIDType tt : types())
		actionList.push_back(new QAction(filterName(tt), this));
	
}

FilterEmbreePlugin::~FilterEmbreePlugin()
{
}

QString FilterEmbreePlugin::pluginName() const
{
    return "FilterEmbree";
}

/**
 * @brief ST() must return the very short string describing each filtering action
 * (this string is used also to define the menu entry)
 * @param filterId: the id of the filter
 * @return the name of the filter
 */
QString FilterEmbreePlugin::filterName(ActionIDType filterId) const
{
	switch(filterId) {
	case FP_OBSCURANCE :
		return QString("Compute Obscurance");
	case FP_AMBIENT_OCCLUSION:
		return QString("Compute Ambient occlusion");
	case FP_SDF:
		return QString("Compute Shape-Diameter Function");
	case FP_SELECT_VISIBLE_FACES:
		return QString("Select Visible Faces ");
	case FP_ANALYZE_NORMALS:
		return QString("Reorient inward pointing faces");
	default :
		assert(0);
		return QString();
	}
}

/**
 * @brief FilterSamplePlugin::pythonFilterName if you want that your filter should have a different
 * name on pymeshlab, use this function to return its python name.
 * @param f
 * @return
 */
QString FilterEmbreePlugin::pythonFilterName(ActionIDType f) const
{
	switch(f) {
	case FP_OBSCURANCE :
		return QString("Compute Obscurance with Embree");
	case FP_AMBIENT_OCCLUSION:
		return QString("Compute Ambient occlusion with Embree");
	case FP_SDF:
		return QString("Compute Shape-Diameter Function with Embree");
	case FP_SELECT_VISIBLE_FACES:
		return QString("Compute Visible Faces Select with Embree");
	case FP_ANALYZE_NORMALS:
		return QString("Reorient inward pointing faces");
	default :
		assert(0);
		return QString();
	}
}


/**
 * @brief // Info() must return the longer string describing each filtering action
 * (this string is used in the About plugin dialog)
 * @param filterId: the id of the filter
 * @return an info string of the filter
 *  
 */
 QString FilterEmbreePlugin::filterInfo(ActionIDType filterId) const
{
	switch(filterId) {
		case FP_OBSCURANCE:
            return QString("Compute ambient Obscurance. <br />"
						   "Ambient obscurance is a computer graphics technique used to simulate the effect of global ambient light in a 3D scene, making the mesh appear more realistic. <br />"
						   "This filter requires two values: <br />"
						   "<ul>"
						   "	<li> the number of rays(defined by the user), which will be shot from the barycenter of each face in order to compute how many time it is visible from these directions;"
						   "	<li> the tau value which represent the T spatial decay; </li>"
						   "</ul>"
						   "The resulting values for the obscurance are saved into face quality and mapped on the mesh into a gray shade. <br />"
						   "<b>For further details see the reference paper: Iones Krupkin Sbert Zhukov Fast, Realistic Lighting for Video Games IEEECG&A 2003 </b> <br />"
						   "This filter uses Embree3 library by INTEL.");
							

		case FP_AMBIENT_OCCLUSION:
			return QString("Compute Ambient Occlusion<br />"
						   "This filter is a shading technique used in computer graphics to simulate the way light interacts with surfaces in a realistic manner. <br/>"
						   "The parameter for the number of rays  is defined by the user; this parameter represent the number of rays which will be shot from the barycenter of each face. <br/>"
						   "The higher the number of rays the longer the time to compute but the better the results.<br />"
						   "This results are saved into face quality and mapped into a gray shade on the mesh. <br />"
						   "This filter uses Embree3 library by INTEL.");
						   

		case FP_SDF:
			return QString("Compute Shape-Diameter Function <br />"
						   "The SDF defines the distance between a point in 3D space and the nearest point on the object's surface."
						   "This filter can be used to find out the thickness of the mesh <br />"
						   "Given a face, a set of rays are shoot inward and an average of the the distance to hit a face is saved in the face quality. The face quality than is mapped into a color ramp. <br /> "
						   "This filter requires two values: <br />"
						   "<ul>"
						   "	<li> the number of rays which will be shot from the barycenter of each face </li>"
						   "	<li> the cone amplitude (in degrees) of the cone which we value as valid for the shooting angle </li>"
						   "</ul>"
						   " <br />"
						   "<b>For further details see the reference paper: Shapira Shamir Cohen-Or, Consistent Mesh Partitioning and Skeletonisation using the shaper diameter function, Visual Comput. J. (2008) </b> <br />"
						   "This filter uses Embree3 library by INTEL.");
							

		case FP_SELECT_VISIBLE_FACES:
            return QString("Select visible face <br />"
						   "This filter shows all the visible faces from a given direction changing the faces color in white if they are visible from the direction, black otherwise"				
						   "This filter uses Embree3 library by INTEL.");
		
		case FP_ANALYZE_NORMALS:
			return QString("Reorient inward pointing faces <br />"
						   "Given the input mesh, this filter using raytracing finds out if any faces are pointing inward and correct their orientation <br />"
						   "The number of rays is defined by ther user, the higher the number the higher the precision but at the cost of the computation time. <br />"
						   "<b> For further details see the reference paper: Kenshi Takayama, Alec Jacobson, Ladislav Kavan, Olga Sorkine-Hornung. </b><br />"
						   "<b> A Simple Method for Correcting Facet Orientations in Polygon Meshes Based on Ray Casting. Journal of Computer Graphics Techniques 3(4), 2014 </b> <br />"
						   "This filter uses Embree3 library by INTEL.");
		default :
			assert(0);
			return QString("Unknown Filter");
	}
}

 /**
 * @brief The FilterClass describes in which generic class of filters it fits.
 * This choice affect the submenu in which each filter will be placed
 * More than a single class can be chosen.
 * @param a: the action of the filter
 * @return the class od the filter
 */
FilterEmbreePlugin::FilterClass FilterEmbreePlugin::getClass(const QAction *a) const
{
    return FilterPlugin::Quality;
	/*
	switch(ID(a)) {	
	default :
		return FilterPlugin::Generic;
	}
	*/
}

/**
 * @brief FilterSamplePlugin::filterArity
 * @return
 */
FilterEmbreePlugin::FilterArity FilterEmbreePlugin::filterArity(const QAction*) const
{
	return SINGLE_MESH;
}

/**
 * @brief FilterSamplePlugin::getPreConditions
 * @return
 */
int FilterEmbreePlugin::getPreConditions(const QAction*) const
{
	return MeshModel::MM_NONE;
}

/**
 * @brief FilterSamplePlugin::postCondition
 * @return
 */
int FilterEmbreePlugin::postCondition(const QAction*) const
{
	return MeshModel::MM_VERTQUALITY | MeshModel::MM_FACEQUALITY;
}

/**
 * @brief This function define the needed parameters for each filter. Return true if the filter has some parameters
 * it is called every time, so you can set the default value of parameters according to the mesh
 * For each parameter you need to define,
 * - the name of the parameter,
 * - the default value
 * - the string shown in the dialog
 * - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
 * @param action
 * @param m
 * @param parlst
 */
RichParameterList FilterEmbreePlugin::initParameterList(const QAction *action,const MeshModel &m)
{
	RichParameterList parlst;
	switch(ID(action)) {
		case FP_OBSCURANCE :
			parlst.addParam(RichInt("Rays", 64, "Number of rays", "The number of rays shoot from the barycenter of the face. The higher the number the higher the definition of the ambient obscurance but at the cost of the calculation time "));
			parlst.addParam(RichFloat("TAU",0.1f,"Tau value", "The value to control spatial decay, the higher the value, the grater the influence that the distance (where the ray hits another face) has on the result "));
			break;
		case FP_AMBIENT_OCCLUSION:
			parlst.addParam(RichInt("Rays", 64, "Number of rays", "The number of rays shoot from the barycenter of the face. The higher the number the higher the definition of the ambient occlusion but at the cost of the calculation time "));
			break;
		case FP_SDF:
			parlst.addParam(RichInt("Rays", 64, "Number of rays", "The number of rays shoot from the barycenter of the face. The higher the number the higher the definition of the SDF but at the cost of the calculation time"));
            parlst.addParam(RichFloat("cone_amplitude",0.01f,"Cone amplitude ", "The value for the angle (in degrees) of the cone for which we consider a ray shooting direction as a valid direction"));
			
			break;
		case FP_SELECT_VISIBLE_FACES:
			parlst.addParam(RichInt("Rays", 64, "Number of rays", "The number of rays shoot from the barycenter of the face."));
			parlst.addParam(RichPosition("dir", Point3f(1.0f, 0.0f, 0.0f), "Direction", "This values indicates the direction of the shadows"));
			break;
		case FP_ANALYZE_NORMALS:
			parlst.addParam(RichInt("Rays", 64, "Number of rays", "The number of rays shoot from the barycenter of the face. The higher the number the higher the definition of the normal analysis but at the cost of the calculation time"));
			break;
		default :
			break;
	}
	return parlst;
}

/**
 * @brief The Real Core Function doing the actual mesh processing.
 * @param action
 * @param md: an object containing all the meshes and rasters of MeshLab
 * @param par: the set of parameters of each filter
 * @param cb: callback object to tell MeshLab the percentage of execution of the filter
 * @return true if the filter has been applied correctly, false otherwise
 */
std::map<std::string, QVariant> FilterEmbreePlugin::applyFilter(const QAction * action, const RichParameterList & parameters, MeshDocument &md, unsigned int& /*postConditionMask*/, vcg::CallBackPos *cb)
{
	
	MeshModel *m = md.mm();
    EmbreeAdaptor<CMeshO> adaptor = EmbreeAdaptor<CMeshO>(m->cm);
	
	switch(ID(action)) {
	case FP_OBSCURANCE:
        m->updateDataMask(MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY | MeshModel::MM_FACEQUALITY | MeshModel::MM_FACECOLOR);
        adaptor.computeObscurance(m->cm, parameters.getInt("Rays"), parameters.getFloat("TAU"));
		tri::UpdateQuality<CMeshO>::VertexFromFace(m->cm);
  		tri::UpdateColor<CMeshO>::PerVertexQualityGray(m->cm); 
		break;
    case FP_AMBIENT_OCCLUSION:
        m->updateDataMask(MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY | MeshModel::MM_FACEQUALITY | MeshModel::MM_FACECOLOR);
        adaptor.computeAmbientOcclusion(m->cm,parameters.getInt("Rays"));
		tri::UpdateQuality<CMeshO>::VertexFromFace(m->cm);
  		tri::UpdateColor<CMeshO>::PerVertexQualityGray(m->cm); 
		break;
	case FP_SDF:
        m->updateDataMask(MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY | MeshModel::MM_FACEQUALITY | MeshModel::MM_FACECOLOR);
        adaptor.computeSDF(m->cm,parameters.getInt("Rays"),parameters.getFloat("cone_amplitude"));
		tri::UpdateQuality<CMeshO>::VertexFromFace(m->cm);
        tri::UpdateColor<CMeshO>::PerVertexQualityRamp(m->cm);
		break;
	case FP_SELECT_VISIBLE_FACES:
        m->updateDataMask(MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY | MeshModel::MM_FACEQUALITY | MeshModel::MM_FACECOLOR);
        adaptor.selectVisibleFaces(m->cm,parameters.getPoint3m("dir"));
		tri::UpdateQuality<CMeshO>::VertexFromFace(m->cm);
  		tri::UpdateColor<CMeshO>::PerVertexQualityGray(m->cm); 
		break;	
	case FP_ANALYZE_NORMALS:
		adaptor.computeNormalAnalysis(m->cm,parameters.getInt("Rays"));
		tri::UpdateNormal<CMeshO>::PerVertexNormalized(m->cm);
		break;
	default :
		wrongActionCalled(action);
	}
	
	return std::map<std::string, QVariant>();
	
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterEmbreePlugin)
