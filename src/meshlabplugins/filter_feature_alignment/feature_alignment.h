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

#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/vertex/component.h>
#include <vcg/simplex/face/base.h>
#include <vcg/simplex/face/component.h>
#include <vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/container/simple_temporary_data.h>
#include <vcg/complex/trimesh/clustering.h>

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

//needed stuffs to declare a mesh type ad hoc for clustering features. Used inside Matching().
class MyEdge;
class MyFace;
class MyVertex : public VertexSimp2<MyVertex, MyEdge, MyFace, vertex::Coord3f, vertex::Normal3f, vertex::BitFlags > {};
class MyFace: public vcg::FaceSimp2<MyVertex,MyEdge,MyFace, vcg::face::VertexRef>{};
class MyMesh : public tri::TriMesh<vector<MyVertex>, vector<MyFace> >{};

/** \brief This class provides a generic framework to automatically align two range maps or point clouds using
  * a feature based approach.
  *
  * The framework select a base of \c nBase features from the mesh \c mMov and for each feature finds the \c k
  * more similar features on the mesh \c mFix . Then the space of all the \c nBase tuples is searched with an
  * efficient Branch & Bound algorithm and promising matching bases are keeped. This steps are repeated \c ransacIter
  * times. At this point all the transformations are computed and tested. An alignment is considered good if the
  * estimated overlap between meshes exceeds the \c consOffset*overlap %,  otherways the alignment is discarded.
  * The framework returns the best alignment found.
  *
  * The class is templated on the \c MESH_TYPE and on the \c FEATURE_TYPE . \c FEATURE_TYPE is a templated class
  * that exposes a specific interface to compute and extract features. In this way the framework can abstract
  * from features implementation's details and work with different kind of features.
  *
  * \par Important Note:
  * For a safe use is very important that the init() function is called only once. If you need to change the input range maps
  * you <em>must</em> destroy the old FeatureAlignment object and create a new one, then you can call the init() on the new
  * object. Calling many times the init() on the same object will couse big memory leaks, becouse the init() allocates
  * global structures but is the destructor that deallocates them.
  */
template<class MESH_TYPE, class FEATURE_TYPE> class FeatureAlignment
{
    public:
        typedef MESH_TYPE MeshType;
        typedef FEATURE_TYPE FeatureType;
        typedef typename MeshType::ScalarType ScalarType;
        typedef typename MeshType::CoordType CoordType;
        typedef typename MeshType::VertexType VertexType;
        typedef typename MeshType::FaceType FaceType;
        typedef Matrix44<ScalarType> Matrix44Type;
        typedef typename MeshType::VertexIterator VertexIterator;
        typedef OverlapEstimation<MeshType> ConsensusType;
        typedef typename ConsensusType::Parameters ConsensusParam;

        /// Different kind of sampling allowed on features.
        enum SamplingStrategies { UNIFORM_SAMPLING=0, POISSON_SAMPLING=1 };

    private:
        /// Inner class needed to perform sampling on pointers to vertexes.
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
        /// Inner class to hold framework's parameters; this avoids endless parameters' list inside functions.
        class Parameters{
            public:

            int samplingStrategy;                       ///< Strategy to extract features from range maps. \c UNIFORM_SAMPLING is the default; \c POISSON_SAMPLING is highly time consuming and doesn't give best results.
            int numFixFeatureSelected;                  ///< Number of feature sampled on \c mFix to match base. As default all the features are matched.
            int numMovFeatureSelected;                  ///< Number of feature sampled on \c mMov to choose a base. Default is \c 250.
            int ransacIter;                             ///< Number of iterations. More iterations means higher success probability, but of course it requires more time.
            int nBase;                                  ///< Number of features of a base. It should be at least \c 3, the default is \c 4, higher values doesn't seem to give higher success probability nor best quality in alignment.
            int k;                                      ///< Number of features picked by kNN search. Higher values should provide higher success probability, especially in case of mesh with many vertexes and/or low discriminant features. The drawback is a critical growth of computation time. Default is \c 75 .
            int fullConsensusSamples;                   ///< Number of samples used to perform full consensus. Default is \c 2500 .
            int short_cons_samples;                     ///< Number of samples used to perform short consensus. Default is \c 200 .
            float consensusDist;                        ///< Consensus distance expressed as percentage of the BBox diagonal of \c mMov . Higher values should weaken the consensus test allowing false positive alignment. Deafault is \c 2% .
            float consensusNormalsAngle;                ///< Holds the the consensus angle for normals, in radians. This parameter is strictly related to \c consensusDist ; to weaken the consensus test both values should be increased. Default is \c 0.965 Rad, about \c 15 Deg.
            float sparseBaseDist;                       ///< Distance used to select sparse bases from \c mMov . Distance is espressed as a percentage of BBox diagonal of \c mMov . Default is \c 2*consensusDist .
            float mutualErrDist;                        ///< Distance used while exploring the matching bases solutions' space to establish if a matching base is admissible or not. Distance is espressed as a percentage of BBox diagonal of \c mMov . Default is \c consensusDist .
            float overlap;                              ///< Overlap percentage; it measure how much \c mMov overlaps with \c mFix . Is very important provide a pecise estimation 'couse too low values can give in false positive alignments, instead too high values can turn down critically the success probability. Default is \c 75 , but user should always provide an appropriate value.
            float succOffset;                           ///< Consensus percentage to early interrupt processing. The alignment found is considered good enough to stop the process. Percentage is related to \c overlap , i.e \c succOffset=100 is  equal to \c overlap. Default is \c 90 .
            float consOffset;                           ///< Consensus percentage to overcome to win consensus test. Percentage is related to \c overlap , i.e \c consOffset=100 is  equal to \c overlap. Default is \c 90 .
            bool normalEqualization;                    ///< If \c true , normal equalization sampling is used during consensus test, otherways uniform sampling is used. \c true is the default.
            bool pickPoints;                            ///< Utility that stores a base and its match into picked points attribute. If \c true points are stored and they can be displayed later.
            bool paint;                                 ///< Utility to paint \c mMov according to consensus. If \c true vertexes of \c mMov are painted accordingly to the best consensus found. To get a detailed description of how the mesh is colored see vcg/complex/trimesh/overlap_estimation.h.
            int maxNumFullConsensus;                    ///< Indicates the max number of bases tested with full consensus procedure. Defailt is \c 15.
            int maxNumShortConsensus;                   ///< Indicates the max number of bases tested with short consensus procedure. Default is \c 50.
            float mMovBBoxDiag;                         ///< BBox diagonal of \c mMov. This is used to compute parameters such as \c consensusDist, \c sparseBaseDist etc.
            void (*log)(int level,const char * f, ... );///< Pointer to log function. Used to print stat infos.

            /// Default constructor. Set all the parameters to their default values.
            Parameters(MeshType& mFix, MeshType& mMov){
                setDefault(mFix, mMov);
            }

            private:
            /// Set parameters to the default values
            void setDefault(MeshType& mFix, MeshType& mMov)
            {
                samplingStrategy = FeatureAlignment::UNIFORM_SAMPLING;
                numFixFeatureSelected = mFix.VertexNumber();
                numMovFeatureSelected = 250;
                ransacIter = 500;
                nBase = 4;
                k = 75;
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

        /// Class to hold the result of the alignment process. It holds many infos about stats and errors too.
        class Result{
            public:

            /// Exit codes of the alignment procedure. \arg \c ALIGNED Framework has found a good alignment. \arg \c NOT_ALIGNED Framework doesn't find a good alignment. \arg \c FAILED Something wrong happened during alignment process.
            enum {ALIGNED, NOT_ALIGNED, FAILED};

            int exitCode;           ///< Holds the exit code of the consensus procedure. It can be tested to know if the process has been completed successfully.
            int errorCode;          ///< It holds an integer code related to an error occured during the process. If no errors occured it should hold \c -1 , a positive integer otherways. A description of the error should be stored in \c errorMsg .
            Matrix44Type tr;        ///< It holds the transformation matrix that aligns the two range maps. If alignment has not been found it holds the identity matrix.
            float bestConsensus;    ///< It holds the estimated overlap for the returned alignment. Overlap is espressed in percentage relative to \c mMov .
            int numBasesFound;      ///< The number of bases selected on \c mMov . This number is limited by the number of iterations done, i.e \c ransacIter .
            int numWonFullCons;     ///< The number of consensus test exceeded.
            int numMatches;         ///< The number of admissible matching bases found after Branch&Bound procedure.
            int totalTime;          ///< The execution time of the the whole alignment process in milliseconds.
            int baseSelectionTime;  ///< The time spent to select bases from \c mMov in milliseconds.
            int matchingTime;       ///< The time spent to perform matching in milliseconds.
            int branchBoundTime;    ///< The time spent to perform Branch&Bound in milliseconds.
            int rankTime;           ///< The time spent to rank matching bases before consensus test in milliseconds.
            int shortConsTime;      ///< The time spent to perform short consensus test on \c maxNumShortConsensus matching bases in milliseconds.
            int fullConsTime;       ///< The time spent to perform short consensus test on \c maxNumFullConsensus matching bases in milliseconds.
            int initTime;           ///< The time spent to initialize structures in milliseconds.
            QString errorMsg;       ///< It holds a string describing what is gone wrong. It does make sense only if \c exitCode==FAILED .

            ///Default connstructor. Initialize values.
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

        /// A type to hold useful infos about candidates. A candidate is an admissible matching base. It also provides functions to rank candidates properly.
        struct CandidateType
        {
            int shortCons;              ///< Short consensus score. Used to rank candidates.
            float summedPointDist;      ///< Sum of the distances betweeneach base point and its matching point, weighted with angle between normals. This is used to get a first ranking of the candidates.
            Matrix44<ScalarType> tr;    ///< Transformation matrix relative to this candidate, i.e th matrix the aligns the base on \c mMov to its matching base on \c mFix .
            FeatureType** matchPtr;     ///< A reference to the matching base on \c mFix . Features are not copied here but just referred through an array of \c nBase pointers.
            FeatureType** basePtr;      ///< A reference to the base on \c mMov . Features are not copied here but just referred through an array of \c nBase pointers.

            /// Default constructor.
            CandidateType(FeatureType** base, FeatureType** match, int cons = -1, float spd = numeric_limits<float>::max()):basePtr(base),matchPtr(match),shortCons(cons),summedPointDist(spd){}
            /// Static function used to sort the candidates according their \c summedPointDist in increasing order.
            static bool SortByDistance(CandidateType i,CandidateType j) { return (i.summedPointDist<j.summedPointDist); }
            /// Static function used to sort the candidates according their \c shortCons in decreasing order.
            static bool SortByScore(CandidateType i,CandidateType j) { return (i.shortCons>j.shortCons); }
        };

        vector<FeatureType*>* vecFFix;              ///< Vector that holds pointers to features extracted from \c mFix .
        vector<FeatureType*>* vecFMov;              ///< Vector that holds pointers to features extracted from \c mMov .
        Result res;                                 ///< Structure that holds result and stats.
        ANNpointArray fdataPts;                     ///< \e ANN structure that holds descriptors from which \e kdTree is built.
        ANNkd_tree* fkdTree;                        ///< \e ANN \e kdTree structure.
        ConsensusType cons;                         ///< Object used to estimate overlap, i.e to perform consensus tests.
        ConsensusParam consParam;                   ///< Structure that hold consensus' paramaters.

        /// Default constructor. It sets pointers to \c NULL for safety and create a struct for result and stats.
        FeatureAlignment():fkdTree(NULL),vecFFix(NULL),vecFMov(NULL),res(Result()){}

        /// Class destructor. It deallocates structures that has been allocated by \c init() into dynamic memory.
        ~FeatureAlignment(){
            //Cleaning extracted features
            if(vecFFix) vecFFix->clear(); delete vecFFix; vecFFix = NULL;
            if(vecFMov) vecFMov->clear(); delete vecFMov; vecFMov = NULL;
            //Cleaning ANN structures
            FeatureAlignment::CleanKDTree(fkdTree, fdataPts, NULL, NULL, NULL, true);
        }

        /** \brief This function initializes structures needed by the framework.
         *
         *  First it computes features for both meshes (if they have not been previosly computed), then features
         *  are extracted from meshes and stored into proper vectors. After ANN's structures (such as \e kdTree etc.)
         *  and consensus' structures are initialized.
         *  @return A Result object, so it can be used to detect faliures and retrieve a proper error message.
         */
        Result& init(MeshType& mFix, MeshType& mMov, Parameters& param, CallBackPos* cb=NULL)
        {
            if(cb) cb(0,"Initializing...");
            QTime timer, t;  //timers
            timer.start();

            //compute feature for mFix and mMov if they are not computed yet
            typename FeatureType::Parameters p;
            FeatureType::SetupParameters(p);
            if(!FeatureType::HasBeenComputed(mFix)){
                t.start(); if(cb) cb(1,"Computing features...");
                if(!FeatureType::ComputeFeature(mFix, p)){ FeatureAlignment::setError(1,res); return res; }
                if(param.log) param.log(3,"Features on mFix computed in %i msec.",t.elapsed());
            }
            if(!FeatureType::HasBeenComputed(mMov)){
                t.start(); if(cb) cb(1,"Computing features...");
                if(!FeatureType::ComputeFeature(mMov, p)){ FeatureAlignment::setError(1,res); return res; }
                if(param.log) param.log(3,"Features on mMov computed in %i msec.",t.elapsed());
            }

            //initialize a static random generator used inside FeatureUniform()
            tri::SurfaceSampling<MeshType, VertexPointerSampler>::SamplingRandomGenerator().initialize(time(NULL));

            //extract features
            vecFFix = FeatureAlignment::extractFeatures(param.numFixFeatureSelected, mFix, FeatureAlignment::UNIFORM_SAMPLING);
            vecFMov = FeatureAlignment::extractFeatures(param.numMovFeatureSelected, mMov, param.samplingStrategy);
            assert(vecFFix); assert(vecFMov);

            //copy descriptors of mFix into ANN structures, then build kdtree...
            FeatureAlignment::SetupKDTreePoints(*vecFFix, &fdataPts, FeatureType::getFeatureDimension());
            fkdTree = new ANNkd_tree(fdataPts,vecFFix->size(),FeatureType::getFeatureDimension());
            assert(fkdTree);                                                        

            //consensus structure initialization
            cons.SetFix(mFix);
            cons.SetMove(mMov);
            consParam.normalEqualization = param.normalEqualization;
            cons.Init(consParam);

            res.initTime = timer.elapsed();  //stop the timer

            return res;
        }

        /** \brief This is the function that performs the actual alignment process.
         *
         *  Alignment process steps:
         *  <ul><li> Performs safety checks and variables initializations.</li><li> Select randomly (at most) \c ransacIter sparse bases from \c mMov .</li>
         *  <li> for each base: </li><ul><li> for each point in the base do a \e kNNSearch to find the \c k more similar feature (\e Matching)</li>
         *  <li> use <em>Branch And Bound</em> to explore the solution space of all the matching bases and put the admissible ones in a candidates' vector</li></ul>
         *  <li> for all candidates: </li><ul><li>compute the transformation matrix </li><li> compute the \c summedPointDist </li></ul>
         *  <li> Sort candidates by increasing \c summedPointDist. </li><li> Performs short consensus test on (at most) first \c maxNumShortConsensus. </li><li> Sort candidates by decreasing \c shortCons. </li>
         *  <li> Performs full consensus test on (at most) first \c maxNumFullConsensus. </li><li> Return data relative to the best candidate found. </li></ul>
         *  @return A Result object containing the transformation matrix and many others stats infos.
         */
        Result& align(MeshType& mFix, MeshType& mMov, Parameters& param, CallBackPos *cb=NULL)
        {
            QTime tot_timer, timer;  //timers
            tot_timer.start();

            assert(vecFMov); assert(vecFFix);

            if(param.nBase>int(vecFMov->size())){ setError(2,res); return res; }  //not enough features to pick a base
            if(param.k>int(vecFFix->size())){ setError(3, res); return res; }     //not enough features to pick k neighboors

            //normalize normals
            assert(mFix.HasPerVertexNormal());  //mesh doesn't have per vertex normals!!!
            assert(mMov.HasPerVertexNormal());
            tri::UpdateNormals<MeshType>::NormalizeVertex(mFix);
            tri::UpdateNormals<MeshType>::NormalizeVertex(mMov);

            //variables declaration
            res.exitCode = Result::NOT_ALIGNED;
            int bestConsensus = 0;                      //best consensus score
            int cons_succ = int((param.consOffset*param.overlap/100.0f)*(param.fullConsensusSamples/100.0f));               //number of vertices to win consensus
            int ransac_succ = int((param.succOffset*param.overlap/100.0f)*(param.fullConsensusSamples/100.0f));             //number of vertices to win RANSAC
            int bestConsIdx = -1;                       //index of the best candidate, needed to store picked points

            //set up params for consensus
            consParam.consensusDist=param.consensusDist;
            consParam.consensusNormalsAngle=param.consensusNormalsAngle;
            consParam.paint = param.paint;

            //auxiliary vectors needed during the process
            vector<FeatureType**>* baseVec = new vector<FeatureType**>();     //vector to hold bases of feature
            vector<CandidateType>* candidates = new vector<CandidateType>();  //vector to hold canidates
            vector<vector<FeatureType*> > matches;                            //vector of vectors of features. Each vector contains all the feature matches for a base point.

            //variables needed for progress bar callback
            float progBar = 1.0f;
            if(cb) cb(int(progBar),"Selecting bases...");

            //loop to select bases from mMov
            timer.start();
            for(int i = 0; i<param.ransacIter; i++)
            {
                int errCode = FeatureAlignment::SelectBase(*vecFMov,*baseVec, param);
                if(errCode) continue; //can't find a base, skip this iteration
            }
            res.numBasesFound=baseVec->size();
            res.baseSelectionTime = timer.elapsed();
            if(param.log) param.log(3,"%i bases found in %i msec.", baseVec->size(), res.baseSelectionTime);

            //loop to find admissible candidates
            for(int i = 0; i<baseVec->size(); i++)
            {
                timer.start();
                int errCode = FeatureAlignment::Matching(*vecFFix, *vecFMov, fkdTree, (*baseVec)[i], matches, param);
                res.matchingTime+= timer.elapsed();

                timer.start();
                errCode = FeatureAlignment::BranchAndBound((*baseVec)[i], matches, *candidates, param);
                matches.clear(); //make matches' vector ready for another iteration
                res.branchBoundTime+= timer.elapsed();

                if(cb){ progBar+=(20.0f/baseVec->size()); cb(int(progBar),"Matching..."); }
            }
            res.numMatches = candidates->size();
            if(param.log) param.log(3,"%matching performed in %i msec.", res.matchingTime);
            if(param.log) param.log(3,"%Branch&Bound performed in %i msec.", res.branchBoundTime);
            if(param.log) param.log(3,"%i candidates found in %i msec.", candidates->size(), res.matchingTime+res.branchBoundTime);

            //loop to compute candidates' transformation matrix and perform a first ranking using summedPointDist
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

            //performs short consensus test on (at most) the first maxNumShortConsensus candidates
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

            //sort candidates by short consensus score
            sort(candidates->begin(), candidates->end(), CandidateType::SortByScore);

            //variables needed for progress bar callback
            offset = (20.0f/param.maxNumFullConsensus);

            //performs short consensus test on (at most) the first maxNumFullConsensus candidates
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

                //verifies if transformation has the best consensus and update the result
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

        /** \brief Return a string describing the error associated to \c code.
         *
         * Here is a list of the error codes, with their meaning and caller functions.
         * \arg error code \c 1 : Error while computing features - Called by Init()
         * \arg error code \c 2 : Features extracted are not enough to pick a base - Called by Matching()
         * \arg error code \c 3 : Features extracted are not enough to pick \c k neighbors - Called by Matching()
         * \arg error code \c 4 : Base isn't enough sparse - Called by Matching()
         * \arg error code \c 5 : Error while computing rigid transformation - Called by FindRigidTransform()
         */
        static QString getErrorMsg(int code)
        {
            switch(code){
                case 1: return QString("Error while computing features.");
                case 2: return QString("Features extracted are not enough to pick a base.");
                case 3: return QString("Features extracted are not enough to pick k neighbors.");
                case 4: return QString("Base isn't enough sparse.");
                case 5: return QString("Error while computing rigid transformation."); 
                default: return QString("An unkown error occurred.");
            }
        }

        /// Set in the struct \c res all the needed stuffs to handle errors
        static void setError(int errorCode, Result& res)
        {
            res.exitCode = Result::FAILED;
            res.errorCode = errorCode;
            res.errorMsg = FeatureAlignment::getErrorMsg(errorCode);
        }

        /** \brief Performs uniform sampling on a vector of features.
         *  @param vecF The vector of features to be sampled.
         *  @param sampleNum The number of features we would like to sample. It is changed to the number of features actually sampled.
         *  \return An array of pointers to the selected features.
         */
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

        /** \brief Performs poisson disk sampling on features related to a mesh. Features have to be stored into a per vertex attribute.
         *  @param samplingMesh The mesh to be sampled.
         *  @param sampleNum The number of features we would like to sample. It is changed to the number of features actually sampled.
         *  \return An array of pointers to the selected features.
         */
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

        /** \brief Performs poisson disk sampling on the mesh \c m . It stores pointers to vertexes sampled into \c sampler .
         *  @param m The mesh to be sampled. @param sampler Sampler object where sampled data are stored.
         *  @param sampleNum The target amount of data to sample.
         */
        static void SampleVertPoissonDisk(MESH_TYPE& m, VertexPointerSampler& sampler, int sampleNum)
        {
            typedef MESH_TYPE MeshType;

            //first of call sampling procedure we have to make sure that mesh has a bbox
            tri::UpdateBounding<MeshType>::Box(m);

            //setup parameters
            float radius = tri::SurfaceSampling<MeshType,VertexPointerSampler>::ComputePoissonDiskRadius(m,sampleNum);
            typename tri::SurfaceSampling<MeshType,VertexPointerSampler>::PoissonDiskParam pp;

            //poisson samplig need a support mesh from which it takes vertexes; we use the same input mesh
            //as support mesh, so we are sure that pointer to vertexes of input mesh are returned.
            //perform sampling: number of samples returned can be greater or smaller of the requested amount
            tri::SurfaceSampling<MeshType,VertexPointerSampler>::Poissondisk(m, sampler, m, radius, pp);
        }                

        /** \brief Retrieve/create an handle to the per vertex attribute associated to a specified feature type. Useful to write short functions.
         *  @param m The mesh in which the attrubute is searched.
         *  @param createAttribute If \c true create the attribute if it doesn't exists.
         *  \return An handle to the attribute, or a NULL handle if it doesn't exists and \createAttribute is \c false.
         */
        static typename MESH_TYPE::template PerVertexAttributeHandle<FEATURE_TYPE> GetFeatureAttribute(MESH_TYPE& m, bool createAttribute = false)
        {
            typedef typename MeshType::template PerVertexAttributeHandle<FeatureType> PVAttributeHandle;

            //checks if the attribute exists
            if(!tri::HasPerVertexAttribute(m,std::string(FeatureType::getName()))){
                //if createAttribute is true and attribute doesn't exist, we add it; else return a NULL handle
                if(createAttribute) tri::Allocator<MeshType>::template AddPerVertexAttribute<FeatureType>(m,std::string(FeatureType::getName()));
                else return PVAttributeHandle(NULL,0);
            }
            //now we can get a handle to the attribute and return it
            return tri::Allocator<MeshType>::template GetPerVertexAttribute<FeatureType> (m,std::string(FeatureType::getName()));
        }        

        /** \brief Extracts \c numRequested features from mesh \c m using the specified \c samplingStrategy.
         *  What features are actually extracted is a responsability of the Subset() function of the specific feature class.
         *  @param numRequested Number of features to extract.
         *  @param m The mesh in which the attrubute is searched.
         *  @param samplingStrategy Strategy used to sample features. Must be one of \c UNIFORM_SAMPLING, \c POISSON_SAMPLING.
         *  \return A pointer to a vector that contains the extracted featurs, or a NULL pointer if features have not been computed. Vector is allocated inside the function and must be deallocated by the caller.
         */
        static vector<FEATURE_TYPE*>* extractFeatures(int numRequested, MESH_TYPE &m, int samplingStrategy, CallBackPos *cb = NULL)
        {
            //Create a vector to hold extracted features
            vector<FeatureType*>* featureSubset = new vector<FeatureType*>();
            //extract numRequested features according to the sampling strategy.
            //If feature has not been computed previously, return a NULL pointer
            if(!FeatureType::Subset(numRequested, m, *featureSubset, samplingStrategy, cb)) return NULL;
            return featureSubset;
        }

        /** \brief Copy features' descriptions from the vector \c vecF to the array \c dataPts that is used as source for ANN's kdTree.
         *  @param vecF The features' vector.
         *  @param dataPts Array where features' description are copied. It is allocated inside function; the caller have to deallocate mamory.
         *  @param pointDim The dimension of the feature's description.
         */
        static void SetupKDTreePoints(vector<FEATURE_TYPE*>& vecF, ANNpointArray* dataPts, int pointDim)
        {
            //fill dataPts with feature's descriptions in vecF
            *dataPts = annAllocPts(vecF.size(), pointDim);		    // allocate data points
            for(unsigned int i = 0; i < vecF.size(); i++){
                for (int j = 0; j < pointDim; j++)
                    (*dataPts)[i][j] = (ANNcoord)(vecF[i]->description[j]);
            }
        }

        /** \brief Utility function to deallocate ANN's stuffs.
         *
         *  Structures are provided as parameters; \c NULL can be used for structures we are not interested to deallocate.
         *  @param kdTree The kdTree structure.
         *  @param dataPts The points used as source by the kdTree.
         *  @param queryPts The points used as queries.
         *  @param idx Indexes to the points in \c dataPts.
         *  @param dists Squared distances of points from the query point.
         *  @param close If \c true the root of the kdTree is used. It should be done only after we are done with ANN.
         */
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

        /** \brief Randomly select a base of \c param.nBase features from vector \c vecFMov and put it in the vector \c baseVec.
         *
         *  A base is added only if each point is far from the others at least \c param.sparseBaseDist \c * \c param.mMovBBoxDiag/100.
         *  \return 0 if everything is right, an error code according to the getErrorMsg() function otherwise.
         */
        static int SelectBase(vector<FEATURE_TYPE*>& vecFMov, vector<FEATURE_TYPE**>& baseVec, Parameters& param)
        {
            assert(param.nBase<=int(vecFMov.size()));  //not enough features to pick a base
            float baseDist = param.sparseBaseDist*(param.mMovBBoxDiag/100.0f); //compute needed params

            FeatureType** base = FeatureAlignment::FeatureUniform(vecFMov, &(param.nBase)); //randomly chooses a base of features from vecFFix
            if(!VerifyBaseDistances(base, param.nBase, baseDist)) return 4; //if base point are not enough sparse, skip
            baseVec.push_back(base);
            return 0;
        }

        /** \brief It performs the matching of each point of the \c base with features contained in the \c kdTree.
         *
         *  For each base point a kNNSearch is done that finds the 'best' \c param.k features; all these are stored in a vector of \c matches.
         *  \return 0 if everything is right, an error code according to the getErrorMsg() function otherwise.
         */
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

        /** \brief It performs the matching of each point of the \c base with features contained in the \c kdTree.
         *
         *  For each base point a kNNSearch is done that finds the 'best' \c param.k features; all these are stored in a vector of \c matches.
         *  \return 0 if everything is right, an error code according to the getErrorMsg() function otherwise.
         */
  /*
        //this is the meatching function that uses clustering
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

            MyMesh clusterMesh;
            tri::Allocator<MyMesh>::AddVertices(clusterMesh,param.k);
            SimpleTempData<MyMesh::VertContainer, FeatureType*> MyTempData(clusterMesh.vert);

            //additional variables needed
            ANNidxArray fnnIdx = new ANNidx[param.k];						    // allocate near neigh indices
            ANNdistArray fdists = new ANNdist[param.k];						    // allocate near neigh dists
            vector<FeatureType*> neighbors;
            tri::Clustering<MyMesh, tri::NearestToCenter<MyMesh> > ClusteringGrid;
            float cellsize = param.consensusDist*(param.mMovBBoxDiag/100.0f); //compute needed params
            //foreach feature in the base find the best matching using fkdTree
            for(int i = 0; i < param.nBase; i++)
            {
                kdTree->annkSearch(fqueryPts[i], param.k, fnnIdx, fdists);  //search the k best neighbor for the current point

                assert(fdists[0]!=ANN_DIST_INF);  //if this check fails, it means that no feature have been found!

                int j; MyMesh::VertexIterator vi;
                for(vi=clusterMesh.vert.begin(), j=0; vi<clusterMesh.vert.end(), j<param.k; vi++, j++){
                    vi->P() = vecFFix[fnnIdx[j]]->pos;
                    vi->N() = vecFFix[fnnIdx[j]]->normal;
                    MyTempData[vi] = vecFFix[fnnIdx[j]];
                }
                if(param.log) param.log(3,"clusterMesh contains %i vertexes",j);
                ClusteringGrid.Init(clusterMesh.bbox,0,cellsize);
                ClusteringGrid.AddPointSet(clusterMesh);
                ClusteringGrid.SelectPointSet(clusterMesh);
                for(vi=clusterMesh.vert.begin(); vi<clusterMesh.vert.end(); vi++){
                    if(vi->IsS()) neighbors.push_back(MyTempData[vi]);
                }
                if(param.log) param.log(3,"neighbors.size %i",neighbors.size());
                matches.push_back(neighbors);           //copy neighbors vec into matches vec
                neighbors.clear();                      //make neighbors vec ready for another iteration

                if(cb){ pBar+=offset; cb((int)pBar, "Matching..."); }
            }

            assert(int(matches.size())==param.nBase);

            //Cleaning ANN structures
            FeatureAlignment::CleanKDTree(NULL, NULL, fqueryPts, fnnIdx, fdists, false);

            return 0;
        }
*/
        /** \brief Given \c param.nbase sets of matching points, explore the space of all the matching bases using a Branch And Bound algorithm and adds
         *  the admisible bases into a vector of candidates.
         *
         *  \return 0 if everything is right, an error code according to the getErrorMsg() function otherwise.
         */
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

        /** \brief Compute the rigid transformation matrix that aligns the base \c movF with the matching base \c fixF and put the matrix into \c tr.
         *
         *  \return 0 if everything is right, an error code according to the getErrorMsg() function otherwise.
         */
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
