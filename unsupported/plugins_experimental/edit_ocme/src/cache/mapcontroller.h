#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "cache.h"

/* controller is an index to update priorities and
   and interface to request objects.

   see cache_gl.cpp for an example of usage

   TODO: flushing... add caches on the fly?
*/

/* templatated on the value of the last cache for easier interface */
template <typename key_t, typename priority_t, typename value_t>
class Controller: public QThread {
 public:

  typedef Provider<key_t, priority_t, value_t>           LastCache;
  typedef BaseCache<key_t, priority_t>                   GenericCache;

  typedef typename BaseCache<key_t, priority_t>::Item    Item;
  typedef typename std::map<key_t, Item *>               Index;
  typedef typename Index::iterator                        IndexIterator;

  //buffering request between main thread and Controller
  class Request {
   public:
    Item *item;
    priority_t priority;
    Request() {}
    Request(Item *_item, priority_t p): item(_item), priority(p) {}
  };


  std::vector<Request> requests; //new priorities are stored in new_priority fields in Item,
                                 //this vector is used to comunicate them to Controller.
  QMutex requests_lock; //guards requests

  QDoor wakeup;    //used to signal controller to update priorities.
  bool quit;       //gracefully terminate.

  int max_items; //max number of items in cache 0. popMin() the overflow.unused at the moment

 public:
  std::vector<GenericCache *> caches;
  Index index;     //this should really be a hash table maps keys-> items (see cache)
                   //you can access index, just remember level and priority are

  Controller(): max_items(-1), quit(false) {}
  ~Controller() { finish(); }

  void addCache(GenericCache *cache) { //last cache added must be a provider and  the top cache and output_t must coincide
    assert(!isRunning());
    caches.push_back(cache);
  }

  //call updatePriorities after you have done with your set of objects
  bool getPriority(key_t key, priority_t &priority) {
    IndexIterator it = this->index.find(key);
    if(it ==  this->index.end()) return false;
    priority = (*it).second->new_priority; //don't care about actual priority used, i want just last setted one.
    return true;
  }
  void setPriority(key_t key, priority_t priority) {
    IndexIterator it = this->index.find(key);
    Item *item = NULL;
    if(it ==  this->index.end()) {     //new item... add to requests
      item = new Item(key, priority, -1);
      index[key] = item;
    } else
      item = (*it).second;
    item->new_priority = priority;
  }

  void updatePriorities() {
    QMutexLocker locker(&requests_lock);

    requests.clear();
    for(IndexIterator i = index.begin(); i != index.end(); i++) {
      Item *item = (*i).second;
      requests.push_back(Request(item, item->new_priority));
    }
    wakeup.open();
  }

  //update priority NOW forces rebuild the heap for the cache the key is in.
  //do NOT use for mass priority changes!
  void setPriorityNow(key_t key, priority_t priority) {
    IndexIterator it = index.find(key);
    Item *item = NULL;
    if(it ==  this->index.end()) {//new item...
      item = new Item(key, priority, -1);
      index[key] = item;
    } else {
      item = (*it).second;
    }

    int level = item->level;
    if(level == -1) { //add item to cache 0
      QMutexLocker locker(caches[0]->heap_lock);
      item->priority = priority;
      caches[0]->heap.push(item);
      level = item->level = 0;
    } else {  //this is risky: what if level changes int the meanwhile? on the other hand
              //heap is robust to priority changes on the fly... (wont be aheap anymore, but it wont crash)
      QMutexLocker locker(caches[level]->heap_lock);
      item->priority = priority;
      caches[level]->heap.rebuild();
      caches[level]->check_queue.open();
    }
  }

  //return true if it is available for get
  bool lock(key_t key) {
    IndexIterator it = this->index.find(key);
    if (it == this->index.end()) return false;
    if(!(*it).second->lock.testAndSetOrdered(0, 1)) {
      //was already locked or is being dropped (1 o -1)
      if((*it).second->lock.testAndSetOrdered(-1, 0)) {
        return false; //was -1: being dropped: bad luck
      }
    }
    //from now on nobody can decrease its level (so we can safely access it.
    if((*it).second->level == this->caches.size()-1)
      return true;//accessto level protected by lock.
    (*it).second->lock.testAndSetOrdered(1, 0);
    return false;
  }

  //only locking you can be sure the data will be there.(there is no sync!)
  bool get(const key_t & key, value_t & value) {
    LastCache *p = reinterpret_cast<LastCache *>(this->caches.back());
    return p->get(key, value);
  }

  void unlock(key_t key) {
    IndexIterator it = this->index.find(key);
    if(it != this->index.end())
      (*it).second->lock.testAndSetOrdered(1, 0);
  }

    void finish() {
    if(!isRunning()) return;
    quit = true;
    wakeup.open();
    wait(); //wait for thread to finish
  }

  void run() {
    //qDebug() << "controller running!";
    //start all caches
    for(unsigned int i = 1; i < caches.size(); i++) //cache 0 is a provider, and his thread is not running.
      caches[i]->start();

    while(1) {
      //waked up everytime someone calls updatePriorities
      wakeup.enter(true);
      if(quit) break;
      _updatePriorities();
    }

    for(unsigned int i = caches.size()-1; i >= 1; i--) {
      caches[i]->quit = true;
      caches[i-1]->check_queue.open(); //cache i listens on queue i-1
      caches[i]->wait();
      caches[i]->flush();
    }
    quit = false; //in case someone want to restart the controller
  }



//TODO how to deal with long transfers? we can't delete items. until transfers are either finished or interrupted.
//we can't kill cache threads: might leave memory leaks around.

  void flush() {
    for(unsigned int i = 0; i < caches.size(); i++)
      caches[i]->flush();

    IndexIterator it = index.begin();
    while(it != index.end()) {
      delete (*it).second;
      ++it;
    }
    index.clear();
    requests.clear();
  }

  void _updatePriorities() {

    std::vector<Request> req; //new keys to be inserted.
    {
      QMutexLocker locker(&requests_lock);
      //quick swap to avoid locking main thread.
      swap(requests, req);
    }

    //lock all caches.
    for(int i = caches.size()-1; i >= 0; i--) {
      caches[i]->heap_lock.lock();
      //assert(caches[i]->heap.isHeap());
    }

    //update priorities
    for(int i = 0; i < req.size(); i++) {
      Item *item = req[i].item;
      item->priority = req[i].priority;
      if(item->level == -1) {
        item->level = 0;
        caches[0]->heap.push_back(item);
      }
    }

    for(int i = caches.size()-1; i >= 0; i--) {
      //mark as dirty: if caches busy transfering there is no point in updating heap.
      caches[i]->heap_dirty = true;
      //caches[i]->heap.rebuild();
 //TODO this should be done by the cache of course
/*      if(i == 0 && max_items != -1)
        while(caches[i]->heap.size() > max_items) {
          Item *item = caches[i]->heap.popMin();
          index.erase(item->key);
          delete item;
        } */
      caches[i]->heap_lock.unlock();
      caches[i]->check_queue.open();
    }
  }
};


#endif // CONTROLLER_H
