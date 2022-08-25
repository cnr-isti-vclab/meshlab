#include <QtGui/QApplication>
#include <QImage>
#include <QPainter>
#include <QTest>

#include <time.h>
#include <vector>
#include <iostream>

#include "cache.h"
#include "controller.h"

using namespace std;

class ImageProvider: public Provider<int, float, QImage> {
  size_t size(const int &/*key*/) {
    return 256*256*4;
  }
  bool get(const int & key, QImage & value) {
    QImage image(256, 256, QImage::Format_ARGB32);
    QPainter painter(&image);
    painter.drawText(QRectF(0, 0, 256, 256), Qt::AlignCenter, QString::number(key));
    QTest::qSleep(10);
  }
};

class ImageMemory: public MemoryCache<int, float, QImage, QImage> {
 public:
  int delay;
  ImageMemory(int size, Provider<int, float, QImage> *input_cache):
    MemoryCache<int, float, QImage, QImage>(size, input_cache) {
    delay = 2;
  }

  bool get(const int & key, QImage & value) {
    QTest::qSleep(delay);
    return MemoryCache<int, float, QImage, QImage>::get(key, value);
  }
  size_t size(const int &/*key*/) {
    return 1;
  }
};


int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  ImageProvider provider;
  provider.setObjectName("provider");
  ImageMemory cache1(10, &provider);
  ImageMemory cache0(5, &cache1);
  cache0.delay = 1;
  cache1.setObjectName("cache1");
  cache0.setObjectName("cache0");
  Controller<int, float, QImage> controller;
  controller.addCache(&provider);
  controller.addCache(&cache1);
  controller.addCache(&cache0);
  controller.start();
  qDebug("Miao!");
  for(int i = 0; i < 40; i++) {
    controller.setPriority(i, i);
    QTest::qSleep(1);
  }
  for(int i = 0; i < 40; i++) {
    controller.setPriority(i, 40-i, false);
  }
  controller.updatePriorities();
  QTest::qSleep(1000);


  qDebug("Miao!");
  QTest::qWait(2000);
  qDebug("main program calling destruction");

  return 0;
}
