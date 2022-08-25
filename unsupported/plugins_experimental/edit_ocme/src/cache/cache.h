#ifndef VCACHE_H
#define VCACHE_H

#include <limits.h>

#include <list>
#include <vector>
#include <map>
#include <iostream>

#include <QAtomicInt>
#include <QMutex>
#include <QThread>
#include <QString>
#include <QtDebug>
#include "iheap.h"
#include "door.h"

using namespace std;

/* this cache system enforce the rule that the items in a cache are always in all the cache below */

template <typename Priority>
class Token {
  //do NOT access directly! use set and get
 public:
  Priority priority;         //this value used by various cache threads to sort objects
  Priority new_priority;     //set this value in the main thread
  Priority tmp_priority;     //swap space used in updatePriorities
  QAtomicInt count;          //reference count of locked items

 public:
  Token(): count(-1) {}
  void setPriority(const Priority &p) {
    new_priority = p;
  }
  Priority getPriority() {
    return new_priority;
  }

	bool isLocked() {return count == 1;}
	bool lock() {
    if(count.testAndSetOrdered(0, 1)) return true;
    //count.ref();
    return false;
/*    int v = count.fetchAndAddOrdered(1);
    if(v == 0) { //was -1: not lockable
      count.deref();
      return false;
    }
    return true; */
/*
    count.ref();
    if(count.testAndSetOrdered(0, -1)) return false;  //not in last cache (or being dropped)!
    return true; */
  }

  bool unlock() {  //assumes it was locked first and 1 unlock for each lock.
    return count.deref();
  }

  void pushPriority() {
    tmp_priority = new_priority;
  }
  void pullPriority() {
    priority = tmp_priority;
  }

  bool operator<(const Token &a) const {
    if(count == a.count)
      return priority < a.priority;
    return count < a.count;
  }
  bool operator>(const Token &a) const {
    if(count == a.count)
      return priority > a.priority;
    return count > a.count;
  }
};


template <typename Token>
class Provider: public QThread {
 public:
  IntervalPtrHeap<Token> heap;  //here are the items in this cache but not in the cache above
  bool heap_dirty;             //signals we need to rebuild heap.
  QMutex heap_lock;            //lock this lock before manipulating heap.

  QDoor check_queue; //signals (to next cache!) priorities have changed or something is available

  Provider(): heap_dirty(false) {}
  virtual ~Provider() {}

  void signalDirty() {
    QMutexLocker locker(&heap_lock);
    heap_dirty = true;
    check_queue.open();
  }
};


//warning: upper cache must have more space then lower ones,
//the system automatically enforces this.

template <typename Token>
class Cache: public Provider<Token> {

 public:
  bool final;              //true if this is the last cache (the one we use the data from)
  bool quit;               //graceful exit
  Provider<Token> *input;  //fetch data from here

 protected:
  int s_max;               //max space available
  int s_curr;              //current space used

 public:
  Cache(int _capacity = INT_MAX):
    final(false), quit(false), input(NULL), s_max(_capacity), s_curr(0) {}
  virtual ~Cache() {}

  void setInputCache(Provider<Token> *p) { input = p; }
  int capacity() { return s_max; }
  int size() { return s_curr; }
  void setCapacity(int c) { s_max = c; }

  void flush() {
    std::vector<Token *> tokens;
    {
      QMutexLocker locker(&(this->heap_lock));
      for(int i = 0; i < this->heap.size(); i++) {
        Token *token = &(this->heap[i]);
        tokens.push_back(token);
        s_curr -= drop(token);
      }
      this->heap.clear();
    }

    assert(s_curr == 0);

    {
      QMutexLocker locker(&(input->heap_lock));
			for(unsigned int i = 0; i < tokens.size(); i++) {
        input->heap.push(tokens[i]);
      }
    }
  }

 protected:
  virtual int size(Token *token) = 0;
  virtual int get(Token *token) = 0;   //returns amount of space used in cache
  virtual int drop(Token *token) = 0;  //return amount removed

  void run() {
    assert(input);
    /* basic operation of the cache:
       1) transfer first element of input_cache if
          cache has room OR first element in input as higher priority of last element
       2) make room until eliminating an element would leave space. */

    while(!this->quit) {
      input->check_queue.enter(true); //wait for cache below to load someghing or priorities to change
      if(this->quit) break;

      if(unload() || load())
        input->check_queue.open();//we signal ourselves to check again
    }
    this->quit = false; //in case someone wants to restart;
  }




  bool unload() {
    Token *remove = NULL;
    //make room int the cache checking that:
    //1 we need to make room (capacity < current)
    //qDebug() << "size: " << size() << " cap: " << capacity();
    if(size() > capacity()) {

      QMutexLocker locker(&(this->heap_lock));
/*      if(final)
        for(int i = 0; i < this->heap.size(); i++)
          assert(this->heap[i].count >= 0);
      else
        for(int i = 0; i < this->heap.size(); i++)
          assert(this->heap[i].count == -1); */

      //2 we have some element not in the upper caches (heap.size()  > 0
      if(this->heap.size()) {
        Token &last = this->heap.min();
        int itemsize = size(&last);


        //3 after removing the item, we are still full (avoids bouncing items)
        if(size() - itemsize > capacity()) {

          //4 item to remove is not locked. (only in last cache. you can't lock object otherwise)
          if(!final) { //not final we can drop when we want
            remove = this->heap.popMin();
          } else {
            bool b = last.count.testAndSetOrdered(0, -1);
            if(b) { //was 0 (not locked and now can't be locked, remove it.
              remove = this->heap.popMin();
            } else { //last item is locked need to reorder stack
              //qDebug() << " rebuild" << endl;
              //this->heap.rebuild();
              remove = this->heap.popMin();
              this->heap.push(remove);
              return true;
            }
          }
          //if(!final || last.count.testAndSetOrdered(0, -1)) { //we test and lock at the same time (to avoid other people locking it.

          //}
        }
      }
    }

    if(remove) {
      int size = drop(remove);
      assert(size >= 0);
      s_curr -= size;

      QMutexLocker input_locker(&(input->heap_lock));
      input->heap.push(remove);
      return true;
    }
    return false;
  }

  bool load() {
    if(QThread::currentThread()->objectName() == "mem") { qDebug() << "trying to load"; }
    Token *insert = NULL;
    Token *last = NULL;
    //this mess is to avoid locking more than 1 heap at once. (might deadlock with controller)

    /* check wether we have room (curr < capacity) or heap is empty.
       empty heap is bad: we cannot drop anything to make room, and cache above has nothing to get.
       this should not happen if we set correct cache sizes, but if it happens.... */
    {
      QMutexLocker locker(&(this->heap_lock));
      //lazy rebuild of heaps... if busy transfering no point in updating heaps.
      if(this->heap_dirty) {
        rebuild(this->heap);
        this->heap_dirty = false;
      }
      if(size() > capacity() && this->heap.size() > 0) {
        last = &(this->heap.min()); //no room, set last so we might check for a swap.
      }
    }

    {
      QMutexLocker input_locker(&(input->heap_lock));
      if(input->heap_dirty) {
        rebuild(input->heap);
        input->heap_dirty = false;
      }
      if(QThread::currentThread()->objectName() == "mem") { qDebug() << "input size: " << input->heap.size(); }
      if(input->heap.size()) {    //we need something in input to tranfer.
        Token &first = input->heap.max();
        if(last && QThread::currentThread()->objectName() == "mem") {
          qDebug() << " last: " << last->priority << " lock: " << last->count;
          qDebug() << " first: " << first.priority << " lock: " << first.count;
        }
        if(!last || last->priority < first.priority) { //if !last we already decided we want a transfer., otherwise check for a swap
          insert = input->heap.popMax(); //remove item from heap, while we transfer it.

        }
      }
    }
    if(insert) { //we want to fetch something

      int size = get(insert);

      if(size > 0) { //success
        s_curr += size;
        {
          QMutexLocker locker(&(this->heap_lock));
          if(final)
            insert->count.ref(); //now lock is 0 and can be locked
          this->heap.push(insert);
        }
        input->check_queue.open();//we signal ourselves to check again
        this->check_queue.open(); //we should signal the parent cache that we have a new item
        return true;
      } else {   //failed transfer put it back, but now? we will keep trying to transfer it...
        QMutexLocker input_locker(&(input->heap_lock));
        input->heap.push(insert);
        return false;
      }
    }
    return false;
  }

  static void rebuild(IntervalPtrHeap<Token> &heap) {
    for(int i = 0; i < heap.size(); i++)
      heap[i].pullPriority();
    heap.rebuild();
  }
};

#endif // VCACHE_H
