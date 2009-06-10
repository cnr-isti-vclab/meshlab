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
#include <feature_alignment.h>
#include <filter_feature_alignment.h>
#include <feature_msc.h>
#include <feature_rgb.h>
#include <meshlabplugins/edit_pickpoints/pickedPoints.h>

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <cmath>
#include <stdio.h>

using namespace std;
using namespace vcg;

GLLogStream *glLog;
void mylogger(const char * f, ... )
{
    char buf[4096];
    va_list marker;
    va_start( marker, f );

    vsprintf(buf,f,marker);
    va_end( marker );
    glLog->Log(GLLogStream::DEBUG,buf);
}

FilterFeatureAlignment::FilterFeatureAlignment()
{
    typeList << AF_COMPUTE_FEATURE
             << AF_EXTRACTION
             << AF_MATCHING
             << AF_RIGID_TRANSFORMATION             
             << AF_CONSENSUS
             << AF_RANSAC
             << AF_RANSAC_DIAGRAM                                   
             << AF_PERLIN_COLOR
             << AF_COLOR_NOISE;

    FilterIDType tt;
    foreach(tt , types())
        actionList << new QAction(filterName(tt), this);
}

FilterFeatureAlignment::~FilterFeatureAlignment()
{
    for (int i = 0; i < actionList.count() ; i++ )
        delete actionList.at(i);
}

const MeshFilterInterface::FilterClass FilterFeatureAlignment::getClass(QAction *a)
{
    switch(ID(a))
    {
        case AF_COMPUTE_FEATURE :
        case AF_EXTRACTION :
        case AF_MATCHING :
        case AF_RIGID_TRANSFORMATION :        
        case AF_CONSENSUS:        
        case AF_PERLIN_COLOR:
        case AF_COLOR_NOISE:                
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

const QString FilterFeatureAlignment::filterName(FilterIDType filter)
{
    switch(filter)
    {
        case AF_COMPUTE_FEATURE : return "Compute feature";
        case AF_EXTRACTION : return "Features extraction";
        case AF_MATCHING : return "Features matching";
        case AF_RIGID_TRANSFORMATION : return "Rigid transformation";
        case AF_CONSENSUS : return "Consensus";
        case AF_RANSAC : return "Ransac";
        case AF_RANSAC_DIAGRAM : return "Ransac Diagram";                            
        case AF_PERLIN_COLOR : return "Perlin color";
        case AF_COLOR_NOISE : return "Color Noise";
        default: assert(0);
    }
    return QString("error!");
}

const QString FilterFeatureAlignment::filterInfo(FilterIDType filterId)
{
    switch(filterId)
    {
        case AF_COMPUTE_FEATURE : return "Compute feature values";
        case AF_EXTRACTION : return "Given a mesh and a valid feature type, extract the best k features from the mesh.";
        case AF_MATCHING : return "";
        case AF_RIGID_TRANSFORMATION : return "";
        case AF_CONSENSUS : return "Performs a randomized consensus, followed by a full consensus only if randomized consensus exceeds a given percentage.";
        case AF_RANSAC : return "Perform ransac algorithm";
        case AF_RANSAC_DIAGRAM : return "Ransac Diagram";        
        case AF_PERLIN_COLOR : return "Colors randomly the vertexes of the mesh";
        case AF_COLOR_NOISE : return "";
        default: assert(0);
    }
    return QString("error!");
}

const int FilterFeatureAlignment::getRequirements(QAction *action)
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
            l << "GMSmooth Curvature"
              << "FeatureRGB";
            par.addEnum("featureType", 0, l,"Feature type", "");
            break;
        }
        case AF_EXTRACTION:
        {
            QStringList l, st;
            l << "GMSmooth Curvature"
              << "FeatureRGB";
            st << "Uniform" << "Poisson";
            par.addEnum("featureType", 0, l,"Feature type", "");
            par.addInt("numFixFeatureSelected", 250, "Number of feature to select", "");
            par.addEnum("samplingStrategy", 0, st,"Sampling", "");
            par.addBool("pickPoints", false, "Store features as picked points", "");
            break;
        }
        case AF_MATCHING:
        {
            QStringList l;
            l << "GMSmooth Curvature"
              << "FeatureRGB";
            par.addEnum("featureType", 0, l,"Feature type: ", "");
            par.addMesh("mFix", 0, "Fix mesh");
            par.addMesh("mMov", 1, "Move mesh");
            par.addInt("numFixFeatureSelected", 250, "Number of feature extracted from fix mesh: ", "");
            par.addInt("nBase", 4, "Number of base features: ", "");            
            par.addInt("k", 150, "Number of neighboor picked: ", "");            
            break;
        }
        case AF_RIGID_TRANSFORMATION :
        {
            QStringList l;
            l << "GMSmooth Curvature"
              << "FeatureRGB";
            par.addEnum("featureType", 0, l,"Feature type: ", "");
            par.addMesh("mFix", 0, "Fix mesh");
            par.addMesh("mMov", 1, "Move mesh");
            par.addInt("numFixFeatureSelected", 250, "Number of feature extracted from fix mesh: ", "");
            par.addInt("nBase", 4, "Number of base features: ", "");           
            par.addInt("k", 150, "Number of neighboor picked: ", "");            
            break;
        }        
        case AF_CONSENSUS :
        {
            par.addMesh("mFix", 0, "Fix mesh");
            par.addMesh("mMov", 1, "Move mesh");
            par.addFloat("consensusDist", 2, "Consensus distance in \% of BBox diagonal of move mesh", "");
            par.addInt("fullConsensusSamples", 2500, "Number of samples", "");
            par.addBool("normEq", true, "Use normal equalization in consensus","");
            par.addBool("paint", true, "Paint move mesh according to consensus","");
            break;
        }
        case AF_RANSAC:
        {
            QStringList l;
            l << "GMSmooth Curvature"
              << "FeatureRGB";
            par.addEnum("featureType", 0, l,"Feature type", "");
            par.addMesh("mFix", 0, "Fix mesh");
            par.addMesh("mMov", 1, "Move mesh");
            par.addInt("ransacIter", 5000, "Max number of RANSAC iterations", "");
            par.addFloat("overlap", 75.0, "Overlap in \%", "");
            par.addFloat("consensusDist", 2, "Consensus distance in \% of BBox diagonal of move mesh", "");
            par.addInt("k", 150, "Number of neighboor picked: ", "");                      
            par.addBool("pickPoints", true, "Store base and match as picked points","");
            break;
        }
        case AF_RANSAC_DIAGRAM:
        {
            QStringList l;
            l << "GMSmooth Curvature"
              << "FeatureRGB";
            par.addEnum("featureType", 0, l,"Feature type", "");
            par.addMesh("mFix", 0, "Fix mesh");
            par.addMesh("mMov", 1, "Move mesh");
            par.addInt("ransacIter", 5000, "Max number of RANSAC iterations", "");
            par.addFloat("overlap", 75.0, "Overlap in \%", "");
            par.addFloat("consensusDist", 2, "Consensus distance in \% of BBox diagonal of move mesh", "");
            par.addInt("k", 150, "Number of neighboor picked: ", "");
            par.addInt("trials", 100, "Number of try", "");
            par.addInt("from", 1000, "From iteration", "");
            par.addInt("to", 5000, "To iteration", "");
            par.addInt("step", 1000, "Step", "");
            break;
        }                 
        case AF_PERLIN_COLOR:
        {
            par.addDynamicFloat("freq", 0.025, 0.001, 1,1,"Frequency","");
            break;
        }
        case AF_COLOR_NOISE:
        {            
            par.addInt("noiseBits", 1, "Bits of noise","");
            break;
        }        
        default: assert(0);
    }
}

bool FilterFeatureAlignment::applyFilter(QAction *filter, MeshDocument &md, FilterParameterSet & par, vcg::CallBackPos * cb)
{
    glLog=log;  //assign log to a global variable. needed to write the log from everywhere in the code

    //define needed typedef MeshType
    typedef CMeshO MeshType;
    //declare variables for parameters
    int featureType, numFixFeatureSelected, numMovFeatureSelected, samplingStrategy, nBase, k, g;
    int fullConsensusSamples, ransacIter, from, to, step, trials;
    float consensusDist, overlap, succOffset, consOffset, shortConsOffset;
    bool pickPoints, normEq, paint;
    MeshModel *mFix, *mMov, *currMesh;
    //read parameters
    currMesh = md.mm(); //get current mesh from document
    if(par.hasParameter("mFix")) mFix = par.getMesh("mFix"); else mFix = currMesh;
    if(par.hasParameter("mMov")) mMov = par.getMesh("mMov"); else mMov = currMesh;
    if(par.hasParameter("featureType")) featureType = par.getEnum("featureType"); else featureType = -1;
    if(par.hasParameter("samplingStrategy")) samplingStrategy = par.getEnum("samplingStrategy"); else samplingStrategy=FeatureAlignment::UNIFORM_SAMPLING;
    if(par.hasParameter("numFixFeatureSelected")) numFixFeatureSelected = par.getInt("numFixFeatureSelected"); else numFixFeatureSelected = 250;
    if(par.hasParameter("nBase")) nBase = par.getInt("nBase"); else nBase = 4;
    if(par.hasParameter("k")) k = par.getInt("k"); else k = 150;
    if(par.hasParameter("ransacIter")) ransacIter = par.getInt("ransacIter"); else ransacIter = 5000;
    if(par.hasParameter("fullConsensusSamples")) fullConsensusSamples = par.getInt("fullConsensusSamples"); else fullConsensusSamples = 2500;
    if(par.hasParameter("trials")) trials = par.getInt("trials"); else trials = 100;
    if(par.hasParameter("from")) from = par.getInt("from"); else from = 1000;
    if(par.hasParameter("to")) to = par.getInt("to"); else to = 5000;
    if(par.hasParameter("step")) step = par.getInt("step"); else step = 1000;
    if(par.hasParameter("overlap")) overlap = math::Clamp<float>(par.getFloat("overlap"),0.0f,100.0f); else overlap = 75.0f;
    if(par.hasParameter("consensusDist")) consensusDist = math::Clamp<float>(par.getFloat("consensusDist"),0.0f,100.0f); else consensusDist = 2.0f;
    if(par.hasParameter("pickPoints")) pickPoints = par.getBool("pickPoints"); else pickPoints = false;
    if(par.hasParameter("normEq")) normEq = par.getBool("normEq"); else normEq = true;
    if(par.hasParameter("paint")) paint = par.getBool("paint"); else paint = false;
    //Other parameters set to default values
    g=50;
    numMovFeatureSelected = mMov->cm.VertexNumber();
    succOffset = 90.0f;
    consOffset = 60.0f;
    shortConsOffset = 40.0f;
FILE* file = fopen("par.txt","w+");
fprintf(file,"currMesh %p\n",currMesh);
fprintf(file,"mFix %p\n",mFix);
fprintf(file,"mMov %p\n",mMov);
fprintf(file,"featureType %i\n",featureType);
fprintf(file,"samplingStrategy %i\n",samplingStrategy);
fprintf(file,"numFixFeatureSelected %i\n",numFixFeatureSelected);
fprintf(file,"nBase %i\n",nBase);
fprintf(file,"k %i\n",k);
fprintf(file,"ransacIter %i\n",ransacIter);
fprintf(file,"fullConsensusSamples %i\n",fullConsensusSamples);
fprintf(file,"trials %i\n",trials);
fprintf(file,"from %i\n",from);
fprintf(file,"to %i\n",to);
fprintf(file,"step %i\n",step);
fprintf(file,"overlap %f\n",overlap);
fprintf(file,"consensusDist %f\n",consensusDist);
fprintf(file,"pickPoints %i\n",pickPoints);
fprintf(file,"normEq %i\n",normEq);
fprintf(file,"paint %i\n",paint);
fprintf(file,"g %i\n",g);
fprintf(file,"numMovFeatureSelected %i\n",numMovFeatureSelected);
fprintf(file,"succOffset %f\n",succOffset);
fprintf(file,"consOffset %f\n",consOffset);
fprintf(file,"shortConsOffset %f\n",shortConsOffset);
fclose(file);
    switch(ID(filter))
    {
        case AF_COMPUTE_FEATURE:
        {            
            switch(featureType){
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    return ComputeFeatureOperation<MeshType,FeatureType>(*currMesh, param, cb);
                }
                case 1:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    return ComputeFeatureOperation<MeshType,FeatureType>(*currMesh, param, cb);
                }                
                default: assert(0);
            }   //end switch(ftype)
            return true;
        }   //end case AF_COMPUTE_FEATURE
        case AF_EXTRACTION:
        {                      
            //set error message just once to avoid code grow long...
            errorMessage = QString("Probably features have not been computed for this mesh.\n");

            switch(featureType){
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    vector<FeatureType*>* vecF = ExtractionOperation<MeshType, FeatureType>(numFixFeatureSelected, *currMesh, samplingStrategy, pickPoints, cb);
                    if(vecF==NULL) return false;
                    vecF->clear(); delete vecF; vecF = NULL;  //clean up
                    return true;
                }
                case 1:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    vector<FeatureType*>* vecF = ExtractionOperation<MeshType, FeatureType>(numFixFeatureSelected, *currMesh, samplingStrategy, pickPoints, cb);
                    if(vecF==NULL) return false;
                    vecF->clear(); delete vecF; vecF = NULL;  //clean up
                    return true;
                }                
                default: assert(0);
            }   //end switch(ftype)
            return true;
        }   //end case AF_EXTRACTION
        case AF_MATCHING :
        {
            //set error message just once to avoid code grow long...
            errorMessage = QString("Probably the cause is one of the following:\n") +
                           QString("1 - Features have not been computed for these meshes.\n") +\
                           QString("2 - 'Number of neighboor picked' is > of 'Number of feature extracted from move mesh'.\n") +\
                           QString("3 - 'Number of base features' is > of 'Number of feature extracted from fix mesh'.\n") +
                           QString("4 - The base of feature selected doesn't pass inter-distance check.\n") +
                           QString("5 - No matches have been found.");

            switch(featureType)
            {                
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    return MatchingOperation<MeshType,FeatureType>(*mFix, *mMov, numFixFeatureSelected, numMovFeatureSelected, nBase, k, cb);
                }
                case 1:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    return MatchingOperation<MeshType,FeatureType>(*mFix, *mMov, numFixFeatureSelected, numMovFeatureSelected, nBase, k, cb);
                }                
                default: assert(0);
            }  //end switch(ftype)
            return false;
        }  //end case AF_MATCHING
        case AF_RIGID_TRANSFORMATION :
        {          
            //fill an array of error messages. this avoid the code grow long...
            QString errMessages[4] = {"","Features have not been computed for these meshes.\n","Something wrong during matching!\nTry filter matching with the same parameters.","Error: can't find a rigid transformation!"};

            switch(featureType)
            {                
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    int exitCode = RigidTransformationOperation<MeshType,FeatureType>(*mFix, *mMov, numFixFeatureSelected, numMovFeatureSelected, nBase, k, cb);
                    if(exitCode){ errorMessage = errMessages[exitCode]; return false; }
                    return true;
                }
                case 1:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    int exitCode = RigidTransformationOperation<MeshType,FeatureType>(*mFix, *mMov, numFixFeatureSelected, numMovFeatureSelected, nBase, k, cb);
                    if(exitCode){ errorMessage = errMessages[exitCode]; return false; }
                    return true;
                }               
                default: assert(0);
            }  //end switch(ftype)
            return false;
        }  //end case AF_RIGID_TRANSFORMATION
        case AF_CONSENSUS :
        {
            //prepare structures for consensus and then compute it
            int consensus = FilterFeatureAlignment::ConsensusOperation<MeshType>(*mFix, *mMov, consensusDist, fullConsensusSamples, normEq, paint, cb);

            //Log results
            Log(GLLogStream::FILTER,"Consensus of %.2f%% (%i/%i vertices).",(float(consensus)/fullConsensusSamples)*100.0f, consensus, fullConsensusSamples);
            return true;
        }
        case AF_RANSAC:
        {
            //set error message just once to avoid code grow long...
            errorMessage = QString("One or more parameters are wrong.\n") +
                           QString("1 - Features have not been computed for these meshes.\n") +\
                           QString("2 - 'Number of neighboor picked' must be > 1.\n");

            //check if parameters are good
            if(k<1) return false;

            switch(featureType){
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    return RansacOperation<MeshType,FeatureType>(*mFix, *mMov, numFixFeatureSelected, numMovFeatureSelected, nBase, ransacIter, consensusDist, fullConsensusSamples, overlap, shortConsOffset, consOffset, succOffset, k, g, samplingStrategy, pickPoints, cb);
                }               
                case 1:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    return RansacOperation<MeshType,FeatureType>(*mFix, *mMov, numFixFeatureSelected, numMovFeatureSelected, nBase, ransacIter, consensusDist, fullConsensusSamples, overlap, shortConsOffset, consOffset, succOffset, k, g, samplingStrategy, pickPoints, cb);
                }                
                default: assert(0);
            }  // end switch(ftype)
            return true;
        }  //end case AF_RANSAC
        case AF_RANSAC_DIAGRAM:
        {                                    
            //set error message just once to avoid code grow long...
            errorMessage = QString("One or more parameters are wrong.\n") +
                           QString("1 - Features have not been computed for these meshes.\n") +\
                           QString("2 - 'Number of neighboor picked' must be > 1.\n");

            //check if parameters are good
            if(k<1) return false;

            switch(featureType){
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    return RansacDiagramOperation<MeshType,FeatureType>(*mFix, *mMov, numFixFeatureSelected, numMovFeatureSelected, nBase, consensusDist, fullConsensusSamples, overlap, shortConsOffset, consOffset, succOffset, k, g, samplingStrategy, trials, from, to, step, cb);
                }
                case 1:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    return RansacDiagramOperation<MeshType,FeatureType>(*mFix, *mMov, numFixFeatureSelected, numMovFeatureSelected, nBase, consensusDist, fullConsensusSamples, overlap, shortConsOffset, consOffset, succOffset, k, g, samplingStrategy, trials, from, to, step, cb);
                }                
                default: assert(0);
            }  // end switch(ftype)
            return true;
        }  //end case AF_RANSAC_DIAGRAM              
        case AF_PERLIN_COLOR:
        {
            //get current mesh from document
            MeshModel* m = md.mm();
            //read parameters
            float freq = par.getDynamicFloat("freq");//default frequency; grant to be the same for all mesh in the document            
            m->updateDataMask(MeshModel::MM_VERTCOLOR);  //make sure color per vertex is enabled

            FeatureAlignment::RandomVertexColor<MeshType>(m->cm, freq);
            return true;
        }
        case AF_COLOR_NOISE:
        {
            //get current mesh from document
            MeshModel* m = md.mm();
            //read parameters
            int noiseBits = par.getInt("noiseBits");

            m->updateDataMask(MeshModel::MM_VERTCOLOR);  //make sure color per vertex is enabled

            FeatureAlignment::VertexColorNoise<MeshType>(m->cm, noiseBits);
            return true;
        }                        
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

template<class MESH_TYPE, class FEATURE_TYPE> vector<FEATURE_TYPE*>* FilterFeatureAlignment::ExtractionOperation(int numFixFeatureSelected, MeshModel& m, int samplingStrategy, bool pickPoints, CallBackPos *cb)
{
    typedef MESH_TYPE MeshType;
    typedef FEATURE_TYPE FeatureType;

    //enables needed attributes
    m.updateDataMask(FeatureType::getRequirements());
    //extract features
    vector<FeatureType*>* vecF = FeatureAlignment::extractFeatures<MeshType,FeatureType>(numFixFeatureSelected, m.cm, samplingStrategy, cb);

    //clear old picked points, then, if requested, add all new points
    FeatureAlignment::ClearPickedPoints(m.cm);
    if(pickPoints && vecF!=NULL) FeatureAlignment::AddPickedPoints(m.cm, *vecF);      //add points

    return vecF;
}

template<class MESH_TYPE, class FEATURE_TYPE>
bool FilterFeatureAlignment::MatchingOperation(MeshModel& mFix, MeshModel& mMov, int numFixFeatureSelected, int numMovFeatureSelected, int nBase, int k, CallBackPos *cb)
{
    typedef MESH_TYPE MeshType;
    typedef FEATURE_TYPE FeatureType;

    //enables needed attributes
    mFix.updateDataMask(FeatureType::getRequirements());
    mMov.updateDataMask(FeatureType::getRequirements());

    //extract features
    vector<FeatureType*>* vecF1 = FeatureAlignment::extractFeatures<MeshType,FeatureType>(numFixFeatureSelected, mFix.cm, FeatureAlignment::UNIFORM_SAMPLING, cb);
    vector<FeatureType*>* vecF2 = FeatureAlignment::extractFeatures<MeshType,FeatureType>(numMovFeatureSelected, mMov.cm, FeatureAlignment::UNIFORM_SAMPLING, cb);
    if(vecF1==NULL || vecF2==NULL) return false; //can't continue; features have not been computed!

    //create vectors to hold tuples of bases and matches
    vector<FeatureType**>* baseVec = new vector<FeatureType**>();
    vector<FeatureType**>* matchesVec = new vector<FeatureType**>();

    //prepare and build kdtree
    ANNpointArray fdataPts;
    FeatureAlignment::SetupKDTreePoints<FeatureType>(*vecF2, &fdataPts, FeatureType::getFeatureDimension());
    ANNkd_tree* fkdTree = new ANNkd_tree(fdataPts,vecF2->size(),FeatureType::getFeatureDimension());

    //execute matching procedure with requested parameters;
    float consensusDist = 2.0f*(mMov.cm.bbox.Diag()/100.0f); //default value for consensusDist
    float sparseBaseDist = 2*consensusDist;
    float mutualErrDist = consensusDist;
    bool ok = FeatureAlignment::Matching<MeshType,FeatureType>(*vecF1, *vecF2, fkdTree, nBase, *baseVec, *matchesVec, k, sparseBaseDist, mutualErrDist, cb);

    //write result in the log
    if(ok) mylogger("%i matches found.",matchesVec->size());

    //cleaning baseVec and matchesVec...
    FeatureAlignment::CleanTuplesVector<FeatureType>(baseVec, true);
    FeatureAlignment::CleanTuplesVector<FeatureType>(matchesVec, true);

    //cleaning vecF1 and vecF2...
    vecF1->clear(); delete vecF1; vecF1 = NULL;
    vecF2->clear(); delete vecF2; vecF2 = NULL;

    //Cleaning ANN structures
    FeatureAlignment::CleanKDTree(fkdTree, fdataPts, NULL, NULL, NULL, true);

    return ok;
}

template<class MESH_TYPE, class FEATURE_TYPE>
int FilterFeatureAlignment::RigidTransformationOperation(MeshModel& mFix, MeshModel& mMov, int numFixFeatureSelected, int numMovFeatureSelected, int nBase, int k, CallBackPos *cb)
{
    typedef MESH_TYPE MeshType;
    typedef FEATURE_TYPE FeatureType;

    //enables needed attributes
    mFix.updateDataMask(FeatureType::getRequirements());
    mMov.updateDataMask(FeatureType::getRequirements());

    //extract features
    vector<FeatureType*>* vecF1 = FeatureAlignment::extractFeatures<MeshType,FeatureType>(numFixFeatureSelected, mFix.cm, FeatureAlignment::UNIFORM_SAMPLING, cb);
    vector<FeatureType*>* vecF2 = FeatureAlignment::extractFeatures<MeshType,FeatureType>(numMovFeatureSelected, mMov.cm, FeatureAlignment::UNIFORM_SAMPLING, cb);
    if(vecF1==NULL || vecF2==NULL) return 1; //can't continue; features have not been computed!

    //create vectors to hold tuples of bases and matches
    vector<FeatureType**>* baseVec = new vector<FeatureType**>();
    vector<FeatureType**>* matchesVec = new vector<FeatureType**>();

    //prepare and build kdtree...
    ANNpointArray fdataPts;
    FeatureAlignment::SetupKDTreePoints<FeatureType>(*vecF2, &fdataPts, FeatureType::getFeatureDimension());
    ANNkd_tree* fkdTree = new ANNkd_tree(fdataPts,vecF2->size(),FeatureType::getFeatureDimension());

    //execute matching procedure with requested parameters;
    float consensusDist = 2.0f*(mMov.cm.bbox.Diag()/100.0f); //default value for consensusDist
    float sparseBaseDist = 2*consensusDist;
    float mutualErrDist = consensusDist;
    bool ok = FeatureAlignment::Matching<MeshType,FeatureType>(*vecF1, *vecF2, fkdTree, nBase, *baseVec, *matchesVec, k, sparseBaseDist, mutualErrDist, cb);
    if(!ok) return 2;   //exit code 2

    assert(baseVec->size()==1);  //now baseVec must hold exactly one base of features

    for(unsigned int j=0; j<matchesVec->size(); j++)
    {
        Matrix44<typename MeshType::ScalarType> tr;
        ok = FeatureAlignment::FindRigidTransformation(mFix.cm, mMov.cm, (*baseVec)[0], (*matchesVec)[j], nBase, tr, cb);
        if(!ok) return 3;  //exit code 3
    }

    //cleaning baseVec and matchesVec...
    FeatureAlignment::CleanTuplesVector<FeatureType>(baseVec, true);
    FeatureAlignment::CleanTuplesVector<FeatureType>(matchesVec, true);

    //cleaning vecF1 and vecF2...
    vecF1->clear(); delete vecF1; vecF1 = NULL;
    vecF2->clear(); delete vecF2; vecF2 = NULL;

    //Cleaning ANN structures
    FeatureAlignment::CleanKDTree(fkdTree, fdataPts, NULL, NULL, NULL, true);

    return 0; //all right
}

template<class MESH_TYPE>
int FilterFeatureAlignment::ConsensusOperation(MeshModel& mFix, MeshModel& mMov, float consensusDist, int fullConsensusSamples, bool normEq, bool paint, CallBackPos *cb)
{
    typedef MESH_TYPE MeshType;
    typedef typename MeshType::ScalarType ScalarType;
    typedef typename MeshType::VertexType VertexType;
    typedef GridStaticPtr<VertexType,ScalarType> MeshGrid;
    typedef tri::VertTmark<MeshType> MarkerVertex;

    //enables needed attributes. These are used by the getClosest functor.
    mFix.updateDataMask(MeshModel::MM_VERTMARK);
    mMov.updateDataMask(MeshModel::MM_VERTMARK);

    //normalize normals
    tri::UpdateNormals<MeshType>::PerVertexNormalized(mFix.cm);
    tri::UpdateNormals<MeshType>::PerVertexNormalized(mMov.cm);

    //if requested, group normals of m2 into 30 buckets. Buckets are used for Vertex Normal Equalization
    //in consensus. Bucketing is done here once for all to speed up consensus.
    vector<vector<int> >* normBuckets = NULL;
    if(normEq){
        normBuckets = FeatureAlignment::BucketVertexNormal<MeshType>(mMov.cm.vert, 30);
        assert(normBuckets!=NULL);
    }

    //variables to manage uniform grid
    MeshGrid grid;
    MarkerVertex markerFunctor;

    //builds the uniform grid containing fix mesh vesrtexes
    grid.Set(mFix.cm.vert.begin(), mFix.cm.vert.end());
    markerFunctor.SetMesh(&(mFix.cm));


    //compute consensus with requested parameters
    consensusDist = consensusDist*(mMov.cm.bbox.Diag()/100.0f);  //compute actual consensus distance depending on bbox diag
    int consensus = FeatureAlignment::Consensus(mFix.cm, mMov.cm, grid, markerFunctor, normBuckets, consensusDist, fullConsensusSamples, 0, 0, paint, cb);
    if(normBuckets!=NULL) delete normBuckets;  //clean up

    //return the consensus score
    return consensus;
}

template<class MESH_TYPE, class FEATURE_TYPE>
bool FilterFeatureAlignment::RansacOperation(MeshModel& mFix, MeshModel& mMov, int numFixFeatureSelected, int numMovFeatureSelected, int nBase, int ransacIter, float consensusDist, int fullConsensusSamples, float overlap, float shortConsOffset, float consOffset, float succOffset, int k, int g, int samplingStrategy, bool pickPoints, CallBackPos *cb)
{
    typedef MESH_TYPE MeshType;
    typedef FEATURE_TYPE FeatureType;
    typedef typename MeshType::ScalarType ScalarType;

    //enables needed attributes. MM_VERTMARK is used by the getClosest functor.
    mFix.updateDataMask(FeatureType::getRequirements() | MeshModel::MM_VERTMARK);
    mMov.updateDataMask(FeatureType::getRequirements() | MeshModel::MM_VERTMARK);

    //extract features
    vector<FeatureType*>* vecF1 = FeatureAlignment::extractFeatures<MeshType,FeatureType>(numFixFeatureSelected, mFix.cm, samplingStrategy, cb);
    vector<FeatureType*>* vecF2 = FeatureAlignment::extractFeatures<MeshType,FeatureType>(numMovFeatureSelected, mMov.cm, FeatureAlignment::UNIFORM_SAMPLING, cb);
    if(vecF1==NULL || vecF2==NULL) return false; //can't continue; features have not been computed!

    //do ransac and get best transformation matrix
    Matrix44<ScalarType> tr = FeatureAlignment::Ransac(mFix.cm, *vecF1, mMov.cm, *vecF2, ransacIter, consensusDist, fullConsensusSamples, overlap, shortConsOffset, consOffset, succOffset, nBase, k, g, pickPoints, true, cb);

    //apply transformation. If ransac don't find a good matrix, identity is returned; so nothing is wrong here...
    mMov.cm.Tr = tr * mMov.cm.Tr;

    //cleaning vecF1 and vecF2...
    vecF1->clear(); delete vecF1; vecF1 = NULL;
    vecF2->clear(); delete vecF2; vecF2 = NULL;

    return true;  //all right
}

template<class MESH_TYPE, class FEATURE_TYPE>
bool FilterFeatureAlignment::RansacDiagramOperation(MeshModel& mFix, MeshModel& mMov, int numFixFeatureSelected, int numMovFeatureSelected, int nBase, float consensusDist, int fullConsensusSamples, float overlap, float shortConsOffset, float consOffset, float succOffset, int k, int g, int samplingStrategy, int trials, int from, int to, int step, CallBackPos *cb)
{
    typedef MESH_TYPE MeshType;
    typedef FEATURE_TYPE FeatureType;
    typedef typename MeshType::ScalarType ScalarType;

    //variables needed for progress bar callback
    float progBar = 0.0f;
    float offset = 100.0f/(trials);

    //enables needed attributes. MM_VERTMARK is used by the getClosest functor.
    mFix.updateDataMask(FeatureType::getRequirements() | MeshModel::MM_VERTMARK);
    mMov.updateDataMask(FeatureType::getRequirements() | MeshModel::MM_VERTMARK);

    FILE* file = fopen("RansacDiagram.txt","w+");
    fprintf(file,"FixMesh#%s\nMoveMesh#%s\nFeature#%s\nVertices#%i\nOverlap#%.2f%%\nShort threshold#%.2f%%#%.2f%% total#\nAcceptance#%.2f%%#%.2f%% total#\nTrials#%i\n",mFix.fileName.c_str(),mMov.fileName.c_str(),FeatureType::getName(),mFix.cm.VertexNumber(),overlap,shortConsOffset,(shortConsOffset*overlap/100.0f),consOffset,(consOffset*overlap/100.0f),trials,g); fflush(file);
    fprintf(file,"Iterations#Num of Succ.#Tot Sec#Mean Time in Sec#Succ. Rate#FailRate/Sec\n0#0#0#0#0#0\n"); fflush(file);
    float prob = 0.0; int counterSucc = 0, dif = 0; time_t start, end;
    int iter=from;
    while(iter<=to)
    {
        for(int i=0; i<trials; i++){
            //callback handling
            if(cb){ progBar+=offset; cb(int(progBar),"Computing diagram..."); }

            time(&start);  //start timer

            //extract features
            vector<FeatureType*>* vecF1 = FeatureAlignment::extractFeatures<MeshType,FeatureType>(numFixFeatureSelected, mFix.cm, samplingStrategy);
            vector<FeatureType*>* vecF2 = FeatureAlignment::extractFeatures<MeshType,FeatureType>(numMovFeatureSelected, mMov.cm, FeatureAlignment::UNIFORM_SAMPLING);
            if(vecF1==NULL || vecF2==NULL) return false; //can't continue; features have not been computed!

            //do ransac
            FeatureAlignment::Ransac(mFix.cm, *vecF1, mMov.cm, *vecF2, iter, consensusDist, fullConsensusSamples, overlap, shortConsOffset, consOffset, succOffset, nBase, k, g, false, true, NULL, &counterSucc);

            time(&end);  //stop timer
            dif += int(difftime(end,start));

            //cleaning vecF1 and vecF2...
            vecF1->clear(); delete vecF1; vecF1 = NULL;
            vecF2->clear(); delete vecF2; vecF2 = NULL;
        }
        prob = (counterSucc/float(trials))*100.0f;
        //fail rate per sec is: (1-k)^(1/t) i.e k=prob succ in 1 iteration, t=sec elapsed to perform N ransac iterations
        fprintf(file,"%i#%i#%i#=%i/%i#=100*%i/%i#=%.0f\n",iter,counterSucc,dif,dif,trials,counterSucc,trials,100*std::pow(float((trials-counterSucc)/trials),float(trials/dif))); fflush(file);
        counterSucc = 0; dif = 0; progBar=0.0f;        
        iter+=step;
    }

    fclose(file);
    return true;  //all right
}

Q_EXPORT_PLUGIN(FilterFeatureAlignment)
