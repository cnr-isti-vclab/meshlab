#ifndef _DYNAMIC_NEEDLEMANWUNSCH_
#define _DYNAMIC_NEEDLEMANWUNSCH_

#include "Eigen/Core"
#include <vector>
#include <iostream>

#include "basics.h"

#ifdef _MSC_VER
#include <float.h>  // for _isnan() on VC++
#define isnan(x) _isnan(x)  // VC++ uses _isnan() instead of isnan()
//#else
//#include <math.h>  // for isnan() everywhere else
#endif

namespace DynamicProg{
  /*!
    Global alignment
    
    cmp: must define an eval function
   */
  template <typename Scalar, class DataType, template <typename, class> class Cmp >
  class NeedlemanWunsch{
    private:
      Cmp<Scalar, DataType> _cmp;
      Scalar _gapPenalty;
      Scalar _confidence;

  public:
      typedef typename Eigen::Matrix<DynamicStep<Scalar>, Eigen::Dynamic, Eigen::Dynamic> StepMatrix;
      
    public:
      inline NeedlemanWunsch(): _gapPenalty(-1), _confidence(0) {}
      
      inline DynamicStep<Scalar> eval (const DataType& v1,
                                       const DataType& v2,
                                       unsigned int x,
                                       unsigned int y,
                                       const StepMatrix& matrix, double multiplier) const;

	  inline Scalar eval_couples (const DataType& v1,
                                       const DataType& v2,
                                       unsigned int x,
                                       unsigned int y,
                                       const StepMatrix& matrix, double multiplier) const;
      
      
      inline void setConfidence(double conf);
                                       
      inline void setGapPenalty(Scalar gapPenalty) {_gapPenalty = gapPenalty;}
      inline Scalar confidence() const { return _confidence;}

     
      
  }; //class NeedlemanWunsch

  
  template <typename Scalar, class DataType, template <typename, class> class Cmp >
  DynamicStep<Scalar>
  NeedlemanWunsch<Scalar, DataType, Cmp >::eval( const DataType& v1,
                                      const DataType& v2,
                                      unsigned int x,
                                      unsigned int y,
                                      const StepMatrix& matrix,  double multiplier) const{
    
	//std::cout << "here" << std::endl; 
    DynamicStep<Scalar>nei[3]; // will contain top, left and topleft
    
   
    nei[2] = DynamicStep<Scalar> (matrix(x-1, y-1).value + _cmp.eval(v1, v2), 
                                  DynamicRelation::TopLeft);

	
	return nei[2];
    
      
  }

  template <typename Scalar, class DataType, template <typename, class> class Cmp >
  Scalar
  NeedlemanWunsch<Scalar, DataType, Cmp >::eval_couples( const DataType& v1,
                                      const DataType& v2,
                                      unsigned int x,
                                      unsigned int y,
                                      const StepMatrix& /*matrix*/, double multiplier) const{
    
	   
   
    return _cmp.eval(v1, v2);
	
    
      
  }
  
  template <typename Scalar, class DataType, template <typename, class> class Cmp >
  void
  NeedlemanWunsch<Scalar, DataType, Cmp >::setConfidence(double conf)
  {

      _confidence = conf;
      
  }



} // namespace DynamicProg


#endif // _DYNAMIC_PATH

