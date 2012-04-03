#ifndef MY_WIDGET_H
#define MY_WIDGET_H

#include <QGLWidget>
#include <QTest>
#include <QTime>
#include <QVector>
#include "controller.h"
#include <algorithm>

class MyWidget: public QGLWidget {
  Q_OBJECT
 public:
  bool inited;
  int locked;
  int size;
  Controller<int, float, GLuint> &controller;
  MyWidget(Controller<int, float, GLuint> &c): controller(c) { inited = false; locked = -1; size = 1000; }

 public slots:
  void initializeGL() {
    inited = true;
    glViewport(0, 0, width(), height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 600, 0, 400, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }
  void quad(float t, float l, float w, float h) {
    glVertex3f(t, l, 0);
    glVertex3f(t, l+h, 0);
    glVertex3f(t+w, l+h, 0);
    glVertex3f(t+w, l, 0);
  }
  void paintGL() {
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1, 0, 0);

    QVector<BaseCache<int, float>::Item> items;
    map<int, BaseCache<int, float>::Item *>::iterator it;
    for(it = controller.index.begin(); it != controller.index.end(); it++)
      items.push_back(*(*it).second);


    sort(items.begin(), items.end());


    glBegin(GL_QUADS);
    for(int i = 0; i < items.size(); i++) {

      BaseCache<int, float>::Item &item = items[i];
      glColor3f(0, item.level/2.0, 0);
      int left = i%80;
      int top = 20*(i - left)/80 + 10;
      left *= 7;

      quad(left, top, 5, 5);
      float val = item.priority;
      if(val < 0) val = 0;
      if(val > 1) val = 1;
      glColor3f(val, 0, 0);
      if(item.key == locked)
        glColor3f(0, 0, 1);
      quad(left, top + 7, 5, 5);
    //quad(10, 10, 10, 10-1000, 2000, 2000);
    }
    glEnd();
  }

  void start() {
    QTime time = QTime::currentTime();
    int ms = time.second();
    assert(inited);
    static int first = 1;
    if(first) {
      first = 0;
      controller.start();
        for(int i = 0; i < size; i++)
          controller.setPriority(i, rand()/((double)RAND_MAX));
        controller.updatePriorities();
        return;
    }
    //everything should have settle by now... lets have a look

    for(int i = 0; i < controller.caches.size(); i++) {
      controller.caches[i]->heap_lock.lock();
    }
    //vector<float> maxes(controller.caches.size(), -1);
    //vector<float> mines(controller.caches.size(), 1e20);

    for(int i = 0; i < controller.caches.size(); i++) {
/*      if(!controller.caches[i]->heap.isHeap()) {
        IntervalPtrHeap<BaseCache<int, float>::Item> &h = controller.caches[i]->heap;
        for(int i = 0; i < h.size(); i++) {
          qDebug() << h[i].priority;
        }
        exit(0);
      } */
/*      for(int k = 0; k < controller.caches[i]->heap.size(); k++)
        BaseCache<int, float>::Item &item = controller.caches[i]->heap[k];*/

    }

    for(int i = 0; i < controller.caches.size(); i++) {
      controller.caches[i]->heap_lock.unlock();
    }

    for(int i = 0; i < size; i++) {
    //  controller.setPriority(i, rand()/((double)RAND_MAX), false);
        controller.setPriority(i, controller.index[i]->new_priority + 0.5*rand()/((double)RAND_MAX) - 0.25);
        //QTest::qSleep(100);
    }
    //controller.setPriority(11, 1000, false);
    if(locked != -1)
      controller.unlock(locked);

    locked = (int)(size*rand()/((double)RAND_MAX));
    if(locked >= size) locked =size-1;
    if(locked < 0) locked = 0;
    controller.lock(locked);
    controller.updatePriorities();
  }
  void closeEvent ( QCloseEvent * event ) {
    controller.finish();
    event->ignore();
  }
};

#endif // MY_WIDGET_H
