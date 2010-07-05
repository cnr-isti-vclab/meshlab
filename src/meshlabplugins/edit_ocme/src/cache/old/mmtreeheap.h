#ifndef MMTREEHEAP_H
#define MMTREEHEAP_H

/*C
 * Original project: Lars Arge, Jeff Chase, Pat Halpin, Laura Toma, Dean
 *                   Urban, Jeff Vitter, Rajiv Wickremesinghe 1999
 *
 * GRASS Implementation: Lars Arge, Helena Mitasova, Laura Toma 2002
 *
 * Copyright (c) 2002 Duke University -- Laura Toma
*/

#include <list>

/*
Supported operations: min, extract_min, insert, max, extract_max in
O(lg n)
*/



template <class T>
class MinMaxHeap {

  class item {
   std::list<item>::iterator iterator;
   public:
    int level; //0 or 1, min or max level
    T value;
    iterator parent;
    iterator leftChild;
    iterator rightChild;
    item(T v): value(v) {}
  };
  std::list<item>::iterator iterator;
protected:
  std::list<item> A;
  /* couple of memory mgt functions to keep things consistent */

public:
  MinMaxHeap() {}

  virtual ~MinMaxHeap(void) { };

  bool empty(void) const { return size() == 0; };
  int size() const;

  //T get(int i) const { assert(i <= size()); return A[i]; }

  void insert(const T& elt);

  bool min(T& elt) const ;
  bool extract_min(T& elt);
  bool max(T& elt) const;
  bool extract_max(T& elt);

private:
  // Changed log2() to log2_() just in case log2() macro was already
  // defined in math.h: e.g., log2() is defined in Cygwin gcc by default.
  //long log2_(long n) const;
  //int isOnMaxLevel(int i) const { return (log2_(i+1) % 2); };
  //int isOnMinLevel(int i) const { return !isOnMaxLevel(i); };
  int isOnMaxLevel(iterator i) const { return (*i).level; };
  int isOnMinLevel(iterator i) const { return !isOnMaxLevel(i); };

  iterator leftChild(iterator i) const { return (*i).leftChild; }
  iterator rightChild(iterator i) const { return (*i).rightChild; }
  int hasRightChild(iterator i) const { return (*i).rightChild != A.end(): }
  iterator parent(iterator i) const { return (*i).parent; }
  iterator grandparent(iterator i) const { *((*i).parent).parent; };
  int hasChildren(iterator i) const { return (*i).leftChild != A.end() ||(*i).rightChild != A.end(); }
  void swap(iterator a, iterator b);



  T leftChildValue(iterator i) const;
  T rightChildValue(iterator i) const;
  iterator smallestChild(iterator i) const;
  iterator smallestChildGrandchild(iterator i) const;
  iterator largestChild(iterator i) const;
  iterator largestChildGrandchild(iterator i) const;
  int isGrandchildOf(iterator i, iterator m) const;

  void trickleDownMin(iterator i);
  void trickleDownMax(iterator i);
  void trickleDown(iterator i);

  void bubbleUp(iterator i);
  void bubbleUpMin(iterator i);
  void bubbleUpMax(iterator i);
};


// index 0 is invalid I hate you can't you just follow C conventions??!
// index <= size

template <class T>
int MinMaxHeap<T>::size() const {
  return A.size();
}

/*template <class T>
long MinMaxHeap<T>::log2_(long n) const {
  long i=-1;
  // let log2_(0)==-1
  while(n) {
  n = n >> 1;
  i++;
  }
  return i;
}*/

template <class T>
void MinMaxHeap<T>::swap(iterator a, iterator b) { //a and b not parent of each other
  item tmp;
  tmp.parent = *a.parent;
  tmp.rightChild = *a.rightChild;
  tmp.leftChild = *a.leftChild;
  *a.parent = *b.parent;
  *a.rightChild = *b.rightChild;
  *a.leftChild = *b.leftChild;
  *b.parent = tmp.parent;
  *b.rightChild = tmp.rightChild;
  *b.leftChild = tmp.leftChild;
}
template <class T>
void MinMaxHeap<T>::swapParent(iterator a, iterator b) { //b parent of a
  item tmp;
  tmp.rightChild = *a.rightChild;
  tmp.leftChild = *a.leftChild;
  if(b.leftChild == a) {
    *a.leftChild = b;
    *a.rightChild = *b.rightChild;
  } else if(b.rightChild == a)
    *a.rightChild == b;
    *a.leftChild = *b.leftChild;
  }
  *a.parent = *b.parent;
  *b.parent = a;
  *b.rightChild = tmp.rightChild;
  *b.leftChild = tmp.leftChild;
}

// child must exist
template <class T>
T MinMaxHeap<T>::leftChildValue(iterator i) const {
  return *(*i.leftChild).value;
}

// child must exist
template <class T>
T MinMaxHeap<T>::rightChildValue(iterator i) const {
  return *(*i.leftChild).value;
}

// returns index of the smallest of children of node
// it is an error to call this function if node has no children
template <class T>
int MinMaxHeap<T>::smallestChild(iterator i) const {
  assert(hasChildren(i));
  if(hasRightChild(i) && (leftChildValue(i) > rightChildValue(i))) {
  return rightChild(i);
  } else {
  return leftChild(i);
  }
}

// ----------------------------------------------------------------------

template <class T>
int MinMaxHeap<T>::largestChild(int i) const {
  assert(hasChildren(i));
  if(hasRightChild(i) && (leftChildValue(i) < rightChildValue(i))) {
  return rightChild(i);
  } else {
  return leftChild(i);
  }
}

// ----------------------------------------------------------------------

// error to call on node without children
template <class T>
int MinMaxHeap<T>::smallestChildGrandchild(iterator i) const {
  iterator p,q;
  iterator minpos;

  assert(hasChildren(i));

  p = leftChild(i);
  if(hasChildren(p)) {
    q = smallestChild(p);
    if((*p).value > (*q).value) p = q;
  }
  // p is smallest of left child, its grandchildren
  minpos = p;

  if(hasRightChild(i)) {
    p = rightChild(i);
    if(hasChildren(p)) {
      q = smallestChild(p);
    if((*p).value > (*q).value) p = q;
  }
  // p is smallest of right child, its grandchildren
  if((*p).value < (*minpos).value) minpos = p;
  }
  return minpos;
}

// error to call on node without children
template <class T>
int MinMaxHeap<T>::largestChildGrandchild(iterator i) const {
  iterator p,q;
  iterator maxpos;

  assert(hasChildren(i));

  p = leftChild(i);
  if(hasChildren(p)) {
    q = latgestChild(p);
    if((*p).value < (*q).value) p = q;
  }
  // p is largest of left child, its grandchildren
  maxpos = p;

  if(hasRightChild(i)) {
    p = rightChild(i);
    if(hasChildren(p)) {
      q = largestChild(p);
    if((*p).value < (*q).value) p = q;
  }
  // p is smallest of right child, its grandchildren
  if((*p).value > (*maxpos).value) maxpos = p;
  }
  return maxpos;
}

// ----------------------------------------------------------------------

// this is pretty loose - only to differentiate between child and grandchild
template <class T>
int MinMaxHeap<T>::isGrandchildOf(iterator i, iterator m) const { //assumes m is child or grandchild of i
  return *m.parent != i;
}

// ----------------------------------------------------------------------

template <class T>
void MinMaxHeap<T>::trickleDownMin(iterator i) {
  iterator m;
  bool done = false;

  while (!done) {

    if (!hasChildren(i)) {
      done = true;
      return;
    }
    m = smallestChildGrandchild(i);
    if(isGrandchildOf(i, m)) {
      if((*m).value < (*i).value) {
         swap(i, m);
         if((*m).value > *(parent(m)).value) {
           swapParent(m, parent(m));
         }
         //trickleDownMin(m);
         i = m;
       } else {
         done = true;
       }
     } else { //child
       if((*m).value < (*i).value) {
         swapParent(i, m);
      }
      done = true;
    }
  }//while
}

// ----------------------------------------------------------------------

// unverified
template <class T>
void MinMaxHeap<T>::trickleDownMax(int i) {
  int m;
  bool done = false;

  while (!done) {
    if(!hasChildren(i)) {
     done = true;
     return;
    }

    m = largestChildGrandchild(i);
    if(isGrandchildOf(i, m)) {
      if(A[m] > A[i]) {
  swap(i, m);
  if(A[m] < A[parent(m)]) {
    swap(m, parent(m));
  }
  //trickleDownMax(m);
  i = m;
      } else {
  done = true;
      }
    } else {
      if(A[m] > A[i]) {
  swap(i, m);
      }
      done = true;
    }
  } //while
}


// ----------------------------------------------------------------------


template <class T>
void MinMaxHeap<T>::trickleDown(int i) {
  if(isOnMinLevel(i)) {
  trickleDownMin(i);
  } else {
  trickleDownMax(i);
  }
}

// ----------------------------------------------------------------------
template <class T>
void MinMaxHeap<T>::bubbleUp(int i) {
  int m;
  m = parent(i);

  if(isOnMinLevel(i)) {
  if (m && (A[i] > A[m])) {
    swap(i, m);
    bubbleUpMax(m);
  } else {
    bubbleUpMin(i);
  }
  } else {
  if (m && (A[i] < A[m])) {
    swap(i, m);
    bubbleUpMin(m);
  } else {
    bubbleUpMax(i);
  }
  }
}


// ----------------------------------------------------------------------
template <class T>
void MinMaxHeap<T>::bubbleUpMin(int i) {
  int m;
  m = grandparent(i);

  while (m && (A[i] < A[m])) {
   swap(i,m);
   //bubbleUpMin(m);
   i = m;
   m = grandparent(i);

  }
}



// ----------------------------------------------------------------------
template <class T>
void MinMaxHeap<T>::bubbleUpMax(int i) {
  int m;
  m = grandparent(i);

  while(m && (A[i] > A[m])) {
  swap(i,m);
  //bubbleUpMax(m);
  i=m;
  m = grandparent(i);
  }
}

template <class T>
void MinMaxHeap<T>::insert(const T& elt) {
  lastindex++;
  A.push_back(elt);
  bubbleUp(lastindex);
}

// ----------------------------------------------------------------------
template <class T>
bool MinMaxHeap<T>::extract_min(T& elt) {
  if(lastindex == 0) return false;

  elt = A[0];
  A[0] = A[lastindex];
  lastindex--;
  trickleDown(1);

  return true;
}

// ----------------------------------------------------------------------
template <class T>
bool MinMaxHeap<T>::extract_max(T& elt) {

  assert(A);

  int p;					// max
  if(lastindex == -1) return false;

  if(hasChildren(1)) {
    p = largestChild(1);
  } else {
    p = 0;
  }
  elt = A[p];
  A[p] = A[lastindex];
  lastindex--;
  trickleDown(p);

  return true;
}

template <class T>
bool MinMaxHeap<T>::min(T& elt) const {

  assert(A);

  if(lastindex == -1) return false;

  elt = A[0];
  return true;
}

// ----------------------------------------------------------------------
template <class T>
bool MinMaxHeap<T>::max(T& elt) const {

  assert(A);

  int p;					// max
  if(lastindex == -1) return false;

  if(hasChildren(1)) {
    p = largestChild(1);
  } else {
    p = 1;
  }
  elt = A[p];
  return true;
}




template <class T>
class MinMaxHeap : public MinMaxHeap<T> {

//using MinMaxHeap<T>::maxsize;
//using MinMaxHeap<T>::lastindex;
//using MinMaxHeap<T>::size;

public:
  MinMaxHeap(int size) : MinMaxHeap<T>(size) {};
  virtual ~MinMaxHeap() {};
  bool full(void) const { return this->size() >= this->maxsize; };
  int get_maxsize() const { return this->maxsize; };
  int fill(T* arr, int n);

protected:
  virtual void grow() { assert(0); exit(1); };
};

#endif // MMHEAP_H

#endif // MMTREEHEAP_H
