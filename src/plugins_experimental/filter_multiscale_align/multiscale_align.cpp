//#define _DEBUG_GLS
//#define DEBUG_SINGLE_POINT

#include "multiscale_align.h"

#include <vcg/complex/algorithms/point_sampling.h>
#include <vcg/complex/algorithms/create/resampler.h>
//#include <vcg/space/index/kdtree/kdtree.h>

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <fstream>

#include "utils.h"


using namespace vcg;
using namespace std;
using namespace Grenaille;

class BaseSampler
{
public:
  BaseSampler(CMeshO* _m){m=_m; uvSpaceFlag = false; qualitySampling=false; tex=0;}
  CMeshO *m;
  QImage* tex;
  int texSamplingWidth;
  int texSamplingHeight;
  bool uvSpaceFlag;
  bool qualitySampling;

  void AddVert(const CMeshO::VertexType &p)
  {
    tri::Allocator<CMeshO>::AddVertices(*m,1);
    m->vert.back().ImportData(p);
  }

  
}; // end class BaseSampler




// Default Constructor
MultiscaleAlign::MultiscaleAlign()
{
    srand (time(NULL));
}

//! This defines the min and max where to perform the comparison
void computeScaleBounds(const DescriptorBase &reference,
                        const DescriptorBase &toAlign,
                        float maxScaleRef, float maxScaleToAl,
                        int& xMin, int& xMax, int& yMin, int& yMax ){
    double logBase=std::log((double)toAlign.multiplier);
    double minGlobScale=min(reference.minScale,toAlign.minScale);
    xMax= (int)(std::log(maxScaleRef/minGlobScale) / logBase);
    yMax= (int)(std::log(maxScaleToAl/minGlobScale) / logBase);
    xMin= (int)(std::log(reference.minScale/minGlobScale) / logBase) +1 ;
    yMin= (int)(std::log(toAlign.minScale/minGlobScale) / logBase) +1;
}


// Precompute the GLS descriptor, only for the selected points
template <class Fit>
void MultiscaleAlign::computeSimilarityMap(const DescriptorBase& reference,
                                           const DescriptorBase& toAlign,
                                           std::vector<Fit>* toAlignDescr,
                                           float maxscaleToAl){

    CMeshO::PerVertexAttributeHandle<std::vector<DerivableSphereFit>* > descriptorsHandler;
    descriptorsHandler = tri::Allocator<CMeshO>::GetPerVertexAttribute<std::vector<DerivableSphereFit>* >(reference.model->cm, std::string("GLSDescr"));

//reset quality to 0
    for (CMeshO::VertexIterator vj = reference.model->cm.vert.begin(); vj != reference.model->cm.vert.end(); ++vj)
        if(! (*vj).IsS()){
            (*vj).Q() = 0;
            (*vj).C() = Color4b(255,255,255,1);
        }


#pragma omp parallel for
    for(int i = 0; i < reference.selection.size(); i++){

        unsigned int ind = reference.selection[i];
        CVertexO &vj=reference.model->cm.vert[ind];

        // Use a local variable for each thread
        float maxScale = vj.Q();
        int xMin, yMin, xMax, yMax;
        computeScaleBounds(reference, toAlign, maxScale, maxscaleToAl, xMin, xMax, yMin, yMax);

        std::vector<DerivableSphereFit> *descr  = descriptorsHandler[ind];

        DynamicSolver ds;
        ds.solve(*toAlignDescr,  toAlignDescr->size(),
                 *descr,         descr->size(),
                 toAlign.multiplier, xMin, xMax, yMin, yMax);

        //std::cout << ind << " " << ds.confidence << std::endl;

        //DynamicProg::VotingStretchEstimation<Scalar> vse;
        //float scale = vse.estimate(ds.xBest, ds.yBest, toAlign.multiplier);

        reference.model->cm.vert[ind].Q() = ds.confidence;
        //reference.model->cm.vert[ind].Q() = 1.f/scale;
        reference.model->cm.vert[ind].C() = Color4b(0,0,255*ds.confidence,1);
    }
}


////// This function is not used now, but it should analyse the points find the border ones, and assign a maxscale w.r.t. borders
void MultiscaleAlign::checkBorders(DescriptorBase model, float radius){


	int borderPoints=0;
	std::cout << "Check borders, scale " << 4.2*radius*radius << endl;
	for (CMeshO::VertexIterator vj = model.model->cm.vert.begin(); vj != model.model->cm.vert.end(); ++vj)
		{
		
			 std::vector<unsigned int> n;
			 std::vector<Scalar> squaredDist;
			 model.kdTree->doQueryDist((*vj).P(), 2.1*radius, n, squaredDist);
			 //std::cout << "Neighbors " << n.size() << endl;
			 if (n.size()<22)
			 {
				 //(*vj).C()=Color4b(255,0,0,1);
				 (*vj).Q()=0.0;
				 //(*vj).C()=Color4b(0,255,0,1);
				 borderPoints++;
			 }
			 else
			 {
				 //(*vj).C()=Color4b(255,0,0,1);
				 (*vj).Q()=-1.0;
			 }
		}
	std::cout << "Border points " << borderPoints << endl;

	std::vector<unsigned int> n;
	std::vector<Scalar> squaredDist;
	if (borderPoints == 0)
	{
		float bigRad = 1.5*model.model->cm.bbox.Diag();
		for (CMeshO::VertexIterator vj = model.model->cm.vert.begin(); vj != model.model->cm.vert.end(); ++vj)
		{
			
			//MyPoint query = model.model->cm.vert[k];
			model.kdTree->doQueryDist((*vj).P(), bigRad, n, squaredDist);

			//std::cout << "RefCoarse: " << refCoarse << " N size " << n.size() << std::endl;
			//model.model->cm.vert[n.back()].C() = Color4b(0, 0, 255, 255);
			//std::sort(squaredDist.begin(), squaredDist.end());
			(*vj).Q() = sqrt(squaredDist.back());
			//std::cout << "RefCoarse: " << reference.maxScale << " N size " << n.size() << std::endl;
			n.clear();
		}
	}
	else
	{
		float radiusSearch = radius;
		float bestRadius = 0;
		//std::vector<unsigned int> n;
		while (n.size()<model.model->cm.vert.size())
		{
			n.clear();
			//std::cout << "Radius " << radiusSearch << endl;
			for (CMeshO::VertexIterator vj = model.model->cm.vert.begin(); vj != model.model->cm.vert.end(); ++vj)
			{
				if ((*vj).Q() == 0.0)
				{
					model.kdTree->doQueryDist((*vj).P(), radiusSearch, n, squaredDist);
					if (n.size() >= model.model->cm.vert.size()) break;
					for (int i = 0; i < n.size(); i++)
					{
						//std::cout << "In N " << endl;
						if (model.model->cm.vert[n[i]].Q() < 0.0f)
						{
							//std::cout << "Set " << radiusSearch << endl;
							model.model->cm.vert[n[i]].Q() = radiusSearch;
							bestRadius = radiusSearch;
						}
					}
					n.clear();
				}

			}
			radiusSearch *= 1.1;
		}

		for (CMeshO::VertexIterator vj = model.model->cm.vert.begin(); vj != model.model->cm.vert.end(); ++vj)
		{
			if ((*vj).Q() < bestRadius/4.0f)
			{
				(*vj).Q() = 0.0;
			}
		}

	}

}

//////// This is the main function decribed inside
float MultiscaleAlign::process_scale(MeshModel* _reference, float radiusRef,     float refCoarse,
                              MeshModel* _toAlign,   float radiusToAlign, float toAlignCoarse,
                              float multiplier, unsigned int nbScales, Options opt){



    float scaleFact=0.0; bool done=false;

	/////// Create the scales vector, and the DescriptorBase objects
	
	DescriptorBase toAlign(_toAlign, radiusToAlign, toAlignCoarse, multiplier, nbScales);
    std::cout << "Compute toAlign KdTree..." << std::flush;
	vcg::ConstDataWrapper<CMeshO::VertexType::CoordType> wrapperVcg(&toAlign.model->cm.vert[0].P(), toAlign.model->cm.vert.size(), size_t(toAlign.model->cm.vert[1].P().V()) - size_t(toAlign.model->cm.vert[0].P().V()));
    toAlign.kdTree = new vcg::KdTree<CMeshO::ScalarType>(wrapperVcg);
    std::cout << "DONE" << std::endl;

	DescriptorBase reference(_reference, radiusRef, refCoarse, multiplier, nbScales);
	std::cout << "Compute reference KdTree..." << std::flush;
	vcg::ConstDataWrapper<CMeshO::VertexType::CoordType> wrapperVcgR(&reference.model->cm.vert[0].P(), reference.model->cm.vert.size(), size_t(reference.model->cm.vert[1].P().V()) - size_t(reference.model->cm.vert[0].P().V()));
	reference.kdTree = new vcg::KdTree<CMeshO::ScalarType>(wrapperVcgR);
    std::cout << "DONE" << std::endl;

	if (opt.checkBorders)
	{
		checkBorders(reference, radiusRef);
		checkBorders(toAlign, radiusToAlign);
		//return 1;

	}

	std::vector<DescrPoint> descrList;
	std::vector< int > seeds; 
#define ONLY_ONE
#ifdef ONLY_ONE

	//int toAlignId = 6701;  // gargoself
	//int toAlignId = 12111;  // gargo200k
	//int toAlignId = 16028;  // gargo200k_cut
	//int toAlignId = 16026;  // gargo200k_cut_noscale
	//int referenceId = 26051;
	int referenceId = 274; // Surrey
	//int referenceId = 169; //200 samples


	typedef typename std::vector<DerivableSphereFit> Container;
	using vcg::tri::Allocator;

	//std::vector<unsigned int> n;
	//std::vector<Scalar> squaredDist;
	//MyPoint query = reference.model->cm.vert[referenceId];
	//reference.kdTree->doQueryDist(reference.model->cm.vert[referenceId].P(), refCoarse, n, squaredDist);
	//
	////std::cout << "RefCoarse: " << refCoarse << " N size " << n.size() << std::endl;
	//reference.model->cm.vert[n.back()].C() = Color4b(0, 0, 255, 255);
	//reference.maxScale = Distance(reference.model->cm.vert[referenceId].P(), reference.model->cm.vert[n.back()].P()) / 1.5f;
	////std::cout << "RefCoarse: " << reference.maxScale << " N size " << n.size() << std::endl;

	reference.maxScale = reference.model->cm.vert[referenceId].Q();
	std::vector<float>Scales;
	Utils::sampleScaleInterval(min(radiusToAlign, radiusRef),
		max(toAlignCoarse, reference.model->cm.vert[referenceId].Q()),
		multiplier,
		nbScales,
		Scales);

	/*CMeshO::PerVertexAttributeHandle<Container* > descriptorsHandler;
	descriptorsHandler =
		Allocator<CMeshO>::GetPerVertexAttribute<Container* >
		(reference.model->cm, std::string("GLSDescr"));

		descriptorsHandler[referenceId] = new std::vector<DerivableSphereFit>();

		computeDescriptor(reference.model->cm.vert[referenceId],
		Scales,
		reference,
		descriptorsHandler[referenceId]);*/

	
#endif



	//std::cout << "Start" << std::endl;

	//unsigned int ind = 13785;
	CMeshO::PerVertexAttributeHandle<std::vector<DerivableSphereFit>* > referenceDescriptorsHandler;
	referenceDescriptorsHandler = tri::Allocator<CMeshO>::GetPerVertexAttribute<std::vector<DerivableSphereFit>* >(reference.model->cm, std::string("GLSDescr"));

	referenceDescriptorsHandler[referenceId] = new std::vector<DerivableSphereFit>();
	computeDescriptor(reference.model->cm.vert[referenceId], Scales, reference, referenceDescriptorsHandler[referenceId]);

	reference.model->cm.vert[referenceId].C() = Color4b(0, 255, 0, 1);
	//toAlign.model->cm.vert[toAlignId].C() = Color4b(0, 255, 0, 1);
	//float maxScale = MAX_SCALE_MULTIPLIER*reference.model->cm.bbox.Diag();

	/*for (CMeshO::VertexIterator vj = toAlign.model->cm.vert.begin(); vj != toAlign.model->cm.vert.end(); ++vj)
		(*vj).Q()=0.0;*/

	std::vector<std::pair<std::pair<float,float>, float>> bestScales;
	std::pair<int, float> init(0, 0.0);
	std::pair<std::pair<int, float>, float> init2(init, 0.0);
	for (int i = 0; i < 20; i++)
		bestScales.push_back(init2);

	/*toAlign.selection.clear();
	toAlign.selection.push_back(4775);
	toAlign.model->cm.vert[4775].C() = Color4b(0, 255, 0, 255);  */

	//std::cout << "Selected points: " << toAlign.selection.size() << std::endl;
	
// output seeds for debug purpose
	for (int i = 0; i < toAlign.selection.size(); i++) {

		
		std::cout << "Seed " << i+1 << " out of " << toAlign.selection.size() << std::endl;
		int indToAl = toAlign.selection[i];
		if (toAlign.model->cm.vert[indToAl].Q() > 0)
		{

			//n.clear();
			//squaredDist.clear();
			//query = toAlign.model->cm.vert[indToAl];
			//toAlign.kdTree->doQueryDist(toAlign.model->cm.vert[indToAl].P(), toAlignCoarse, n, squaredDist);

			////std::cout << "toAlignCoarse: " << toAlignCoarse << " N size " << n.size() << std::endl;
			//toAlign.model->cm.vert[n.back()].C() = Color4b(0, 0, 255, 255);
			//toAlign.maxScale = Distance(toAlign.model->cm.vert[indToAl].P(), toAlign.model->cm.vert[n.back()].P())/1.5f;
			////std::cout << "toAlignCoarse: " << toAlign.maxScale << " N size " << n.size() << std::endl;

			toAlign.maxScale = toAlign.model->cm.vert[indToAl].Q();
			// Use a local variable for each thread
			//float maxscaleToAl = MAX_SCALE_MULTIPLIER*toAlign.model->cm.bbox.Diag();
			int xMin, yMin, xMax, yMax;
			computeScaleBounds(reference, toAlign, reference.maxScale, toAlign.maxScale, xMin, xMax, yMin, yMax);

			//std::cout << "Computed Scale bounds!: "  << std::endl;

			//std::cout << "Computing GLS...: " << std::endl;

			CMeshO::PerVertexAttributeHandle<std::vector<DerivableSphereFit>* > descriptorsHandlerAlign;
			descriptorsHandlerAlign = tri::Allocator<CMeshO>::GetPerVertexAttribute<std::vector<DerivableSphereFit>* >(toAlign.model->cm, std::string("GLSDescr"));

			descriptorsHandlerAlign[indToAl] = new std::vector<DerivableSphereFit>();

			computeDescriptor(toAlign.model->cm.vert[indToAl],
				Scales,
				toAlign,
				descriptorsHandlerAlign[indToAl]);

			//reference.model->cm.vert[ind].C() = Color4b(0, 255, 0, 1);
			std::vector<DerivableSphereFit> *descrAlign = descriptorsHandlerAlign[indToAl];

			//std::cout << "Done!: " << std::endl;

			//std::cout << "xMin: " << xMin << "xMax: " << xMax << "yMin: " << yMin << "yMax: " << yMax << "Maxscale: " << reference.maxScale << "MaxScaleToAl: " << toAlign.maxScale << std::endl;
			DynamicSolver ds;
			ds.solve(*referenceDescriptorsHandler[referenceId], referenceDescriptorsHandler[referenceId]->size(),
				*descrAlign, descrAlign->size(),
				toAlign.multiplier, xMin, xMax, yMin, yMax);

			while (ds.estScales.size() > 10)
				ds.estScales.pop_back();

			for (int i = 0; i < ds.estScales.size(); i++)
			{
				//std::cout << "Confidence " << ds.estScales[i].second << std::endl;
				ds.estScales[i].second = recalcConfidence(reference, toAlign, ds.estScales[i].first, nbScales, referenceId, indToAl);
				//std::cout << "Confidence after " << ds.estScales[i].second << std::endl;

			}

			std::sort(ds.estScales.begin(), ds.estScales.end(), [](auto &left, auto &right) {
				return left.second > right.second;
			});

			/*for (int j = 0; j != ds.estScales.size(); j++)
			{*/
			std::pair<int, float> bestFirst(indToAl, ds.estScales[0].first);
			if (ds.estScales[0].second > bestScales.back().second)
			{
				std::pair<std::pair<int, float>, float> best(bestFirst, ds.estScales[0].second);
				bestScales.push_back(best);
				std::sort(bestScales.begin(), bestScales.end(), [](auto &left, auto &right) {
					return left.second > right.second;
				});
				bestScales.pop_back();


			}
			//std::cout << "Scale: " << ds.estScales[j].first << " Confidence: " << ds.estScales[j].second << endl;
		//}
		}


	}

	for (int j = 0; j != bestScales.size(); j++)
	{
		toAlign.model->cm.vert[bestScales[j].first.first].C() = Color4b(bestScales[j].second*255, 0, 0, 1);
		//toAlign.model->cm.vert[bestScales[j].first.first].Q() = bestScales[j].first.second;
		std::cout << "Id: " << bestScales[j].first.first << " Scale: " << bestScales[j].first.second << " Confidence: " << bestScales[j].second << endl;

	}

	return 1.;
	
}

float MultiscaleAlign::recalcConfidence(DescriptorBase _reference, DescriptorBase _toAlign, float scale, unsigned int nbScales, int referenceID, int toAlignID)
{
	float newRadiusToAlign = _toAlign.minScale*scale;
	float newToAlignCoarse = _toAlign.maxScale*scale;

	//std::cout << "MinScaleD " << _reference.minScale << " MaxScaleD " << _reference.maxScale << " MinScaleA " << _toAlign.minScale << " MaxScaleA " << _toAlign.maxScale << std::endl;

	//std::cout << "Scale " << scale << " oldMin " << _toAlign.minScale << " newMin " << newRadiusToAlign << " oldMax " << _toAlign.maxScale << " newMax " << newToAlignCoarse << std::endl;

	float minRadius = std::max(_reference.minScale, newRadiusToAlign);
	float maxRadius = std::min(_reference.maxScale, newToAlignCoarse);
	float step = (maxRadius - minRadius) / nbScales;

	//std::cout << "Step " << step << std::endl;
	//std::cout << "MinRadius " << minRadius << " MaxRadius " << maxRadius << std::endl;

	std::vector<float>ScalesDesc;
	std::vector<float>ScalesToAl;
	for (int i = 0; i < nbScales; i++)
	{
		ScalesDesc.push_back(minRadius + i*step);
		ScalesToAl.push_back(ScalesDesc[i] / scale);
		//std::cout << "ScaleDescr " << ScalesDesc[i] << " ScaleToAl " << ScalesToAl[i] << std::endl;
	}

	//std::cout << "NbScales " << ScalesDesc.size() << std::endl;

	_toAlign.minScale = ScalesDesc[0]/ scale;
	_toAlign.maxScale = ScalesDesc.back() / scale;
	_reference.minScale = ScalesDesc[0];
	_reference.maxScale = ScalesDesc.back();

	std::vector<DerivableSphereFit> *refDescr = new std::vector<DerivableSphereFit>();
	std::vector<DerivableSphereFit>* toAlDescr = new std::vector<DerivableSphereFit>();

	computeDescriptor(_reference.model->cm.vert[referenceID], ScalesDesc, _reference, refDescr);
	computeDescriptor(_toAlign.model->cm.vert[toAlignID], ScalesToAl, _toAlign, toAlDescr);

	DynamicSolver ds;
	ds.solve_givenScale(*refDescr, *toAlDescr, 0, 1, 0, ScalesDesc.size(), 0, ScalesDesc.size());

	//std::cout << "Confidence " << ds.confidence << std::endl;


	return ds.confidence/ ScalesDesc.size();

}

////// This function makes the main search for points, see comments inside
std::vector < Cand > MultiscaleAlign::searchMatches(DescriptorBase toAlign, DescriptorBase reference, int ind, std::vector<float>Scales, bool computeAlignDescr)
{
	std::cout << "Computing toAlign descriptor" << std::endl;

    CMeshO::PerVertexAttributeHandle<std::vector<DerivableSphereFit>* > descriptorsToAlign;
    descriptorsToAlign = tri::Allocator<CMeshO>::GetPerVertexAttribute<std::vector<DerivableSphereFit>* >(toAlign.model->cm, std::string("GLSDescr"));
    
    if (computeAlignDescr){
        descriptorsToAlign[ind] = new std::vector<DerivableSphereFit>();
        computeDescriptor(toAlign.model->cm.vert[ind], Scales, toAlign, descriptorsToAlign[ind]);
    }


    CMeshO::PerVertexAttributeHandle<std::vector<DerivableSphereFit>* > descriptorsReference;
    descriptorsReference = tri::Allocator<CMeshO>::GetPerVertexAttribute<std::vector<DerivableSphereFit>* >(reference.model->cm, std::string("GLSDescr"));

	float maxscaleToAl=toAlign.model->cm.vert[ind].Q();



  ////////
	// FIRST POINT: Given the first point, all the seams in the reference model are checked, and the best FIRST_POINTS points are put in a list
	//////////////

	std::vector < Cand > firstPoints;
	Cand cpoint; cpoint.confidence=0.0; cpoint.ind=-1;
	firstPoints.push_back(cpoint);
		
    std::cout << "Searching first point..." << std::endl;

#pragma omp parallel for//num_threads(2)
    for(int selK = 0; selK < reference.selection.size(); selK++){
    //for (int k =0; k < reference.model->cm.vn; k++) {
        unsigned int k = reference.selection[selK];

        CVertexO vj=reference.model->cm.vert[k];

            // Use a local variable for each thread
            float maxScaleRef = vj.Q();
            int xMin, yMin, xMax, yMax;
            computeScaleBounds(reference, toAlign, maxScaleRef, maxscaleToAl, xMin, xMax, yMin, yMax);

            std::vector<DerivableSphereFit> *descr = descriptorsReference[k];
            std::vector<DerivableSphereFit>* result = descriptorsToAlign[ind];

            DynamicSolver ds;
            ds.solve(*descr,  descr->size(),
					 *result, result->size(),
                     toAlign.multiplier, xMin, xMax, yMin, yMax);

#pragma omp critical
            {
                std::vector<Cand>::iterator it;
                it = firstPoints.begin();
                //std::cout << "Confidence " << ds.confidence()  <<std::endl;
                if (firstPoints.back().confidence < ds.confidence)
                {
                    DynamicProg::ConvolutionStretchEstimation<Scalar> vse;
                    cpoint.confidence=ds.confidence;
                    cpoint.ind = k;
                    //std::cout << "Pre-estimate " <<std::endl;
                    cpoint.scale=vse.estimate(ds.stepMatrix(), toAlign.multiplier);
                    //std::cout << "Post-estimate " <<std::endl;
                    if (firstPoints.back().ind==-1)
                    {
                        //std::cout << "First" <<std::endl;
                        firstPoints[0].confidence=cpoint.confidence;
                        firstPoints[0].ind=cpoint.ind;
                        firstPoints[0].scale=cpoint.scale;
                        //firstPoints.erase(it+1);
                    }
                    else for(unsigned int r=0; r<firstPoints.size(); r++)
                    {
                        //std::cout << "Gotcha" <<std::endl;
                        if(firstPoints[r].confidence<cpoint.confidence)
                        {
                            firstPoints.insert(it+r, cpoint);
                            if(firstPoints.size()>FIRST_POINTS)
                                firstPoints.pop_back();
                            break;
                        }
                    }
                }
            }
        //}else
        //    std::cout << "Reject Ind: " << k << std::endl;
	}
		

    if (firstPoints[0].ind != -1){
      
		std::cout << "Ind of Candidate " << ind << std::endl;
        for(unsigned int l=0; l<firstPoints.size(); l++)
		{

			reference.model->cm.vert[firstPoints[l].ind].C() = Color4b(255,0,0,1);

//// This saves the data of the list of matched points in a cvs file
#ifdef SAVE_CVS
			QString filename1=QString("File_Fitness%1.csv")
                .arg(l);
			QString filename2=QString("File_Kappa%1.csv")
                .arg(l);
			QString filename3=QString("File_Tau%1.csv")
                .arg(l);
			QFile file1(filename1);
			QFile file2(filename2);
			QFile file3(filename3);
			file1.open(QFile::WriteOnly);
			QTextStream stream1(&file1);
			file2.open(QFile::WriteOnly);
			QTextStream stream2(&file2);
			file3.open(QFile::WriteOnly);
			QTextStream stream3(&file3);

			double dMax=0.0;
            for (unsigned int i=0; i<descriptorsToAlign[ind]->size(); i++)
			{
				if(descriptorsToAlign[ind]->at(i).geomVar()>dMax)
				{
					dMax=descriptorsToAlign[ind]->at(i).geomVar();
				}
			}

			MyPoint query (reference.model->cm.vert[firstPoints[l].ind]);
			query.normal().normalize();

			reference.maxScale=reference.model->cm.vert[firstPoints[l].ind].Q();

			std::vector<DerivableSphereFit> descr;
			computeDescriptor(query, Scales, reference, &descr);
			for (unsigned int i = 0; i<Scales.size(); i++)
			{
			  stream1 << Scales[i] << "\t" ; // this writes first line with 
			  stream2 << Scales[i] << "\t" ; // this writes first line with 
			  stream3 << Scales[i] << "\t" ; // this writes first line with 
			}
			stream1 << "\n" ;
			stream2 << "\n" ;
			stream3 << "\n" ;
			for (unsigned int i = 0; i<descriptorsToAlign[ind]->size(); i++)
			{
			  stream1 << descriptorsToAlign[ind]->at(i).fitness() << "\t" ; // this writes first line with 
			  stream2 << descriptorsToAlign[ind]->at(i).kappa() << "\t" ; // this writes first line with 
			  stream3 << descriptorsToAlign[ind]->at(i).tau() << "\t" ; // this writes first line with 
			}
			stream1 << "\n" ;
			stream2 << "\n" ;
			stream3 << "\n" ;
			//std::cout << "Eval first column" << std::endl;
			for (unsigned int j = 0; j<descr.size(); j++)
			{
			  stream1 << descr[j].fitness() << "\t" ; // this writes first line with 
			  stream2 << descr[j].kappa() << "\t" ; // this writes first line with 
			  stream3 << descr[j].tau() << "\t" ; // this writes first line with 
			}
			stream1 << "\n" ;
			stream2 << "\n" ;
			stream3 << "\n" ;
			file1.close();
			file2.close();
			file3.close();

#endif
	        
	        
			std::cout << firstPoints[l].ind
					  << " confidence " << firstPoints[l].confidence
					  << " scale " << firstPoints[l].scale
					  //<< " max " << simSolver.path()[0].value
					  << std::endl;
		}
			

        

    }else {// Show the un-matched point in black
        //reference->cm.vert[queryId].C()=Color4b(0,0,0,1);
        std::cout << "Point not matched !! (conf:"
                 // << ds.confidence
                  << ")"
                  << std::endl;
    }


	return firstPoints;

}

bool MultiscaleAlign::searchTranf(const DescriptorBase &toAlign,
                                  const DescriptorBase &reference,
                                  const std::vector<int> &seeds,
                                  const std::vector<std::vector<Cand> > &seedList,
                                  const std::vector<float> &Scales, Options opt)
{
    const int candSeed=seedList.size()-1;
	//std::cout << "Candseed " << candSeed << std::endl;

	for (int i=0; i<seedList[candSeed].size(); i++)
	{
		const Cand &sPoint=seedList[candSeed][i];
		for(int j=0; j<candSeed; j++)
        {
			for(int k=0; k<seedList[j].size(); k++)
            {
				//std::cout << "Try a scale " << seedList[j][k].scale << " versus " << opt.expectedScale*1.4 << " and " << opt.expectedScale/1.4 << std::endl;
				if(seedList[j][k].scale<(opt.expectedScale*1.4) && seedList[j][k].scale>(opt.expectedScale/1.4))
				{
					std::vector <std::pair<int, int> > corrs;
					//std::cout << "Try a scale " << seedList[j][k].scale << std::endl;
					corrs.reserve(seedList[j].size());
					//std::cout << "Try..." << sPoint.scale << " " << seedList[j][k].scale << std::endl;
					if( isScaleOk(sPoint.scale,seedList[j][k].scale,toAlign.multiplier) &&
						isCoupleOk(toAlign.model->cm.vert[seeds[j]],
								   reference.model->cm.vert[seedList[j][k].ind],
								   toAlign.model->cm.vert[seeds[candSeed]],
								   reference.model->cm.vert[sPoint.ind],
								   sPoint.scale ) )
					{
						//std::cout << "Selected!" << std::endl;
						corrs.push_back( std::pair<int, int>( seeds[j],        seedList[j][k].ind));
						corrs.push_back( std::pair<int, int>( seeds[candSeed], sPoint.ind));

						//std::cout << "Searching third points..." << std::endl;
						
						for(unsigned int s2=0; s2<seeds.size(); s2++)
						{
							if(seeds[s2]!=corrs[0].first && seeds[s2]!=corrs[1].first)
							{
								//std::cout << "Trying with seed " << s2 << std::endl;
								std::vector < Cand > thirdPoints = getCandidates(toAlign, reference, seeds[s2], corrs, sPoint.scale, Scales);
								if(thirdPoints[0].ind!=-1)
								{
									//std::cout << "Third point list size " << thirdPoints.size() << std::endl;

									corrs.push_back(std::pair<int, int>(seeds[s2], thirdPoints[0].ind));

									if(!opt.useQuadriplets)
									{
										//std::cout << "Checking Triplets " << g+1 << " out of  " << thirdPoints.size() << std::endl;
										if(checkTriplets(toAlign, reference, corrs, thirdPoints,sPoint.scale, opt.alignError))
										{
											return true;
										}
										//corrs.pop_back();
									}
									else for (unsigned int g=0; g<thirdPoints.size(); g++)
									{
										corrs[2].second=thirdPoints[g].ind;
										for(unsigned int s3=0; s3<seeds.size(); s3++)
										{
											if(seeds[s3]!=corrs[0].first && seeds[s3]!=corrs[1].first && seeds[s3]!=corrs[2].first)
											{
												//std::cout << "Fourth point for seed " << s3 << std::endl;
												std::vector < Cand >fourthPoints = getCandidates(toAlign, reference, seeds[s3], corrs, sPoint.scale, Scales);
												if(fourthPoints[0].ind!=-1)
												{
													corrs.push_back(std::pair<int, int>(seeds[s3], fourthPoints[0].ind));
													//std::cout << "Fourth point list size " << fourthPoints.size() << std::endl;
													
													//std::cout << "Checking Quadriplets " << t+1 << " out of " << fourthPoints.size() << std::endl;
													if(checkQuadriplets(toAlign, reference, corrs, fourthPoints,sPoint.scale, opt.alignError))
													{
														return true;
													}
													corrs.pop_back();
													
												}
											}
										}
										//corrs.pop_back();

										
									}
									corrs.pop_back();
								}
							}
						}
						

					}
				}
			}
		}
	}
	
	return false;

}



////// Given a set of couples of points and a point on the toAlign model, it finds a list of matches in the right area
std::vector<Cand>
MultiscaleAlign::getCandidates(const DescriptorBase& toAlign,
                               const DescriptorBase& reference,
                               int ind,
                               const std::vector <std::pair<int, int> >& corrs,
                               float scaleFact,
                               const std::vector<float> &Scales)
{
	std::vector<Cand> output;
	output.clear();

//	DynamicProg::VotingStretchEstimation<Scalar> vse;
	Cand cpoint; cpoint.confidence=0.0; cpoint.ind=-1;
	output.push_back(cpoint);

	
	//toAlign.model->cm.vert[ind].C()=Color4b(0,0,255,1);
	CMeshO::PerVertexAttributeHandle<std::vector<DerivableSphereFit>* > descriptorsToAlign;
    descriptorsToAlign = tri::Allocator<CMeshO>::GetPerVertexAttribute<std::vector<DerivableSphereFit>* >(toAlign.model->cm, std::string("GLSDescr"));
	descriptorsToAlign[ind] = new std::vector<DerivableSphereFit>();
	computeDescriptor(toAlign.model->cm.vert[ind], Scales, toAlign, descriptorsToAlign[ind]);

    CMeshO::PerVertexAttributeHandle<std::vector<DerivableSphereFit>* > descriptorsReference;
    descriptorsReference = tri::Allocator<CMeshO>::GetPerVertexAttribute<std::vector<DerivableSphereFit>* >(reference.model->cm, std::string("GLSDescr"));

	float maxscaleToAl=toAlign.model->cm.vert[ind].Q();

#pragma omp parallel for
    //for (int l =0; l < reference.model->cm.vn; l++)
    for(int selK = 0; selK < reference.selection.size(); selK++)
    {
        unsigned int l = reference.selection[selK];
        if(/*reference.model->cm.vert[l].IsS() && */isReferenceOk(l, reference.model, ind, toAlign.model, corrs, scaleFact))
		{
			//std::cout << "Try " << l << " of " << reference.model->cm.vn << std::endl;	
			//reference->cm.vert[l].C()=Color4b(0,0,255,1);
			//std::cout << "Candidato" << std::endl;	
			CVertexO vj=reference.model->cm.vert[l];
            MyPoint query (vj);
            query.normal().normalize();
            float maxScaleRef = vj.Q();
            std::vector<DerivableSphereFit> *descr = descriptorsReference[l];
//            std::vector<DerivableSphereFit> descr;
//            computeDescriptor(query, Scales, reference, &descr);
			std::vector<DerivableSphereFit>* result = descriptorsToAlign[ind];

//			double logBase=std::log((double)toAlign.multiplier);
//			double minGlobScale=min(reference.minScale,toAlign.minScale);
//            int xMax= (int)(std::log(reference.maxScale/minGlobScale) / logBase) + 1;
//            int yMax= (int)(std::log(toAlign.maxScale/minGlobScale) / logBase) + 1;
//			int xMin= (int)(std::log(reference.minScale/minGlobScale) / logBase) +1 ;
//			int yMin= (int)(std::log(toAlign.minScale/minGlobScale) / logBase) +1;

            int xMin, yMin, xMax, yMax;
            computeScaleBounds(reference, toAlign, maxScaleRef, maxscaleToAl, xMin, xMax, yMin, yMax);

			//std::cout << "Solve " << std::endl;	
			/////// This calculates the confidence only for the scale defined by the first match
            DynamicSolver ds;
            ds.solve_givenScale(*descr, *result,
					 toAlign.multiplier, scaleFact,  xMin, xMax, yMin, yMax);
			
			//std::cout << ds.confidence << std::endl;	

#pragma omp critical
            {
                std::vector<Cand>::iterator it;
                it = output.begin();

                //std::cout << "Confidence " << ds.confidence()  <<std::endl;
                if (output.back().confidence < ds.confidence)
                {
                    cpoint.confidence=ds.confidence;
                    cpoint.ind = l;
                    //std::cout << "Pre-estimate " <<std::endl;
                    cpoint.scale=scaleFact;
                    //std::cout << "Post-estimate " <<std::endl;
                    if (output.back().ind==-1)
                    {
                        //std::cout << "First" <<std::endl;
                        output[0].confidence=cpoint.confidence;
                        output[0].ind=cpoint.ind;
                        output[0].scale=cpoint.scale;
                        //firstPoints.erase(it+1);
                    }
                    else for(unsigned int r=0; r<output.size(); r++)
                    {
                        //std::cout << "Gotcha" <<std::endl;
                        if(output[r].confidence<cpoint.confidence)
                        {
                            output.insert(it+r, cpoint);
                            if(output.size()>OTHER_POINTS)
                                output.pop_back();
                            break;
                        }
                    }
                }
            }
		}
		
	}

	return output;
	

	
}


/*!
 * \brief MultiscaleAlign::extractSIFTPoints
 * \param mesh
 * \param list
 *
 * This method assumes that the mesh descriptor has already been computed.
 * Need to construct a dedicated kdtree containing only the selected points
 */
//void MultiscaleAlign::extractSIFTPoints(const DescriptorBase& mesh,
//                                        std::vector< SiftPoint > &list){
//
//    //std::cout << "Init local KdTree" << std::endl;
//
//    // Init the local KdTree
//    MultiscaleAlignNamespace::KdTree<float> tree (mesh.selection.size());
//    for(int i = 0; i < mesh.selection.size(); i++){
//        const unsigned int& ind = mesh.selection[i];
//        tree.set(ind, mesh.model->cm.vert[ind].cP());
//    }
//    tree.finalize();
//    //std::cout << "Done" << std::endl;
//
//    CMeshO::PerVertexAttributeHandle<std::vector<DerivableSphereFit>* > descriptors;
//    descriptors = tri::Allocator<CMeshO>::GetPerVertexAttribute<std::vector<DerivableSphereFit>* >(mesh.model->cm, std::string("GLSDescr"));
//
//    // number of vertices to compare
//    tree.setMaxNofNeighbors(16);
//
//    //std::cout << "1" << std::endl;
//
////#pragma omp parallel for
//    for(int i = 0; i < mesh.selection.size(); i++)
//    {
//        const unsigned int& ind = mesh.selection[i];
//        //std::cout << "Analyse element " << ind << std::endl;
//
//        std::vector<DerivableSphereFit>* descr = descriptors[ind];
//
//        unsigned int nbScale = descr->size();
//
//        // Collect the neighborhood only when needed
//        bool neiCollected = false;
//
//        //std::cout << "Data collected" << std::endl;
//
//        /// 1 Detect if we are on a local curvature minimum along scale axis
//        for(unsigned int s = nbScale-2; s > 0 ; s--){
//
//            float kprev = descr->at(s-1).kappa();
//            float k     = descr->at(s  ).kappa();
//            float knext = descr->at(s+1).kappa();
//
//            if(! isnan(kprev) && ! isnan(k) && ! isnan(knext) ){
//
//                bool isMin = ( kprev > k && knext > k );
//                bool isMax = ( kprev < k && knext < k );
//
//                if ( isMin || isMax ){
//                    //std::cout << "This guy is an extrema at scale "<< s << std::endl;
//
//                    bool currentExtr = true;
//
//                    if (! neiCollected){
//                        //std::cout << "We need to collect its neighborhood" << std::endl;
//                        neiCollected = true;
//                        MyPoint query(mesh.model->cm.vert[ind]);
//
//#pragma omp critical
//                        tree.doQueryK(query.pos());
//
//                        //std::cout << "Done" << std::endl;
//                    }
//
//                    for(unsigned int ki = 0; ki!=tree.getNofFoundNeighbors(); ki++){
//                        unsigned int nId = tree.getNeighbor(ki).originalId;
//
//                        //std::cout << "Compare with neighbor " << nId << std::endl;
//
//                        if (isMin){
//                            if (k > descriptors[nId]->at(s-1).kappa() ||
//                                k > descriptors[nId]->at(s  ).kappa() ||
//                                k > descriptors[nId]->at(s+1).kappa()) {
//                                currentExtr = false;
//                                //std::cout << "Minimum rejected" << std::endl;
//                                break;
//                            }
//                        }else if (isMax){
//                            if (k < descriptors[nId]->at(s-1).kappa() ||
//                                k < descriptors[nId]->at(s  ).kappa() ||
//                                k < descriptors[nId]->at(s+1).kappa()) {
//                                currentExtr = false;
//                                //std::cout << "Maximum rejected" << std::endl;
//                                break;
//                            }
//                        }
//                    }
//
//                    // We are currently on a local extrema
//                    if (currentExtr){
//                        //std::cout << "Extrema selected and kept !" << std::endl;
//
//                        SiftPoint sp;
//                        sp.ind     = ind;
//                        sp.scaleId = s;
//
//#pragma omp critical
//                        list.push_back(sp);
//
//                        //std::cout << "Update Its color" << std::endl;
//                        mesh.model->cm.vert[ind].C()=Color4b(255,
//                                                             255*int(float(s+1)/float(nbScale)),
//                                                             255*int(float(s+1)/float(nbScale)),
//                                                             1);
//                        break;
//                    }
//                }
//            }
//        }// for scales
//    }// for points
//}


/*!
 * \brief MultiscaleAlign::extractSIFTPoints
 * \param mesh
 * \param list
 *
 * This method assumes that the mesh descriptor has already been computed.
 * Need to construct a dedicated kdtree containing only the selected points
 */
void MultiscaleAlign::extractDescriptivePoints(const DescriptorBase& mesh,
                                        std::vector< DescrPoint > &list){

	

    const float numSel = (float)mesh.selection.size();

	//std::cout << "Got size" << std::endl;
	CMeshO::PerVertexAttributeHandle<std::vector<DerivableSphereFit>* > descriptor;
	descriptor = tri::Allocator<CMeshO>::GetPerVertexAttribute<std::vector<DerivableSphereFit>* >(mesh.model->cm, std::string("GLSDescr"));

	std::vector<DerivableSphereFit> *descr = descriptor[mesh.selection[0]];

    //std::cout << "Got descriptor" << std::endl;

	std::vector<float> meanK;
	std::vector<float> meanT;

	for(int k=0; k<descr->size(); k++)
	{
		meanK.push_back(0.0);
		meanT.push_back(0.0);
	}

	//std::cout << "Initialize" << std::endl;
   
//#pragma omp parallel for
    std::vector<float>::iterator kIt, tIt;
    for(int i = 0; i < mesh.selection.size(); i++)
    {
        const unsigned int& ind = mesh.selection[i];

        //std::cout << "Analyse element " << ind << std::endl;
        descr = descriptor[ind];
        kIt = meanK.begin();
        tIt = meanT.begin();
        for(std::vector<DerivableSphereFit>::const_iterator it=descr->begin();
            it != descr->end(); it++, kIt++, tIt++)
        {
            if(!isnan((*it).kappa()))
            {
                (*kIt) += ((*it).kappa()/numSel);
                (*tIt) += ((*it).tau()/numSel);
            }
        }
    }
    /*for(int i = 0; i <mesh.model->cm.vn; i++)
    {
		mesh.model->cm.vert[i].C()=Color4b(0,0,0,1);

	}*/

	//#pragma omp parallel for
	
    for(int i = 0; i < mesh.selection.size(); i++)
    {
		const unsigned int& ind = mesh.selection[i];
        kIt = meanK.begin();
        tIt = meanT.begin();

		float descript=0.0; float diffK, diffT;
        descr = descriptor[ind];
        for(std::vector<DerivableSphereFit>::const_iterator it=descr->begin();
            it != descr->end(); it++, kIt++, tIt++)
		{
            if(!isnan((*it).kappa()))
			{
                diffK = (*it).kappa()- (*kIt);
                diffT = (*it).tau()  - (*tIt);
                descript += (diffK*diffK+diffT*diffT);
			}
		}
		DescrPoint p; p.ind=ind; p.descrip=descript;
		list.push_back(p);
		/*mesh.model->cm.vert[mesh.selection[i]].Q()=descript;
		mesh.model->cm.vert[mesh.selection[i]].C()=Color4b(255*descript,255*descript,255*descript,1);*/
		//std::cout << "Descriptiveness " << descript << std::endl;


	}
	
}


//
////////// This is the main function decribed inside
//float MultiscaleAlign::process_scale(MeshModel* _reference, float radiusRef, float refCoarse,
//	MeshModel* _toAlign, float radiusToAlign, float toAlignCoarse,
//	float multiplier, unsigned int nbScales, Options opt) {
//
//
//
//	float scaleFact = 0.0; bool done = false;
//
//	/////// Create the scales vector, and the DescriptorBase objects
//	std::vector<float>Scales;
//	Utils::sampleScaleInterval(min(radiusToAlign, radiusRef),
//		max(toAlignCoarse, refCoarse),
//		multiplier,
//		nbScales,
//		Scales);
//
//	DescriptorBase toAlign(_toAlign, radiusToAlign, toAlignCoarse, multiplier, nbScales);
//	std::cout << "Compute toAlign KdTree..." << std::flush;
//	toAlign.kdTree = Utils::makeKNNTree<float>(toAlign.model);
//	std::cout << "DONE" << std::endl;
//
//	DescriptorBase reference(_reference, radiusRef, refCoarse, multiplier, nbScales);
//	std::cout << "Compute reference KdTree..." << std::flush;
//	reference.kdTree = Utils::makeKNNTree<float>(reference.model);
//	std::cout << "DONE" << std::endl;
//
//	std::vector<DescrPoint> descrList;
//	std::vector< int > seeds;
//#define ONLY_ONE
//#ifdef ONLY_ONE
//
//	int toAlignId = 12111;
//	int referenceId = 13785;
//
//	typedef typename std::vector<DerivableSphereFit> Container;
//	using vcg::tri::Allocator;
//
//	CMeshO::PerVertexAttributeHandle<Container* > descriptorsHandler;
//	descriptorsHandler =
//		Allocator<CMeshO>::GetPerVertexAttribute<Container* >
//		(reference.model->cm, std::string("GLSDescr"));
//
//	descriptorsHandler[referenceId] = new std::vector<DerivableSphereFit>();
//
//	computeDescriptor(reference.model->cm.vert[referenceId],
//		Scales,
//		reference,
//		descriptorsHandler[referenceId]);
//
//	CMeshO::PerVertexAttributeHandle<Container* > descriptorsHandler2;
//	descriptorsHandler2 =
//		Allocator<CMeshO>::GetPerVertexAttribute<Container* >
//		(toAlign.model->cm, std::string("GLSDescr"));
//
//	descriptorsHandler2[toAlignId] = new std::vector<DerivableSphereFit>();
//
//	computeDescriptor(toAlign.model->cm.vert[toAlignId],
//		Scales,
//		toAlign,
//		descriptorsHandler2[toAlignId]);
//
//#else
//	QTime time;
//	time.start();
//	std::cout << "[ToAlign] Computing GLS descriptors ..." << std::endl;
//	preComputeDescriptor(toAlign, Scales, true);
//	std::cout << "[ToAlign] GLS decriptors computed in " << time.elapsed() / 1000 << " secondes " << endl;
//
//	//if (opt.useDescriptive)
//	//{
//	//	/// Compute GLS descriptor for the toAlign model
//	//	
//	//	//time.start();
//	//	std::cout << "[ToAlign] Computing GLS descriptors ..." << std::endl;
//	//	preComputeDescriptor(toAlign, Scales, true);
//	//	//std::cout << "[ToAlign] GLS decriptors computed in " <<time.elapsed()/1000 << " secondes "  << endl;
//
//	//	
//	//	/// Extract Sift Point in the ToAlign model
//	//	
//	//	//time.start();
//	//	std::cout << "[ToAlign] Extract Descriptive Points ..." << std::endl;
//	//	extractDescriptivePoints(toAlign, descrList);
//
//	//	seeds=selectSeedsDescr(toAlign, descrList, false);
//	//	
//	//}
//
//
//	//else
//	//{
//	//				
//
//
//	//	/// Init memory to store the GLS descriptor for the toAlignModel
//	//	//the PerVertexAttribute handles is create and each of the vector capacity set to the maximum possible
//	//	std::cout << "[ToAlign] Preparing GLS descriptors ..." << std::endl;
//	//	CMeshO::PerVertexAttributeHandle<std::vector<DerivableSphereFit>* > descriptorsToAlign;
//	//	descriptorsToAlign = tri::Allocator<CMeshO>::GetPerVertexAttribute<std::vector<DerivableSphereFit>* >(toAlign.model->cm, std::string("GLSDescr"));
//	//	std::cout << "Done." << endl;
//
//	//	
//	//	int init=rand() % toAlign.selection.size();
//	//	std::cout << "init= " << init << " ind= " << toAlign.selection[init] << std::endl;
//
//	//#ifdef DEBUG_SINGLE_POINT
//
//	//	seeds.resize(1);
//
//	//	seeds[0] = 23285;
//	//	int seeId = seeds[0];
//
//	//	_toAlign->cm.vert[seeId].Q()=MAX_SCALE_MULTIPLIER*_toAlign->cm.bbox.Diag();
//	//	std::cout << "Quality of seed: " << _toAlign->cm.vert[seeId].Q()  << std::endl;
//
//	//	reference.selection.clear();
//	//	reference.selection.push_back(seeId);
//
//	//	for (CMeshO::VertexIterator vj = _reference->cm.vert.begin(); vj != _reference->cm.vert.end(); ++vj)
//	//		(*vj).ClearS();
//
//	//	_reference->cm.vert[seeId].SetS();
//	//	_reference->cm.vert[seeId].Q()=MAX_SCALE_MULTIPLIER*_reference->cm.bbox.Diag();
//	//	_reference->cm.vert[seeId].C()=Color4b(255,255,0,1);
//
//	//	CMeshO::PerVertexAttributeHandle<std::vector<DerivableSphereFit>* > referenceDescriptorsHandler;
//	//	referenceDescriptorsHandler = tri::Allocator<CMeshO>::GetPerVertexAttribute<std::vector<DerivableSphereFit>* >(reference.model->cm, std::string("GLSDescr"));
//
//	//	referenceDescriptorsHandler[seeId] = new std::vector<DerivableSphereFit>();
//	//	computeDescriptor(reference.model->cm.vert[seeId], Scales, reference, referenceDescriptorsHandler[seeId]);
//
//	//	std::cout << "Testing on one point only" << std::flush;
//
//	//#else
//	//	seeds = selectSeeds(toAlign.model, reference.model, toAlign.selection[init]);
//	//	//seeds = selectSeeds(toAlign.model, reference.model, -1);
//	//#endif
//	//}
//
//
//	/// Compute GLS descriptor for the reference model
//	//QTime time;
//	time.start();
//	std::cout << "[Reference] Computing GLS descriptors ..." << std::endl;
//	preComputeDescriptor(reference, Scales, true);
//	std::cout << "[Reference] GLS decriptors computed in " << time.elapsed() / 1000 << " secondes " << endl;
//
//
//
//
//#ifdef DEBUG_SIMILARITY_MAP
//	seeds.resize(1);
//	seeds[0] = 6639;
//#endif
//
//	// output seeds for debug purpose
//	for (unsigned int j = 0; j < seeds.size(); j++) {
//		std::cout << "Seed " << j << ": ind " << seeds[j] << ", quality " << toAlign.model->cm.vert[seeds[j]].Q() << std::endl;
//		toAlign.model->cm.vert[seeds[j]].C() = Color4b(255, 0, 0, 1);
//	}
//
//	//return -1;
//#endif
//#define TEST
//#ifdef TEST
//	/// Build simimarity map
//
//	std::cout << "Start" << std::endl;
//
//	//unsigned int ind = 13785;
//	CMeshO::PerVertexAttributeHandle<std::vector<DerivableSphereFit>* > referenceDescriptorsHandler;
//	referenceDescriptorsHandler = tri::Allocator<CMeshO>::GetPerVertexAttribute<std::vector<DerivableSphereFit>* >(reference.model->cm, std::string("GLSDescr"));
//
//	referenceDescriptorsHandler[referenceId] = new std::vector<DerivableSphereFit>();
//	computeDescriptor(reference.model->cm.vert[referenceId], Scales, reference, referenceDescriptorsHandler[referenceId]);
//
//	reference.model->cm.vert[referenceId].C() = Color4b(0, 255, 0, 1);
//	//float maxScale = MAX_SCALE_MULTIPLIER*reference.model->cm.bbox.Diag();
//
//	for (CMeshO::VertexIterator vj = toAlign.model->cm.vert.begin(); vj != toAlign.model->cm.vert.end(); ++vj)
//		(*vj).Q() = 0.0;
//
//	std::cout << "Selected points: " << toAlign.selection.size() << std::endl;
//	toAlign.selection.clear();
//	toAlign.selection.push_back(toAlignId);
//	//toAlign.selection.push_back(2026);
//
//	// output seeds for debug purpose
//	for (int i = 0; i < toAlign.selection.size(); i++) {
//
//		unsigned int indToAl = toAlign.selection[i];
//
//		// Use a local variable for each thread
//		//float maxscaleToAl = MAX_SCALE_MULTIPLIER*toAlign.model->cm.bbox.Diag();
//		int xMin, yMin, xMax, yMax;
//		computeScaleBounds(reference, toAlign, refCoarse, toAlignCoarse, xMin, xMax, yMin, yMax);
//
//		CMeshO::PerVertexAttributeHandle<std::vector<DerivableSphereFit>* > descriptorsHandlerAlign;
//		descriptorsHandlerAlign = tri::Allocator<CMeshO>::GetPerVertexAttribute<std::vector<DerivableSphereFit>* >(toAlign.model->cm, std::string("GLSDescr"));
//
//		//reference.model->cm.vert[ind].C() = Color4b(0, 255, 0, 1);
//		std::vector<DerivableSphereFit> *descrAlign = descriptorsHandlerAlign[indToAl];
//
//		//std::cout << "xMin: " << xMin << "xMax: " << xMax << "yMin: " << yMin << "yMax: " << yMax << "Maxscale: " << maxScale << "MaxScaleToAl: " << maxscaleToAl << std::endl;
//		DynamicSolver ds;
//		ds.solve(*referenceDescriptorsHandler[referenceId], referenceDescriptorsHandler[referenceId]->size(),
//			*descrAlign, descrAlign->size(),
//			toAlign.multiplier, xMin, xMax, yMin, yMax);
//
//		//std::cout << "xBest: " << ds.xBest << "yBest: " << ds.yBest << "multiplier: " << toAlign.multiplier << std::endl;
//
//		float scaleEst = Scalar(1.0) / std::pow(toAlign.multiplier, ds.yBest - ds.xBest);
//		std::cout << "Scale: " << scaleEst << std::endl;
//		toAlign.model->cm.vert[indToAl].Q() = scaleEst;
//		int confi = (int)(ds.confidence*255.0);
//		toAlign.model->cm.vert[indToAl].C() = Color4b(confi, confi, confi, 1);
//
//	}
//
//	//culo
//
//	//toAlign.model->cm.vert[seeds[0]].C() = Color4b(0, 255, 0, 1);
//	//toAlign.model->cm.vert[seeds[0]].Q() = 10;
//
//
//	return 1.;
//#endif
//
//#ifdef DEBUG_SIMILARITY_MAP
//	/// Build simimarity map
//	std::vector<DerivableSphereFit>* descr = new std::vector<DerivableSphereFit>();
//	computeDescriptor(toAlign.model->cm.vert[seeds[0]], Scales, toAlign, descr);
//
//	float maxscaleToAl = toAlign.model->cm.vert[seeds[0]].Q();
//
//	// Compute the similarity and put it into the quality field
//	computeSimilarityMap(reference, toAlign, descr, maxscaleToAl);
//
//	toAlign.model->cm.vert[seeds[0]].C() = Color4b(0, 255, 0, 1);
//	//toAlign.model->cm.vert[seeds[0]].Q() = 10;
//
//
//	return 1.;
//#endif
//	//unsigned int count=1;
//	//done=true;
//
//	//#define ONLY_THE_FIRST_SEARCH 
//#ifdef ONLY_THE_FIRST_SEARCH
//	CVertexO vj = reference.model->cm.vert[seeds[0]];
//
//	MyPoint query(vj);
//	query.normal().normalize();
//
//	// Use a local variable for each thread
//	float maxScaleRef = vj.Q();
//	float maxscaleToAl = toAlign.model->cm.vert[seeds[0]].Q();
//	int xMin, yMin, xMax, yMax;
//	computeScaleBounds(reference, toAlign, maxScaleRef, maxscaleToAl, xMin, xMax, yMin, yMax);
//	//std::cout << "xMin: " << xMin << " xMax: " << xMax << " yMin: " << yMin << " yMax: " << yMax <<std::endl;
//
//	std::vector<DerivableSphereFit> descrRef, descrAlign;
//	computeDescriptor(query, Scales, reference, &descrRef);
//	computeDescriptor(toAlign.model->cm.vert[seeds[0]], Scales, toAlign, &descrAlign);
//
//	DynamicSolver ds;
//	ds.solve(descrRef, descrRef.size(),
//		descrAlign, descrAlign.size(),
//		toAlign.multiplier, xMin, xMax, yMin, yMax);
//
//	float scale = Scalar(1.0) / std::pow(toAlign.multiplier, ds.yBest - ds.xBest);
//	std::cout << "Scale: " << scale << std::endl;
//
//	return -1;
//#else
//
//	std::vector< std::vector < Cand > > seedList; int reIter = 0;
//	for (int i = 0; i<seeds.size(); i++)
//	{
//		if (done || reIter == 5)
//		{
//			break;
//		}
//
//
//		//// The first seed is chosen
//		//toAlign.maxScale=toAlign.model->cm.vert[seeds[i]].Q();
//		std::cout << "Computing seed " << i + 1 << " of " << seeds.size() << std::endl;
//		//std::cout << "Seed point: " << seeds[0] << " toAlignCoarse: " << toAlign.model->cm.vert[seeds[0]].Q() << std::endl;
//
//		///// Given the first seed, we search for the scale and transformation to find the alignmen
//		/// The last parameter avoid to recompute the toAlign descriptor when it is already done
//		seedList.push_back(searchMatches(toAlign, reference, seeds[i], Scales, !opt.useDescriptive));
//
//
//
//		//std::cout << "Done searchcorr" << std::endl;
//		if (seedList.size()>1)
//		{
//			done = searchTranf(toAlign, reference, seeds, seedList, Scales, opt);
//			//done=true;
//
//		}
//
//		if (i == seeds.size() - 1)
//		{
//			reIter++;
//			std::cout << "Shuffling the seeds, try n. " << reIter + 1 << std::endl;
//			seeds.clear();
//			seedList.clear();
//			//std::cout << "Seeds size " << seeds.size() << std::endl;
//			if (!opt.useDescriptive)
//			{
//				int init = rand() % toAlign.selection.size();
//				std::cout << "init= " << init << " ind= " << toAlign.selection[init] << std::endl;
//				seeds = selectSeeds(toAlign.model, reference.model, toAlign.selection[init]);
//				i = -1;
//			}
//			else
//			{
//				//reIter=5;
//				seeds = selectSeedsDescr(toAlign, descrList, true);
//				i = -1;
//			}
//
//
//		}
//
//
//
//	}
//
//
//	///// Actually, no scale factor is returned by the function, but the transformation is applied
//	if (done == false)
//		return -1;
//	else
//		return getScaleFromTransf(toAlign.model->cm.Tr);
//
//#endif
//}


