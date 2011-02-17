/*
 * The following code is slightly based on the C# application
 * SynthExport (http://synthexport.codeplex.com/) by Christoph Hausner
 */

#ifndef SYNTHDATA_H
#define SYNTHDATA_H

#include <QString>
#include <QScriptValue>
#include <QtSoapHttpTransport>
#include <assert.h>
#include <common/interfaces.h>
#include <QMutex>

using namespace vcg;

typedef struct Point
{
  float _x;
  float _y;
  float _z;
  uchar _r;
  uchar _g;
  uchar _b;
} Point;

typedef struct Image
{
  int _ID;
  int _width;
  int _height;
  int _exifWidth;
  int _exifHeight;
  QString _url;
  QString _localPath;
  bool _shouldBeDownloaded;
} Image;

class CameraParameters
{
public:
  enum Field
  {
    FIRST = 0,
    POS_X = FIRST,
    POS_Y,
    POS_Z,
    ROT_X,
    ROT_Y,
    ROT_Z,
    ASPECT_RATIO,
    FOCAL_LENGTH, //apparently the value provided by photosynth is: ccd_dimension_in_mm / focal_length_in_mm
    LAST = FOCAL_LENGTH
  };

  CameraParameters() : _ccdWidth(0), _focalLength(0), _pixelSizeMm(0) {}
  Point3f getTranslation();
  Matrix44f getRotation();

  //use this to access camera parameters values taken from photosynth json, using enum Field values as argument
  inline qreal &operator [] (const int i)
  {
    assert(i >= 0 && i < 8);
    return _fields[i];
  }

  int _camID;
  int _imageID; //the image whose this object represents the camera that took the shot
  qreal _fields[8]; //contains position, rotation, aspect ratio, photosynth focal length, in the order of enum Field as taken from photosynth json
  qreal _distortionRadius1; //from photosynth json
  qreal _distortionRadius2; //from photosynth json
  //intrinsics
  float _ccdWidth; //taken from image exif data, or (when missing in exif) computed using image resolution in pixel and in dpi
  float _focalLength; //estimated using _fields[8] and _ccdWidth
  float _pixelSizeMm; //estimated using _ccdWidth and image resolution in pixel or, when the first is missing, with image resolution in dpi
};

/*
 * Represents a set of points
 */
class PointCloud : public QObject
{
public:
  PointCloud(int coordSysID, int binFileCount, QObject *parent = 0);

public:
  //the coordinate system id within the synth which this set belongs to
  int _coordinateSystem;
  //this is the n parameter in the points_m_n.bin files containing the synth point clounds
  //and tells how many files this cloud is split into
  int _binFileCount;
  int _numberOfPoints;
  QList<Point> _points;
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

public:
  //this is the m parameter in the points_m_n.bin files containing the synth point clounds
  int _id;
  bool _shouldBeImported;
  PointCloud *_pointCloud;
  QList<CameraParameters> _cameraParametersList;
};

/*
 * Represents the options of the import process
 */
class ImportSettings
{
public:
  ImportSettings() { };
  ImportSettings(QString url, int clusterID, QString imageSavePath = QString(""));

public:
  //the synth url
  QString _url;
  //specifies which coordinate system (cluster of point) has to be imported, -1 means all
  int _clusterID;
  //specifies the path where the images have to be saved to; if it is equal to the empty string, images won't be downloaded
  QString _imageSavePath;
};

/*
 * Represents a Synth
 */
class SynthData : public QObject
{
  Q_OBJECT

public:
  //contains errors descriptions
  static const QString errors[];
  //contains the strings used by cb() funcion
  static const char *steps[];

  enum Error
  {
    WRONG_URL = 0,
    WRONG_PATH,
    WEBSERVICE_ERROR,
    NEGATIVE_RESPONSE,
    UNEXPECTED_RESPONSE,
    WRONG_COLLECTION_TYPE,
    JSON_PARSING,
    EMPTY,
    READING_BIN_DATA,
    BIN_DATA_FORMAT,
    CREATE_DIR,
    SAVE_IMG,
    SYNTH_NO_ERROR,
    PENDING
  };

  enum Step
  {
    WEB_SERVICE = 0,
    DOWNLOAD_JSON,
    PARSE_JSON,
    DOWNLOAD_BIN,
    LOADING_BIN,
    DOWNLOAD_IMG
  };

  SynthData(ImportSettings &settings, QObject *parent = 0);
  ~SynthData();
  bool isValid();

public:
  void downloadSynthInfo(vcg::CallBackPos *cb);
  int progressInfo();

private slots:
  void readWSresponse(const QtSoapMessage &response);
  void parseJsonString(QNetworkReply *httpResponse);
  void loadBinFile(QNetworkReply *httpResponse);
  void saveImages(QNetworkReply *httpResponse);

private:
  void parseImageMap(QScriptValue &map);
  void downloadJsonData(QString jsonURL);
  void downloadBinFiles();
  void downloadImages();

public:
  //this is the cid parameter taken from the url used to access the synth on photosynth.net
  QString _collectionID;
  //the base url of the binary files points_m_n.bin containing point clouds data
  QString _collectionRoot;
  //Each coordinate system is a different cluster of point in the synth
  QList<CoordinateSystem*> *_coordinateSystems;
  //a dictionary mapping images id to image representation
  QHash<int,Image> *_imageMap;
  //tells if this synth is valid, or if errors were encountered during the import process
  Error _state;
  //tells the action the filter is performing during import process
  Step _step;
  //tells the progress (in percentage) of the step being executed
  int _progress;
  //during processing this string is set accordingly to the step is being executed
  QString _info;
  //when a SynthData is instantiated _dataReady == false
  //until the data are downloaded from photosynth server
  bool _dataReady;
  ///Number of images of this synth
  int _numImages;
  int _imagesToDownloadCount;
  //the callback function to inform the user about the progress of the filter
  vcg::CallBackPos *_cb;
  //contains import options
  ImportSettings _settings;
  QMutex _mutex;

private:
  //used to count how many responses to bin files requests have been processed
  //when _semaphore reaches 0 _dataReady can be set to true
  //used also to count how many responses to images requests have been processed
  //when _semaphore reaches _numImages, all images have been downloaded
  int _semaphore;
  int _totalBinFilesCount;
  //the images will be saved here
  QString _savePath;
  //QtSoapHttpTransport *_transport;
};

/*********************
 * Utility functions *
 *********************/

int readCompressedInt(QIODevice *device, bool &error);
float readBigEndianSingle(QIODevice *device, bool &error);
unsigned short readBigEndianUInt16(QIODevice *device, bool &error);
void printPoint(Point *p);

#endif // SYNTHDATA_H
