#ifndef SYNTHDATA_H
#define SYNTHDATA_H

//#define FILTER_PHOTOSYNTH_DEBUG_SOAP
//#define PRINT_JSON

//#include <QObject>
#include <QString>
#include <QtSoapHttpTransport>

typedef struct Point
{
  float _x;
  float _y;
  float _z;
  uchar _r;
  uchar _g;
  uchar _b;
} Point;

/*
 * Represents a set of points
 */
class PointCloud : public QObject
{
public:
  PointCloud(int coordSysID, int binFileCount, QObject *parent = 0);
  ~PointCloud();
  int binFileCount() const;
  void addPoint(Point p);
  const QList<Point> *points() const;

private:
  //the coordinate system id within the synth which this set belongs to
  int _coordinateSystem;
  //this is the n parameter in the points_m_n.bin files containing the synth point clounds
  //and tells how many files this cloud is split into
  int _binFileCount;
  int _numberOfPoints;
  QList<Point> *_points;
};

/*
 * Represents an independent cluster of points within the synth,
 * it is identified by an ID, contains a point cloud and
 * has its own camera parameters
 */
class CoordinateSystem : public QObject
{
public:
  CoordinateSystem(int id, QObject *parent = 0);
  void setPointCloud(PointCloud *pointCloud);
  const PointCloud *pointCloud();
  int id();

private:
  //this is the m parameter in the points_m_n.bin files containing the synth point clounds
  int _id;
  bool _shouldBeExported;
  PointCloud *_pointCloud;
  //CameraParameterList _cameraParameterList;
};

/*
 * Represents a Synth
 */
class SynthData : public QObject
{
  Q_OBJECT

  static QtSoapHttpTransport transport;

public:
  //contains errors descriptions
  static const QString errors[];
  //contains the strings used by cb() funcion
  static const char *progress[];

  enum Errors
  {
    WRONG_URL = 0,
    WEBSERVICE_ERROR,
    NEGATIVE_RESPONSE,
    UNEXPECTED_RESPONSE,
    WRONG_COLLECTION_TYPE,
    JSON_PARSING,
    READING_BIN_DATA,
    BIN_DATA_FORMAT,
    NO_ERROR,
    PENDING
  };

  enum Progress
  {
    WEB_SERVICE = 0,
    DOWNLOAD_JSON,
    PARSE_JSON,
    DOWNLOAD_BIN,
    LOADING_BIN
  };

  SynthData(QObject *parent = 0);
  SynthData(const SynthData &other);
  ~SynthData();
  bool isValid();
  bool dataReady();
  int state();
  int step();
  const char* progressInfo();
  const QList<CoordinateSystem*> *coordinateSystems();

public:
  static SynthData *downloadSynthInfo(QString url);

private slots:
  void readWSresponse();
  void parseJsonString(QNetworkReply *httpResponse);
  void loadBinFile(QNetworkReply *httpResponse);

private:
  void setCollectionID(QString id);
  void setCollectionRoot(QString collectionRoot);
  void downloadJsonData(QString jsonURL);
  void downloadBinFiles();

private:
  //this is ths cid parameter taken from the url used to access the synth on photosynth.net
  QString _collectionID;
  //the base url of the binary files points_m_n.bin containing point clouds data
  QString _collectionRoot;
  //Each coordinate system is a different cluster of point in the synth
  QList<CoordinateSystem*> *_coordinateSystems;
  //tells if this synth is valid, or if errors were encountered during the import process
  Errors _state;
  Progress _progress;
  //when a SynthData is instantiated _dataReady == false
  //until the data are downloaded from photosynth server
  bool _dataReady;
  //used to count how many responses to bin files requests have been processed
  //when _semaphore reaches 0 _dataReady can be set to true
  int _semaphore;
};

/*
 * Represents the options of the import process
 */
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
  //specifies if the synth has to be downloaded from a url or loaded from a zip file on the filesystem
  ImportSettings::ImportSource _source;
  //can be the cid parameter taken from the synth url or a path on a filesystem
  QString _sourcePath;
  //specifies if the point clouds have to be imported
  bool _importPointClouds;
  //specifies if the camera parameters have to be imported
  bool _importCameraParameters;
};

/*********************
 * Utility functions *
 *********************/

int readCompressedInt(QIODevice *device, bool &error);
float readBigEndianSingle(QIODevice *device, bool &error);
unsigned short readBigEndianUInt16(QIODevice *device, bool &error);
void printPoint(Point *p);

#endif // SYNTHDATA_H
