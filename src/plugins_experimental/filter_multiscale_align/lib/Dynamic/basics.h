#ifndef _DYNAMIC_BASICS_
#define _DYNAMIC_BASICS_

namespace DynamicProg{

  namespace DynamicRelation{
    enum {
      Left    = 0x01,      
      Top     = 0x02,
      TopLeft = 0x04,
      Nothing = 0x08,
    };  
  }


  template <typename Scalar>
  class DynamicStep {
    public:
      Scalar value;
      int relationFlags;
    
      inline DynamicStep(Scalar v  = 0,
                         int flags = DynamicRelation::Nothing)
             :value(v), relationFlags(flags) {}
  }; //struct DynamicStep 
  
  template <typename Scalar>
  inline bool operator< (const DynamicStep<Scalar>& s1, 
                         const DynamicStep<Scalar>& s2){
    return s1.value < s2.value;
  }
  
  template <typename Scalar>
  inline bool operator> (const DynamicStep<Scalar>& s1, 
                         const DynamicStep<Scalar>& s2){
    return s1.value > s2.value;
  }
  
  template <typename Scalar>
  inline bool operator<= (const DynamicStep<Scalar>& s1, 
                         const DynamicStep<Scalar>& s2){
    return s1.value <= s2.value;
  }
  
  
  template <typename Scalar>
  static inline
  std::ostream & operator<< (std::ostream &o,
                            const DynamicStep<Scalar> &step){
      o << "(" << step.value;
      if(step.relationFlags & DynamicProg::DynamicRelation::TopLeft)
        o << ",topleft";
      else if(step.relationFlags & DynamicProg::DynamicRelation::Top)
        o << ",top";
      else  if(step.relationFlags & DynamicProg::DynamicRelation::Left)
        o << ",left";
      o << ")";
      return o;
  }
  
  
  template <typename Scalar>
  struct DynamicStepCoordinates {
    unsigned int x, y;
    Scalar value;
  }; //struct DynamicStep 
  template <typename Scalar>
  static inline
  std::ostream & operator<< (std::ostream &o,
                            const DynamicStepCoordinates<Scalar> &dstep){
      o << "step( " << dstep.x;
      o << " , "   << dstep.y;
      o << " ) = " << dstep.value;
      o << "";
      return o;
  }
  
} // namespace DynamicProg

#endif // _DYNAMIC_SOLVER_

