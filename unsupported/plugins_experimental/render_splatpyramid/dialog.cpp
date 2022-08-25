#include "dialog.h"
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>


#define DECFACTOR 100000.0f

using namespace vcg;

Dialog::Dialog(PointBasedRenderer* r, QGLWidget* gla, QWidget *parent)
  : QDockWidget(parent) {

  renderer = r;
  glarea = gla;

  ui.setupUi(this);
  this->setWidget(ui.frame);
  this->setFeatures(QDockWidget::AllDockWidgetFeatures);
  this->setAllowedAreas(Qt::LeftDockWidgetArea);
  this->setFloating(true);

  colorSignalMapper = new QSignalMapper(this);
  valueSignalMapper = new QSignalMapper(this);

	
  QGridLayout * qgrid = new QGridLayout(ui.uvTab);
  qgrid->setColumnMinimumWidth(0, 45);
  qgrid->setColumnMinimumWidth(1, 40);
  qgrid->setColumnMinimumWidth(2, 40);
  qgrid->setColumnMinimumWidth(3, 40);

  QLabel *perVertexColorLabel = new QLabel(this);
  perVertexColorLabel->setText("Use PerVertex Color");

  QCheckBox *perVertexColorCBox = new QCheckBox(this);

  //	rendMode->colorMode =  GLW::CMNone;

  connect(perVertexColorCBox, SIGNAL(stateChanged(int)), this, SLOT(setColorMode(int)));
  qgrid->addWidget(perVertexColorLabel, 0, 0);
  qgrid->addWidget(perVertexColorCBox, 0, 1);
	
  // 	connect(colorSignalMapper, SIGNAL(mapped(const QString &)), this, SLOT(setColorValue(const QString &)));
  // 	connect(valueSignalMapper, SIGNAL(mapped(const QString &)), this, SLOT(valuesChanged(const QString &)));

  QLabel *radiusSizeLabel = new QLabel(this);
  radiusSizeLabel->setText("Radius Size");

  QDoubleSpinBox *radiusSizeSBox = new QDoubleSpinBox(this);
  connect(radiusSizeSBox, SIGNAL(valueChanged(double)), this, SLOT(setRadiusSize(double)));
  qgrid->addWidget(radiusSizeLabel, 1, 0);
  qgrid->addWidget(radiusSizeSBox, 1, 1);
  radiusSizeSBox->setMaximum(99.0);
  radiusSizeSBox->setValue(0.001);
  radiusSizeSBox->setDecimals(4);
  radiusSizeSBox->setSingleStep(0.01);


  QLabel *prefilterSizeLabel = new QLabel(this);
  prefilterSizeLabel->setText("Prefilter Size");

  QDoubleSpinBox *prefilterSizeSBox = new QDoubleSpinBox(this);
  connect(prefilterSizeSBox, SIGNAL(valueChanged(double)), this, SLOT(setPrefilterSize(double)));
  qgrid->addWidget(prefilterSizeLabel, 2, 0);
  qgrid->addWidget(prefilterSizeSBox, 2, 1);
  prefilterSizeSBox->setMaximum(99.0);
  prefilterSizeSBox->setValue(1.0);
  prefilterSizeSBox->setDecimals(3);
  prefilterSizeSBox->setSingleStep(0.01);


  QLabel *minimumSizeLabel = new QLabel(this);
  minimumSizeLabel->setText("Minimum Radius Size");

  QDoubleSpinBox *minimumSizeSBox = new QDoubleSpinBox(this);
  connect(minimumSizeSBox, SIGNAL(valueChanged(double)), this, SLOT(setMinimumSize(double)));
  qgrid->addWidget(minimumSizeLabel, 3, 0);
  qgrid->addWidget(minimumSizeSBox, 3, 1);
  minimumSizeSBox->setMaximum(99.0);
  minimumSizeSBox->setValue(0.1);
  minimumSizeSBox->setDecimals(3);
  minimumSizeSBox->setSingleStep(0.01);

  //	this->setWindowFlags(Qt::WindowStaysOnTopHint);
  connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));
}

Dialog::~Dialog()
{
}

void Dialog::setRadiusSize(double value) {
  renderer->setReconstructionFilterSize(value);
  glarea->updateGL();
}

void Dialog::setPrefilterSize(double value) {
  renderer->setPrefilterSize(value);
  glarea->updateGL();
}

void Dialog::setMinimumSize(double value) {
  renderer->setMinimumRadiusSize(value);
  glarea->updateGL();
}


void Dialog::setColorMode(int state) {
  if (state == Qt::Checked) {
	rendMode->colorMode = GLW::CMPerVert;
  } else {
	rendMode->colorMode = GLW::CMNone;
  }
  glarea->updateGL();
}

void Dialog::changeTexturePath(int i) {
  shaderInfo->textureInfo[i].path = textLineEdits[i]->text();
  reloadTexture(i);
}

void Dialog::browseTexturePath(int i) {
  QFileDialog fd(0,"Choose new texture");

  QDir shadersDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
  if (shadersDir.dirName() == "debug" || shadersDir.dirName() == "release")
	shadersDir.cdUp();
#elif defined(Q_OS_MAC)
  if (shadersDir.dirName() == "MacOS") {
	shadersDir.cdUp();
	shadersDir.cdUp();
	shadersDir.cdUp();
  }
#endif
  shadersDir.cd("textures");
	
  fd.setDirectory(shadersDir);
  fd.move(500, 100);

  QStringList newPath;
  if (fd.exec())
	{
	  newPath = fd.selectedFiles();
	  textLineEdits[i]->setText(newPath.at(0));
	  shaderInfo->textureInfo[i].path = newPath.at(0);
	  reloadTexture(i);
	} 
	
}

void Dialog::reloadTexture(int i) {
  glDeleteTextures( 1, &shaderInfo->textureInfo[i].tId);

  glEnable(shaderInfo->textureInfo[i].Target);
  QImage img, imgScaled, imgGL;
  img.load(shaderInfo->textureInfo[i].path);
  // image has to be scaled to a 2^n size. We choose the first 2^N <= picture size.
  int bestW=pow(2.0,floor(::log(double(img.width() ))/::log(2.0)));
  int bestH=pow(2.0,floor(::log(double(img.height()))/::log(2.0)));
  imgScaled=img.scaled(bestW,bestH,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
  imgGL=QGLWidget::convertToGLFormat(imgScaled);

  glGenTextures( 1, &(shaderInfo->textureInfo[i].tId) );
  glBindTexture( shaderInfo->textureInfo[i].Target, shaderInfo->textureInfo[i].tId );
  glTexImage2D( shaderInfo->textureInfo[i].Target, 0, 3, imgGL.width(), imgGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgGL.bits() );
  glTexParameteri( shaderInfo->textureInfo[i].Target, GL_TEXTURE_MIN_FILTER, shaderInfo->textureInfo[i].MinFilter );
  glTexParameteri( shaderInfo->textureInfo[i].Target, GL_TEXTURE_MAG_FILTER, shaderInfo->textureInfo[i].MagFilter ); 
  glTexParameteri( shaderInfo->textureInfo[i].Target, GL_TEXTURE_WRAP_S, shaderInfo->textureInfo[i].WrapS ); 
  glTexParameteri( shaderInfo->textureInfo[i].Target, GL_TEXTURE_WRAP_T, shaderInfo->textureInfo[i].WrapT ); 
  glTexParameteri( shaderInfo->textureInfo[i].Target, GL_TEXTURE_WRAP_R, shaderInfo->textureInfo[i].WrapR ); 

  glarea->updateGL();
}
