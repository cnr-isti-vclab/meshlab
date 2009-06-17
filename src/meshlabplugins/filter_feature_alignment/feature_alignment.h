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
#include <meshlab/meshmodel.h>

#include <math.h>
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/bounding.h>
#ifndef ANN_H
    #include <ANN/ANN.h>
#endif

#include <vcg/math/perlin_noise.h>
#include <vcg/math/gen_normal.h>
#include <vcg/math/random_generator.h>
#include <vcg/math/point_matching.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/complex/trimesh/closest.h>
#include <vcg/complex/trimesh/point_sampling.h>
#include <meshlabplugins/edit_pickpoints/pickedPoints.h>

using namespace std;
using namespace vcg;

inline int LocRnd(int n)
{
    static math::SubtractiveRingRNG myrnd(time(NULL));
    return myrnd.generate(n);
}

template <class MeshType>
class VertexPointerSampler
{
        public:
        typedef typename MeshType::CoordType	CoordType;
        typedef typename MeshType::VertexType	VertexType;
        typedef typename MeshType::FaceType	FaceType;
        typedef typename vector<VertexType*>::iterator VertexPointerIterator;

        MeshType* m;  //this is needed for advanced sampling (i.e poisson sampling)

        VertexPointerSampler(){ m = new MeshType(); (*m).Tr.SetIdentity(); (*m).sfn=0; }
        ~VertexPointerSampler(){ if(m) delete m; }
        vector<VertexType*> sampleVec;

        void AddVert(VertexType &p){ sampleVec.push_back(&p); }

        void AddFace(const FaceType &f, const CoordType &p){}

        void AddTextureSample(const FaceType &, const CoordType &, const Point2i &){}
};

template<class FEATURE_TYPE, class SCALAR_TYPE> struct MData {
    typedef FEATURE_TYPE FeatureType;
    typedef SCALAR_TYPE ScalarType;

    int shortCons;
    float summedPointDist;
    Matrix44<ScalarType> tr;
    FeatureType** matchPtr;
    FeatureType** basePtr;
};

template<class FEATURE_TYPE, class SCALAR_TYPE> bool MDataCompareDist(MData<FEATURE_TYPE,SCALAR_TYPE> i,MData<FEATURE_TYPE,SCALAR_TYPE> j) { return (i.summedPointDist<j.summedPointDist); }
template<class FEATURE_TYPE, class SCALAR_TYPE> bool MDataCompareCons(MData<FEATURE_TYPE,SCALAR_TYPE> i,MData<FEATURE_TYPE,SCALAR_TYPE> j) { return (i.shortCons>j.shortCons); }

template<class MESH_TYPE, class FEATURE_TYPE> class FeatureAlignment
{
    public:

        typedef MESH_TYPE MeshType;
        typedef FEATURE_TYPE FeatureType;
        typedef typename MeshType::ScalarType ScalarType;
        typedef typename MeshType::VertexType VertexType;
        typedef GridStaticPtr<VertexType, ScalarType > MeshGrid;
        typedef tri::VertTmark<MeshType> MarkerVertex;
        typedef Matrix44<ScalarType> Matrix44Type;

        enum SamplingStrategies { UNIFORM_SAMPLING=0, POISSON_SAMPLING=1 };

        class Parameters{
            public:

            int samplingStrategy;        //strategy to extract features from range maps
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
            float shortConsOffset;                      //consensus %, depending on overlap, to win short consensus;
            bool normalEqualization;                    //to use normal equalization in consensus
            bool pickPoints;                            //to store bases/points into picked points attribute
            bool paint;                                 //to paint mMov according to consensus
            bool isFullConsensus;                       //tells to consensus procedure how to compute short/full success threshold
            int maxNumFullConsensus;                    //max num of bases tested with full consensus procedure
            int maxNumShortConsensus;                   //max num of bases tested with short consensus procedure
            float mMovBBoxDiag;                         //mMov bbox diagonal; used to compute distances...
            int* counterSucc;                           //used to count successes during diagram operation

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
                shortConsOffset = 40.0f;
                normalEqualization = true;
                pickPoints = false;
                paint = false;
                isFullConsensus = true;
                maxNumFullConsensus = 8;
                maxNumShortConsensus = 50;
                mMovBBoxDiag = mMov.bbox.Diag();
                counterSucc = NULL;
            }
        };

        vector<vector<int> >* normBuckets;          //structure to hold normals bucketing. Needed for normal equalized sampling during consensus
        MeshGrid* gridFix;                          //variable to manage uniform grid
        MarkerVertex markerFunctorFix;              //variable to manage uniform grid
        vector<FeatureType*>* vecFFix;              //vector to hold pointers to features extracted from mFix
        vector<FeatureType*>* vecFMov;              //vector to hold pointers to features extracted from mMov
        ANNpointArray fdataPts;                     //ANN structure to hold descriptors from which kdTree is build
        ANNkd_tree* fkdTree;                        //ANN kdTree structure        

        FeatureAlignment(){
            //set pointers to NULL for safety
            normBuckets = NULL;
            gridFix = NULL;
            fkdTree = NULL;
            vecFFix = NULL;
            vecFMov = NULL;
        }

        bool init(MeshType& mFix, MeshType& mMov, Parameters& param){
            //if requested, group normals of mMov into 30 buckets. Buckets are used for Vertex Normal Equalization
            //in consensus. Bucketing is done here once for all to speed up consensus.            
            if(param.normalEqualization){
                normBuckets = FeatureAlignment::BucketVertexNormal(mMov.vert, 30);
                assert(normBuckets!=NULL);
            }

            gridFix = new MeshGrid();
            FeatureAlignment::SetupGrid(mFix, *gridFix, markerFunctorFix); //builds the uniform grid with mFix vertices

            //extract features
            vecFFix = FeatureAlignment::extractFeatures(param.numFixFeatureSelected, mFix, FeatureAlignment::UNIFORM_SAMPLING);
            vecFMov = FeatureAlignment::extractFeatures(param.numMovFeatureSelected, mMov, param.samplingStrategy);
            if(vecFFix==NULL || vecFMov==NULL) return false; //can't continue; features have not been computed!

            //copy descriptors of mMov into ANN structures, then build kdtree...            
            FeatureAlignment::SetupKDTreePoints(*vecFFix, &fdataPts, FeatureType::getFeatureDimension());
            fkdTree = new ANNkd_tree(fdataPts,vecFFix->size(),FeatureType::getFeatureDimension());
        }

        Matrix44Type align(MeshType& mFix, MeshType& mMov, Parameters& param, CallBackPos *cb=NULL)
        {
            //normalize normals
            tri::UpdateNormals<MeshType>::PerVertexNormalized(mFix);
            tri::UpdateNormals<MeshType>::PerVertexNormalized(mMov);

            //variables declaration
            Matrix44Type bestTr = Matrix44Type::Identity();     //best transformation matrix found
            int bestConsensus = 0;                                              //best consensus score                        
            int short_cons_succ = int((param.shortConsOffset*param.overlap/100.0f)*(param.short_cons_samples/100.0f));      //number of vertices to win short consensus
            int cons_succ = int((param.consOffset*param.overlap/100.0f)*(param.fullConsensusSamples/100.0f));                       //number of vertices to win consensus
            int ransac_succ = int((param.succOffset*param.overlap/100.0f)*(param.fullConsensusSamples/100.0f));                     //number of vertices to win ransac
            int done = 0, doneShort = 0, won = 0, wonShort = 0; //just debug
            int bestConsIdx = -1;                       //index of the best MData, needed to store picked points
            int skipped = 0;  //just debug

            //auxiliary vectors needed inside the loop
            vector<FeatureType**>* baseVec = new vector<FeatureType**>();
            vector<FeatureType**>* matchesVec = new vector<FeatureType**>();
            vector<MData<FeatureType,ScalarType> >* candidates = new vector<MData<FeatureType,ScalarType> >();

            //variables needed for progress bar callback
            float progBar = 0.0f;
            float offset = 30.0f/param.ransacIter;

            time_t start_loop, end_loop; int dif;  //timers

            time(&start_loop);
            //loop of ransac iterations to find all possible matches at once
            for(int i = 0; i<param.ransacIter; i++)
            {
                //callback handling
                if(cb){ progBar+=offset; cb(int(progBar),"Computing ransac..."); }

                bool ok = FeatureAlignment::Matching(*vecFFix, *vecFMov, fkdTree, *baseVec, *matchesVec, param);
                if(!ok){ skipped++; continue;} //something wrong; can't find a base, skip this...

                assert(baseVec->size()>=1);
                int currBase = baseVec->size()-1;

                //scan all the new matches found and computes datas for each one
                for(unsigned int j=candidates->size(); j<matchesVec->size(); j++)
                {
                    MData<FeatureType,ScalarType> data; //alloca struct
                    data.shortCons = -param.fullConsensusSamples; //init is necessary to get structures well sorted later
                    data.matchPtr = (*matchesVec)[j];   //set the pointer to the match
                    data.basePtr = (*baseVec)[currBase];  //set the pointer to the relative base

                    //computes the rigid transformation matrix that overlaps the two points sets
                    Matrix44Type tr;
                    ok = FeatureAlignment::FindRigidTransformation(mFix, mMov, (*baseVec)[currBase], (*matchesVec)[j], param.nBase, tr);
                    if(!ok) continue; //something wrong; can't find a rigid transformation, skip this...

                    data.tr = tr;  //store transformation

                    Matrix44Type oldTr = ApplyTransformation(mMov, tr);       //apply transformation
                    data.summedPointDist = SummedPointDistances(mFix, mMov, (*baseVec)[currBase], (*matchesVec)[j], param.nBase);  //compute and store the sum of points distances
                    ResetTransformation(mMov, oldTr);                                 //restore old tranformation                    

                    candidates->push_back(data);
                }
                assert(candidates->size()==matchesVec->size());
            }

            time(&end_loop);
            dif = int(difftime(end_loop,start_loop));
            if(param.counterSucc==NULL){
                //mylogger("It took %i Sec to complete matches look up",dif);
                //mylogger("%i candidates found",candidates->size());
            }

            //sort candidates by summed point distances
            sort(candidates->begin(), candidates->end(),MDataCompareDist<FeatureType,ScalarType>);

            //variable needed for progress bar callback
            offset = (40.0f/math::Min(param.maxNumShortConsensus,int(candidates->size())));

            time(&start_loop);
            //evaluetes at most first g candidates
            for(unsigned int j=0; j<candidates->size() && j<param.maxNumShortConsensus; j++)
            {
                //callback handling
                if(cb){ progBar+=offset; cb(int(progBar),"Computing ransac..."); }

                doneShort++;
                Matrix44Type currTr = (*candidates)[j].tr;              //load the right transformation
                Matrix44Type oldTr = ApplyTransformation(mMov, currTr); //apply transformation
                param.isFullConsensus = false;                          //set parameters for short consensus
                (*candidates)[j].shortCons = Consensus(mFix, mMov, *gridFix, markerFunctorFix, normBuckets, param);  //compute short consensus
                ResetTransformation(mMov, oldTr);                                 //restore old tranformation

                if((*candidates)[j].shortCons >= short_cons_succ) wonShort++;  //count how many won, and use this as bound later
            }

            time(&end_loop);
            dif = int(difftime(end_loop,start_loop));
            if(param.counterSucc==NULL); //mylogger("It took %i Sec to compute all short consensus",dif);

            //sort candidates by short consensus
            sort(candidates->begin(), candidates->end(),MDataCompareCons<FeatureType,ScalarType>);

            //variables needed for progress bar callback
            offset = (40.0f/math::Min(param.maxNumFullConsensus,int(candidates->size())));

            time(&start_loop);
            //evaluetes at most maxNumFullConsensus candidates beetween those who won short consensus
            for(int j=0; j<wonShort && j<param.maxNumFullConsensus; j++)
            {
                //callback handling
                if(cb){ progBar+=offset; cb(int(progBar),"Computing ransac..."); }

                done++;
                Matrix44Type currTr = (*candidates)[j].tr;              //load the right transformation
                Matrix44Type oldTr = ApplyTransformation(mMov, currTr); //apply transformation
                param.isFullConsensus = true;                           //set parameters for full consensus
                int consensus = Consensus(mFix, mMov, *gridFix, markerFunctorFix, normBuckets, param, bestConsensus);  //compute full consensus
                ResetTransformation(mMov, oldTr);                                 //restore old tranformation

                if(consensus >= cons_succ) won++;

                //verify if transformation has the best consensus and update the result
                if( (consensus >= cons_succ) && (consensus > bestConsensus) )
                {
                    bestTr = currTr;
                    bestConsensus = consensus;
                    bestConsIdx = j;
                    if(consensus >= ransac_succ){
                        if(param.counterSucc==NULL); //mylogger("Consensus of %.2f%%. Ransac terminated succesfully!", (float(consensus)/samples)*100.0f);
                        break;
                    }
                }
                if(param.counterSucc==NULL); //mylogger("Consensus of %.2f%%", (float(consensus)/samples)*100.0f);
            }
            time(&end_loop);
            dif = int(difftime(end_loop,start_loop));
            if(param.counterSucc==NULL){
                //mylogger("It took %i Sec to complete all consensus computations",dif);
                //mylogger("%i(%i short) consensus computed, %i(%i short) over succes threshold", done, doneShort, won, wonShort);
                //mylogger("Bases %i, matches %i, %.2f%% consensus",maxRanIt-skipped, candidates->size(),(float(bestConsensus)/samples)*100.0f);
            }

            if(param.counterSucc!=NULL && won>0){ //log this just if you are running ransac diagram
                param.counterSucc[0]++;
                //mylogger("Found consensus of %.2f%%", (float(bestConsensus)/samples)*100.0f);
            }

            //if flag 'points' is checked, clear old picked points and save the new points
            if(param.pickPoints){
                if(bestConsIdx!=-1) StorePickedPoints(mFix, (*candidates)[bestConsIdx].basePtr, mFix.Tr, param.nBase, "B");
                else ClearPickedPoints(mFix);
                if(bestConsIdx!=-1) StorePickedPoints(mMov, (*candidates)[bestConsIdx].matchPtr, (*candidates)[bestConsIdx].tr * mMov.Tr, param.nBase, "M");
                else ClearPickedPoints(mMov);
            }

            //Clean structures...
            FeatureAlignment::CleanTuplesVector(baseVec, true);
            FeatureAlignment::CleanTuplesVector(matchesVec, true);
            candidates->clear(); if(candidates) delete candidates; candidates = NULL;

            return bestTr;
        }
        void finalize()
        {
            //Cleaning extracted features
            if(vecFFix) vecFFix->clear(); delete vecFFix; vecFFix = NULL;
            if(vecFMov) vecFMov->clear(); delete vecFMov; vecFMov = NULL;
            //Cleaning ANN structures
            FeatureAlignment::CleanKDTree(fkdTree, fdataPts, NULL, NULL, NULL, true);
            //Cleaning bucketing structures
            if(normBuckets!=NULL) delete normBuckets; normBuckets = NULL;
            //Cleaning grid structures
            if(gridFix) delete gridFix; gridFix = NULL;
        }

        static FEATURE_TYPE** FeatureUniform(vector<FEATURE_TYPE*>& vecF, int* sampleNum)
        {
            typedef MESH_TYPE MeshType;
            typedef FEATURE_TYPE FeatureType;
            typedef typename vector<FeatureType*>::iterator FeatureIterator;

            if(*sampleNum>=int(vecF.size())) *sampleNum = vecF.size();

            vector<FeatureType*>* vec = new vector<FeatureType*>();
            for(FeatureIterator fi=vecF.begin();fi!=vecF.end();++fi) vec->push_back(*fi);

            assert(vec->size()==vecF.size());

            tri::SurfaceSampling<MeshType, VertexPointerSampler<MeshType> >::SamplingRandomGenerator().initialize(clock());
            unsigned int (*p_myrandom)(unsigned int) = tri::SurfaceSampling<MeshType, VertexPointerSampler<MeshType> >::RandomInt;
            std::random_shuffle(vec->begin(),vec->end(), p_myrandom);

            FeatureType** sampledF = new FeatureType*[*sampleNum];
            for(int i =0; i<*sampleNum; ++i) sampledF[i] = (*vec)[i];

            //cleaning...
            vec->clear(); delete vec;

            return sampledF;
        }

        static FEATURE_TYPE** FeaturePoisson(MESH_TYPE& samplingMesh, int* sampleNum)
        {
            typedef MESH_TYPE MeshType;
            typedef FEATURE_TYPE FeatureType;
            typedef typename MeshType::template PerVertexAttributeHandle<FeatureType*> PVAttributeHandle;

            VertexPointerSampler<MeshType> samp = VertexPointerSampler<MeshType>();
            SampleVertPoissonDisk(samplingMesh, samp, *sampleNum);
            *sampleNum = samp.sampleVec.size();  //poisson sampling can return untill 50% more of requested samples!
            PVAttributeHandle fh = FeatureAlignment::GetFeatureAttribute(samplingMesh);
            if(!tri::Allocator<MeshType>::IsValidHandle(samplingMesh,fh)) return NULL;
            FeatureType** sampler = new FeatureType*[*sampleNum];
            for(int i=0; i<*sampleNum; i++) sampler[i]=fh[samp.sampleVec[i]];
            return sampler;
        }

        //template<class MESH_TYPE>
        static void SampleVertPoissonDisk(MESH_TYPE& m, VertexPointerSampler<MESH_TYPE>& sampler, int sampleNum)
        {
            typedef MESH_TYPE MeshType;

            //first of call sampling procedure we have to make sure that mesh has a bbox
            tri::UpdateBounding<MeshType>::Box(m);

            //setup parameters
            float radius = tri::SurfaceSampling<MeshType,VertexPointerSampler<MeshType> >::ComputePoissonDiskRadius(m,sampleNum);
            typename tri::SurfaceSampling<MeshType,VertexPointerSampler<MeshType> >::PoissonDiskParam pp;

            //poisson samplig need a support mesh from which it takes vertexes; we use the same input mesh
            //as support mesh, so we are sure that pointer to vertex of input mesh are returned
            //perform sampling: number of samples returned can be 50% greater of the requested amount
            tri::SurfaceSampling<MeshType,VertexPointerSampler<MeshType> >::Poissondisk(m, sampler, m, radius, pp);
        }

        static bool SampleVertNormalEqualized(vector<typename MESH_TYPE::VertexPointer>& vert, vector<vector<int> >* BKT, int SampleNum)
        {
            // vettore di contatori per sapere quanti punti ho gia' preso per ogni bucket
            vector<int> BKTpos(BKT->size(),0);

            if(SampleNum >= int(vert.size())) SampleNum= int(vert.size()-1);

            int ind;
            for(int i=0;i<SampleNum;){
                ind=LocRnd(BKT->size()); // Scelgo un Bucket
                int &CURpos = BKTpos[ind];
                vector<int> &CUR = (*BKT)[ind];

                if(CURpos<int(CUR.size())){
                    swap(CUR[CURpos], CUR[ CURpos + LocRnd((*BKT)[ind].size()-CURpos)]);
                    swap(vert[i],vert[CUR[CURpos]]);
                    ++BKTpos[ind];
                    ++i;
                }
            }

            vert.resize(SampleNum);
            return true;
        }

        static void SampleVertUniform(MESH_TYPE& m, vector<typename MESH_TYPE::VertexPointer>& vert, int sampleNum)
        {
            typedef MESH_TYPE MeshType;

            VertexPointerSampler<MeshType> sampler;
            tri::SurfaceSampling<MeshType, VertexPointerSampler<MeshType> >::VertexUniform(m, sampler, sampleNum);
            for(unsigned int i=0; i<sampler.sampleVec.size(); i++) vert.push_back(sampler.sampleVec[i]);
        }

        static typename MESH_TYPE::template PerVertexAttributeHandle<FEATURE_TYPE*> GetFeatureAttribute(MESH_TYPE& m, bool createAttribute = false)
        {
            typedef MESH_TYPE MeshType;
            typedef FEATURE_TYPE FeatureType;
            typedef typename MeshType::template PerVertexAttributeHandle<FeatureType*> PVAttributeHandle;

            //checks if the attribute exist
            if(!tri::HasPerVertexAttribute(m,std::string(FeatureType::getName()))){
                //if createAttribute is true and attribute doesn't exist, we add it; else return a NULL handle
                if(createAttribute) tri::Allocator<MeshType>::template AddPerVertexAttribute<FeatureType*>(m,std::string(FeatureType::getName()));
                else return PVAttributeHandle(NULL,0);
            }
            //now we can get a handle to the attribute and return it
            return tri::Allocator<MeshType>::template GetPerVertexAttribute<FeatureType*> (m,std::string(FeatureType::getName()));
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

        //compute the randomized consensus beetween m1 e m2 (without taking in account any additional transformation)
        //IMPORTANT: per vertex normals of m1 and m2 MUST BE PROVIDED JET NORMALIZED!!!       
        static int Consensus(MESH_TYPE& mFix, MESH_TYPE& mMov, GridStaticPtr<typename MESH_TYPE::VertexType, typename MESH_TYPE::ScalarType>& grid, tri::VertTmark<MESH_TYPE>& markerFunctor, vector<vector<int> >* normBuckets, Parameters& param, int bestCons = 0,CallBackPos *cb = NULL)
        {
            typedef MESH_TYPE MeshType;
            typedef typename MeshType::ScalarType ScalarType;
            typedef typename MeshType::VertexType VertexType;
            typedef typename MeshType::VertexPointer VertexPointer;
            typedef typename MeshType::CoordType CoordType;
            typedef typename MeshType::VertexIterator VertexIterator;
            typedef typename VertexPointerSampler<MeshType>::VertexPointerIterator VertexPointerIterator;

            //pointer to a function to compute distance beetween points
            vertex::PointDistanceFunctor<ScalarType> PDistFunct;

            int samples = param.fullConsensusSamples;
            if(!param.isFullConsensus) samples = param.short_cons_samples;
            vector<VertexPointer>* queryVert = NULL;
            //if no buckets are provided get a vector of vertex pointers sampled uniformly; used as query points
            if(normBuckets==NULL){
                queryVert = new vector<VertexPointer>();                
                SampleVertUniform(mMov, *queryVert, samples);
            }
            else{
                //else, get a vector of vertex pointers sampled in a normal equalized manner; used as query points
                queryVert = new vector<VertexPointer>(mMov.vert.size(),NULL);
                for(unsigned int i=0; i<mMov.vert.size(); i++) (*queryVert)[i] = &(mMov.vert[i]);//do a copy of pointers to vertexes
                FeatureAlignment::SampleVertNormalEqualized(*queryVert, normBuckets, samples);
            }
            assert(queryVert!=NULL);

            //init variables for consensus
            float consDist = param.consensusDist*(param.mMovBBoxDiag/100.0f);               //consensus distance
            int cons_succ;                      //score needed to pass consensus
            int consensus = 0;                  //counts vertices in consensus
            float dist;                         //holds the distance of the closest vertex found
            VertexType* closestVertex = NULL;   //pointer to the closest vertex
            Point3<ScalarType> queryNrm;        //the query point normal for consensus
            CoordType queryPnt;                 //the query point for consensus
            CoordType closestPnt;               //the closest point found in consensus
            Matrix33<ScalarType> mat2(mMov.Tr,3); //3x3 matrix needed to transform normals
            Matrix33<ScalarType> inv_mat1(Inverse(mFix.Tr),3);  //3x3 matrix needed to transform normals

            //compute cons_succ depending on the type of consensus, i.e short/full
            if(param.isFullConsensus) cons_succ = int((param.consOffset*param.overlap/100.0f)*(param.fullConsensusSamples/100.0f));
            else cons_succ = int((param.shortConsOffset*param.overlap/100.0f)*(param.short_cons_samples/100.0f));

            //Colors handling: white = not tested; blue = not in consensus; red = in consensus;
            //yellow = not in consensus becouse of normals
            //Colors are stored in a buffer vector; at the end of consensus loop they are applied
            //to the mesh ONLY IF full consensus overcome succes threshold AND the best consensus.
            vector<Color4b>* colorBuf = NULL;
            if(param.paint) colorBuf = new vector<Color4b>(mMov.VertexNumber(),Color4b::White); //buffer vector for colors

            //consensus loop
            for(VertexPointerIterator vi=queryVert->begin(); vi!=queryVert->end(); vi++)
            {
                int i = (vi - queryVert->begin());  //iteration counter
                assert(i>=0 && i<mMov.VertexNumber());

                dist = -1.0f;
                //set query point; vertex coord is transformed properly in fix mesh coordinates space; the same for normals
                queryPnt = Inverse(mFix.Tr) * (mMov.Tr * (*vi)->P());
                queryNrm = inv_mat1 * (mat2 * (*vi)->N());

                //if query point is bbox, the look for a vertex in cDist from the query point
                if(mFix.bbox.IsIn(queryPnt)) closestVertex = grid.GetClosest(PDistFunct,markerFunctor,queryPnt,consDist,dist,closestPnt);
                else closestVertex=NULL;  //out of bbox, we consider the point not in consensus...

                if(closestVertex!=NULL && dist < consDist){
                    assert(closestVertex->P()==closestPnt); //coord and vertex pointer returned by getClosest must be the same

                    //point is in consensus distance, now we check if normals are near
                    if(queryNrm.dot(closestVertex->N())>param.consensusNormalsAngle)  //15 degrees
                    {
                        consensus++;  //got consensus
                        if(param.paint){ (*colorBuf)[i] = Color4b::Red; }  //paint of red if needed
                    }
                    else{
                        if(param.paint){ (*colorBuf)[i] = Color4b::Yellow; }  //paint of yellow if needed
                    }
                }
                else{
                    if(param.paint){ (*colorBuf)[i] = Color4b::Blue; } //not in consensus due to distance, paint of blue
                }
            }

            //apply colors if consensus is the best ever found.
            //NOTE: we got to do this here becouse we need a handle to sampler. This is becouse vertex have been shuffled
            //and so colors have been stored not in order in the buffer!
            if(param.paint){
                if(consensus>=bestCons && consensus>=cons_succ){
                    for(VertexPointerIterator vi=queryVert->begin(); vi!=queryVert->end(); vi++)
                        if(!(*vi)->IsD()) (*vi)->C() = (*colorBuf)[vi-queryVert->begin()];
                }
            }

            //clear and delete buffer for colors, if needed
            if(param.paint){ colorBuf->clear(); delete colorBuf; }

            //delete vector of query vertexes; do it as last thing, 'couse it is used for coloring too.
            if(queryVert!=NULL) delete queryVert;

            return consensus;
        }


        static vector<vector<int> >* BucketVertexNormal(typename MESH_TYPE::VertContainer& vert, int bucketDim = 30)
        {
            static vector<Point3f> NV;
            if(NV.size()==0) GenNormal<float>::Uniform(bucketDim,NV);

            // Bucket vector dove, per ogni normale metto gli indici
            // dei vertici ad essa corrispondenti
            vector<vector<int> >* BKT = new vector<vector<int> >(NV.size()); //NV size is greater then bucketDim, so don't change this!

            int ind;
            for(int i=0;i<vert.size();++i){
                ind=GenNormal<float>::BestMatchingNormal(vert[i].N(),NV);
                (*BKT)[ind].push_back(i);
            }

            return BKT;
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

        static void CleanTuplesVector(vector<FEATURE_TYPE**>* tuplesVec, bool deleteVec)
        {
            //clean vectors contents...
            for(unsigned int j=0; j<tuplesVec->size(); j++){
                delete [] (*tuplesVec)[j]; (*tuplesVec)[j] = NULL;
            }
            tuplesVec->clear();

            //if deleteVec is set to true, deallocates the vector too...
            if(deleteVec){
                delete tuplesVec; tuplesVec = NULL;
            }
        }

        static bool Matching(vector<FEATURE_TYPE*>& vecFFix, vector<FEATURE_TYPE*>& vecFMov, ANNkd_tree* kdTree, vector<FEATURE_TYPE**>& baseVec, vector<FEATURE_TYPE**>& matchesVec, Parameters& param, CallBackPos *cb=NULL)
        {
            typedef MESH_TYPE MeshType;
            typedef FEATURE_TYPE FeatureType;
            typedef typename vector<FeatureType*>::iterator FeatureIterator;

            float pBar = 0, offset = 100.0f/param.nBase;  //used for progresss bar callback
            if(cb) cb(0, "Matching...");

            if(param.nBase>int(vecFMov.size())) return false; //not enough features to pick a base
            if(param.k>int(vecFFix.size())) return false;  //not enough features in kdtree to pick k neighboors

            //compute needed params
            float baseDist = param.sparseBaseDist*(param.mMovBBoxDiag/100.0f);
            float errDist = param.mutualErrDist*(param.mMovBBoxDiag/100.0f);

            FeatureType** base = FeatureAlignment::FeatureUniform(vecFMov, &(param.nBase)); //randomly chooses a base of features from vecFFix
            if(!VerifyBaseDistances(base, param.nBase, baseDist)) return false; //if base point are not enough sparse, skip
            baseVec.push_back(base);

            assert(baseVec.size()>=1);
            assert((int)vecFFix.size()>=param.nBase);

            //fill fqueryPts with feature's descriptions in base
            ANNpointArray fqueryPts;
            FeatureAlignment::SetupKDTreeQuery(base, param.nBase, &fqueryPts, FeatureType::getFeatureDimension());

            //additional variables needed
            ANNidxArray fnnIdx = new ANNidx[param.k];						        // allocate near neigh indices
            ANNdistArray fdists = new ANNdist[param.k];						    // allocate near neigh dists
            vector<vector<FeatureType*>* >* matchedVec = new vector<vector<FeatureType*>* >();            

            //foreach feature in the base find the best matching using fkdTree
            for(int i = 0; i < param.nBase; i++)
            {
                kdTree->annkSearch(fqueryPts[i], param.k, fnnIdx, fdists);  //search the k best neighbor for the current point

                assert(fdists[0]!=ANN_DIST_INF);  //if this check fails, it means that no feature have been found!

                vector<FeatureType*>* matches = new vector<FeatureType*>();

                for(int j=0; j<param.k; j++) matches->push_back(vecFFix[fnnIdx[j]]); //store all features

                matchedVec->push_back(matches);

                if(cb){ pBar+=offset; cb((int)pBar, "Matching..."); }
            }

            assert(int(matchedVec->size())==param.nBase);

            //branch and bound
            int* curSolution = new int[param.nBase];
            for(int i=0; i<param.nBase; i++) curSolution[i] = 0;              //initialization
            FeatureAlignment::Match(baseVec, *matchedVec, param.nBase, 0, curSolution, matchesVec, errDist);

            //Cleaning ANN structures
            FeatureAlignment::CleanKDTree(NULL, NULL, fqueryPts, fnnIdx, fdists, false);

            //Cleaning matching structures
            for(typename vector<vector<FeatureType*>* >::iterator it=matchedVec->begin(); it!=matchedVec->end(); it++){
                if(*it!=NULL){ delete *it; *it = NULL; }
            }
            delete matchedVec; matchedVec = NULL;
            return true;
        }

        static bool FindRigidTransformation(MESH_TYPE& mFix, MESH_TYPE& mMov, FEATURE_TYPE* fixF[], FEATURE_TYPE* movF[], int nBase, Matrix44<typename MESH_TYPE::ScalarType>& tr, CallBackPos *cb=NULL)
        {
            typedef MESH_TYPE MeshType;
            typedef FEATURE_TYPE FeatureType;
            typedef typename MESH_TYPE::ScalarType ScalarType;

            if(cb) cb(0,"Computing rigid transformation...");

            //computes the rigid transformation matrix that overlaps the two points sets
            vector< Point3<ScalarType> > fixPoints;
            vector< Point3<ScalarType> > movPoints;

            for(int i = 0; i<nBase; i++)
            {
                fixPoints.push_back(mMov.Tr * fixF[i]->pos);
                movPoints.push_back(mFix.Tr * movF[i]->pos);
            }
            PointMatching<ScalarType>::ComputeRigidMatchMatrix(tr, movPoints, fixPoints);
            return true;
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

        static inline void SetupGrid(MESH_TYPE& m, GridStaticPtr<typename MESH_TYPE::VertexType, typename MESH_TYPE::ScalarType>& grid, tri::VertTmark<MESH_TYPE>& markerFunctor)
        {
            grid.Set(m.vert.begin(), m.vert.end());
            markerFunctor.SetMesh(&m);
        }

        static void Match(vector<FEATURE_TYPE**>& baseVec, vector<vector<FEATURE_TYPE*>* >& matchedVec, int nBase, int level, int curSolution[], vector<FEATURE_TYPE**>& solutionsVec, float errDist, CallBackPos *cb = NULL)
        {
            typedef FEATURE_TYPE FeatureType;

            assert(level<nBase);

            for(unsigned int j=0; j<matchedVec[level]->size(); j++){
                curSolution[level] = j;
                if(MatchSolution(baseVec, matchedVec, level, curSolution, errDist)){
                    if(level==nBase-1){
                        FeatureType** solution = new FeatureType*[nBase];
                        for(int h=0; h<nBase; h++){
                            solution[h] = (*(matchedVec[h]))[curSolution[h]];
                        }
                        solutionsVec.push_back(solution);
                    }
                    else
                        Match(baseVec, matchedVec, nBase, level+1, curSolution, solutionsVec, errDist);
                }
            }
            curSolution[level] = 0;
        }

        static bool MatchSolution(vector<FEATURE_TYPE**>& baseVec, vector<vector<FEATURE_TYPE*>* >& matchedVec, int level, int curSolution[], float errDist)
        {
            if (level==0) return true;

            int currBase = baseVec.size()-1;

            for(int j=0; j<level; j++){
                float distF = vcg::Distance(baseVec[currBase][level]->pos, baseVec[currBase][j]->pos);
                float distM = vcg::Distance((*(matchedVec[level]))[curSolution[level]]->pos, (*(matchedVec[j]))[curSolution[j]]->pos);
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
                //transform normals properly; normals inside features are jet normalized
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
