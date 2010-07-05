#ifndef MMHEAP_H
#define MMHEAP_H

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

#include <vector>

template <class T>
class MinMaxHeap: public std::vector<T> {
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

  T &max() {   //max is biggest of the two children of root (or root if no children
    return at(smallestChild(0, true));
  }

  T popMax() {
    int p = smallestChild(0, true);
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

  int level(int n) const {
    int i=-1;
    while(n) {
      n = n >> 1;
      i++;
    }
    return i;
  }

  int isOnMaxLevel(int i) const { return level(i+1) & 1; /*% 2*/; }

  int parent(int i) const { return ((i+1)/2)-1; }
  int grandparent(int i) const { return ((i+1)/4)-1; }
  int leftChild(int i) const { return 2*i + 1; }
  int leftGranChild(int i) const { return 4*i + 3; }

  void swap(int a, int b) { T tmp = at(a); at(a) = at(b); at(b) = tmp; }

  int smallestChild(int i, bool invert) { //return smallest of children or self if no children
    int l = leftChild(i);
    if(l >= this->size()) return i; //no children, return self

    int r = l+1; //right child
    if(r < this->size()) {
      T &lv = at(l);
      T &rv = at(r);
      if((rv < lv)^invert) return r;
    }
    return l;
  }

  int smallestGrandChild(int i, bool invert) {//return smallest of grandchildren or self if no children
    int l = leftGranChild(i);
    if(l >= this->size()) return i;
    T lv = at(l);
    int min = l;
    for(int r = l+1; r < this->size() & r < l+4; r++) { //iterate on three grandsiblings (they are consecutive)
      T &rv = at(r);
      if((rv < lv)^invert) {
        lv = rv;
        min = r;
      }
    }
    return min;
  }

  void trickleDown(int i, bool invert) {   //assert(invert == isOnMaxLevel(i));
    while(1) {
      //enforce min-max property on level(i), we need to check children and grandchildren
      int m = smallestChild(i, invert);
      if(m == i) break; //no children
      if((at(m) < at(i))^invert) //swap children, max property on level(i)+1 automatically enforced
        swap(i, m);

      int j = smallestGrandChild(i, invert);
      if(j == i) break; //no grandchildren
      if((at(j) < at(i))^invert) {
        swap(i, j);
        i = j; //we need to enforce min-max property on level(j) now.
      } else
        break; //no swap, finish
    }
  }

  void bubbleUp(int i) {
    int m;
    m = parent(i);
    bool invert = isOnMaxLevel(i);
    if (m>=0 && ((at(i) > at(m))^invert)) {
      swap(i, m);
      i = m;
      invert = !invert;
    }

    m = grandparent(i);
    while (m>=0 && ((at(i) < at(m))^invert)) {
      swap(i,m);
      i = m;
      m = grandparent(i);
    }
  }

};

#endif
