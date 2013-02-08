//=============================================================================
//
//  CLASS MutablePriorityQueue
//
//=============================================================================


#ifndef COMISO_MUTABLEPRIORITYQUEUET_HH
#define COMISO_MUTABLEPRIORITYQUEUET_HH


//== INCLUDES =================================================================

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================

	      

/** \class MutablePriorityQueue MutablePriorityQueue.hh 

    Brief Description.
  
    A more elaborate description follows.
*/


// Helper Class for sorting triples
template<class A, class B, class C>
class Triple
{
public: 

  // default constructor
  Triple() {}
  Triple(const A& _a, const B& _b, const C& _c) : first(_a), second(_b), third(_c) {}

  bool operator<(const Triple& _t) const
  { return (  this->first <  _t.first || 
	      (this->first == _t.first && ( this->second <  _t.second || 
					    ( this->second == _t.second && this->third < _t.third))));}
	
  void print_info()
  { std::cerr << first << " " << second << " " << third << std::endl; }

  A first;
  B second;
  C third;
};

template<class VType, class IType>
class MutablePriorityQueueT
{
public:


  typedef Triple<VType,IType,unsigned int> TripleVII;
   
  /// Default constructor
  MutablePriorityQueueT() {}
 
  /// Destructor
  ~MutablePriorityQueueT() {}

  // reset timestamps
  void clear(int _n)
  {
    timestamp_.clear();
    timestamp_.resize(_n,0);
  }

  // update value of index _id
  void update(const IType& _id, const VType& _value)
  {
    queue_.insert( TripleVII(_value,_id, ++timestamp_[_id]));
  }

  // get index of next element4
  IType get_next()
  {
    while(!queue_.empty())
    {
      // get and delete first element
      TripleVII tri = *queue_.begin();
      queue_.erase(queue_.begin());

      // if valid -> return
      if( timestamp_[tri.second] == tri.third)
	return tri.second;
    }

    // if empty return dummy
    return IType(0);
  }


  // priority queue empty?
  bool empty()
  {
    while(!queue_.empty())
    {
      // get and delete first element
      TripleVII tri = *queue_.begin();
      
      // if valid -> return true
      if( timestamp_[tri.second] == tri.third)
	return false;

      queue_.erase(queue_.begin());
    }
    return true;
  }
  
private:

  // timestamps
  std::vector<unsigned int> timestamp_;

  // priority queue
  std::set<TripleVII> queue_;
};


//=============================================================================
} // namespace COMISO
//=============================================================================
#endif // COMISO_MUTABLEPRIORITYQUEUET_HH defined
//=============================================================================

