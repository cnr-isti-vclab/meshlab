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

#include "filter_fractal.h"

using namespace std;
using namespace vcg;

// ------- MeshFilterInterface implementation ------------------------
FilterFractal::FilterFractal()
{
	typeList << CR_FRACTAL_TERRAIN << FP_FRACTAL_MESH << FP_CRATERS;
	ActionIDType tt;
	foreach(tt , types())
		actionList << new QAction(filterName(tt), this);
}

QString FilterFractal::pluginName() const
{
	return "FilterFractal";
}

QString FilterFractal::filterName(ActionIDType filterId) const
{
	switch (filterId)
	{
	case CR_FRACTAL_TERRAIN:
		return QString("Fractal Terrain");
		break;
	case FP_FRACTAL_MESH:
		return QString("Fractal Displacement");
		break;
	case FP_CRATERS:
		return QString("Craters Generation");
		break;
	default:
		assert(0); return QString("error");
		break;
	}
}

QString FilterFractal::filterInfo(ActionIDType filterId) const
{
	QString filename, description;
	switch (filterId) {
	case CR_FRACTAL_TERRAIN:
	case FP_FRACTAL_MESH:
		description =
				"Generates a fractal terrain perturbation with five different algorithms.<br />"
                "Some good parameter values to start with are:<br />"
                "<table align=\"center\">"
                "    <tr style=\"border:1px solid black\">"
                "        <td> - </td>"
                "        <td align=\"center\"> Seed </td>"
                "        <td align=\"center\"> Octaves </td>"
                "        <td align=\"center\"> Lacunarity </td>"
                "        <td align=\"center\"> Fractal increment </td>"
                "        <td align=\"center\"> Offset </td>"
                "        <td align=\"center\"> Gain </td>"
                "    </tr>"
                "    <tr>"
                "        <td>fBM</td>"
                "        <td align=\"center\">1</td>"
                "        <td align=\"center\">10</td>"
                "        <td align=\"center\">2</td>"
                "        <td align=\"center\">1.2</td>"
                "        <td align=\"center\">-</td>"
                "        <td align=\"center\">-</td>"
                "    </tr>"
                "    <tr>"
                "        <td>Standard multifractal</td>"
                "        <td align=\"center\">1</td>"
                "        <td align=\"center\">8</td>"
                "        <td align=\"center\">2</td>"
                "        <td align=\"center\">0.9</td>"
                "        <td align=\"center\">0.9</td>"
                "        <td align=\"center\">-</td>"
                "    </tr>"
                "    <tr>"
                "        <td>Heterogeneous multifractal</td>"
                "        <td align=\"center\">1</td>"
                "        <td align=\"center\">8</td>"
                "        <td align=\"center\">3</td>"
                "        <td align=\"center\">0.9</td>"
                "        <td align=\"center\">0.4</td>"
                "        <td align=\"center\">-</td>"
                "    </tr>"
                "    <tr>"
                "        <td>Hybrid multifractal</td>"
                "        <td align=\"center\">1</td>"
                "        <td align=\"center\">8</td>"
                "        <td align=\"center\">4</td>"
                "        <td align=\"center\">0.1</td>"
                "        <td align=\"center\">0.3</td>"
                "        <td align=\"center\">-</td>"
                "    </tr>"
                "    <tr>"
                "        <td>Ridged multifractal</td>"
                "        <td align=\"center\">2</td>"
                "        <td align=\"center\">8</td>"
                "        <td align=\"center\">4</td>"
                "        <td align=\"center\">0.5</td>"
                "        <td align=\"center\">0.9</td>"
                "        <td align=\"center\">2</td>"
                "    </tr>"
                "</table>"
                "<br /><br />"
                "Detailed algorithms descriptions can be found in:<br />"
                "<i>Ebert, D.S., Musgrave, F.K., Peachey, D., Perlin, K., and Worley, S.</i><br />"
                "<b>Texturing and Modeling: A Procedural Approach</b><br />"
                "Morgan Kaufmann Publishers Inc., San Francisco, CA, USA, 2002.<br>";
		break;
	case FP_CRATERS:
		description =
				"Generates craters onto a mesh using radial functions.<br />"
                "There must be at least two layers to apply this filter:<br />"
                "<ul>"
                "    <li>the layer that contains the target mesh; we assume that this mesh is sufficiently refined;</li>"
                "    <li>the layer that contains the samples which represent the central points of craters.</li>"
                "</ul>"
                "There are three radial functions available to generate craters, two of which are Gaussian and Multiquadric, "
                "and the third is a variant of multiquadric. Blending functions are also provided to blend "
                "the crater elevation towards the mesh surface. "
                "If you want the preview to work, be sure to select the target mesh layer before launching the "
                "filter. You can select this layer by clicking on it in the layer dialog.";
		break;
	default:
		assert(0); return QString("error");
		break;
	}

	//    QFile f(filename);
	//    if(f.open(QFile::ReadOnly))
	//    {
	//        QTextStream stream(&f);
	//        description = stream.readAll();
	//        f.close();
	//    }

	if(filterId == FP_FRACTAL_MESH)
	{
		description += "<br /><br />Hint: search a good compromise between offset and height factor parameter.";
	}

	return description;
}

void FilterFractal::initParameterList(const QAction* filter,MeshDocument &md, RichParameterList &par)
{
	switch(ID(filter))
	{
	case CR_FRACTAL_TERRAIN:
	case FP_FRACTAL_MESH:
		initParameterSetForFractalDisplacement(filter, md, par);
		break;
	case FP_CRATERS:
		initParameterSetForCratersGeneration(md, par);
		break;
	}
}

void FilterFractal::initParameterSetForFractalDisplacement(const QAction *filter, MeshDocument &md, RichParameterList &par)
{
	bool terrain_filter = (ID(filter) == CR_FRACTAL_TERRAIN);

	if(terrain_filter) {
		par.addParam(RichInt("steps", 8, "Subdivision steps:", "Defines the detail of the generated terrain. Allowed values are in range [2,9]. Use values from 6 to 9 to obtain reasonable results."));
		par.addParam(RichDynamicFloat("maxHeight", 0.2f, 0.0f, 1.0f, "Max height:", "Defines the maximum perturbation height as a fraction of the terrain's side."));
	} else {
		float diag = md.mm()->cm.bbox.Diag();
		par.addParam(RichAbsPerc("maxHeight", 0.02 * diag, 0, 0.5*diag, "Max height:", "Defines the maximum height for the perturbation."));
	}

	par.addParam(RichDynamicFloat("scale", 1.0f, 0.0f, 10.0f, "Scale factor:", "Scales the fractal perturbation in and out. Values larger than 1 mean zoom out; values smaller than one mean zoom in."));
	if (!terrain_filter)
	{
		par.addParam(RichInt("smoothingSteps", 5, "Normals smoothing steps:", "Face normals will be smoothed to make the perturbation more homogeneous. This parameter represents the number of smoothing steps." ));
	}
	par.addParam(RichFloat("seed", 2.0f, "Seed:", "By varying this seed, the terrain morphology will change.\nDon't change the seed if you want to refine the current terrain morphology by changing the other parameters."));

	QStringList algList;
	algList << "fBM (fractal Brownian Motion)" << "Standard multifractal" << "Heterogeneous multifractal" << "Hybrid multifractal terrain" << "Ridged multifractal terrain";
	par.addParam(RichEnum("algorithm", 4, algList, "Algorithm", "The algorithm with which the fractal terrain will be generated."));
	par.addParam(RichDynamicFloat("octaves", 8.0f, 1.0f, 20.0f, "Octaves:", "The number of Perlin noise frequencies that will be used to generate the terrain. Reasonable values are in range [2,9]."));
	par.addParam(RichFloat("lacunarity", 4.0, "Lacunarity:", "The gap between noise frequencies. This parameter is used in conjunction with fractal increment to compute the spectral weights that contribute to the noise in each octave."));
	par.addParam(RichFloat("fractalIncrement", terrain_filter? 0.5f : 0.2f, "Fractal increment:", "This parameter defines how rough the generated terrain will be. The range of reasonable values changes according to the used algorithm, however you can choose it in range [0.2, 1.5]."));
	par.addParam(RichFloat("offset", 0.9f, "Offset:", "This parameter controls the multifractality of the generated terrain. If offset is low, then the terrain will be smooth."));
	par.addParam(RichFloat("gain", 2.5f, "Gain:", "Ignored in all the algorithms except the ridged one. This parameter defines how hard the terrain will be."));
	par.addParam(RichBool("saveAsQuality", false, "Save as vertex quality", "Saves the perturbation value as vertex quality."));
}

void FilterFractal::initParameterSetForCratersGeneration(MeshDocument &md, RichParameterList &par)
{
	int meshCount = md.meshList.size();

	// tries to detect the target mesh
	MeshModel* target = md.mm();
	MeshModel* samples = md.mm();
	MeshModel* tmpMesh;
	if (samples->cm.fn != 0){ // this is probably not the samples layer
		for(int i=0; i<meshCount; i++)
		{
			tmpMesh = md.meshList.at(i);
			if (tmpMesh->cm.fn == 0)
			{
				samples = tmpMesh;
				break;
			}
		}
	}

	par.addParam(RichMesh("target_mesh", target, &md, "Target mesh:", "The mesh on which craters will be generated."));
	par.addParam(RichMesh("samples_mesh", samples, &md, "Samples layer:", "The samples that represent the central points of craters."));
	par.addParam(RichInt("seed", 0, "Seed:", "The seed with which the random number generator is initialized. The random generator generates radius and depth for each crater into the given range."));
	par.addParam(RichInt("smoothingSteps", 5, "Normals smoothing steps:", "Vertex normals are smoothed this number of times before generating craters."));

	QStringList algList;
	algList << "f1 (Gaussian)" << "f2 (Multiquadric)" << "f3";
	par.addParam(RichEnum("rbf", 1, algList, "Radial function:", "The radial function used to generate craters."));

	par.addParam(RichDynamicFloat("min_radius", 0.1f, 0.0f, 1.0f, "Min crater radius:", "Defines the minimum radius of craters in range [0, 1]. Values near 0 mean very small craters."));
	par.addParam(RichDynamicFloat("max_radius", 0.35f, 0.0f, 1.0f, "Max crater radius:", "Defines the maximum radius of craters in range [0, 1]. Values near 1 mean very large craters."));
	par.addParam(RichDynamicFloat("min_depth", 0.05f, 0.0f, 1.0f, "Min crater depth:", "Defines the minimum depth of craters in range [0, 1]."));
	par.addParam(RichDynamicFloat("max_depth", 0.15f, 0.0f, 1.0f, "Max crater depth:", "Defines the maximum depth of craters in range [0, 1]. Values near 1 mean very deep craters."));
	par.addParam(RichDynamicFloat("elevation", 0.4f, 0.0f, 1.0f, "Elevation:", "Defines how much the crater rise itself from the mesh surface, giving an \"impact-effect\"."));

	QStringList blendList;
	blendList << "Exponential blending" << "Linear blending" << "Gaussian blending" << "f3 blending";
	par.addParam(RichEnum("blend", 3, blendList, "Blending algorithm:", "The algorithm that is used to blend the perturbation towards the mesh surface."));
	par.addParam(RichDynamicFloat("blendThreshold", 0.8f, 0.0f, 1.0f, "Blending threshold:", "The fraction of craters radius beyond which the radial function is replaced with the blending function."));
	par.addParam(RichBool("successiveImpacts", true, "Successive impacts", "If not checked, the impact-effects of generated craters will be superimposed with each other."));
	par.addParam(RichBool("ppNoise", true, "Postprocessing noise", "Slightly perturbates the craters with a noise function."));
	par.addParam(RichBool("invert", false, "Invert perturbation", "If checked, inverts the sign of radial perturbation to create bumps instead of craters."));
	par.addParam(RichBool("save_as_quality", false, "Save as vertex quality", "Saves the perturbation as vertex quality."));
	return;
}

std::map<std::string, QVariant> FilterFractal::applyFilter(
		const QAction* filter,
		const RichParameterList &par,
		MeshDocument &md,
		unsigned int& /*postConditionMask*/,
		vcg::CallBackPos* cb)
{
	if(this->getClass(filter) == FilterPlugin::MeshCreation)
		md.addNewMesh("",this->filterName(ID(filter)));
	switch(ID(filter))
	{
	case CR_FRACTAL_TERRAIN:
	case FP_FRACTAL_MESH:
	{
		MeshModel* mm = md.mm();
		Scalarm maxHeight = .0;
		int smoothingSteps = 0;

		if(ID(filter) == CR_FRACTAL_TERRAIN)
		{
			int steps = par.getInt("steps");
			steps = ((steps<2)? 2: steps);
			Scalarm gridSide = .0;
			FractalUtils<CMeshO>::GenerateGrid(mm->cm, steps, gridSide);
			maxHeight = par.getDynamicFloat("maxHeight") * gridSide;
		} else {
			maxHeight = par.getAbsPerc("maxHeight");
			smoothingSteps = par.getInt("smoothingSteps");
		}

		FractalUtils<CMeshO>::FractalArgs args
				(mm, par.getEnum("algorithm"),par.getFloat("seed"),
				 par.getFloat("octaves"), par.getFloat("lacunarity"),
				 par.getFloat("fractalIncrement"), par.getFloat("offset"), par.getFloat("gain"),
				 maxHeight, par.getDynamicFloat("scale"), smoothingSteps, par.getBool("saveAsQuality"));

		if(args.saveAsQuality)
			mm->updateDataMask(MeshModel::MM_VERTQUALITY);
		bool ret = FractalUtils<CMeshO>::ComputeFractalPerturbation(mm->cm, args, cb);
		if (!ret)
			throw MLException("Error while running " + filter->text());
	}
		break;
	case FP_CRATERS:
	{
		if (md.meshList.size() < 2) {
			throw MLException("There must be at least two layers to apply the craters generation filter.");
		}

		CMeshO* samples = &(par.getMesh("samples_mesh")->cm);
		if (samples->face.size() > 0) {
			throw MLException("The sample layer selected should be a points cloud.");
		}
		CMeshO* target = &(par.getMesh("target_mesh")->cm);
		if (samples == target) {
			throw MLException("The sample layer and the target layer must be different.");
		}

		Scalarm minRadius = par.getDynamicFloat("min_radius");
		Scalarm maxRadius = par.getDynamicFloat("max_radius");
		if (maxRadius <= minRadius)  {
			throw MLException("Min radius is greater than max radius.");
		}

		Scalarm minDepth = par.getDynamicFloat("min_depth");
		Scalarm maxDepth = par.getDynamicFloat("max_depth");
		if (maxDepth <= minDepth) {
			throw MLException("Min depth is greater than max depth.");
		}

		// reads parameters
		CratersUtils<CMeshO>::CratersArgs args(par.getMesh("target_mesh"), par.getMesh("samples_mesh"), par.getEnum("rbf"),
											   par.getInt("seed"), minRadius, maxRadius, minDepth, maxDepth,
											   par.getInt("smoothingSteps"), par.getBool("save_as_quality"), par.getBool("invert"),
											   par.getBool("ppNoise"), par.getBool("successiveImpacts"),
											   par.getDynamicFloat("elevation"), par.getEnum("blend"),
											   par.getDynamicFloat("blendThreshold"));

		bool ret = CratersUtils<CMeshO>::GenerateCraters(args, cb);
		if (!ret)
			throw MLException("Error while running " + filter->text());
	}
		break;
	default:
		wrongActionCalled(filter);
	}
	return std::map<std::string, QVariant>();
}

FilterPlugin::FilterClass FilterFractal::getClass(const QAction* filter) const
{
	switch(ID(filter)) {
	case CR_FRACTAL_TERRAIN:
		return FilterPlugin::MeshCreation;
		break;
	case FP_FRACTAL_MESH:
	case FP_CRATERS:
		return FilterPlugin::Smoothing;
		break;
	default: assert(0);
		return FilterPlugin::Generic;
	}
}

int FilterFractal::getRequirements(const QAction *filter)
{
	switch(ID(filter)) {
	case CR_FRACTAL_TERRAIN:
		return MeshModel::MM_NONE;
		break;
	case FP_FRACTAL_MESH:
	case FP_CRATERS:
		return MeshModel::MM_FACEFACETOPO;
		break;
	default: assert(0);
	}
	return MeshModel::MM_NONE;
}

int FilterFractal::postCondition(const QAction *filter) const
{
	switch(ID(filter))
	{
	case CR_FRACTAL_TERRAIN:
	case FP_FRACTAL_MESH:
	case FP_CRATERS:
		return MeshModel::MM_ALL;
		break;
	default: assert(0);
	}
	return MeshModel::MM_ALL;
}

FilterPlugin::FilterArity FilterFractal::filterArity(const QAction* act ) const
{
	switch(ID(act))
	{
	case FP_FRACTAL_MESH:
		return FilterPlugin::SINGLE_MESH;
	case CR_FRACTAL_TERRAIN:
		return FilterPlugin::NONE;
	case FP_CRATERS:
		return FilterPlugin::VARIABLE;
	}
	return FilterPlugin::NONE;
}

// ----------------------------------------------------------------------
MESHLAB_PLUGIN_NAME_EXPORTER(FilterFractal)

