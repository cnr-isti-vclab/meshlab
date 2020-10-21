#include <QtCore/QCoreApplication>

#include <time.h>
#include <iostream>
#include <algorithm>
//#include "mmheap.h"
#include "iheap.h"

#define MINMAX
#define SIZE 30
using namespace std;

void fillHeap(std::vector<int> *heap) {
/*  int seed = 0;
  srand(0);
  for(int i  = 0; i < 10; i++)
    heap->push_back(rand()%10);
return; */
  srand(2);



  for(int i = 0; i < SIZE; i++) {
    int r = 100*(rand()/(double)RAND_MAX);
    cout << "R: " << r << endl;
    heap->push_back(r);
  }
}


int emptyDHeapMax(IHeap<int> &heap) {
  int max, oldmax = 1000000000;
 while(heap.size() > 0) {
     max = heap.popMax();
     cerr << "max: " << max << endl;
     assert(heap.isHeap());
     if(oldmax < max) {
       cerr << "AAAARRRGGHHH: " << oldmax << " max: " << max << endl;
       getchar();
     }
     oldmax = max;
  }
  return max;
}



int main(int argc, char *argv[]) {
  IHeap<float> h;
  h.push(0.163359);
  h.push(0.811858);
  h.push(0.150119);
  h.push(0.630451);
  assert(h.isHeap());
  h.push(0.258985);
  assert(h.isHeap());
  return -1;

  IHeap<int> heap;

  fillHeap(&heap);
/*  int size = 10;
  for(int i = 0; i < size; i++)
    heap.push_back(i);
  */
  heap.rebuild();
  assert(heap.isHeap());
  //return 0;
  emptyDHeapMax(heap);

  for(int i = 0; i < 10000; i++) {
    //int r = 100*(rand()/(double)RAND_MAX);
    int r = 5*(rand()/(double)RAND_MAX);
    switch(r) {
      case 0: if(heap.size()) heap.popMin(); break;
      case 1: if(heap.size()) heap.popMax(); break;
      default:

        r = 100*(rand()/(double)RAND_MAX);
        cout << "r: " << r << endl;
        heap.push(r);
    }
    assert(heap.isHeap());
  }
  emptyDHeapMax(heap);

  return 0;
}
