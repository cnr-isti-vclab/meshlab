#include <QtCore/QCoreApplication>
#include <QTest>
#include "../controller.h"
#include <vector>
#include <iostream>
using namespace std;

class MyToken: public Token<float> {
 public:
  MyToken(int i): number(i) { setPriority(0); }
  int number;
};


class RamCache: public Cache<MyToken> {
 public:
  int get(MyToken *token) {
    //QTest::qSleep(2);
    return 1;
  }
  int drop(MyToken *token) {
    //QTest::qSleep(1);
    return 1;
  }
  int size(MyToken *token) { return 1; }
};

class GpuCache: public Cache<MyToken> {
 public:
  int get(MyToken *token) {
    return 1;
  }
  int drop(MyToken *token) {
    return 1;
  }
  int size(MyToken *token) { return 1; }
};


int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  //generate tokens
  vector<MyToken> tokens;
  for(int i = 0; i < 10000; i++)
    tokens.push_back(MyToken(i));

  //create ram and gpu caches
  RamCache ram;
  ram.setCapacity(5000);
  GpuCache gpu;
  gpu.setCapacity(1000);

  //create controller and add caches
  Controller<MyToken> controller;
  controller.addCache(&ram);
  controller.addCache(&gpu);

  //tell controller about tokens and start (you can change order of this
  for(int i = 0; i < tokens.size(); i++) {
    tokens[i].setPriority(rand()/((double)RAND_MAX));
    controller.addToken(&tokens[i]);
  }

  controller.start();

  double mean = 0;
  double total = 0;
  float range = 0.2;
  for(int  i = 0; i < 1000; i++) {
    for(int i = 0; i < tokens.size(); i++) {
      float p = tokens[i].getPriority() + range*rand()/((double)RAND_MAX) - range/2;
      if(p < 0) p = 0;
      if(p > 1) p = 1;
      tokens[i].setPriority(p);
    }
    controller.updatePriorities();

    QTest::qSleep(2);
    //locko quelli sopra 0.75 e poi li unlockko
    float threshold = 0.75;
    //int count = 0;
    //int total = 0;
    for(int i = 0; i < tokens.size(); i++) {
      if(tokens[i].getPriority() > threshold) {
        total++;
        bool success = tokens[i].lock();
        if(success) {
          mean++;
          tokens[i].unlock();
        }
      }

    }
    //cout << "Locked: " << mean << " / " << total << " gpu: " << gpu.heap.size() << " ram: " << ram.heap.size() << endl;
  }
  cout << "Lock success: " << mean/total << endl;
  controller.finish();
  //return a.exec();
}
