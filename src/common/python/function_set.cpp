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
#include "function_set.h"

#include <QRegularExpression>
#include "../mlexception.h"
#include <algorithm>
#include "python_utils.h"
#include <vcg/complex/algorithms/create/platonic.h>

pymeshlab::FunctionSet::FunctionSet()
{
}

pymeshlab::FunctionSet::FunctionSet(const PluginManager& pm)
{
	//dummy MeshDocument use to compute default value parameters
	//the mesh used is a 1x1x1 cube (with extremes [-0.5; 0.5])
	MeshDocument dummyMeshDocument;
	Box3m b(Point3m(-0.5,-0.5,-0.5),Point3m(0.5,0.5,0.5));
	CMeshO dummyMesh;
	vcg::tri::Box<CMeshO>(dummyMesh,b);
	dummyMeshDocument.addNewMesh(dummyMesh, "cube");
	int mask = 0;
	mask |= vcg::tri::io::Mask::IOM_VERTQUALITY;
	mask |= vcg::tri::io::Mask::IOM_FACEQUALITY;
	dummyMeshDocument.mm()->Enable(mask);

	for (auto inputFormat : pm.inputMeshFormatList()){
		QString originalFilterName = inputFormat;
		QString pythonFilterName = inputFormat.toLower();
		Function f(pythonFilterName, originalFilterName, "Load " + inputFormat + " format.");
		IOPlugin* plugin = pm.inputMeshPlugin(inputFormat);
		RichParameterList rps;
		plugin->initPreOpenParameter(inputFormat, rps);
		plugin->initOpenParameter(inputFormat, *dummyMeshDocument.mm(), rps);

		//filename parameter
		QString sv = "file_name." + inputFormat;
		QStringList sl(inputFormat);
		RichOpenFile of("file_name", sv, sl, "File Name", "The name of the file to load");
		FunctionParameter par(of);
		f.addParameter(par);

		for (const RichParameter& rp : rps){
			FunctionParameter par(rp);
			f.addParameter(par);
		}
		loadMeshSet.insert(f);
	}

	for (auto outputFormat : pm.outputMeshFormatList()){
		QString originalFilterName = outputFormat;
		QString pythonFilterName = outputFormat.toLower();
		Function f(pythonFilterName, originalFilterName, "Save " + outputFormat + " format.");
		IOPlugin* plugin = pm.outputMeshPlugin(outputFormat);
		RichParameterList rps;
		plugin->initSaveParameter(outputFormat, *dummyMeshDocument.mm(), rps);

		//filename parameter
		QString sv = "file_name." + outputFormat;
		RichSaveFile of("file_name", sv, outputFormat, "File Name", "The name of the file to save");
		FunctionParameter par(of);
		f.addParameter(par);

		for (const RichParameter& rp : rps){
			FunctionParameter par(rp);
			f.addParameter(par);
		}

		//data to save
		updateSaveParameters(plugin, outputFormat, f);

		saveMeshSet.insert(f);
	}

	for (auto inputRasterFormat : pm.inputRasterFormatList()){
		QString originalFilterName = inputRasterFormat;
		QString pythonFilterName = inputRasterFormat.toLower();
		Function f(pythonFilterName, originalFilterName, "Load " + inputRasterFormat + " format.");

		//filename parameter
		QString sv = "file_name." + inputRasterFormat;
		QStringList sl(inputRasterFormat);
		RichOpenFile of("file_name", sv, sl, "File Name", "The name of the file to load");
		FunctionParameter par(of);
		f.addParameter(par);

		loadRasterSet.insert(f);
	}

	for (FilterPlugin* fp : pm.filterPluginIterator()){
		for (QAction* act : fp->actions()) {
			QString originalFilterName = fp->filterName(act);
			QString description = fp->filterInfo(act);
			QString pythonFilterName = fp->pythonFilterName(act);
			Function f(pythonFilterName, originalFilterName, description);

			RichParameterList rps;
			fp->initParameterList(act, dummyMeshDocument, rps);

			for (const RichParameter& rp : rps){
				FunctionParameter par(rp);
				f.addParameter(par);
			}
			filterSet.insert(f);
		}
	}
}

std::list<std::string> pymeshlab::FunctionSet::pythonFilterFunctionNames() const
{
	std::list<std::string> fnames;
	for (const Function& f: filterSet){
		fnames.push_back(f.pythonFunctionName().toStdString());
	}
	return fnames;
}

const pymeshlab::Function& pymeshlab::FunctionSet::filterFunction(const QString& pythonFunctionName) const
{
	auto it = filterSet.find(Function(pythonFunctionName, "", ""));
	if (it == filterSet.end())
		throw MLException(pythonFunctionName + " filter not found.");
	return *it;
}

bool pymeshlab::FunctionSet::containsFilterFunction(const QString& pythonFunctionName) const
{
	return filterSet.find(Function(pythonFunctionName, "", "")) != filterSet.end();
}

const pymeshlab::Function& pymeshlab::FunctionSet::loadMeshFunction(const QString& pythonFunctionName) const
{
	auto it = loadMeshSet.find(Function(pythonFunctionName, "", ""));
	if (it == loadMeshSet.end())
		throw MLException(pythonFunctionName + " format for loading mesh not found.");
	return *it;
}

bool pymeshlab::FunctionSet::containsLoadMeshFunction(const QString& pythonFunctionName) const
{
	return loadMeshSet.find(Function(pythonFunctionName, "", "")) != loadMeshSet.end();
}

const pymeshlab::Function& pymeshlab::FunctionSet::saveMeshFunction(const QString& pythonFunctionName) const
{
	auto it = saveMeshSet.find(Function(pythonFunctionName, "", ""));
	if (it == saveMeshSet.end())
		throw MLException(pythonFunctionName + " format for saving mesh not found.");
	return *it;
}

bool pymeshlab::FunctionSet::containsSaveMeshFunction(const QString& pythonFunctionName) const
{
	return saveMeshSet.find(Function(pythonFunctionName, "", "")) != saveMeshSet.end();
}

const pymeshlab::Function& pymeshlab::FunctionSet::loadRasterFunction(const QString& pythonFunctionName) const
{
	auto it = loadRasterSet.find(Function(pythonFunctionName, "", ""));
	if (it == loadRasterSet.end())
		throw MLException(pythonFunctionName + " format for loading raster not found.");
	return *it;
}

bool pymeshlab::FunctionSet::containsLoadRasterFunction(const QString& pythonFunctionName) const
{
	return loadRasterSet.find(Function(pythonFunctionName, "", "")) != loadRasterSet.end();
}

pymeshlab::FunctionSet::FunctionRangeIterator pymeshlab::FunctionSet::filterFunctionIterator() const
{
	return FunctionRangeIterator(filterSet);
}

pymeshlab::FunctionSet::FunctionRangeIterator pymeshlab::FunctionSet::loadMeshFunctionIterator() const
{
	return FunctionRangeIterator(loadMeshSet);
}

pymeshlab::FunctionSet::FunctionRangeIterator pymeshlab::FunctionSet::saveMeshFunctionIterator() const
{
	return FunctionRangeIterator(saveMeshSet);
}

pymeshlab::FunctionSet::FunctionRangeIterator pymeshlab::FunctionSet::loadRasterFunctionIterator() const
{
	return FunctionRangeIterator(loadRasterSet);
}

void pymeshlab::FunctionSet::updateSaveParameters(IOPlugin* plugin,
		const QString& outputFormat,
		pymeshlab::Function& f)
{
	int capabilityBits, defaultBits;
	plugin->exportMaskCapability(outputFormat, capabilityBits, defaultBits);

	for (unsigned int i = 0; i < capabilitiesBits.size(); ++i){
		if (capabilityBits & capabilitiesBits[i]){
			bool def = defaultBits & capabilitiesBits[i];
			RichBool rb(
						saveCapabilitiesStrings[i], def,
						saveCapabilitiesStrings[i], saveCapabilitiesStrings[i]);
			FunctionParameter par(rb);
			f.addParameter(par);

		}
	}


}
