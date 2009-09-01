/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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

#include "align_parameter.h"

using namespace vcg;

AlignParameter::AlignParameter(){}

// given a RichParameterSet get back the alignment parameter  (dual of the buildParemeterSet)
void AlignParameter::buildAlignParameters(RichParameterSet &fps , AlignPair::Param &app)
{
	app.SampleNum=fps.getInt("SampleNum");
  app.MinDistAbs=fps.getFloat("MinDistAbs");
	app.TrgDistAbs=fps.getFloat("TrgDistAbs");
	app.MaxIterNum=fps.getInt("MaxIterNum");
	app.SampleMode= fps.getBool("SampleMode")?AlignPair::Param::SMNormalEqualized  : AlignPair::Param::SMRandom;
	app.ReduceFactor=fps.getFloat("ReduceFactor");
	app.MatchMode=fps.getBool("MatchMode")? AlignPair::Param::MMRigid : AlignPair::Param::MMClassic;
}

// given an alignment parameter builds the corresponding RichParameterSet (dual of the retrieveParemeterSet)
void AlignParameter::buildRichParameterSet(AlignPair::Param &app, RichParameterSet &fps)
{
	fps.clear();
	fps.addParam(new RichInt("SampleNum",app.SampleNum,"Sample Number","Number of samples that we try to choose at each ICP iteration"));
	fps.addParam(new RichFloat("MinDistAbs",app.MinDistAbs,"Minimal Starting Distance","For all the chosen sample on one mesh we consider for ICP only the samples nearer than this value."
							                             "If MSD is too large outliers could be included, if it is too small convergence will be very slow. "
							                             "A good guess is needed here, suggested values are in the range of 10-100 times of the device scanning error."
							                             "This value is also dynamically changed by the 'Reduce Distance Factor'"));
	fps.addParam(new RichFloat("TrgDistAbs",app.TrgDistAbs,"Target Distance","When 50% of the chosen samples are below this distance we consider the two mesh aligned. Usually it should be a value lower than the error of the scanning device. "));
	fps.addParam(new RichInt("MaxIterNum",app.MaxIterNum,"Max Iteration Num","The maximum number of iteration that the ICP is allowed to perform."));
	fps.addParam(new RichBool("SampleMode",app.SampleMode == AlignPair::Param::SMNormalEqualized,"Normal Equalized Sampling","if true (default) the sample points of icp are choosen with a  distribution uniform with respect to the normals of the surface. Otherwise they are distributed in a spatially uniform way."));
	fps.addParam(new RichFloat("ReduceFactor",app.ReduceFactor,"MSD Reduce Factor","At each ICP iteration the Minimal Starting Distance is reduced to be 5 times the <Reduce Factor> percentile of the sample distances (e.g. if RF is 0.9 the new Minimal Starting Distance is 5 times the value <X> such that 90% of the sample lies at a distance lower than <X>."));
	fps.addParam(new RichBool("MatchMode",app.MatchMode == AlignPair::Param::MMRigid,"Rigid matching","If true the ICP is cosntrained to perform matching only throug roto-translations (no scaling allowed). If false a more relaxed transformation matrix is allowed (scaling and shearing can appear)."));
}
