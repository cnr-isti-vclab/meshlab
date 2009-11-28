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

#include <Qt>
#include <QtGui>
#include "feature_alignment.h"
#include "filter_feature_alignment.h"
#include "feature_msc.h"
#include "feature_rgb.h"
#include <meshlabplugins/edit_pickpoints/pickedPoints.h>

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <cmath>
#include <stdio.h>

using namespace std;
using namespace vcg;

GLLogStream *glLog;
void mylogger(int level, const char * f, ... )
{
    char buf[4096];
    va_list marker;
    va_start( marker, f );

    vsprintf(buf,f,marker);
    va_end( marker );
    glLog->Log(level,buf);
}

FilterFeatureAlignment::FilterFeatureAlignment()
{
    typeList << AF_COMPUTE_FEATURE
             << AF_EXTRACTION
             << AF_CONSENSUS
             << AF_RANSAC             
             << AF_RANSAC_DIAGRAM;

    FilterIDType tt;
    foreach(tt , types())
        actionList << new QAction(filterName(tt), this);
}

FilterFeatureAlignment::~FilterFeatureAlignment()
{
    for (int i = 0; i < actionList.count() ; i++ )
        delete actionList.at(i);
}

 MeshFilterInterface::FilterClass FilterFeatureAlignment::getClass(QAction *a)
{
    switch(ID(a))
    {
        case AF_COMPUTE_FEATURE :
        case AF_EXTRACTION :
        case AF_MATCHING :
        case AF_RIGID_TRANSFORMATION :        
        case AF_CONSENSUS:                            
        case AF_RANSAC_DIAGRAM:        
        case AF_RANSAC : return MeshFilterInterface::RangeMap;
        default: assert(0);
    }
}

bool FilterFeatureAlignment::autoDialog(QAction *a)
{
  switch(ID(a))
  {
    default : return true;
  }
  assert(0);
}

 QString FilterFeatureAlignment::filterName(FilterIDType filter) const
{
    switch(filter)
    {
        case AF_COMPUTE_FEATURE : return "Features computation";
        case AF_EXTRACTION : return "Features extraction";
        case AF_MATCHING : return "Matching";
        case AF_RIGID_TRANSFORMATION : return "Rigid transformation";
        case AF_CONSENSUS : return "Consensus";
        case AF_RANSAC : return "Feature based alignment";
        case AF_RANSAC_DIAGRAM : return "Alignment diagram";       
        default: assert(0);
    }
    return QString("error!");
}

 QString FilterFeatureAlignment::filterInfo(FilterIDType filterId) const
{
    switch(filterId)
    {
        case AF_COMPUTE_FEATURE : return "Computes features for the current mesh. Selected feature is computed per-vertex and feature values are stored in a proper per-vertex attribute of the mesh.";
        case AF_EXTRACTION : return "Extracts the requested number of feature points. Feature points are chosen according to the specified sampling strategy. Optionally, extracted feature points can be stored into PickedPoints attribute to be displayed leter on. NOTICE: this plugin requires that features have been computed previously.";
        case AF_MATCHING : return "Extracts feature points and performs matching. The number of matched bases is returned. A base of points is randomly picked on Move Mesh and matches are efficiently retrieved on Fix Mesh through kNN search and Branch & Bound algorithm. NOTICE: this plugin requires that features have been computed previously.";
        case AF_RIGID_TRANSFORMATION : return "Extracts feature points, performs matching and computes the rigid transformation for all matched bases found. NOTICE: this plugin requires that features have been computed previously.";
        case AF_CONSENSUS : return "Performs consensus procedure and returns the percentage of consensus. In addiction, Move Mesh can be painted accordingly to consensus. NOTICE: this plugin requires that features have been computed previously.";
        case AF_RANSAC : return "Performs the complete alignment process. Automatic alignment procedure is feature based and uses the iterative non deterministic RANSAC algorithm to look for a good coarse alignment. It is very important to provide a precise esteem of overlap percentage between the meshes in order to get a good success probability and avoid too rough results. Pre-semplification of meshes speed up the process. NOTICE: this plugin requires that features have been computed previously.";
        case AF_RANSAC_DIAGRAM : return "Useful to perform tests and to evaluate performances. It repeats the alignment process several times with different settings and outputs stats in the file .\\Diagram.txt. The file is in a format suitable for OpenOffice, to easily create diagrams. NOTICE: this plugin requires that features have been computed previously.";        
        default: assert(0);
    }
    return QString("error!");
}

 int FilterFeatureAlignment::getRequirements(QAction *action)
{
    return 0;
}

void FilterFeatureAlignment::initParameterSet(QAction *a, MeshDocument& md, FilterParameterSet & par)
{
    switch(ID(a))
    {
        case AF_COMPUTE_FEATURE:
        {
            QStringList l;
            l << "GMSmooth curvature"
              << "APSS curvature"
              << "RGB";
            par.addEnum("featureType", 0, l,"Feature type:", "The feature that you want to compute for the current mesh.");
            break;
        }
        case AF_EXTRACTION:
        {
            QStringList l, st;
            l << "GMSmooth curvature"
              << "RGB";
            st << "Uniform" << "Poisson disk";
            par.addEnum("featureType", 0, l,"Feature type:", "The feature that you want to compute for the current mesh.");
            par.addInt("numMovFeatureSelected", 250, "Number of features:", "The number of feature points you want to extract from the current mesh.");
            par.addEnum("samplingStrategy", 0, st,"Sampling strategy:", "The sampling strategy used to select feature points:<br>Uniform -> random selection<br>Poisson disk -> Poisson disk sampling.");
            par.addBool("pickPoints", false, "Store feature points", "Turns on and off storing of feature points into PickedPoints attribute. Check this if you plan of showing feature points. NOTICE: rendering more of two hundred feature points can be very slow.");
            break;
        }
        case AF_MATCHING:
        {
            QStringList l;
            l << "GMSmooth curvature"
              << "RGB feature";
            par.addEnum("featureType", 0, l,"Feature type:", "The feature that you want to compute for the current mesh.");
            par.addMesh("mFix", 0, "Fix mesh:", "The mesh that stays still and grow large after alignment.");
            par.addMesh("mMov", 1, "Move mesh:", "The mesh that moves to fit Fix Mesh.");
            par.addInt("numMovFeatureSelected", 250, "Number of features:", "The number of feature points you want to extract from Move Mesh. The base of feature points that has to be matched is randomly picked in this subset.");
            par.addInt("nBase", 4, "Number of base features:", "Number of feature points that make a base. At least 3 are required, 4 get a better result.");
            par.addInt("k", 75, "Number of neighboors:", "Number of neighboor feature points picked by kNN search during matching. Greater values produce a greater success probability but make the alignment process slower.");
            break;
        }
        case AF_RIGID_TRANSFORMATION :
        {
            QStringList l;
            l << "GMSmooth curvature"
              << "RGB";
            par.addEnum("featureType", 0, l,"Feature type:", "The feature that you want to compute for the current mesh.");
            par.addMesh("mFix", 0, "Fix mesh:", "The mesh that stays still and grow large after alignment.");
            par.addMesh("mMov", 1, "Move mesh:", "The mesh that moves to fit Fix Mesh.");
            par.addInt("numMovFeatureSelected", 250, "Number of features:", "The number of feature points you want to extract from Move Mesh. The base of feature points that has to be matched is randomly picked in this subset.");
            par.addInt("nBase", 4, "Number of base features:", "Number of feature points that make a base. At least 3 are required, 4 get a better result.");
            par.addInt("k", 75, "Number of neighboors:", "Number of neighboor feature points picked by kNN search during matching. Greater values produce a greater success probability but make the alignment process slower.");
            break;
        }        
        case AF_CONSENSUS :
        {            
            par.addMesh("mFix", 0, "Fix mesh:", "The mesh that stays still and grow large after alignment.");
            par.addMesh("mMov", 1, "Move mesh:", "The mesh that moves to fit Fix Mesh.");
            par.addFloat("consensusDist", 2, "Consensus distance:","Consensus distance expressed in percentage of Move Mesh bounding box diagonal. It states how close two verteces must be to be in consensus.");
            par.addInt("fullConsensusSamples", 2500, "Number of samples:", "Number of samples used to perform consensus procedure. Greater values get a more actual esteem but takes more time to be computed.");
            par.addBool("normEq", true, "Normal equalization","If checked samples are selected according a normal equilezed strategy, elsewhere a random selection is performed. Normal equalization is computationally more expensive but provides a better esteem, specially with few samples.");
            par.addBool("paint", true, "Paint Move Mesh","If checked Move Mesh is painted according to consensus as follows:<br>white -> not tested<br>blue -> too far<br>yellow -> close but badly oriented<br>red -> in consensus.");
            break;
        }
        case AF_RANSAC:
        {
            QStringList l;
            l << "GMSmooth curvature"
              << "APSS curvature"
              << "RGB";
            par.addEnum("featureType", 0, l,"Feature type:", "The feature that you want to compute for the current mesh.");
            par.addMesh("mFix", 0, "Fix mesh:", "The mesh that stays still and grow large after alignment.");
            par.addMesh("mMov", 1, "Move mesh:", "The mesh that moves to fit Fix Mesh.");
            par.addInt("ransacIter", 500, "Iterations:", "Number of iterations of the RANSAC algorithm. Greater values provides a greater success probability but requires more time.");
            par.addFloat("overlap", 75.0, "Overlap:", "A measure, expressed in percentage, of how much Move Mesh overlaps with Fix Mesh. It is very important to provide an actual esteem: lower values can produce false positive results or too rough alignments; higher values produce a small success probability and no alignements at all.");
            par.addFloat("consensusDist", 2, "Consensus distance:","Consensus distance expressed in percentage of Move Mesh bounding box diagonal. It states how close two verteces must be to be in consensus.");
            par.addInt("k", 75, "Number of neighboors:", "Number of neighboor feature points picked by kNN search during matching. Greater values produce a greater success probability but make the alignment process slower.");
            par.addBool("pickPoints", false, "Store best match", "If checked, feature points of the base picked on Move Mesh and feature points of the best matching base on Fix Mesh are stored into PickedPoints attribute. Bases are stored only if alignment process ends successfully, i.e at least one base have to exceed full consensus.");
            break;
        }
        case AF_RANSAC_DIAGRAM:
        {
            QStringList l;
            l << "GMSmooth curvature"
              << "APSS curvature"
              << "RGB";
            par.addEnum("featureType", 0, l,"Feature type:", "The feature that you want to compute for the current mesh.");
            par.addMesh("mFix", 0, "Fix mesh:", "The mesh that stays still and grow large after alignment.");
            par.addMesh("mMov", 1, "Move mesh:", "The mesh that moves to fit Fix Mesh.");
            par.addFloat("overlap", 75.0, "Overlap:", "A measure, expressed in percentage, of how much Move Mesh overlaps with Fix Mesh. It is very important to provide an actual esteem: lower values can produce false positive results or too rough alignments; higher values produce a small success probability and no alignements at all.");
            par.addFloat("consensusDist", 2, "Consensus distance:","Consensus distance expressed in percentage of Move Mesh bounding box diagonal. It states how close two verteces must be to be in consensus.");
            par.addInt("k", 75, "Number of neighboors:", "Number of neighboor feature points picked by kNN search during matching. Greater values produce a greater success probability but make the alignment process slower.");
            par.addInt("trials", 100, "Trials:", "How many times the alignment process is repeated with the same amount of RANSAC iterations.");
            par.addInt("from", 100, "From iteration:", "Number of RANSAC iteration used to perform the first battery of alignments.");
            par.addInt("to", 1000, "To iteration:", "Number of RANSAC iteration over which no more alignments are performed.");
            par.addInt("step", 100, "Step:", "Step used to increment RANSAC iterations after that the specified number of attempts has been done.");
            break;
        }                                 
        default: assert(0);
    }
}

template<class ALIGNER_TYPE>
void FilterFeatureAlignment::setAlignmentParameters(typename ALIGNER_TYPE::MeshType& mFix, typename ALIGNER_TYPE::MeshType& mMov, FilterParameterSet& par, typename ALIGNER_TYPE::Parameters& param)
{        
    typedef ALIGNER_TYPE AlignerType;
    typedef typename AlignerType::MeshType MeshType;

    if(par.hasParameter("samplingStrategy")) param.samplingStrategy = par.getEnum("samplingStrategy");
    if(par.hasParameter("numMovFeatureSelected")) param.numMovFeatureSelected = math::Clamp(par.getInt("numMovFeatureSelected"),0,mMov.VertexNumber());
    if(par.hasParameter("nBase")){ param.nBase = par.getInt("nBase"); if(param.nBase<4) param.nBase=4; }
    if(par.hasParameter("k")){ param.k = par.getInt("k"); if(param.k<1) param.k=1; }
    if(par.hasParameter("ransacIter")){ param.ransacIter = par.getInt("ransacIter"); if(param.ransacIter<0) param.ransacIter = 0;}
    if(par.hasParameter("fullConsensusSamples")) param.fullConsensusSamples = math::Clamp(par.getInt("fullConsensusSamples"),1,mMov.VertexNumber());
    if(par.hasParameter("overlap")) param.overlap = math::Clamp<float>(par.getFloat("overlap"),0.0f,100.0f);
    if(par.hasParameter("consensusDist")) param.consensusDist = math::Clamp<float>(par.getFloat("consensusDist"),0.0f,100.0f);
    if(par.hasParameter("pickPoints")) param.pickPoints = par.getBool("pickPoints");
    if(par.hasParameter("normEq")) param.normalEqualization = par.getBool("normEq");
    if(par.hasParameter("paint")) param.paint = par.getBool("paint");
}

template<class ALIGNER_TYPE>
bool FilterFeatureAlignment::logResult(FilterIDType filter, typename ALIGNER_TYPE::Result& res, QString& errorMsg)
{
    typedef ALIGNER_TYPE AlignerType;
    typedef typename AlignerType::Result ResultType;

    switch(filter)
    {
        case AF_MATCHING:{
            switch(res.exitCode){
                case ResultType::ALIGNED :
                case ResultType::NOT_ALIGNED : {mylogger(GLLogStream::FILTER,"Matching done: %i matches, %i msec.", res.numMatches, res.totalTime); return true;}
                case ResultType::FAILED : {errorMsg = res.errorMsg; return false; }
                default : assert(0);
            }
        }
        case AF_RIGID_TRANSFORMATION:{
                switch(res.exitCode){
                    case ResultType::ALIGNED : {mylogger(GLLogStream::FILTER,"Transformations computed: %i computed, %i matches, %i msec.", res.numWonFullCons, res.numMatches, res.totalTime); return true;}
                    case ResultType::NOT_ALIGNED : {mylogger(GLLogStream::FILTER,"No trasformations computed: %i matches, %i msec.", res.numMatches, res.totalTime); return true;}
                    case ResultType::FAILED : {errorMsg = res.errorMsg; return false; }
                    default : assert(0);
            }
        }                
        case AF_RANSAC:{
            switch(res.exitCode){
                case ResultType::ALIGNED : {mylogger(GLLogStream::FILTER,"Alignemnt found: %.2f%% consensus.", res.bestConsensus); return true;}
                case ResultType::NOT_ALIGNED : {mylogger(GLLogStream::FILTER,"Alignemnt not found: best consensus found %.2f%%.", res.bestConsensus); return true;}
                case ResultType::FAILED : {errorMsg = res.errorMsg; return false; }
                default : assert(0);
            }
        }
        case AF_RANSAC_DIAGRAM:{
            switch(res.exitCode){
                case ResultType::ALIGNED :
                case ResultType::NOT_ALIGNED : return true;
                case ResultType::FAILED : {errorMsg = res.errorMsg; return false; }
                default : assert(0);
            }
        }
        default: return true;
    }
    assert(0);
}

bool FilterFeatureAlignment::applyFilter(QAction *filter, MeshDocument &md, FilterParameterSet & par, vcg::CallBackPos * cb)
{
    glLog=log;  //assign log to a global variable. needed to write the log from everywhere in the code

    //define needed typedef MeshType
    typedef CMeshO MeshType;

    //declare variables for parameters
    int featureType, from, to, step, trials;
    MeshModel *mFix, *mMov, *currMesh;
    //read parameters
    currMesh = md.mm(); //get current mesh from document
    if(par.hasParameter("mFix")) mFix = par.getMesh("mFix"); else mFix = currMesh;
    if(par.hasParameter("mMov")) mMov = par.getMesh("mMov"); else mMov = currMesh;
    if(par.hasParameter("featureType")) featureType = par.getEnum("featureType"); else featureType = -1;
    if(par.hasParameter("trials")) trials = par.getInt("trials"); else trials = 100;
    if(par.hasParameter("from")) from = par.getInt("from"); else from = 1000;
    if(par.hasParameter("to")) to = par.getInt("to"); else to = 5000;
    if(par.hasParameter("step")) step = par.getInt("step"); else step = 1000;

    switch(ID(filter))
    {
        case AF_COMPUTE_FEATURE:
        {            
            switch(featureType){
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 6> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    return ComputeFeatureOperation<MeshType,FeatureType>(*currMesh, param, cb);
                }
                case 1:{
                    typedef APSSCurvatureFeature<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    return ComputeFeatureOperation<MeshType,FeatureType>(*currMesh, param, cb);
                }
                case 2:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    return ComputeFeatureOperation<MeshType,FeatureType>(*currMesh, param, cb);
                }                
                default: assert(0);
            }   //end switch(ftype)
            assert(0);
        }   //end case AF_COMPUTE_FEATURE
        case AF_EXTRACTION:
        {
            switch(featureType){
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 6> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    bool ok = ExtractionOperation<AlignerType>(*currMesh, alignerParam, cb);
                    return ok;
                }
                case 1:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType                    
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);                    
                    setAlignmentParameters<AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    bool ok = ExtractionOperation<AlignerType>(*currMesh, alignerParam, cb);
                    return ok;
                }                
                default: assert(0);
            }   //end switch(ftype)
            assert(0);
        }   //end case AF_EXTRACTION
        case AF_MATCHING :
        {            
            switch(featureType)
            {                
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 6> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    typedef AlignerType::Result ResultType;
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    ResultType res = MatchingOperation<AlignerType>(*mFix, *mMov, alignerParam, cb);
                    return logResult<AlignerType>(ID(filter), res, errorMessage);
                }
                case 1:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    typedef AlignerType::Result ResultType;
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    ResultType res = MatchingOperation<AlignerType>(*mFix, *mMov, alignerParam, cb);
                    return logResult<AlignerType>(ID(filter), res, errorMessage);
                }                
                default: assert(0);
            }  //end switch(ftype)
            assert(0);
        }  //end case AF_MATCHING
        case AF_RIGID_TRANSFORMATION :
        {                      
            switch(featureType)
            {                
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 6> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    typedef AlignerType::Result ResultType;
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    ResultType res = RigidTransformationOperation<AlignerType>(*mFix, *mMov, alignerParam, cb);
                    return logResult<AlignerType>(ID(filter), res, errorMessage);
                }
                case 1:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    typedef AlignerType::Result ResultType;
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    ResultType res = RigidTransformationOperation<AlignerType>(*mFix, *mMov, alignerParam, cb);
                    return logResult<AlignerType>(ID(filter), res, errorMessage);
                }               
                default: assert(0);
            }  //end switch(ftype)
            assert(0);
        }  //end case AF_RIGID_TRANSFORMATION
        case AF_CONSENSUS :
        {                        
            typedef OverlapEstimation<CMeshO> ConsensusType;
            ConsensusType::Parameters consParam;
            //set up params for consensus
            consParam.samples = math::Clamp(par.getInt("fullConsensusSamples"),1,mMov->cm.VertexNumber());
            consParam.consensusDist=math::Clamp<float>(par.getFloat("consensusDist"),0.0f,100.0f);
            consParam.paint = par.getBool("paint");
            consParam.normalEqualization = par.getBool("normEq");
            consParam.threshold = 0.0f;
            consParam.bestScore = 0;
            float result = ConsensusOperation<ConsensusType>(*mFix, *mMov, consParam, cb);
            if(result<0){
                errorMessage = "Consensus Initialization fails.";
                return false; }

            Log("Consensus of %.2f%%", 100*result);
            return true;
        }
        case AF_RANSAC:
        {                                
            switch(featureType){
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 6> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    typedef AlignerType::Result ResultType;
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<AlignerType>(mFix->cm, mMov->cm, par, alignerParam);                    
                    ResultType res = RansacOperation<AlignerType>(*mFix, *mMov, alignerParam, cb);
                    return logResult<AlignerType>(ID(filter), res, errorMessage);
                }
                case 1:{
                    typedef APSSCurvatureFeature<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    typedef AlignerType::Result ResultType;
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    ResultType res = RansacOperation<AlignerType>(*mFix, *mMov, alignerParam, cb);
                    return logResult<AlignerType>(ID(filter), res, errorMessage);
                }
                case 2:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    typedef AlignerType::Result ResultType;
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    ResultType res = RansacOperation<AlignerType>(*mFix, *mMov, alignerParam, cb);
                    return logResult<AlignerType>(ID(filter), res, errorMessage);
                }                
                default: assert(0);
            }  // end switch(ftype)            
            assert(0);
        }  //end case AF_RANSAC
        case AF_RANSAC_DIAGRAM:
        {                                                
            switch(featureType){
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 6> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    typedef AlignerType::Result ResultType;
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    ResultType res = RansacDiagramOperation<AlignerType>(*mFix, *mMov, alignerParam, trials, from, to, step, cb);
                    return logResult<AlignerType>(ID(filter), res, errorMessage);
                }
                case 1:{
                    typedef APSSCurvatureFeature<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    typedef AlignerType::Result ResultType;
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    ResultType res = RansacDiagramOperation<AlignerType>(*mFix, *mMov, alignerParam, trials, from, to, step, cb);
                    return logResult<AlignerType>(ID(filter), res, errorMessage);
                }
                case 2:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    typedef AlignerType::Result ResultType;
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    ResultType res = RansacDiagramOperation<AlignerType>(*mFix, *mMov, alignerParam, trials, from, to, step, cb);
                    return logResult<AlignerType>(ID(filter), res, errorMessage);
                }                
                default: assert(0);
            }  // end switch(ftype)
            assert(0);
        }  //end case AF_RANSAC_DIAGRAM                                         
        default: assert(0);
    } // end switch(ID(filter))
    return false;
}

///--------------------------------------------------------------------------///
/// PLUGIN WRAPPERS                                                          ///
///--------------------------------------------------------------------------///

template<class MESH_TYPE, class FEATURE_TYPE> bool FilterFeatureAlignment::ComputeFeatureOperation(MeshModel& m, typename FEATURE_TYPE::Parameters& param, CallBackPos *cb)
{
    typedef MESH_TYPE MeshType;
    typedef FEATURE_TYPE FeatureType;

    //enables needed attributes
    m.updateDataMask(FeatureType::getRequirements());    

    //compute feature for the given mesh
    return FeatureType::ComputeFeature(m.cm, param, cb);
}

template<class ALIGNER_TYPE>
bool FilterFeatureAlignment::ExtractionOperation(MeshModel& m, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb)
{
    typedef ALIGNER_TYPE AlignerType;
    typedef typename AlignerType::MeshType MeshType;
    typedef typename AlignerType::FeatureType FeatureType;

    //enables needed attributes
    m.updateDataMask(FeatureType::getRequirements());
    //extract features
    vector<FeatureType*>* vecF = AlignerType::extractFeatures(param.numMovFeatureSelected, m.cm, param.samplingStrategy, cb);
    if(!vecF) return false;  //something wrong!

    //clear old picked points, then, if requested, add all new points
    AlignerType::ClearPickedPoints(m.cm);
    if(param.pickPoints) AlignerType::AddPickedPoints(m.cm, *vecF);      //add points

    //clean up
    vecF->clear(); delete vecF; vecF = NULL;

    return true;
}

template<class ALIGNER_TYPE>
typename ALIGNER_TYPE::Result FilterFeatureAlignment::MatchingOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb)
{
    typedef ALIGNER_TYPE AlignerType;
    typedef typename AlignerType::MeshType MeshType;
    typedef typename AlignerType::FeatureType FeatureType;
    typedef typename AlignerType::Result ResultType;
    typedef typename AlignerType::CandidateType CandidateType;

    QTime timer;
    timer.start(); //start timer

    //create vectors to hold tuples of bases and matches
    vector<FeatureType**>* baseVec = new vector<FeatureType**>();
    vector<vector<FeatureType*> > matches;
    vector<CandidateType>* candidates = new vector<CandidateType>();

    AlignerType aligner;
    ResultType res = aligner.init(mFix.cm, mMov.cm, param);
    if(res.exitCode==ResultType::FAILED) return res;

    //execute matching procedure with requested parameters;
    int errCode = AlignerType::SelectBase(*(aligner.vecFMov),*baseVec,param);
    if(errCode){ AlignerType::setError(errCode, res); return res; }
    errCode = AlignerType::Matching(*(aligner.vecFFix), *(aligner.vecFMov), aligner.fkdTree, (*baseVec)[baseVec->size()-1], matches, param, cb);
    errCode = AlignerType::BranchAndBound((*baseVec)[baseVec->size()-1], matches, *candidates, param);

    res.numMatches = candidates->size(); //store the numeber of matches found

    //Clean structures...
    for(int i=0; i<baseVec->size();i++){ delete[] (*baseVec)[i];}
    for(int i=0; i<candidates->size();i++){ delete[] (*candidates)[i].basePtr; delete[] (*candidates)[i].matchPtr;}
    delete baseVec; delete candidates;

    res.totalTime = timer.elapsed();

    return res;
}

template<class ALIGNER_TYPE>
typename ALIGNER_TYPE::Result FilterFeatureAlignment::RigidTransformationOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb)
{
    typedef ALIGNER_TYPE AlignerType;
    typedef typename AlignerType::MeshType MeshType;
    typedef typename AlignerType::FeatureType FeatureType;
    typedef typename MeshType::ScalarType ScalarType;
    typedef typename AlignerType::Result ResultType;
    typedef Matrix44<ScalarType> Matrix44Type;
    typedef typename AlignerType::CandidateType CandidateType;

    QTime timer;
    timer.start(); //start timer

    //create vectors to hold tuples of bases and matches
    vector<FeatureType**>* baseVec = new vector<FeatureType**>();
    vector<vector<FeatureType*> > matches;
    vector<CandidateType>* candidates = new vector<CandidateType>();

    AlignerType aligner;
    ResultType res = aligner.init(mFix.cm, mMov.cm, param);
    if(res.exitCode==ResultType::FAILED) return res;

    //execute matching procedure with requested parameters;
    int errCode = AlignerType::SelectBase(*(aligner.vecFMov),*baseVec,param);
    if(errCode){ AlignerType::setError(errCode, res); return res; }
    errCode = AlignerType::Matching(*(aligner.vecFFix), *(aligner.vecFMov), aligner.fkdTree, (*baseVec)[baseVec->size()-1], matches, param, cb);
    errCode = AlignerType::BranchAndBound((*baseVec)[baseVec->size()-1], matches, *candidates, param);

    res.numMatches = candidates->size(); //store the numeber of matches found
    assert(baseVec->size()==1);  //now baseVec must hold exactly one base of features

    for(unsigned int j=0; j<candidates->size(); j++)
    {
        CandidateType currCandidate = (*candidates)[j];
        Matrix44Type tr;
        errCode = AlignerType::FindRigidTransformation(mFix.cm, mMov.cm, currCandidate.basePtr, currCandidate.matchPtr, param.nBase, tr, cb);
        if(errCode) { AlignerType::setError(errCode,res); return res; }

        res.numWonFullCons++;  //use this variable to increase num of found transform
        res.exitCode = ResultType::ALIGNED;  //this means at least one matrix found
    }

    //Clean structures...
    for(int i=0; i<baseVec->size();i++){ delete[] (*baseVec)[i];}
    for(int i=0; i<candidates->size();i++){ delete[] (*candidates)[i].basePtr; delete[] (*candidates)[i].matchPtr;}
    delete baseVec; delete candidates;

    res.totalTime = timer.elapsed();

    return res; //all right
}

template<class CONSENSUS_TYPE>
float FilterFeatureAlignment::ConsensusOperation(MeshModel& mFix, MeshModel& mMov, typename CONSENSUS_TYPE::Parameters& param, CallBackPos *cb)
{
    typedef CONSENSUS_TYPE ConsensusType;
    typedef typename ConsensusType::MeshType MeshType;
    typedef typename MeshType::ScalarType ScalarType;
    typedef typename MeshType::VertexType VertexType;    
    typedef typename ConsensusType::Parameters ParamType;

    //enables needed attributes. These are used by the getClosest functor.
    mFix.updateDataMask(MeshModel::MM_VERTMARK);
    mMov.updateDataMask(MeshModel::MM_VERTMARK);    

    ConsensusType cons;    
    cons.SetFix(mFix.cm);
    cons.SetMove(mMov.cm);

    if(!cons.Init(param)) return -1.0f;

    return cons.Compute(param);
}

template<class ALIGNER_TYPE>
typename ALIGNER_TYPE::Result FilterFeatureAlignment::RansacOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb)
{
    typedef ALIGNER_TYPE AlignerType;
    typedef typename AlignerType::MeshType MeshType;
    typedef typename AlignerType::FeatureType FeatureType;
    typedef typename MeshType::ScalarType ScalarType;
    typedef typename AlignerType::Result ResultType;

    //enables needed attributes. MM_VERTMARK is used by the getClosest functor.
    mFix.updateDataMask(MeshModel::MM_VERTMARK|FeatureType::getRequirements());
    mMov.updateDataMask(MeshModel::MM_VERTMARK|FeatureType::getRequirements());

    AlignerType aligner;
    param.log = &mylogger; //set the callback used to print infos inside the procedure
    ResultType res = aligner.init(mFix.cm, mMov.cm, param, cb);
    if(res.exitCode==ResultType::FAILED) return res;

    //perform RANSAC and get best transformation matrix
    res = aligner.align(mFix.cm, mMov.cm, param, cb);

    //apply transformation. If ransac don't find a good matrix, identity is returned; so nothing is wrong here...
    mMov.cm.Tr = res.tr * mMov.cm.Tr;

    return res;
}

template<class ALIGNER_TYPE>
typename ALIGNER_TYPE::Result FilterFeatureAlignment::RansacDiagramOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, int trials, int from, int to, int step, CallBackPos *cb)
{
    typedef ALIGNER_TYPE AlignerType;
    typedef typename AlignerType::MeshType MeshType;
    typedef typename AlignerType::FeatureType FeatureType;
    typedef typename MeshType::ScalarType ScalarType;    
    typedef typename AlignerType::Result ResultType;

    //variables needed for progress bar callback
    float progBar = 0.0f;
    float offset = 100.0f/(trials);

    //enables needed attributes. MM_VERTMARK is used by the getClosest functor.
    mFix.updateDataMask(MeshModel::MM_VERTMARK|FeatureType::getRequirements());
    mMov.updateDataMask(MeshModel::MM_VERTMARK|FeatureType::getRequirements());

    ResultType res;

    FILE* file = fopen("Diagram.txt","w+");
    fprintf(file,"Fix Mesh#%s\nMove Mesh#%s\nFeature#%s\nNum. of vertices of Fix Mesh#%i\nNum. of vertices of Move Mesh#%i\nOverlap#%.2f%%\nFull consensus threshold#%.2f%% overlap#%.2f%% of Move Mesh#\nTrials#%i\n",mFix.fileName.c_str(),mMov.fileName.c_str(),FeatureType::getName(),mFix.cm.VertexNumber(),mMov.cm.VertexNumber(),param.overlap,param.consOffset,(param.consOffset*param.overlap/100.0f),trials); fflush(file);

    fprintf(file,"Iterazioni#Tempo di inizializzazione#Tempo di esecuzione#Prob. Succ.#Prob. Fall. per Sec#Num. medio basi\n0#0#0#0#0\n"); fflush(file);

    float probSucc = 0.0f, meanTime = 0.0f, meanInitTime = 0.0f, failPerSec = -1.0f;
    int numWon = 0, trialsTotTime = 0, trialsInitTotTime = 0, numBases = 0;
    param.ransacIter = from;
    //param.log = &mylogger; //this is the way to assign a pointer to log function
    //move res here

    while(param.ransacIter<=to)
    {
        for(int i=0; i<trials; i++){
            //callback handling
            if(cb){ progBar+=offset; cb(int(progBar),"Computing diagram..."); }            

            AlignerType aligner;
            res = aligner.init(mFix.cm, mMov.cm, param);
            if(res.exitCode==ResultType::FAILED) return res;
            trialsInitTotTime+=res.initTime;

            res = aligner.align(mFix.cm, mMov.cm, param);

            trialsTotTime+=res.totalTime;
            numBases+=res.numBasesFound;
            if(res.exitCode==ResultType::ALIGNED) numWon++;
            if(res.exitCode==ResultType::FAILED) return res;  //failure: stop everything and return error            
        }

        probSucc = numWon/float(trials);                    //k = prob succ in 1 iteration
        meanTime = trialsTotTime/float(trials);             //t=sec elapsed to perform N ransac iterations
        meanInitTime = trialsInitTotTime/float(trials);
        failPerSec = std::pow(1-probSucc,1.0f/(meanTime/1000));    //fail rate per sec is: (1-k)^(1/t)
        fprintf(file,"%i#=%.0f/1000#=%.0f/1000#=%.0f/100#=%.0f/100#%i\n", param.ransacIter, meanInitTime, meanTime, 100*probSucc, 100*failPerSec,numBases/trials); fflush(file);
        numWon = 0; trialsTotTime = 0; trialsInitTotTime=0; progBar=0.0f; numBases=0;
        param.ransacIter+=step;
    }

    fclose(file);
    return res;  //all right
}

Q_EXPORT_PLUGIN(FilterFeatureAlignment)
