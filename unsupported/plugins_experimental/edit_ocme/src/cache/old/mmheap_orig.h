#ifndef MMHEAP_H
#define MMHEAP_H

/*C
 * Original project: Lars Arge, Jeff Chase, Pat Halpin, Laura Toma, Dean
 *                   Urban, Jeff Vitter, Rajiv Wickremesinghe 1999
 *
 * GRASS Implementation: Lars Arge, Helena Mitasova, Laura Toma 2002
 *
 * Copyright (c) 2002 Duke University -- Laura Toma
*/

#include <vector>

/*
Supported operations: min, extract_min, insert, max, extract_max in
O(lg n)
*/



template <class T>
class MinMaxHeap {

protected:
  int maxsize;
  int lastindex; // last used position (0 unused) (?)
  std::vector<T> A;
  /* couple of memory mgt functions to keep things consistent */

public:
  MinMaxHeap(int size) : maxsize(size) {
    lastindex = 0;
  }

  virtual ~MinMaxHeap(void) { };

  bool empty(void) const { return size() == 0; };
  int size() const;

  T get(int i) const { assert(i <= size()); return A[i]; }

  void insert(const T& elt);

  bool min(T& elt) const ;
  bool extract_min(T& elt);
  bool max(T& elt) const;
  bool extract_max(T& elt);

  void reset();

private:
  // Changed log2() to log2_() just in case log2() macro was already
  // defined in math.h: e.g., log2() is defined in Cygwin gcc by default.
  long log2_(long n) const;
  int isOnMaxLevel(int i) const { return (log2_(i+1) % 2); };
  int isOnMinLevel(int i) const { return !isOnMaxLevel(i); };

  int leftChild(int i) const { return 2*i + 1; } //return 2*(i+1) -1; };
  int rightChild(int i) const { return 2*(i+1); }//{ return 2*(i+1) -1 + 1; };
  int hasRightChild(int i) const { return (rightChild(i) < size()); };
  int parent(int i) const { return ((i+1)/2)-1; };
  int grandparent(int i) const { return ((i+1)/4)-1; };
  int hasChildren(int i) const { return (2*(i+1)) < size(); }; // 1 or more
  void swap(int a, int b);

  T leftChildValue(int i) const;
  T rightChildValue(int i) const;
  int smallestChild(int i) const;
  int smallestChildGrandchild(int i) const;
  int largestChild(int i) const;
  int largestChildGrandchild(int i) const;
  int isGrandchildOf(int i, int m) const;

  void trickleDownMin(int i);
  void trickleDownMax(int i);
  void trickleDown(int i);

  void bubbleUp(int i);
  void bubbleUpMin(int i);
  void bubbleUpMax(int i);
};


// index 0 is invalid I hate you can't you just follow C conventions??!
// index <= size

template <class T>
int MinMaxHeap<T>::size() const {
  return a.size();
}

template <class T>
long MinMaxHeap<T>::log2_(long n) const {
  long i=-1;
  // let log2_(0)==-1
  while(n) {
  n = n >> 1;
  i++;
  }
  return i;
}

template <class T>
void MinMaxHeap<T>::swap(int a, int b) {
  T tmp;
  tmp = A[a];
  A[a] = A[b];
  A[b] = tmp;
}

// child must exist
template <class T>
T MinMaxHeap<T>::leftChildValue(int i) const {
  int p = leftChild(i);
  assert(p < size());
  return A[p];
}

// child must exist
template <class T>
T MinMaxHeap<T>::rightChildValue(int i) const {
  int p = rightChild(i);
  assert(p < size());
  return A[p];
}

// returns index of the smallest of children of node
// it is an error to call this function if node has no children
template <class T>
int MinMaxHeap<T>::smallestChild(int i) const {
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
int MinMaxHeap<T>::smallestChildGrandchild(int i) const {
  int p,q;
  int minpos = 0;

  assert(hasChildren(i));

  p = leftChild(i);
  if(hasChildren(p)) {
  q = smallestChild(p);
  if(A[p] > A[q]) p = q;
  }
  // p is smallest of left child, its grandchildren
  minpos = p;

  if(hasRightChild(i)) {
  p = rightChild(i);
  if(hasChildren(p)) {
    q = smallestChild(p);
    if(A[p] > A[q]) p = q;
  }
  // p is smallest of right child, its grandchildren
  if(A[p] < A[minpos]) minpos = p;
  }
  return minpos;
}

// ----------------------------------------------------------------------

template <class T>
int MinMaxHeap<T>::largestChildGrandchild(int i) const {
  int p,q;
  int maxpos = 0;

  assert(hasChildren(i));

  p = leftChild(i);
  if(hasChildren(p)) {
  q = largestChild(p);
  if(A[p] < A[q]) p = q;
  }
  // p is smallest of left child, its grandchildren
  maxpos = p;

  if(hasRightChild(i)) {
  p = rightChild(i);
  if(hasChildren(p)) {
    q = largestChild(p);
    if(A[p] < A[q]) p = q;
  }
  // p is smallest of right child, its grandchildren
  if(A[p] > A[maxpos]) maxpos = p;
  }
  return maxpos;
}

// ----------------------------------------------------------------------

// this is pretty loose - only to differentiate between child and grandchild
template <class T>
int MinMaxHeap<T>::isGrandchildOf(int i, int m) const {
  return ((m+1) >= (i+1)*4);
}

// ----------------------------------------------------------------------

template <class T>
void MinMaxHeap<T>::trickleDownMin(int i) {
  int m;
  bool done = false;

  while (!done) {

    if (!hasChildren(i)) {
      done = true;
      return;
    }
    m = smallestChildGrandchild(i);
    if(isGrandchildOf(i, m)) {
      if(A[m] < A[i]) {
  swap(i, m);
  if(A[m] > A[parent(m)]) {
    swap(m, parent(m));
  }
  //trickleDownMin(m);
  i = m;
      } else {
  done = true;
      }
    } else {
      if(A[m] < A[i]) {
  swap(i, m);
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
