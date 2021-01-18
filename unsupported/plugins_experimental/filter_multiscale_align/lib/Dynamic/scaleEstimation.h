#ifndef _SCALE_ESTIMATION_
#define _SCALE_ESTIMATION_

#include "Eigen/Core"
#include <vector>
#include <set>

#include "basics.h"

#include <iostream>

namespace DynamicProg{

  /*! 
    Estimate the relative stretch involved in a similarity matrix. 
    The reference scale is associated to data on the y-axis, and stretch is 
    given for the data on the x-axis.
   */
  template <typename Scalar>
  class VotingStretchEstimation{   
    public:
      typedef typename Eigen::Matrix<DynamicStep<Scalar>, Eigen::Dynamic, Eigen::Dynamic> StepMatrix;
      typedef typename std::vector< DynamicStepCoordinates<Scalar> > Path;
      
    public:
    /*!
     */    
      inline Scalar estimate(int xBest, int yBest, double multiplier)const {

          //if (xBest<yBest)
			  return 1.0/pow(multiplier, yBest-xBest);
          //else
            //  return pow(multiplier, xBest-yBest);

      }

  }; //class DynamicSolver

  /*!
    Simulate a convolution by getting the best score from the bottom and right borders
    of the StepMatrix
   */
  template <typename Scalar>
  class ConvolutionStretchEstimation{
    public:
      typedef typename Eigen::Matrix<DynamicStep<Scalar>, Eigen::Dynamic, Eigen::Dynamic> StepMatrix;
      typedef typename std::vector< DynamicStepCoordinates<Scalar> > Path;

  protected:
      int    m_offset;   // offset
      Scalar m_score; // score
      Scalar m_multiplier;

    public:

      inline ConvolutionStretchEstimation()
          :m_offset(0), m_score(0.), m_multiplier(1.) {}

      /*
       * Iterate on the bottom and right border, and record the position with the
       * highest score
       */
      inline Scalar estimate(const StepMatrix& matrix, Scalar multiplier) {

          const int w = matrix.cols();
          const int h = matrix.rows();

		  m_multiplier = multiplier;
          m_offset     = 0;
          m_score      = 0.f;

          for(int s = 0; s != w; s++){
              const Scalar& v = matrix.template bottomRows<1>()(s).value;
              if (v > m_score) {
                  m_offset = s-w+1;
                  m_score  = v;
              }
          }

          for(int s = 1; s < h; s++){
              const Scalar& v = matrix.template rightCols<1>()(h-s-1).value;
              if (v > m_score) {
                  m_offset = s;
                  m_score  = v;
              }
          }

          return prevEstimation();
      }

      template <class stream>
      inline
      void print (const StepMatrix& matrix, stream& str) const {

          const int w = matrix.cols();
          const int h = matrix.rows();

          for(int s = 0; s != w; s++)
              str << s-w+1 << " "
                  << matrix.template bottomRows<1>()(s).value << std::endl;

          for(int s = 1; s < h; s++)
              str << s << " "
                  << matrix.template rightCols<1>()(h-s-1).value << std::endl;
      }

      //! \brief Read access to the previous estimated scale
      inline Scalar prevEstimation() const { return Scalar(1.0)/std::pow(m_multiplier, m_offset); }
      //! \brief Read access to the score associated to the previous estimated scale
      inline Scalar prevConfidence() const { return m_score; }

  };
  
} // namespace DynamicProg

#endif // _DYNAMIC_SOLVER_

