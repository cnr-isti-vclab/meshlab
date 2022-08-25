#ifndef VCACHE_H
#define VCACHE_H

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

//TO CHECK: use QAtomicInt to avoid priority lock? probably not


//key usually is an int
//priority is a float
//value_t should be reference counted object (QImage for example)
//or at least something that is cheap to copy. (and you have to manage its life using transform() and remove())

template <typename key_t, typename priority_t>
class BaseCache: public QThread {
 public:

  class Item {
   public:
    key_t key;
    priority_t priority;     //managed by controller and caches
    priority_t new_priority; //managed by main thread. (sync is done in Controller::updatePriorities)

    unsigned int level; //level -1 means not in any heap still, cache 0 means the provider has it
    QAtomicInt lock; //0 unlocked, 1 locked -1 being dropped//lock protects access to level:
                     //if lock= 1 level wont be changed.

    Item(): level(0), lock(0) {}
    Item(key_t _key, priority_t _priority, int _level):
      key(_key), priority(_priority), new_priority(_priority), level(_level), lock(0) {}

    bool operator<(const Item &a) const {
      if(lock == a.lock)
        return priority < a.priority;
      return lock < a.lock;
    }
    bool operator>(const Item &a) const {
      if(lock == a.lock)
        return priority > a.priority;
      return lock > a.lock;
    }
  };

 protected:
  int s_max;             //max space available
  int s_curr;            //current space used

 public:
  IntervalPtrHeap<Item> heap;  //here are the items in this cache but not in the cache above
  bool heap_dirty;             //signals we need to rebuild heap.
  QMutex heap_lock;


  QDoor check_queue; //this signals priorities have changed
                     //the thread closes it when it detects it has nothing to do
  bool quit;         //gracefull exit

  BaseCache(): quit(false), heap_dirty(false) {}

  int capacity() { return s_max; }
  int size() { return s_curr; }
  void setCapacity(int c) { s_max = c; }

  virtual void flush() {
    QMutexLocker locker(&(this->heap_lock));
    this->heap.clear();
  }

};

/* this is cache[0], the first to be added to the controller, you only need to
   subclass and implement 'get' */

template <typename key_t, typename priority_t, typename value_t>
class Provider: public BaseCache<key_t, priority_t> {
 public:
  Provider() {}
  virtual ~Provider() {}

  virtual bool get(const key_t & key, value_t & value) = 0; //MUST occour after a lock
};


//warning: upper cache must have more space then lower ones,
//the system automatically enforces this.

template <typename key_t, typename priority_t, typename output_t, typename input_t>
class Cache: public Provider <key_t, priority_t, output_t> {

 public:
  typedef Provider<key_t, priority_t, output_t>   OutputProvider;
  typedef Provider<key_t, priority_t, input_t>   InputProvider;
  typedef BaseCache<key_t, priority_t> Base;
  typedef typename Base::Item Item;

 private:

 protected:
  InputProvider *input;  //fetch data from here

 public:
  //input_cache must have output_t equal to this input_t
  Cache(int _capacity, InputProvider *input_cache): input(input_cache) {
    this->s_max = _capacity;
    this->s_curr = 0;
  }
  virtual ~Cache() {}

  void flush() {
    QMutexLocker locker(&(this->heap_lock));
    this->heap.clear();
    this->clear();
    this->s_curr = 0;
  }

 protected:
  virtual int itemSize(const key_t & /*key*/) = 0;
  virtual int put(const key_t &/*key*/, const input_t &/*value*/)  = 0; //returns amount inserted
  virtual int del(const key_t &/*key*/) = 0; //return amount deleted
  virtual void clear() = 0; //remove all stuff from cache.

//remember to subclass clear()
  //return true if it worked
  bool fetch(key_t key) {
    input_t in;
    if(!input->get(key, in)) return false;
    int size = put(key, in);
    if(size < 0) return false;
    this->s_curr += size;
    return true;
  }

  void run() {
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
    Item *remove = NULL;
    //make room int the cache checking that:
    //1 we need to make room (capacity < current)
    if(this->s_curr > this->capacity()) {

      QMutexLocker locker(&(this->heap_lock));

      //2 we have some element not in the upper caches (heap.size()  > 0
      if(this->heap.size()) {
        int itemsize = itemSize(this->heap.min().key);

        //3 after removing the item, we are still full (avoids bouncing items)
        if(this->s_curr - itemsize > this->capacity()) {

          //4 item to remove is not locked.
          if(this->heap.min().lock.testAndSetOrdered(0, -1)) { //not locked, set as in being dropped: lock will return false.(and fail!)
            remove = this->heap.popMin();
          }
        }
      }
    }

    if(remove) {
      int size = del(remove->key);
      assert(size >= 0);
      this->s_curr -= size;
      remove->level--;

      QMutexLocker input_locker(&(input->heap_lock));
      input->heap.push(remove);

      remove->lock.fetchAndStoreOrdered(0);
      return true;

    }
    return false;
  }

  bool load() {
    Item *item = NULL;
    Item *last = NULL;
    //this mess is to avoid locking more than 1 heap at once. (might deadlock with controller)
    //step 1 check wether we have room (curr < capacity) or heap is empty.
    //empty heap is bad: we cannot drop anything to make room, and cache above has nothing to get.
    //this should not happen if we set correct cache sizes, but if it happens....
    {
      QMutexLocker locker(&(this->heap_lock));

      //lazy rebuild of heaps... if busy transfering no point in updating heaps.
      if(this->heap_dirty) {
        this->heap.rebuild();
        this->heap_dirty = false;
      }
      if(this->s_curr > this->capacity() && this->heap.size() > 0) {
        last = &(this->heap.min()); //no room, set last so we might check for a swap.
      }
    }

    {
      QMutexLocker input_locker(&(input->heap_lock));
      if(input->heap_dirty) {
        input->heap.rebuild();
        input->heap_dirty = false;
      }
      if(input->heap.size()) {    //we need something in input to tranfer.
        Item &first = input->heap.max();
        if(!last || *last < first) { //if !last we already decided we want a transfer., otherwise check for a swap
          item = input->heap.popMax(); //remove item from heap, while we transfer it.
        }
      }
    }
    if(item != NULL) { //we want to fetch something
      if(fetch(item->key)) {

        item->level++; //level should be a qatomicint? maybe, but here is not needed
        {
          QMutexLocker locker(&(this->heap_lock));
          this->heap.push(item);
        }
        input->check_queue.open();//we signal ourselves to check again
        this->check_queue.open(); //we should signal the parent cache that we have a new item
        return true;
      } else { //failed transfer put it back, but now? we will keep trying to transfer it...
        QMutexLocker input_locker(&(input->heap_lock));
        input->heap.push(item);
        return false;
      }
    }
    return false;
  }
};

/*
Simple cache where data is kept using a map key->object to use it you only need to
implement remove() and transform()
*/

template <typename key_t,  typename priority_t, typename output_t, typename input_t>
class MapCache: public Cache<key_t, priority_t, output_t, input_t> {
 public:
  typedef std::map<key_t, output_t>                value_container;
  typedef std::map<key_t, int>                     size_container;
  typedef typename value_container::iterator         value_iterator;
  typedef typename value_container::const_iterator   value_const_iterator;
  typedef typename size_container::iterator         size_iterator;
  typedef typename size_container::const_iterator   size_const_iterator;

  value_container values;
  size_container sizes;

  MapCache(int c, Provider<key_t, priority_t, input_t> *input_cache):
    Cache<key_t, priority_t, output_t, input_t>(c, input_cache) {}

  //tranform returns the size of the item in the cache (the cost if you pregfer)
  //return < 0 and the transfer is considered failed.
 protected:
  virtual int transform(const input_t &in, output_t &out) = 0;
  virtual void remove(output_t &/*out*/) = 0; //in case the object 'out' self destructs there is nothing to do.


  //converts input into output and stores it.
  int put(const key_t & key, const input_t & value)	{
    output_t out;
    int size = transform(value, out);
    if(size < 0) return -1;
    values.insert(std::make_pair(key, out));
    sizes.insert(std::make_pair(key, size));
    return size;
  }

  bool get(const key_t & key, output_t & value) {
    value_iterator it = values.find(key);
    if (it == values.end()) return false;
    value = (*it).second;
    return true;
  }

  int del(const key_t & key) {
    value_iterator it = values.find(key);
    if (it == values.end()) return -1;
    remove((*it).second);
    this->values.erase(it);
    size_iterator kt = sizes.find(key);
    int size = (*kt).second;
    this->sizes.erase(kt);
    return size;
  }
  int itemSize(const key_t &key) {
    size_iterator kt = sizes.find(key);
    return (*kt).second;
  }
  void clear() {
    for(value_iterator i = values.begin(); i != values.end(); i++)
       remove((*i).second);
    values.clear();
    sizes.clear();
  }
};

#endif // VCACHE_H
