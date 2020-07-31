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
void AlignParameter::RichParameterSetToAlignPairParam(const RichParameterList &rps , AlignPair::Param &app)
{
  app.SampleNum       =rps.getInt(  "SampleNum");
  app.MinDistAbs      =rps.getFloat("MinDistAbs");
  app.TrgDistAbs      =rps.getFloat("TrgDistAbs");
  app.MaxIterNum      =rps.getInt(  "MaxIterNum");
  app.SampleMode      =rps.getBool( "SampleMode")?AlignPair::Param::SMNormalEqualized  : AlignPair::Param::SMRandom;
  app.ReduceFactorPerc=rps.getFloat("ReduceFactorPerc");
  app.PassHiFilter    =rps.getFloat("PassHiFilter");
  app.MatchMode       =rps.getBool( "MatchMode")? AlignPair::Param::MMRigid : AlignPair::Param::MMSimilarity;
}

// given an alignment parameter builds the corresponding RichParameterSet (dual of the retrieveParemeterSet)
void AlignParameter::AlignPairParamToRichParameterSet(const AlignPair::Param &app, RichParameterList &rps)
{
  rps.clear();
  rps.addParam(new RichInt("SampleNum",app.SampleNum,"Sample Number","Number of samples that we try to choose at each ICP iteration"));
  rps.addParam(new RichFloat("MinDistAbs",app.MinDistAbs,"Minimal Starting Distance","For all the chosen sample on one mesh we consider for ICP only the samples nearer than this value."
                             "If MSD is too large outliers could be included, if it is too small convergence will be very slow. "
                             "A good guess is needed here, suggested values are in the range of 10-100 times of the device scanning error."
                             "This value is also dynamically changed by the 'Reduce Distance Factor'"));
  rps.addParam(new RichFloat("TrgDistAbs",app.TrgDistAbs,"Target Distance","When 50% of the chosen samples are below this distance we consider the two mesh aligned. Usually it should be a value lower than the error of the scanning device. "));
  rps.addParam(new RichInt("MaxIterNum",app.MaxIterNum,"Max Iteration Num","The maximum number of iteration that the ICP is allowed to perform."));
  rps.addParam(new RichBool("SampleMode",app.SampleMode == AlignPair::Param::SMNormalEqualized,"Normal Equalized Sampling","if true (default) the sample points of icp are chosen with a distribution uniform with respect to the normals of the surface. Otherwise they are distributed in a spatially uniform way."));
  rps.addParam(new RichFloat("ReduceFactorPerc",app.ReduceFactorPerc,"MSD Reduce Factor","At each ICP iteration the Minimal Starting Distance is reduced to be 5 times the <Reduce Factor> percentile of the sample distances (e.g. if RF is 0.9 the new Minimal Starting Distance is 5 times the value <X> such that 90% of the sample lies at a distance lower than <X>."));
  rps.addParam(new RichFloat("PassHiFilter",app.PassHiFilter,"Sample Cut High","At each ICP iteration all the sample that are farther than the <cuth high> percentile are discarded ( In practice we use only the <cut high> best results )."));
  rps.addParam(new RichBool("MatchMode",app.MatchMode == AlignPair::Param::MMRigid,"Rigid matching","If true the ICP is cosntrained to perform matching only through roto-translations (no scaling allowed). If false a more relaxed transformation matrix is allowed (scaling and shearing can appear)."));
}

void AlignParameter::RichParameterSetToMeshTreeParam(const RichParameterList &fps , MeshTree::Param &mtp)
{
  mtp.arcThreshold=fps.getFloat("arcThreshold");
  mtp.OGSize = fps.getInt("OGSize");
  mtp.recalcThreshold = fps.getFloat("recalcThreshold");
}

void AlignParameter::MeshTreeParamToRichParameterSet(const MeshTree::Param &mtp, RichParameterList &rps)
{
  rps.clear();
  rps.addParam(new RichInt("OGSize",mtp.OGSize,"Occupancy Grid Size","To compute the overlap between range maps we discretize them into voxel and count them (both for area and overlap); This parameter affect the resolution of the voxelization process. Using a too fine voxelization can "));
  rps.addParam(new RichFloat("arcThreshold",mtp.arcThreshold,"Arc Area Thr.","We run ICP on every pair of mesh with a relative overlap greater than this threshold. The relative overlap is computed as overlapArea / min(area1,area2)"));
  rps.addParam(new RichFloat("recalcThreshold",mtp.recalcThreshold,"Recalc Fraction","Every time we start process we discard the <recalc> fraction of all the arcs in order to recompute them and hopefully improve the final result. It corresponds to iteratively recalc the bad arcs."));

}

