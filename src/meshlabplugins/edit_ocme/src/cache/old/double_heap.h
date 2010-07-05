#ifndef DD_HEAP_H
#define DD_HEAP_H

/* double heap (min and max) base on
  Min-Max Heaps and Generalized Priority Queues
  M. D. ATKINSON,J.-R. SACK, N. SANTORO,and T. STROTHOTTE

 root is level 0
 even level are min levels, odd are max level;
 all descendent of a min level element a are bigger than a
 all descendent of a max level element a are smaller than a

 invert means check > instead of < allows to use same function for min and max levels.

 implemented on an array as usual

 compared to a stl heap, rebuild is 30% longer, extraction is 2.5x longer (we need to check grandchildren)
 written by Federico Ponchio
*/
#include <assert.h>
#include <vector>

using namespace std;

template <class T>
class DoubleHeap: public std::vector<T> {
public:

  void push(const T& elt) {
    push_back(elt);
    bubbleUp(this->size()-1);
  }

  T &min() { return this->front(); } //root is smaller element
  T popMin() {
    T elt = this->front();
    this->front() = this->back();
    this->pop_back();
    trickleDown(0, false); //enforce minmax heap property
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
    trickleDown(p, true); //enforce minmax heap property
    return elt;
  }

  void rebuild() { //just reinsert all elements, (no push back necessary, of course
    for(int i = 0; i < this->size(); i++)
      bubbleUp(i);
  }

private:
  T &at(int n) { return std::vector<T>::at(n); }

  int isMax(int e) const { return e & 1; }
  int parent(int i) const { return ((i+1)<<1) -1; } //parent interval
  int leftChild(int i) const { return ((e+1)>>1) -1; } //left child interval

//  int parent(int i) const { return (((i+2)>>2)<<1) - 2; }
//  int leftChild(int i) const { return (((i+2)>>1)<<2) -2; }

  void swap(int a, int b) { T tmp = at(a); at(a) = at(b); at(b) = tmp; }

  int smallestChild(int i, bool invert) { //return smallest of children or self if no children
    int l = leftChild(i) + invert;
    if(l >= this->size()) return i; //no children, return self

    int r = l+2; //right child
    if(r < this->size()) {
      T &lv = at(l);
      T &rv = at(r);
      if((rv < lv)^invert) return r;
    }
    return l;
  }

  void trickleDownMin(int i) {   //assert(invert == isOnMaxLevel(i));
    int j;
    while(1) {
      int m = smallestChild(i); //check children intervals
      if(m == i) break; //no children

      if(at(m) < at(i)) {
        swap(i, m);
        i = m;
      } else
        break;

      m = i+1;       //enforce order in interval
      if(m >= this->size()) break;
      if(at(m) < at(i))
        swap(i, m);
    }
  }

  void trickleDownMax(int i) {   //assert(invert == isOnMaxLevel(i));
    int j;
    while(1) {
      int m = greatestChild(i); //check children intervals
      if(m == i) break; //no children

      if(at(m) > at(i)) {
        swap(i, m);
        i = m;
      } else
        break;

      m = i-1;       //enforce order in interval
      if(m >= this->size()) break;
      if(at(m) > at(i))
        swap(i, m);
    }
  }




  void trickleDown(int i, int invert) {   //assert(invert == isOnMaxLevel(i));
    int j;
    while(1) {
      if(invert) j = i-1;
      else j = i+1;
      if(j >= this->size()) break;
      if((at(i) > at(j))^invert)
        swap(i, j);

      int m = smallestChild(i, invert);
      if(m == i) break; //no children

      if((at(m) < at(i))^invert) {
        swap(i, m);
        i = m;
      } else
        break;
    }
  }

  void bubbleUp(int i) {
    if(i == 0) return;
    int invert = isOnMaxLevel(i); //i -> 0 means invert = false;

    int j;
    if(invert)
      j = i -1; //check min just above
    else
      j = parent(i) + 1; //check max of parent
    if((at(i) > at(j))^invert) {
      swap(i, j);
      i = j;
      invert = !invert;
    }

    int m = parent(i) + invert;
    assert(i > m);
    while (m>=0 && ((at(i) < at(m))^invert)) {
      swap(i, m);
      i = m;
      m = parent(i) + invert;
    }
  }
};

#endif
