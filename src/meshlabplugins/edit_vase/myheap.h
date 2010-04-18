/**
 * @file MyHeaps.h
 * @author Andrea Tagliasacchi
 * @date 26 March 2008
 * @copyright (c) Andrea Tagliasacchi - All rights reserved
 *
 * CHANGE LOG:
 * - 18 April 2010: modified to remove bug in push with back-indexes
 */

#ifndef MYHEAPS_H_
#define MYHEAPS_H_

#include <vector>
#include <exception> // general exception
#include <stdexcept> // out_of_range
#include <iostream>
#include <cassert>
#include <algorithm>
#include "float.h"

using namespace std;

// macros for navigation in the hard coded binary tree
#define PARENT(pos) ((pos-1)>>1) 	// equivalent to floor(pos/2)
#define LEFT(pos)   ((pos<<1)+1) // equivalent to pos*2 + 1
#define RIGHT(pos)  ((pos<<1)+2) // equivalent to pos*2 + 2

/// EXCEPTION
class HeapEmptyException : public out_of_range{
	public:
	HeapEmptyException(const string &message) : out_of_range(message) {;}
};
class InvalidKeyIncreaseException : public out_of_range{
	public:
	InvalidKeyIncreaseException(const string &message) : out_of_range(message) {;}
};
class InvalidIndexException : public out_of_range{
	public:
	InvalidIndexException(const string &message) : out_of_range(message) {;}	
};

/**
 * This class provides a back-inxedex heap (MinHeap) structure where indexes of 
 * elements already in the heap are kept updated to allow for random access
 * update of elements (done automatically in push if element with 
 * "idx" is already contained in the heap )
 * 
 * Refer to the following textbook for details:
 * @book{cormen1990ia,
 *   title={{Introduction to algorithms}},
 *   author={Cormen, T.T. and Leiserson, C.E. and Rivest, R.L.},
 *   year={1990},
 *   publisher={MIT Press Cambridge, MA, USA}
 * }
 */
template <class Tkey>
class MinHeap{

private:
	/// root is assumed to be at end of the vector
	vector< pair<Tkey,int> > heap;
	/**
	 *  maintain a list of back indexes.
	 *  * -1           not in heap
	 *  * other 	   index that point to cell in vector heap
	 */
	vector< int > backIdx;
	/**
	 * If useBackIdx==false it means that the current structure
	 * is not making use of a backindexed heap. Thus, no update 
	 * is available
	 */
	bool useBackIdx;
	
public:
	/// back indexes constructor used for cross updates
	MinHeap( int Nindex ){
		// initialize the back indexes with pseudo-null pointers
		backIdx.resize( Nindex, -1 );
		useBackIdx = true;
	}
	/// Simple constructor with NO cross updates
	MinHeap(){
		useBackIdx = false;
	}
	
	/// pushes a new value in the heap
    bool push( Tkey key, int index ){
		//cout << "pushing " << index << endl;
		if( useBackIdx && index >= (int) backIdx.size() )
			throw InvalidIndexException("the index in the push must be smaller than the maximal allowed index (specified in constructor)");
				
		// If key is not in backindexes or there is no backindexes AT ALL.... complete push (no update)
		if( !useBackIdx ){
			// add to the back of the vector
			heap.push_back( make_pair(key, index) );
			// recursive call to increase key
			heapDecreaseKey( heap.size()-1, key );
            // No back indexes, always push
            return true;
        }
		else{
            if( backIdx[index] == -1 ){
				// add to the back of the vector
				heap.push_back( make_pair(key, index) );
				//initially point to back
				backIdx[ index ] = heap.size()-1;
				// recursive call to increase key
				heapDecreaseKey( heap.size()-1, key );
				// USE STL STUFF
				//push_heap(heap.begin(),heap.end());
                return true;
            }
			// update push (a key exists)
			else {
				heapDecreaseKey( backIdx[index], key );
                return false;
			}
		}
	}
	
	/// return a constant reference to the MINIMAL KEY element stored in the head of the heap
	const pair<Tkey,int>& top() throw(HeapEmptyException){
		if( heap.empty() )
			throw new HeapEmptyException("Impossible to get top element, empty heap");
		else
			return heap[0];
	}
	
	/// removes the top element of the queue (minimal)
	void pop() throw(HeapEmptyException){
		if( heap.size() < 1 ) //a.k.a. heap.empty()
			throw new HeapEmptyException("heap underflow");
		
		// overwrite top with tail element
		heap[0] = heap.back();
				
		// USE STL FUNCTIONALITIES (NOT ALLOW BACKINDEXs)
		//pop_heap(heap.begin(), heap.end());
		
		// shorten the vector
		heap.pop_back();
		
		// start heapify from root
		minHeapify(0);
	}
	
	/// returns the size of the heap
	int size(){ 
		return heap.size();
	}
	
	/// check for emptyness
	bool empty(){
		return heap.empty();
	}
	
	// this does not work, how do you provide a new ordering function to is_heap??
	/// check recursively if the substructures is correct using STL provided algorithm
	//bool verifyHeap( ){
	//	return std::__is_heap(heap.begin(), heap.end() );
	//}
	
	/// computes full heap sort and returns the corresponding indexing structure
	/// Requires the indexes to be allocated already.
	void heapsort(vector<int>& indexes){
		// until empty... keep popping
		int i = 0;
		while( empty() == false ){
			pair<Tkey,int> t = top();
			pop();
			indexes[i++] = t.second;
		}
	}
	
private:
	/// check and applies MaxHeap Correctness down the subtree with index "currIdx"
	void minHeapify(int currIdx){
		unsigned int leftIdx = LEFT( currIdx );
		unsigned int rightIdx = RIGHT( currIdx );
		
		// decide if and where ta swap, left or right, then swap
		// current is the best choice (defalut)
		int smallerIdx;
		
		// is left a better choice? (exists an invalid placed smaller value on the left side)
		if( leftIdx < heap.size() && heap[leftIdx].first < heap[currIdx].first )
			smallerIdx = leftIdx;
		else
			smallerIdx = currIdx;
		
		// is right a better choice? (exists an invalid placed smaller value on the right side)
		if( rightIdx < heap.size() && heap[rightIdx].first < heap[smallerIdx].first )
			smallerIdx = rightIdx;
	
		// a better choice exists?
		if( smallerIdx != currIdx ){
			// swap elements
			swap( currIdx, smallerIdx );
			
			// recursively call this function on alterated subtree
			minHeapify( smallerIdx );
		}
	}
	
	/// swap the content of two elements in position pos1 and pos2
	void swap(int pos1, int pos2){
		assert( !heap.empty() );
		assert( pos1>=0 && pos1<(int)heap.size() );
		assert( pos2>=0 && pos2<(int)heap.size() );
			
		// update backindexes
		if( useBackIdx ){
			backIdx[ heap[pos1].second ] = pos2;
			backIdx[ heap[pos2].second ] = pos1;
		}
		
		// update heap
		pair<Tkey,int> temp = heap[pos1];
		heap[pos1] = heap[pos2];
		heap[pos2] = temp;
	}
	
	/// propagates the correctness (in heap sense) down from a vertex currIdx
	void heapDecreaseKey( int currIdx, Tkey key ){
		// check if given key update is actually an increase
		if( key > heap[currIdx].first )
			throw InvalidKeyIncreaseException("In MinHeaps only decrease in key updates are legal");

		// update value with current key
		heap[currIdx].first = key;
				
		// traverse the tree up making necessary swaps
		int parentIdx = PARENT(currIdx);
		while( currIdx > 0 ){
			if( heap[ parentIdx ].first > heap[ currIdx ].first ){
				// make swap
				swap( currIdx, parentIdx );
				// move up
				currIdx = parentIdx;
				parentIdx = PARENT(currIdx);
			} else {
				break;
			}					
		}
	}

	/// print an internal representation of the heap (debug purposes)
	public: void print() {
		cout << "idxs";
		for ( int i=0; i < size(); i++)
			cout << " " << heap[i].second << " ";
		cout << endl;
		
		cout << "csts";
		for ( int i=0; i < size(); i++)
			cout << " " << heap[i].first << " ";
		cout << endl;
		
//		cout << "";
//		for ( int i=0; i < size(); i++)
//			cout << heap[i].first << " in off: " << backIdx[heap[i].first] << ", ";
//		cout << endl;
	
		cout << endl;
	}
};

#endif /*MYHEAPS_H_*/
