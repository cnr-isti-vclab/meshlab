#ifndef SYNTHDATA_H
#define SYNTHDATA_H

#define FILTER_PHOTOSYNTH_DEBUG_SOAP

#include <QString>
#include <QtSoapHttpTransport>

typedef struct Point
{
  qreal _x;
  qreal _y;
  qreal _z;
  uchar _r;
  uchar _g;
  uchar _b;
} Point;

class PointCloud
{
private:
  int _coordinateSystem;
  int _binFileCount;
  int _numberOfPoints;
  QList<Point> _points;
};

class CoordinateSystem
{
private:
    int _id;
    bool _shouldBeExported;
    PointCloud _pointCloud;
    //CameraParameterList _cameraParameterList;
};

class SynthData : public QObject
{
  Q_OBJECT

  static QtSoapHttpTransport transport;

public:
  SynthData(QObject *parent = 0);
  SynthData(const SynthData &other);
  ~SynthData();
  bool isValid();

public:
  static SynthData *downloadSynthInfo(QString url);

private slots:
  void readWSresponse();

private:
  void setCollectionID(QString id);
  void setCollectionRoot(QString collectionRoot);
  QString downloadJsonData(QString jsonURL);
  bool parseJsonString(QString jsonString);

private:
  QString _collectionID;
  QString _collectionRoot;
  QList<CoordinateSystem> *_coordinateSystems;
};

class ImportSettings
{
public:
  enum ImportSource { WEB_SITE, ZIP_FILE };

  ImportSettings(ImportSource source, QString sourcePath, bool importPointClouds, bool importCameraParameters);
  ImportSettings::ImportSource source();
  QString sourcePath();
  bool importPointClouds();
  bool importCameraParameters();

private:
  ImportSettings::ImportSource _source;
  QString _sourcePath;
  bool _importPointClouds;
  bool _importCameraParameters;
};

#endif // SYNTHDATA_H
