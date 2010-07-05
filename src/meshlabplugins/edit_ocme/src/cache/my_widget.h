#ifndef MY_WIDGET_H
#define MY_WIDGET_H

#include <QGLWidget>
#include <QTest>
#include <QTime>
#include <QVector>
#include <QPainter>
#include "controller.h"
#include <algorithm>

#ifndef WIN32
 #include <X11/Xlib.h>
#endif


class ImageToken: public Token<float> {
  public:
   int number;
   QImage image;
   GLuint tex;
   int level;
   ImageToken(): level(0) {}
};

/*class ImageNetwork: public Cache<ImageToken> {
 public:

}*/

class ImageMemory: public Cache<ImageToken> {
 public:
  int get(ImageToken *token) {
    QImage &image = token->image;
    image = QImage(256, 256, QImage::Format_ARGB32);
    QPainter painter(&image);
    painter.drawText(QRectF(0, 0, 256, 256), Qt::AlignCenter, QString::number(token->number));
    QTest::qSleep(100);
    token->level = 1;
    return image.width()*image.height()*4;
  }
  int drop(ImageToken *token) {
    qDebug() << "Dropping: " << token->number;
    int s = token->image.width()*token->image.height()*4;
    token->image = QImage();
    token->level = 0;
    return s;
  }
  int size(ImageToken *token) {
    return token->image.width()*token->image.height()*4;
  }
};


class ImageGpu: public Cache<ImageToken>  {
 public:
  QGLWidget *fake;
  ImageGpu(size_t cap = INT_MAX): Cache<ImageToken>(cap), fake(NULL) {}
  bool share(QGLWidget *shareWidget) {
    fake = new QGLWidget(NULL, shareWidget);
    return(fake->isValid() && fake->isSharing());
  }

  int get(ImageToken *token) {
    QImage &image = token->image;
    fake->makeCurrent();
    glEnable(GL_TEXTURE_2D);
    token->tex = fake->bindTexture(image);
    glFinish();
    QTest::qSleep(5);
    if(!token->tex) return -1;
        token->level = 2;
    return image.width()*image.height()*4;
  }

  int drop(ImageToken *token) {
    int s = token->image.width()*token->image.height()*4;
    fake->makeCurrent();
    fake->deleteTexture(token->tex);
    glFinish();
        token->level = 1;
    return s;
  }
  int size(ImageToken *token) {
    return token->image.width()*token->image.height()*4;
  }
};

class MyWidget: public QGLWidget {
  Q_OBJECT
 public:
  bool inited;
  int locked;
  int size;
  Controller<ImageToken> controller;
  ImageMemory memory;
  ImageGpu gpu;
  QVector<ImageToken> tokens;

  MyWidget() {
    inited = false;
    locked = -1;
    size = 1000;
    int unit = 262144; //image


  //window->setAutoBufferSwap(true);

    memory.setCapacity(100*unit);
    gpu.setCapacity(30*unit);
    gpu.setInputCache(&memory);

    memory.setObjectName("mem");
    gpu.setObjectName("gpu");
    controller.addCache(&memory);
    controller.addCache(&gpu);

		for(int i = 0; i < 1000; i++) {
			ImageToken token;
			token.number = i;
			tokens.push_back(token);
		}
		for(int i = 0; i < tokens.size(); i++)
			controller.addToken(&tokens[i]);
  }

 public slots:
  void initializeGL() {
    inited = true;
    glViewport(0, 0, width(), height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 600, 0, 400, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gpu.share(this);
    controller.start();
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

    QVector<ImageToken> items;
		controller.pause();
		for(int i = 0; i < tokens.size(); i++)
			items.push_back(tokens[i]);
		controller.resume();

    sort(items.begin(), items.end());

    glBegin(GL_QUADS);
    for(int i = 0; i < items.size(); i++) {

      ImageToken &token = items[i];
      glColor3f(0, (token.level + 1)/3.0, 0);
      int left = i%80;
      int top = 20*(i - left)/80 + 10;
      left *= 7;

      quad(left, top, 5, 5);
      float val = token.getPriority();
      if(val < 0) val = 0;
      if(val > 1) val = 1;
      glColor3f(val, 0, 0);
      quad(left, top + 7, 5, 5);
    //quad(10, 10, 10, 10-1000, 2000, 2000);
    }
    glEnd();
  }

  void update() {
    QTime time = QTime::currentTime();
    int ms = time.second();
    assert(inited);
    static int first = 1;

    //everything should have settle by now... lets have a look

    for(int i = 0; i < tokens.size(); i++) {
      tokens[i].setPriority(tokens[i].getPriority() + 0.5*rand()/((double)RAND_MAX) - 0.25);
    }
/*    //controller.setPriority(11, 1000, false);
    if(locked != -1)
      tokens[locked].unlock();

    locked = (int)(size*rand()/((double)RAND_MAX));
    if(locked >= size) locked = size-1;
    if(locked < 0) locked = 0;
    if(!tokens[locked].lock()) locked = -1; */
    controller.updatePriorities();
  }
  void closeEvent ( QCloseEvent * event ) {
    controller.finish();
    //event->ignore();
  }
};

#endif // MY_WIDGET_H
