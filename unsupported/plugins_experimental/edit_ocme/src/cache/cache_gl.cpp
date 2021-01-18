#include <QtGui/QApplication>
#include <QImage>
#include <QPainter>
#include <QTest>
#include <QGLWidget>
#include <QGLPixelBuffer>
#include <QTimer>

#include <time.h>
#include <vector>
#include <iostream>

#include "cache.h"
#include "controller.h"
#include "my_widget.h"

using namespace std;


int main(int argc, char *argv[]) {
#ifndef WIN32
  XInitThreads();
#endif
  QApplication app(argc, argv);
  MyWidget *window = new MyWidget;

  //see my_widget.h for controller and cache creation

  window->show();

  window->update();
  QTimer *timer = new QTimer(window);
  QObject::connect(timer, SIGNAL(timeout()), window, SLOT(update()));
  timer->start(2000);

  QTimer *timer1 = new QTimer(window);
  QObject::connect(timer1, SIGNAL(timeout()), window, SLOT(updateGL()));
  timer1->start(20);

  QTimer *timer2 = new QTimer(window);
  QObject::connect(timer2, SIGNAL(timeout()), window, SLOT(close()));
  timer2->start(20000);

  app.exec();
  timer->stop();
  timer1->stop();
  timer2->stop();

  QTest::qWait(2000);
  qDebug("main program calling destruction");
  return 0;
}
