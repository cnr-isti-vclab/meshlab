#ifndef _DYNAMIC_SOLVER_
#define _DYNAMIC_SOLVER_

#include "Eigen/Core"
#include <vector>

#include "basics.h"

#include <iostream>


#ifdef _MSC_VER
#include <float.h>  // for _isnan() on VC++
#define isnan(x) _isnan(x)  // VC++ uses _isnan() instead of isnan()
//#else
//#include <math.h>  // for isnan() everywhere else
#endif

namespace DynamicProg{

  /*!
    Generic class that delegates the computation to DynProcess. You can use for
    instance NeedlemanWunsch.
   */
  template <typename Scalar, class DynProcess>
  class DynamicSolver{
    public:
      typedef Eigen::Matrix<DynamicStep<Scalar>, Eigen::Dynamic, Eigen::Dynamic> StepMatrix;

      int xMin,xMax,yMin,yMax;
	  //std::vector<std::pair<float, float>> estScales;
      //int xBest,yBest;
	  std::vector<std::pair<float, float>> estScales;
      //Scalar best;
      //int size;
      Scalar confidence;
      //Scalar bestMax;

    private:
      //! \brief Relation matrix
      StepMatrix                                    _matrix;
      //! \brief Delegate that perform the dynamic programming processing
      DynProcess                                    _delegate;
      //! \brief Output path computed by the solver
      std::vector< DynamicStepCoordinates<Scalar> > _path;

    public:
      template <class DataArray>
      inline void solve(const DataArray &d1, unsigned int d1Size,
                        const DataArray &d2, unsigned int d2Size,
                        double multiplier, int _xMin, int _xMax, int _yMin, int _yMax,
                        bool normalize = true) {
        const unsigned int h = d1Size;
        const unsigned int w = d2Size;

        xMin=_xMin;
        xMax=_xMax;
        yMin=_yMin;
        yMax=_yMax;
        _matrix = StepMatrix::Zero(w, h);

        // compensate for invalid descriptor values
        while(xMax-1 > xMin && isnan(d1[xMax-1].kappa())) xMax--;
        while(yMax-1 > yMin && isnan(d2[yMax-1].kappa())) yMax--;
        while(xMin < xMax && isnan(d1[xMin].kappa()))     xMin++;
        while(yMin < yMax && isnan(d2[yMin].kappa()))     yMin++;

        // deal with degenerate cases
       /* if(xMin >= xMax || yMin >= yMax){
            size = 0;
            confidence = 0;

            return;
        }*/

        // Build cost matrix
        for (unsigned int j = yMin; j != yMax; j++)
          for (unsigned int i = xMin; i != xMax; i++)
              _matrix(i,j) = _delegate.eval(d1[i], d2[j], i, j, _matrix, multiplier);

		int maxDiag = std::min((yMax - yMin + 1), (xMax - xMin + 1));
		//std::cout << "MaxDiag: " << maxDiag  << endl;

		for (int j = yMin; j != yMax; j++)
		{ 
			int size = j - xMax +1;
			float scaleEst = 1.0f / std::pow((float)multiplier, (float)size);
			//std::cout << "Size: " << size << " j: " << j << " xMax: " << xMax << " Scaleest: " << scaleEst << endl;
			_matrix(xMax - 1, j).value /= std::min((float)(j - yMin + 1), (float)maxDiag);
			std::pair<float, float> cand (scaleEst, _matrix(xMax - 1, j).value);
			estScales.push_back(cand);
		}

		for (int j = xMin; j != xMax -1; j++)
		{
			float scaleEst = 1.0f / std::pow((float)multiplier, (float)(yMax - j - 1));
			//std::cout << "j: " << j << " yMax: " << yMax << "Scaleest: " << scaleEst << endl;
			_matrix(j, yMax - 1).value /= std::min((float)(j - xMin + 1), (float)maxDiag);
			std::pair<float, float> cand(scaleEst, _matrix(j, yMax - 1).value);
			estScales.push_back(cand);
		}

		std::sort(estScales.begin(), estScales.end(), [](auto &left, auto &right) {
			return left.second > right.second;
		});

		//std::cout << "xMin " << xMin << " " << " xMax " << xMax << " yMin " << yMin << " yMax " << yMax  << endl;
		/*for (int j = 0; j != estScales.size(); j++)
		{
			std::cout << "Scale: " << estScales[j].first << " Confidence: " << estScales[j].second << endl;
		}*/

		

   //     // normalize by the number of scales and get the best value
   //      best=0.0;
   //      for (int j=yMin; j<yMax; j++){
   //          for (int i=xMin; i<xMax; i++)
   //          {

   //              if(normalize){
   //                  int diag   = std::max(i,j);
   //                  int offset = std::max(std::abs(diag-i), std::abs(diag-j));
   //                  float norm = diag-offset;
   //                  norm *= norm;
   //                  _matrix(i,j).value /= norm;
   //              }

   //              if(_matrix(i,j).value>best)
   //              {
   //                  best=_matrix(i,j).value;
   //                  xBest=i;
   //                  yBest=j;
   //              }
   //          }
   //      }

		 //vector<float> maxVal;
		 //vector<float> minVal;
		 //for (unsigned int i = 0; i < h; i++)
		 //{
			//maxVal.push_back(0.0f);
			//minVal.push_back(1000.0f);
		 //}
		 //for (unsigned int j = yMin; j <= yMax; j++)
		 //{
			// float maxx = 0.0; float minn = 1000.0;
			// for (unsigned int i = xMin; i <= xMax; i++)
			// {
			//	 if (_matrix(i, j).value > maxx)
			//		 maxx = _matrix(i, j).value;
			//	 if (_matrix(i, j).value < minn)
			//		 minn = _matrix(i, j).value;
			// }
			// maxVal[j] = maxx;
			// minVal[j] = minn;
		 //}
		 //for (unsigned int j = 0; j != h - 1; j++)
		 //{
			// for (unsigned int i = 0; i != w - 1; i++)
			// {
			//	 if (maxVal[j] == 0.0f)
			//		 _matrix(i, j).value = 0;
			//	 else
			//		 _matrix(i, j).value= (_matrix(i, j).value - minVal[j]) / (maxVal[j] - minVal[j]);
			// }
			// 
		 //}

#define SAVE_MATRIX_CSV
#ifdef SAVE_MATRIX_CSV

		 QFile file2("matrix.xls");
		 file2.open(QFile::WriteOnly);
		 QTextStream stream2(&file2);
		 for (unsigned int j = 0; j != h - 1; j++)
		 {
			 for (unsigned int i = 0; i != w - 1; i++)
			 {
				stream2 << _matrix(i, j).value << "\t";
			 }
			stream2 << "\n";
		 }
		 file2.close();
#endif

//		for (int j=yMin; j<yMax; j++)
//			for (int i=xMin; i<xMax; i++)
//			{
//				if(_matrix(i,j).value>best)
//				{
//					best=_matrix(i,j).value;
//					xBest=i;
//					yBest=j;
//				}

//			}
        //size=std::min(Scalar(yBest-yMin)+1,Scalar(xBest-xMin)+1);
        //////
        // This is a final normalization of the value, I'm not sure it will be useful... ask me to explain, in the case. I commented
        /////
        //std::cout << "size " << size << " " << " best " << best << endl;
        /*bestMax=0.0; double dMax=0.0;
        for(int i=0; i<d2.size(); i++)
        {
            if(abs(d2[i].kappa())>dMax)
                dMax=abs(d2[i].kappa());
        }
        for(int i=yMin; i<=yMax; i++)
        {
            if(abs(d2[i].kappa())>bestMax)
                bestMax=abs(d2[i].kappa());
        }
        confidence=best*bestMax/(size*dMax);*/

        /*confidence=best/(double)size;
        std::cout << "confidence " << confidence << endl;*/

        
      }


      //! \brief Read-access to the raw relation matrix used by the solver
      inline const StepMatrix& stepMatrix() const
      {return _matrix; }


    inline void
      setConfidence(Scalar conf){
          _delegate.setConfidence(conf);
    }

    ////// This is the solve when only a single scale is needed
    template <class DataArray>
      inline void solve_givenScale(const DataArray &d1, const DataArray &d2, double multiplier,
                        double scale, int _xMin, int _xMax, int _yMin, int _yMax) {

        xMin=_xMin;
        xMax=_xMax;
        yMin=_yMin;
        yMax=_yMax;

        double logBase=std::log((double)multiplier);
        int shift = (int)(std::log(scale) / logBase + 0.5);
		//std::cout << "Shift " << shift << std::endl;
		//std::cout << "xMax " << xMax << " yMax " << yMax << std::endl;

        std::vector<double> compare;

        //std::cout << "Calc diff" << std::endl;

       for (unsigned int j = yMin; j != yMax; j++)
       {
           if ((j+shift)>xMin && (j+shift)<xMax)
           {
               float val;
               if (compare.size()==0)
                   val=(_delegate.eval_couples(d1[j+shift], d2[j], j+shift, j, _matrix, multiplier));
               else
                   val=compare.back() + (_delegate.eval_couples(d1[j+shift], d2[j], j+shift, j, _matrix, multiplier));
               //std::cout << "Val " << val  << " d1 " << j+shift << " d2 " << j  << endl;
               compare.push_back(val);
           }
       }
	   confidence = compare.back();
       //std::cout << "End calc diff" << std::endl;
        //////
        // This is a final normalization of the value, I'm not sure it will be useful... ask me to explain, in the case
        /////
        /*best=0.0;
        size=std::min(Scalar(yBest-yMin)+1,Scalar(xBest-xMin)+1);
        for (int j=0; j<compare.size(); j++)
        {
            if(compare[j]>best)
            {
                best=compare[j];
                size=j;
            }

        }
        confidence=best/size;*/
        //std::cout << "End calc best" << std::endl;
        /*bestMax=0.0; double dMax=0.0;
        for(int i=0; i<d2.size(); i++)
        {
            if(abs(d2[i].kappa())>dMax)
                dMax=abs(d2[i].kappa());
        }
        for(int i=yMin; i<=yMax; i++)
        {
            if(abs(d2[i].kappa())>bestMax)
                bestMax=abs(d2[i].kappa());
        }
        confidence=best*bestMax/(size*dMax);*/
        //std::cout << "End calc conf" << std::endl;

        //std::cout << "shift " << shift  << " Best " << best << " dMax " << dMax << " BestMax " << dMax << " tMax " << tMax << " Size " << size << " Confidence " << confidence << endl;
      }

 ////// This is the solve when only a single scale is needed
    
  }; //class DynamicSolver



} // namespace DynamicProg

#endif // _DYNAMIC_SOLVER_

