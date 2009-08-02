/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2007                                                \/)\/    *
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
#ifndef FEATURE_ALIGNMENT_H
#define FEATURE_ALIGNMENT_H

#include <meshlab/meshmodel.h>

#include <math.h>
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <ANN/ANN.h>
#include <vcg/math/point_matching.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/complex/trimesh/closest.h>
#include <vcg/complex/trimesh/point_sampling.h>
#include <vcg/complex/trimesh/overlap_estimation.h>
#include <meshlabplugins/edit_pickpoints/pickedPoints.h>

#include <qdatetime.h>

using namespace std;
using namespace vcg;

template<class MESH_TYPE, class FEATURE_TYPE> class FeatureAlignment
{
    public:
        typedef MESH_TYPE MeshType;
        typedef FEATURE_TYPE FeatureType;
        typedef typename MeshType::ScalarType ScalarType;
        typedef typename MeshType::CoordType	CoordType;
        typedef typename MeshType::VertexType	VertexType;
        typedef typename MeshType::FaceType	FaceType;
        typedef Matrix44<ScalarType> Matrix44Type;
        typedef OverlapEstimation<MeshType> ConsensusType;
        typedef typename ConsensusType::Parameters ConsensusParam;

        enum SamplingStrategies { UNIFORM_SAMPLING=0, POISSON_SAMPLING=1 };

    private:
        class VertexPointerSampler
        {
            public:

            MeshType* m;  //this is needed for advanced sampling (i.e poisson sampling)

            VertexPointerSampler(){ m = new MeshType(); m->Tr.SetIdentity(); m->sfn=0; }
            ~VertexPointerSampler(){ if(m) delete m; }
            vector<VertexType*> sampleVec;

            void AddVert(VertexType &p){ sampleVec.push_back(&p); }

            void AddFace(const FaceType &f, const CoordType &p){}

            void AddTextureSample(const FaceType &, const CoordType &, const Point2i &){}
        };

    public:
        class Parameters{
            public:

            int samplingStrategy;                       //strategy to extract features from range maps
            int numFixFeatureSelected;                  //number of feature sampled on mFix, to choose a base
            int numMovFeatureSelected;                  //number of feature sampled on mMov, to match base
            int ransacIter;                             //number of ransac iterations
            int nBase;                                  //number of features of a base
            int k;                                      //number of feature points picked by kNN search
            int fullConsensusSamples;                   //number of samples to perform full consensus
            int short_cons_samples;                     //number of samples to perform short consensus
            float consensusDist;                        //consensus distance
            float consensusNormalsAngle;                //holds the the consensus angle for normals, in gradients.
            float sparseBaseDist;                       //distance to pick sparse base
            float mutualErrDist;                        //distance to choose admissible beses on mMov
            float overlap;                              //overlap %; measure how much mMov overlaps mFix
            float succOffset;                           //consensus %, depending on overlap, to win ransac;
            float consOffset;                           //consensus %, depending on overlap, to win full consensus;
            bool normalEqualization;                    //to use normal equalization in consensus
            bool pickPoints;                            //to store bases/points into picked points attribute
            bool paint;                                 //to paint mMov according to consensus
            int maxNumFullConsensus;                    //max num of bases tested with full consensus procedure
            int maxNumShortConsensus;                   //max num of bases tested with short consensus procedure
            float mMovBBoxDiag;                         //mMov bbox diagonal; used to compute distances...
            void (*log)(int level,const char * f, ... );          //pointer to log function

            Parameters(MeshType& mFix, MeshType& mMov){
                setDefault(mFix, mMov);
            }

            void setDefault(MeshType& mFix, MeshType& mMov)
            {
                samplingStrategy = FeatureAlignment::UNIFORM_SAMPLING;
                numFixFeatureSelected = mFix.VertexNumber();
                numMovFeatureSelected = 250;
                ransacIter = 5000;
                nBase = 4;
                k = 150;
                fullConsensusSamples = 2500;
                short_cons_samples = 200;
                consensusDist = 2.0f;
                consensusNormalsAngle = 0.965f;   //15 degrees.
                sparseBaseDist = 2*consensusDist;
                mutualErrDist = consensusDist;
                overlap = 75.0f;
                succOffset = 90.0f;
                consOffset = 60.0f;
                normalEqualization = true;
                pickPoints = false;
                paint = false;
                maxNumFullConsensus = 15;
                maxNumShortConsensus = 50;
                mMovBBoxDiag = mMov.bbox.Diag();
                log = NULL;
            }
        };

        class Result{
            public:

            enum {ALIGNED, NOT_ALIGNED, FAILED};

            int exitCode;
            int errorCode;
            Matrix44Type tr;
            float bestConsensus;
            int numBasesFound;
            int numWonFullCons;
            int numMatches;
            int totalTime;
            int baseSelectionTime;
            int matchingTime;
            int branchBoundTime;
            int rankTime;
            int shortConsTime;
            int fullConsTime;
            int initTime;
            QString errorMsg;

            Result(){
                exitCode = NOT_ALIGNED;
                errorCode = -1;
                tr = Matrix44Type::Identity();
                bestConsensus = 0.0f;
                numBasesFound = 0;
                numWonFullCons = 0;
                numMatches = 0;
                totalTime = 0;
                initTime = 0;
                matchingTime = 0;
                shortConsTime = 0;
                fullConsTime = 0;
                baseSelectionTime = 0;
                branchBoundTime = 0;
                rankTime = 0;
                errorMsg = "An unkown error occurred.";
            }
        };

        struct CandidateType
        {
            int shortCons;
            float summedPointDist;
            Matrix44<ScalarType> tr;
            FeatureType** matchPtr;
            FeatureType** basePtr;

            CandidateType(FeatureType** base, FeatureType** match, int cons = -1, float spd = numeric_limits<float>::max()):basePtr(base),matchPtr(match),shortCons(cons),summedPointDist(spd){}
            static bool SortByDistance(CandidateType i,CandidateType j) { return (i.summedPointDist<j.summedPointDist); }
            static bool SortByScore(CandidateType i,CandidateType j) { return (i.shortCons>j.shortCons); }
        };

        vector<FeatureType*>* vecFFix;              //vector to hold pointers to features extracted from mFix
        vector<FeatureType*>* vecFMov;              //vector to hold pointers to features extracted from mMov
        Result res;                                 //structure to hold result and stats
        ANNpointArray fdataPts;                     //ANN structure to hold descriptors from which kdTree is build
        ANNkd_tree* fkdTree;                        //ANN kdTree structure
        ConsensusType cons;
        ConsensusParam consParam;

        //set pointers to NULL for safety and create struct for result and stats
        FeatureAlignment():fkdTree(NULL),vecFFix(NULL),vecFMov(NULL),res(Result()){}

        ~FeatureAlignment(){
            //Cleaning extracted features
            if(vecFFix) vecFFix->clear(); delete vecFFix; vecFFix = NULL;
            if(vecFMov) vecFMov->clear(); delete vecFMov; vecFMov = NULL;
            //Cleaning ANN structures
            FeatureAlignment::CleanKDTree(fkdTree, fdataPts, NULL, NULL, NULL, true);
        }

        Result& init(MeshType& mFix, MeshType& mMov, Parameters& param, CallBackPos* cb=NULL)
        {
            QTime timer, t;  //timers
            timer.start();

            //compute feature for mFix and mMov if they are not computed yet
            if(cb) cb(0,"Computing features...");
            typename FeatureType::Parameters p;
            FeatureType::SetupParameters(p);
            if(!FeatureType::HasBeenComputed(mFix)){
                t.start();
                if(!FeatureType::ComputeFeature(mFix, p)){
                    FeatureAlignment::setError(6,res);
                    return res;
                }
                if(param.log) param.log(3,"Features on mFix computed in %i msec.",t.elapsed());
            }
            if(!FeatureType::HasBeenComputed(mMov)){
                t.start();
                if(!FeatureType::ComputeFeature(mMov, p)){
                    FeatureAlignment::setError(6,res);
                    return res;
                }
                if(param.log) param.log(3,"Features on mMov computed in %i msec.",t.elapsed());
            }

            //initialize a static random generator used later on to select bases
            tri::SurfaceSampling<MeshType, VertexPointerSampler>::SamplingRandomGenerator().initialize(time(NULL));

            //extract features
            vecFFix = FeatureAlignment::extractFeatures(param.numFixFeatureSelected, mFix, FeatureAlignment::UNIFORM_SAMPLING);
            vecFMov = FeatureAlignment::extractFeatures(param.numMovFeatureSelected, mMov, param.samplingStrategy);
            if( (vecFFix==NULL) | (vecFMov==NULL) ){
                //can't continue; features have not been computed! set error stuff and return
                FeatureAlignment::setError(1,res);
                return res;
            }                        

            //copy descriptors of mFix into ANN structures, then build kdtree...
            FeatureAlignment::SetupKDTreePoints(*vecFFix, &fdataPts, FeatureType::getFeatureDimension());
            fkdTree = new ANNkd_tree(fdataPts,vecFFix->size(),FeatureType::getFeatureDimension());
            assert(fkdTree);                                                        

            //consensus structure initialization
            cons.SetFix(mFix);
            cons.SetMove(mMov);
            consParam.normalEqualization = param.normalEqualization;
            cons.Init(consParam);

            res.initTime = timer.elapsed();

            return res;
        }

        Result& align(MeshType& mFix, MeshType& mMov, Parameters& param, CallBackPos *cb=NULL)
        {
            QTime tot_timer, timer;  //timers
            tot_timer.start();

            assert(vecFMov); assert(vecFFix);

            if(param.nBase>int(vecFMov->size())){ setError(2,res); return res; }  //not enough features to pick a base
            if(param.k>int(vecFFix->size())){ setError(3, res); return res; }     //not enough features to pick k neighboors

            //normalize normals
            assert(mFix.HasPerVertexNormal());  //mesh don't have per vertex normals!!!
            assert(mMov.HasPerVertexNormal());
            tri::UpdateNormals<MeshType>::NormalizeVertex(mFix);
            tri::UpdateNormals<MeshType>::NormalizeVertex(mMov);

            //variables declaration
            res.exitCode = Result::NOT_ALIGNED;
            int bestConsensus = 0;                                              //best consensus score                        
            int cons_succ = int((param.consOffset*param.overlap/100.0f)*(param.fullConsensusSamples/100.0f));               //number of vertices to win consensus
            int ransac_succ = int((param.succOffset*param.overlap/100.0f)*(param.fullConsensusSamples/100.0f));             //number of vertices to win ransac
            int bestConsIdx = -1;                       //index of the best candidate, needed to store picked points

            //set up params for consensus
            consParam.consensusDist=param.consensusDist;
            consParam.consensusNormalsAngle=param.consensusNormalsAngle;
            consParam.paint = param.paint;

            //auxiliary vectors needed inside the loop
            vector<FeatureType**>* baseVec = new vector<FeatureType**>();
            vector<vector<FeatureType*> > matches;
            vector<CandidateType>* candidates = new vector<CandidateType>();

            //variables needed for progress bar callback
            float progBar = 1.0f;
            if(cb) cb(int(progBar),"Selecting bases...");

            //loop of ransac iterations to find all possible matches at once
            timer.start();
            for(int i = 0; i<param.ransacIter; i++)
            {
                int errCode = FeatureAlignment::SelectBase(*vecFMov,*baseVec, param);
                if(errCode) continue; //can't find a base, skip this iteration
            }
            res.numBasesFound=baseVec->size();
            res.baseSelectionTime = timer.elapsed();
            if(param.log) param.log(3,"%i bases found in %i msec.", baseVec->size(), res.baseSelectionTime);

            for(int i = 0; i<baseVec->size(); i++)
            {
                timer.start();
                int errCode = FeatureAlignment::Matching(*vecFFix, *vecFMov, fkdTree, (*baseVec)[i], matches, param);
                res.matchingTime+= timer.elapsed();

                timer.start();
                errCode = FeatureAlignment::BranchAndBound((*baseVec)[i], matches, *candidates, param);
                matches.clear(); //make matches vec ready for another iteration
                res.branchBoundTime+= timer.elapsed();

                if(cb){ progBar+=(20.0f/baseVec->size()); cb(int(progBar),"Matching..."); }
            }
            res.numMatches = candidates->size();
            if(param.log) param.log(3,"%matching performed in %i msec.", res.matchingTime);
            if(param.log) param.log(3,"%Branch&Bound performed in %i msec.", res.branchBoundTime);
            if(param.log) param.log(3,"%i candidates found in %i msec.", candidates->size(), res.matchingTime+res.branchBoundTime);

            timer.start();
            for(unsigned int j=0; j<candidates->size(); j++)
            {
                CandidateType& currCandidate = (*candidates)[j];

                //computes the rigid transformation matrix that overlaps the two points sets
                Matrix44Type tr;
                int errCode = FeatureAlignment::FindRigidTransformation(mFix, mMov, currCandidate.basePtr, currCandidate.matchPtr, param.nBase, tr);
                if(errCode) continue; //can't find a rigid transformation, skip this iteration

                currCandidate.tr = tr;  //store transformation

                Matrix44Type oldTr = ApplyTransformation(mMov, tr);       //apply transformation
                currCandidate.summedPointDist = SummedPointDistances(mFix, mMov, currCandidate.basePtr, currCandidate.matchPtr, param.nBase);  //compute and store the sum of points distance
                ResetTransformation(mMov, oldTr);                         //restore old tranformation

                if(cb){ progBar+=(20.0f/candidates->size()); cb(int(progBar),"Ranking candidates..."); }
            }
            //sort candidates by summed point distances
            sort(candidates->begin(), candidates->end(), CandidateType::SortByDistance);

            res.rankTime = timer.elapsed();
            if(param.log) param.log(3,"Ranking performed in %i msec.", res.rankTime);

            //variable needed for progress bar callback
            float offset = (20.0f/math::Min(param.maxNumShortConsensus,int(candidates->size())));

            //evaluetes at most first g candidates
            timer.start();
            for(unsigned int j=0; j<candidates->size() && j<param.maxNumShortConsensus; j++)
            {
                CandidateType& currCandidate = (*candidates)[j];
                Matrix44Type currTr = currCandidate.tr;              //load the right transformation
                Matrix44Type oldTr = ApplyTransformation(mMov, currTr); //apply transformation
                consParam.samples=param.short_cons_samples;                
                currCandidate.shortCons = cons.Check(consParam);     //compute short consensus
                ResetTransformation(mMov, oldTr);                    //restore old tranformation

                if(cb){ progBar+=offset; cb(int(progBar),"Short consensus..."); }
            }
            res.shortConsTime = timer.elapsed();
            if(param.log) param.log(3,"%i short consensus performed in %i msec.",(param.maxNumShortConsensus<int(candidates->size()))? param.maxNumShortConsensus : candidates->size(), res.shortConsTime);

            //sort candidates by short consensus
            sort(candidates->begin(), candidates->end(), CandidateType::SortByScore);

            //variables needed for progress bar callback
            offset = (20.0f/param.maxNumFullConsensus);

            //evaluetes at most maxNumFullConsensus candidates beetween those who won short consensus
            timer.start();
            for(int j=0; j<param.maxNumFullConsensus; j++)
            {
                CandidateType& currCandidate = (*candidates)[j];
                Matrix44Type currTr = currCandidate.tr;              //load the right transformation
                Matrix44Type oldTr = ApplyTransformation(mMov, currTr); //apply transformation
                consParam.samples=param.fullConsensusSamples;                                
                consParam.threshold = param.consOffset*param.overlap/100.0f;
                consParam.bestScore = bestConsensus;
                int consensus = cons.Check(consParam);              //compute full consensus
                ResetTransformation(mMov, oldTr);                   //restore old tranformation

                //verify if transformation has the best consensus and update the result
                if( (consensus >= cons_succ))
                {
                    res.numWonFullCons++;
                    if(consensus > bestConsensus)
                    {
                        res.exitCode = Result::ALIGNED;
                        res.tr = currTr;
                        bestConsensus = consensus;
                        res.bestConsensus = 100.0f*(float(bestConsensus)/param.fullConsensusSamples);
                        bestConsIdx = j;
                        if(consensus >= ransac_succ) break;    //very good alignment, no more iterations are done
                    }
                }//else we store the consensus score anyway, so even if we fail we can provide an estimation
                else{
                    if(100*(float(consensus)/param.fullConsensusSamples)>res.bestConsensus)
                        res.bestConsensus = 100.0f*(float(consensus)/param.fullConsensusSamples);
                }
                if(cb){ progBar+=offset; cb(int(progBar),"Full consensus..."); }
            }
            res.fullConsTime = timer.elapsed();
            res.totalTime = tot_timer.elapsed();
            if(param.log) param.log(3,"%i full consensus performed in %i msec.", param.maxNumFullConsensus, res.fullConsTime);

            //if flag 'points' is checked, clear old picked points and save the new points
            if(param.pickPoints){
                if(bestConsIdx!=-1) StorePickedPoints(mFix, (*candidates)[bestConsIdx].basePtr, mFix.Tr, param.nBase, "B");
                else ClearPickedPoints(mFix);
                if(bestConsIdx!=-1) StorePickedPoints(mMov, (*candidates)[bestConsIdx].matchPtr, (*candidates)[bestConsIdx].tr * mMov.Tr, param.nBase, "M");
                else ClearPickedPoints(mMov);
            }

            //Clean structures...
            for(int i=0; i<baseVec->size();i++){ delete[] (*baseVec)[i];}
            for(int i=0; i<candidates->size();i++){ delete[] (*candidates)[i].basePtr; delete[] (*candidates)[i].matchPtr;}
            delete baseVec; delete candidates;

            return res;
        }     

        /*******************************************************************************************
        ERROR TABLE:
        error code 1 : Feature has not been computed - init
        error code 2 : Features extracted are not enough to pick a base - Matching
        error code 3 : Features extracted are not enough to pick k neighboors - Matching
        error code 4 : Base isn't enough sparse - Matching
        error code 5 : Error while computing rigid transformation - FindRigidTransform
        error code 6 : Error while computing features - Init
        *******************************************************************************************/
        static QString getErrorMsg(int code)
        {
            switch(code){
                case 1: return QString("Features have not been computed for one or more meshes.");
                case 2: return QString("Features extracted are not enough to pick a base.");
                case 3: return QString("Features extracted are not enough to pick k neighboors.");
                case 4: return QString("Base isn't enough sparse.");
                case 5: return QString("Error while computing rigid transformation.");
                case 6: return QString("Error while computing features.");
                default: return QString("An unkown error occurred.");
            }
        }

        static void setError(int errorCode, Result& res)
        {
            res.exitCode = Result::FAILED;
            res.errorCode = errorCode;
            res.errorMsg = FeatureAlignment::getErrorMsg(errorCode);
        }

        static FEATURE_TYPE** FeatureUniform(vector<FEATURE_TYPE*>& vecF, int* sampleNum)
        {
            typedef typename vector<FeatureType*>::iterator FeatureIterator;

            if(*sampleNum>=int(vecF.size())) *sampleNum = vecF.size();

            vector<FeatureType*> vec;
            for(FeatureIterator fi=vecF.begin();fi!=vecF.end();++fi) vec.push_back(*fi);

            assert(vec.size()==vecF.size());

            unsigned int (*p_myrandom)(unsigned int) = tri::SurfaceSampling<MeshType, VertexPointerSampler>::RandomInt;
            std::random_shuffle(vec.begin(),vec.end(), p_myrandom);

            FeatureType** sampledF = new FeatureType*[*sampleNum];
            for(int i =0; i<*sampleNum; ++i) sampledF[i] = vec[i];

            return sampledF;
        }

        static FEATURE_TYPE** FeaturePoisson(MESH_TYPE& samplingMesh, int* sampleNum)
        {
            typedef typename MeshType::template PerVertexAttributeHandle<FeatureType> PVAttributeHandle;

            VertexPointerSampler samp = VertexPointerSampler();
            SampleVertPoissonDisk(samplingMesh, samp, *sampleNum);
            *sampleNum = samp.sampleVec.size();  //poisson sampling can return untill 50% more of requested samples!
            PVAttributeHandle fh = FeatureAlignment::GetFeatureAttribute(samplingMesh);
            if(!tri::Allocator<MeshType>::IsValidHandle(samplingMesh,fh)) return NULL;
            FeatureType** sampler = new FeatureType*[*sampleNum];
            for(int i=0; i<*sampleNum; i++) sampler[i]=&(fh[samp.sampleVec[i]]);
            return sampler;
        }

        static void SampleVertPoissonDisk(MESH_TYPE& m, VertexPointerSampler& sampler, int sampleNum)
        {
            typedef MESH_TYPE MeshType;

            //first of call sampling procedure we have to make sure that mesh has a bbox
            tri::UpdateBounding<MeshType>::Box(m);

            //setup parameters
            float radius = tri::SurfaceSampling<MeshType,VertexPointerSampler>::ComputePoissonDiskRadius(m,sampleNum);
            typename tri::SurfaceSampling<MeshType,VertexPointerSampler>::PoissonDiskParam pp;

            //poisson samplig need a support mesh from which it takes vertexes; we use the same input mesh
            //as support mesh, so we are sure that pointer to vertex of input mesh are returned
            //perform sampling: number of samples returned can be 50% greater of the requested amount
            tri::SurfaceSampling<MeshType,VertexPointerSampler>::Poissondisk(m, sampler, m, radius, pp);
        }                

        static typename MESH_TYPE::template PerVertexAttributeHandle<FEATURE_TYPE> GetFeatureAttribute(MESH_TYPE& m, bool createAttribute = false)
        {
            typedef MESH_TYPE MeshType;
            typedef FEATURE_TYPE FeatureType;
            typedef typename MeshType::template PerVertexAttributeHandle<FeatureType> PVAttributeHandle;

            //checks if the attribute exist
            if(!tri::HasPerVertexAttribute(m,std::string(FeatureType::getName()))){
                //if createAttribute is true and attribute doesn't exist, we add it; else return a NULL handle
                if(createAttribute) tri::Allocator<MeshType>::template AddPerVertexAttribute<FeatureType>(m,std::string(FeatureType::getName()));
                else return PVAttributeHandle(NULL,0);
            }
            //now we can get a handle to the attribute and return it
            return tri::Allocator<MeshType>::template GetPerVertexAttribute<FeatureType> (m,std::string(FeatureType::getName()));
        }        

        static vector<FEATURE_TYPE*>* extractFeatures(int numRequested, MESH_TYPE &m, int samplingStrategy, CallBackPos *cb = NULL)
        {
            typedef MESH_TYPE MeshType;
            typedef FEATURE_TYPE FeatureType;

            //Create a vector to hold extracted features
            vector<FeatureType*>* featureSubset = new vector<FeatureType*>();

            //extract numRequested features according the sampling method.
            //If feature has not been computed previously, return a NULL pointer
            if(!FeatureType::Subset(numRequested, m, *featureSubset, samplingStrategy, cb)) return NULL;

            return featureSubset;
        }

        static void SetupKDTreePoints(vector<FEATURE_TYPE*>& vecF, ANNpointArray* dataPts, int pointDim)
        {
            //fill dataPts with feature's descriptions in vecF
            *dataPts = annAllocPts(vecF.size(), pointDim);		    // allocate data points
            for(unsigned int i = 0; i < vecF.size(); i++){
                for (int j = 0; j < pointDim; j++)
                    (*dataPts)[i][j] = (ANNcoord)(vecF[i]->description[j]);
            }
        }

        static void CleanKDTree(ANNkd_tree* kdTree, ANNpointArray dataPts, ANNpointArray queryPts, ANNidxArray idx, ANNdistArray dists, bool close)
        {
            //Cleaning ANN structures
            if(kdTree){ delete kdTree; kdTree = NULL; }
            if(dataPts) annDeallocPts(dataPts);
            if(queryPts) annDeallocPts(queryPts);
            if(idx){ delete [] idx; idx = NULL; }
            if(dists){ delete [] dists; dists = NULL; }
            if(close) annClose();				//done with ANN; clean memory shared among all the kdTrees
        }

        static int SelectBase(vector<FEATURE_TYPE*>& vecFMov, vector<FEATURE_TYPE**>& baseVec, Parameters& param)
        {
            assert(param.nBase<=int(vecFMov.size()));  //not enough features to pick a base
            float baseDist = param.sparseBaseDist*(param.mMovBBoxDiag/100.0f); //compute needed params

            FeatureType** base = FeatureAlignment::FeatureUniform(vecFMov, &(param.nBase)); //randomly chooses a base of features from vecFFix
            if(!VerifyBaseDistances(base, param.nBase, baseDist)) return 4; //if base point are not enough sparse, skip
            baseVec.push_back(base);
            return 0;
        }

        static int Matching(vector<FEATURE_TYPE*>& vecFFix, vector<FEATURE_TYPE*>& vecFMov, ANNkd_tree* kdTree, FEATURE_TYPE** base, vector<vector<FeatureType*> >& matches, Parameters& param, CallBackPos *cb=NULL)
        {
            float pBar = 0, offset = 100.0f/param.nBase;  //used for progresss bar callback
            if(cb) cb(0, "Matching...");

            assert(param.k<=int(vecFFix.size()));       //not enough features in kdtree to pick k neighbors
            assert((int)vecFFix.size()>=param.nBase);
            assert(matches.size()==0);                  //matches vectors have to be provided empty

            //fill fqueryPts with feature's descriptions in base
            ANNpointArray fqueryPts;
            FeatureAlignment::SetupKDTreeQuery(base, param.nBase, &fqueryPts, FeatureType::getFeatureDimension());

            //additional variables needed
            ANNidxArray fnnIdx = new ANNidx[param.k];						    // allocate near neigh indices
            ANNdistArray fdists = new ANNdist[param.k];						    // allocate near neigh dists
            vector<FeatureType*> neighbors;
            //foreach feature in the base find the best matching using fkdTree
            for(int i = 0; i < param.nBase; i++)
            {
                kdTree->annkSearch(fqueryPts[i], param.k, fnnIdx, fdists);  //search the k best neighbor for the current point

                assert(fdists[0]!=ANN_DIST_INF);  //if this check fails, it means that no feature have been found!

                for(int j=0; j<param.k; j++) neighbors.push_back(vecFFix[fnnIdx[j]]); //store all features
                matches.push_back(neighbors);           //copy neighbors vec into matches vec
                neighbors.clear();                      //make neighbors vec ready for another iteration

                if(cb){ pBar+=offset; cb((int)pBar, "Matching..."); }
            }

            assert(int(matches.size())==param.nBase);

            //Cleaning ANN structures
            FeatureAlignment::CleanKDTree(NULL, NULL, fqueryPts, fnnIdx, fdists, false);

            return 0;
        }

        static int BranchAndBound(FEATURE_TYPE** base, vector<vector<FeatureType*> >& matches, vector<CandidateType>& candidates, Parameters& param)
        {
            //compute needed params
            float errDist = param.mutualErrDist*(param.mMovBBoxDiag/100.0f);

            //branch and bound
            int curSolution[param.nBase];
            for(int i=0; i<param.nBase; i++) curSolution[i] = 0;              //initialization
            FeatureAlignment::Match(base, matches, param.nBase, 0, curSolution, candidates, errDist);
            return 0;
        }

        static int FindRigidTransformation(MESH_TYPE& mFix, MESH_TYPE& mMov, FEATURE_TYPE* fixF[], FEATURE_TYPE* movF[], int nBase, Matrix44<typename MESH_TYPE::ScalarType>& tr, CallBackPos *cb=NULL)
        {
            if(cb) cb(0,"Computing rigid transformation...");

            //computes the rigid transformation matrix that overlaps the two points sets
            vector< Point3<ScalarType> > fixPoints;
            vector< Point3<ScalarType> > movPoints;

            for(int i = 0; i<nBase; i++)
            {
                movPoints.push_back(mMov.Tr * fixF[i]->pos); //points on mFix
                fixPoints.push_back(mFix.Tr * movF[i]->pos); //points on mMov
            }

            //this compute transformation that brings movPoints over fixPoints
            bool ok = PointMatching<ScalarType>::ComputeRigidMatchMatrix(tr, fixPoints, movPoints);
            if(!ok) return 5; //Error while computing rigid transformation

            return 0;
        }

        static void AddPickedPoints(MESH_TYPE& m, vector<FEATURE_TYPE*>& vecF, char* prefix = NULL)
        {
            typedef MESH_TYPE MeshType;
            typedef FEATURE_TYPE FeatureType;
            typedef typename MeshType::template PerMeshAttributeHandle<PickedPoints*> PMAttributeHandle;

            PMAttributeHandle pph;
            //if attribute doesn't exist, we add it; then we can get a handle to the attribute
            if(!tri::HasPerMeshAttribute(m, PickedPoints::Key)){
                pph = tri::Allocator<MeshType>::template AddPerMeshAttribute<PickedPoints*>(m,PickedPoints::Key);
                pph() = new PickedPoints();
            }
            else pph = tri::Allocator<MeshType>::template GetPerMeshAttribute<PickedPoints*>(m, PickedPoints::Key);

            for(unsigned int i=0; i<vecF.size(); i++){
                //build up a point name made of an id and feature value...
                char name[100]; int j, limit;
                if(prefix==NULL) prefix="";
                limit = sprintf(name,"%s%i - ",prefix,i);
                for(j=0; j<FeatureType::getFeatureDimension()-1; j++){
                    limit += sprintf(name+limit,"%.2f,",vecF[i]->description[j]);
                }
                sprintf(name+limit,"%.2f",vecF[i]->description[j]);

                pph()->addPoint(name, m.Tr * vecF[i]->pos, true); //add point
            }
        }

        static void ClearPickedPoints(MESH_TYPE& m)
        {
            typedef MESH_TYPE MeshType;
            typedef typename MeshType::template PerMeshAttributeHandle<PickedPoints*> PMAttributeHandle;

            PMAttributeHandle pph;
            //we get a handle to the attribute if it exists
            if(!tri::HasPerMeshAttribute(m, PickedPoints::Key)) return;
            pph = tri::Allocator<MeshType>::template GetPerMeshAttribute<PickedPoints*>(m, PickedPoints::Key);

            //delete previous picked points
            (pph()->getPickedPointVector())->clear();
        }

    private:

        static void StorePickedPoints(MESH_TYPE& m, FEATURE_TYPE* vecF[], Matrix44<typename MESH_TYPE::ScalarType> tr, int size, char* prefix = NULL)
        {
            typedef MESH_TYPE MeshType;
            typedef FEATURE_TYPE FeatureType;
            typedef typename MeshType::template PerMeshAttributeHandle<PickedPoints*> PMAttributeHandle;

            PMAttributeHandle pph;
            //if attribute doesn't exist, we add it; then we can get a handle to the attribute
            if(!tri::HasPerMeshAttribute(m, PickedPoints::Key)){
                pph = tri::Allocator<MeshType>::template AddPerMeshAttribute<PickedPoints*>(m,PickedPoints::Key);
                pph() = new PickedPoints();
            }
            else pph = tri::Allocator<MeshType>::template GetPerMeshAttribute<PickedPoints*>(m, PickedPoints::Key);

            (pph()->getPickedPointVector())->clear();  //clear old contents

            for(int i=0; i<size; i++){
                //build up a point name made of an id and feature value...
                char name[100]; int j, limit;
                if(prefix==NULL) prefix = "";
                limit = sprintf(name,"%s%i - ",prefix,i);
                for(j=0; j<FeatureType::getFeatureDimension()-1; j++){
                    limit += sprintf(name+limit,"%.2f,",vecF[i]->description[j]);
                }
                sprintf(name+limit,"%.2f",vecF[i]->description[j]);

                pph()->addPoint(name, tr * vecF[i]->pos, true); //add point
            }
        }

        static void SetupKDTreeQuery(FEATURE_TYPE* queryPointsArray[], int queryPointsSize, ANNpointArray* queryPts, int pointDim)
        {
            //fill query points with feature's description
            *queryPts = annAllocPts(queryPointsSize, pointDim);      //allocate query points
            for (int i = 0; i < queryPointsSize; i++){
                for (int j = 0; j < pointDim; j++)
                    (*queryPts)[i][j] = (ANNcoord)(queryPointsArray[i]->description[j]);
            }
        }       

        static void Match(FEATURE_TYPE** base, vector<vector<FEATURE_TYPE*> >& matches, int nBase, int level, int curSolution[], vector<CandidateType>& candidates, float errDist, CallBackPos *cb = NULL)
        {
            assert(level<nBase);

            for(unsigned int j=0; j<matches[level].size(); j++){
                curSolution[level] = j;
                if(MatchSolution(base, matches, level, curSolution, errDist)){
                    if(level==nBase-1){
                        FeatureType** solution = new FeatureType*[nBase];
                        for(int h=0; h<nBase; h++){
                            solution[h] = matches[h][curSolution[h]];
                        }
                        candidates.push_back(CandidateType(base,solution));
                    }
                    else
                        Match(base, matches, nBase, level+1, curSolution, candidates, errDist);
                }
            }
            curSolution[level] = 0;
        }

        static bool MatchSolution(FEATURE_TYPE** base, vector<vector<FEATURE_TYPE*> >& matches, int level, int curSolution[], float errDist)
        {
            if (level==0) return true;

            for(int j=0; j<level; j++){
                float distF = Distance(base[level]->pos, base[j]->pos);
                float distM = Distance(matches[level][curSolution[level]]->pos, matches[j][curSolution[j]]->pos);
                if( math::Abs(distF-distM)>errDist) return false;
            }
            return true;
        }

        //Verify that all points in a base are enough sparse        
        static bool VerifyBaseDistances(FEATURE_TYPE* base[], int nBase, float baseDist )
        {
            typedef FEATURE_TYPE FeatureType;

            for(int i=0; i<nBase-1; i++){
                for(int j=i+1; j<nBase; j++){
                    if(Distance(base[i]->pos, base[j]->pos)<=baseDist) return false;
                }
            }
            return true;
        }

        static Matrix44<typename MESH_TYPE::ScalarType> ApplyTransformation(MESH_TYPE& m, Matrix44<typename MESH_TYPE::ScalarType>& tr)
        {
            typedef MESH_TYPE MeshType;
            typedef typename MeshType::ScalarType ScalarType;

            Matrix44<ScalarType> oldMat = m.Tr;    //save old transformation matrix of m
            m.Tr = tr * m.Tr;                      //apply transformation
            return oldMat;                         //return the old transformation matrix
        }

        static void ResetTransformation(MESH_TYPE& m, Matrix44<typename MESH_TYPE::ScalarType>& tr)
        {
            m.Tr = tr;
        }

        static float SummedPointDistances(MESH_TYPE& mFix, MESH_TYPE& mMov, FEATURE_TYPE* baseF[], FEATURE_TYPE* matchF[], int nBase)
        {
            typedef MESH_TYPE MeshType;
            typedef typename MeshType::ScalarType ScalarType;

            Matrix33<ScalarType> matMov(mMov.Tr,3); //3x3 matrix needed totransform normals
            Matrix33<ScalarType> matFix(mFix.Tr,3); //3x3 matrix needed totransform normals
            Point3<ScalarType> fixNrm, movNrm;    //normals
            float spd, snd, result = 0.0f;

            for(int i=0; i<nBase;i++){
                //transform normals properly; normals inside features are yet normalized
                fixNrm = matFix * matchF[i]->normal;
                movNrm = matMov * baseF[i]->normal;

                //compute distance beetween points and then beetween normals
                spd = Distance( mFix.Tr * matchF[i]->pos, mMov.Tr * baseF[i]->pos );
                snd = math::Clamp(1-fixNrm.dot(movNrm),0.0f,1.0f);

                //weight distances with normals and update result
                result+=spd*snd;
            }
            return result;
        }
};
#endif //FEATURE_ALIGNMENT_H
