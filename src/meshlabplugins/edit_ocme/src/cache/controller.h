#ifndef VCONTROLLER_H
#define VCONTROLLER_H

#include "cache.h"

template <class Token>
class Controller {
 public:
  std::vector<Token *> tokens; //tokens waiting to be added
  QMutex priority_lock;        //guards transfer of priorities to caches.
  bool quit;                   //gracefully terminate.

 public:
  Provider<Token> provider;
  std::vector<Cache<Token> *> caches;

  Controller(): quit(false) {}
  ~Controller() { finish(); }

  void addCache(Cache<Token> *cache) {
    if(caches.size() == 0)
      cache->setInputCache(&provider);
    else
      cache->setInputCache(caches.back());
    assert(cache->input);
    caches.push_back(cache);
  }

  void addToken(Token *token) {
    tokens.push_back(token);
  }

  void pushPriorities(Provider<Token> *pro) {
    for(int i = 0; i < pro->heap.size(); i++)
      pro->heap[i].pushPriority();
  }

  void updatePriorities() {

    if(tokens.size()) {
      QMutexLocker l(&provider.heap_lock);
      for(int i = 0; i < tokens.size(); i++)
        provider.heap.push(tokens[i]);
      tokens.clear();
    }

    QMutexLocker locker(&priority_lock);
    pushPriorities(&provider);
    provider.signalDirty();
    for(unsigned int i = 0; i < caches.size(); i++) {
      pushPriorities(caches[i]);
      caches[i]->signalDirty();
    }
  }

  void start() {
    assert(caches.size() > 1);
    caches.back()->final = true;
    for(unsigned int i = 0; i < caches.size(); i++) //cache 0 is a provider, and his thread is not running.
      caches[i]->start();
  }

  void finish() {
    //stop threads
			if( caches.empty() || (!caches.empty() &&!caches[0]->isRunning()) ) return;
		for(int i = caches.size()-1; i >= 0; i--) {
      caches[i]->quit = true;          //hmmmmmmmmmmmmmm not very clean.
      if(i == 0)
        provider.check_queue.open();
      else
        caches[i-1]->check_queue.open(); //cache i listens on queue i-1
      caches[i]->wait();
    }
    //flush data.
    for(int i = caches.size()-1; i >= 0; i--)
      caches[i]->flush();

		provider.heap.clear();
  }

  void pause() {
    provider.heap_lock.lock();
    for(unsigned int i = 0; i < caches.size(); i++)
      caches[i]->heap_lock.lock();
  }
  void resume() {
    provider.heap_lock.unlock();
    for(unsigned int i = 0; i < caches.size(); i++)
      caches[i]->heap_lock.unlock();
  }

  void flush() {
    for(unsigned int i = caches.size()-1; i >= 0; i--)
      caches[i]->flush();
  }
};


#endif // CONTROLLER_H
