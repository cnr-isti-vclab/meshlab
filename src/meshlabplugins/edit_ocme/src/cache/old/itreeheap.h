#ifndef ITREEHEAP_H
#define ITREEHEAP_H


/* interval heap (min and max) inspired
  Min-Max Heaps and Generalized Priority Queues
  M. D. ATKINSON,J.-R. SACK, N. SANTORO,and T. STROTHOTTE

 This is a heap of intervals [min, max] where the children interval
 are included in the parent interval

 implemented using a tree

 compared to a stl heap, rebuild is 15% longer, extraction is 2x longer
 written by Federico Ponchio
*/
#include <assert.h>
#include <vector>

using namespace std;

template <class T>
class IntervalTreeHeap: public std::list<T> {
public:
  std::list<T>::iterator iterator;
  class Item {
   public:
    T value;
    iterator left, right, parent, twin;
    bool isMin;
  };

  void push(const T& elt) {

    push_back(elt);
    bubbleUp(this->size()-1);
  }

  T &min() { return this->front(); } //root is smaller element
  T popMin() {
    T elt = this->front();
    this->front() = this->back();
    this->pop_back();
    trickleDownMin(0); //enforce minmax heap property
    return elt;
  }

  T &max() {
    if(this->size() == 1) return at(0); //max is second element
    return at(1);
  }

  T popMax() {
    int p = 1;
    if(this->size() == 1) p = 0;
    T elt = at(p);
    at(p) = this->back(); //max is replaced with last item.
    this->pop_back();
    trickleDownMax(p); //enforce minmax heap property
    return elt;
  }

  void rebuild() { //just reinsert all elements, (no push back necessary, of course
    for(unsigned int i = 0; i < this->size(); i++)
      bubbleUp(i);
  }

protected:
  T &at(int n) { return std::vector<T>::at(n); }

  int isMax(int e) const { return e & 1; }
  int parentMin(int i) const { return (((i+2)>>2)<<1) - 2; }
  int parentMax(int i) const { return (((i+2)>>2)<<1) - 1; }
  int leftChildMin(int i) const { return (((i+2)>>1)<<2) -2; }
  int leftChildMax(int i) const { return (((i+2)>>1)<<2) -1; }

  void swap(int a, int b) { T tmp = at(a); at(a) = at(b); at(b) = tmp; }

  int smallestChild(int i) { //return smallest of children or self if no children
    int l = leftChildMin(i);
    if(l >= this->size()) return i; //no children, return self

    int r = l+2; //right child
    if(r < this->size() && at(r) < at(l))
      return r;
    return l;
  }

  int greatestChild(int i) { //return smallest of children or self if no children
    int l = leftChildMax(i);
    if(l >= this->size()) return i; //no children, return self

    int r = l+2; //right child
    if(r < this->size() && at(r) > at(l))
      return r;
    return l;
  }

//all while involving swaps could be optimized perofming circular swaps.
  void trickleDownMin(int i) {   //assert(invert == isOnMaxLevel(i));
    while(1) {

      //find smallest child
      int m = leftChildMin(i);
      if(m >= this->size()) break;
      int r = m+2;
      if(r < this->size() && at(r) < at(m))
        m = r;

      if(at(m) < at(i)) { //if child is smaller swap
        swap(i, m);
        i = m;
      } else //no swap? finish.
        break;

      m = i+1;       //enforce order in interval
      if(m >= this->size()) break;
      if(at(m) < at(i))
        swap(i, m);
    }
  }

  void trickleDownMax(int i) {
    //T tmp = at(i);
    //int n = i;
    while(1) {
       //find greatest child
      int m = leftChildMax(i);
      if(m >= this->size()) break;
      int r = m+2;
      if(r < this->size() && at(r) > at(m))
        m = r;

      if(at(m) > at(i)) {
        swap(i, m);
        //at(i) = at(m);
       // n = m;
        i = m;
      } else
        break;

      m = i-1;       //enforce order in interval
      if(m >= this->size()) break;
      if(at(m) > at(i)) {
        swap(i, m);
        //at(i) = at(m);
        //at(m) = tmp;
       // tmp = at(i);
        //n = i;

      }
    }
    //at(n) = tmp;
  }

  void bubbleUpMin(int i) {
    while(1) {
      int m = parentMin(i);
      if(m < 0) break;
      if(at(m) > at(i)) {
        swap(i, m);
        i = m;
      } else
        break;
    }
  }
  void bubbleUpMax(int i) {
    while(1) {
      int m = parentMax(i);
      //cout << "bubble up max: " << m << " of " << i << endl;
      if(m < 0) break;
      if(at(m) < at(i)) {
        swap(i, m);
        i = m;
      } else
        break;
    }
  }
  void bubbleUp(int i) {
    //cout << "bubble up " << i << endl;
    if(isMax(i)) {
      int m = i-1;
      if(at(m) > at(i)) {
        swap(i, m);
        bubbleUpMin(m);
      } else
        bubbleUpMax(i);
    } else {
      int m = parentMax(i);
      if(m < 0) return;
      if(at(m) < at(i)) {
        swap(i, m);
        bubbleUpMax(m);
      } else
        bubbleUpMin(i);
    }
  }
};

template <class T>
class IntervalPtrHeap {
 private:
  class Item {
   public:
   T *value;
   bool operator<(const Item &i) const { return *value < *i.value; }
   bool operator>(const Item &i) const { return *value > *i.value; }

  };
  IntervalHeap<Item> heap;

 public:
  T *push(T &t) {
    Item i;
    i.value = new T(t);
    heap.push(i);
    return i.value;
  }
  int size() { return heap.size(); }
  T &min() { Item &i = heap.min(); return *i.value; }
  T popMin() { Item i = heap.popMin(); T t = *i.value; delete i.value; return t; }

  T &max() { Item &i = heap.max(); return *i.value; }
  T popMax() { Item i = heap.popMax(); T t = *i.value; delete i.value; return t; }
  //T &popMax() { Item i = heap.popMax(); return *i.value; }
  void rebuild() { heap.rebuild(); }
};

#endif // ITREEHEAP_H
