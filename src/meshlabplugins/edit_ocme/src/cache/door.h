#ifndef CACHE_DOOR_H
#define CACHE_DOOR_H

#include <QSemaphore>

//a door needs to be open for the thread to continue,
//if it is open the thread enter and closes the door
//this mess is to avoid [if(!open.available()) open.release(1)]
class QDoor {
 private:
  QSemaphore _open;
  QSemaphore _close;
 public:
  QDoor(): _open(0), _close(1) {} //this means closed
  void open() {
    if(_close.tryAcquire(1)) //check it is not open
      _open.release(1); //open
  }
  void close() {
    if(_open.tryAcquire(1)) //check not already cloed
      _close.release(1);
  }
  void enter(bool close = false) {
    _open.acquire(1);
    if(close)
      _close.release(1); //and close door behind
    else
      _open.release(1); //and leave door opened
  }
  bool isOpen() { return _open.available() == 1; }
};

#endif
