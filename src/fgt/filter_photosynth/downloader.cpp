/*
 * The following code is slightly based on the C# application
 * SynthExport (http://synthexport.codeplex.com/) by Christoph Hausner
 */

#include "synthData.h"
#include <math.h>
#include <QtSoapMessage>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QScriptEngine>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QImage>
#include <QFile>

#define SET_STATE(val1,val2) { _state = val1; _dataReady = val2; return; }
#define CHECK(condition,val1,val2) { if(condition) { SET_STATE(val1,val2) } }
#define SET_STATE_DELETE(val1,val2) { _state = val1; _dataReady = val2; httpResponse->deleteLater(); return; }
#define CHECK_DELETE(condition,val1,val2) { if(condition) { SET_STATE_DELETE(val1,val2) } }
#define CHECK_ERRORS(errorCode) { if(error) { _state = (errorCode); _dataReady = true; httpResponse->deleteLater(); return; } }

/********************
 * CameraParameters *
 ********************/

/*
Point3f CameraParameters::getTranslation()
{
  return Point3f(_fields[CameraParameters::POS_X], _fields[CameraParameters::POS_Y], _fields[CameraParameters::POS_Z]);
}
*/
/*
Matrix44f CameraParameters::getRotation()
{
  qreal a = _fields[CameraParameters::ROT_X] * _fields[CameraParameters::ROT_X] +
            _fields[CameraParameters::ROT_Y] * _fields[CameraParameters::ROT_Y] +
            _fields[CameraParameters::ROT_Z] * _fields[CameraParameters::ROT_Z];
  qreal w2 = 1 - a;
  qreal w = sqrt(w2);
  Quaternion<float> q((float)w,(float)_fields[CameraParameters::ROT_X],(float)_fields[CameraParameters::ROT_Y],(float)_fields[CameraParameters::ROT_Z]);
  //Quaternion<float> q((float)w,(float)cam[CameraParameters::ROT_Y],(float)cam[CameraParameters::ROT_Z],(float)cam[CameraParameters::ROT_X]);
  Matrix44f rot;
  q.ToMatrix(rot);
  //permutation matrix;
  const float _p[] = {0,1,0,0, 0,0,1,0, 1,0,0,0, 0,0,0,1};
  Matrix44f p((float *)(&_p[0]));
  Matrix44f permRot = p * rot;

  Matrix44f flip;
  flip.SetRotateDeg(180,Point3f(1,0,0));
  Matrix44f flippedRot = rot * flip;

  return rot;
}
*/
/**************
 * PointCloud *
 **************/

PointCloud::PointCloud(int coordSysID, int binFileCount, QObject *parent)
  : QObject(parent)
{
  _coordinateSystem = coordSysID;
  _binFileCount = binFileCount;
}

/********************
 * CoordinateSystem *
 ********************/

CoordinateSystem::CoordinateSystem(int id, QObject *parent)
  : QObject(parent)
{
  _id = id;
  _shouldBeImported = true;
  _pointCloud = 0;
}

/*************
 * SynthData *
 *************/

//contains the error descriptions relative to the errors that may occurr during the import process
//each position index in the array corresponds to the same value in the enum Errors of SynthData class.
//These strings are used by applyFilter in case of error.
const QString SynthData::errors[] =
{
  "The provided URL is invalid",
  "Save path is missing: specify one",
  "The web service returned an error",
  "The requested Synth is unavailable",
  "Could not parse web service response: unexpected response",
  "This filter is compatible with photosynths belonging to \"Synth\" category only",
  "Error parsing collection data",
  "This synth is empty",
  "Error reading binary data, file may be corrupted",
  "The point cloud is stored in an incompatible format and cannot be loaded",
  "Error creating output directory for images"
  "Error saving images to the filesystem"
};

const char *SynthData::progress[] =
{
  "Contacting web service...",
  "Downloading json data...",
  "Parsing json data...",
  "Downloading point cloud bin files...",
  "Loading point cloud data...",
  "Downloading images..."
};

SynthData::SynthData(QObject *parent)
  : QObject(parent)
{
  _coordinateSystems = new QList<CoordinateSystem*>();
  _imageMap = new QHash<int,Image>();
  _state = PENDING;
  _progress = WEB_SERVICE;
  _dataReady = false;
  _semaphore = 0;
}

SynthData::~SynthData()
{
  delete _coordinateSystems;
  delete _imageMap;
}

/*
 * Returns true if this SynthData represents a Synth downloaded from photosynth.net.
 * Upon a successful download this function returns true if called on the download operation result.
 * Use this function to determine if the data were successfully downloaded.
 */
bool SynthData::isValid()
{
  return (_state == NO_ERROR);
}

QtSoapHttpTransport SynthData::transport;

/*
 * Contacts the photosynth web service to retrieve informations about
 * the synth whose identifier is contained within the given url.
 */
SynthData *SynthData::downloadSynthInfo(QString url, QString path)
{
  SynthData *synthData = new SynthData();

  if(url.isNull() || url.isEmpty())
  {
    synthData->_state = WRONG_URL;
    synthData->_dataReady = true;
    return synthData;
  }

  if(path.isNull())
  {
    synthData->_state = WRONG_PATH;
    synthData->_dataReady = true;
    return synthData;
  }
  synthData->_savePath = path;

  //extracts the synth identifier
  int i = url.indexOf("cid=",0,Qt::CaseInsensitive);
  if(i < 0 || url.length() < i + 40)
  {
    synthData->_state = WRONG_URL;
    synthData->_dataReady = true;
    return synthData;
  }

  ImportSettings::ImportSource importSource = ImportSettings::WEB_SITE;
  QString cid = url.mid(i + 4, 36);
  bool importPointClouds = true;
  bool importCameraParameters = false;
  ImportSettings settings(importSource,cid,importPointClouds,importCameraParameters);
  synthData->_collectionID = cid;

  if(settings._source == ImportSettings::WEB_SITE)
  {
    QtSoapMessage message;
    message.setMethod("GetCollectionData", "http://labs.live.com/");
    message.addMethodArgument("collectionId", "", settings._sourcePath);
    message.addMethodArgument("incrementEmbedCount", "", false, 0);

    transport.setAction("http://labs.live.com/GetCollectionData");
    transport.setHost("photosynth.net");
    QObject::connect(&transport, SIGNAL(responseReady()), synthData, SLOT(readWSresponse()));

    transport.submitRequest(message, "/photosynthws/PhotosynthService.asmx");
  }
  synthData->_state = PENDING;
  return synthData;
}

/*
 * Handles the photosynth web service response.
 */
void SynthData::readWSresponse()
{
  const QtSoapMessage &response = transport.getResponse();
  CHECK(response.isFault(), WEBSERVICE_ERROR, true)
  const QtSoapType &returnValue = response.returnValue();
  if(returnValue["Result"].isValid())
    //the requested synth was found
    if(returnValue["Result"].toString() == "OK")
    {
      //we can only extract point clouds from synths
      if(returnValue["CollectionType"].toString() == "Synth")
      {
        _collectionRoot = returnValue["CollectionRoot"].toString();
        //the url of the json string containing data about the synth coordinate systems (different clusters of points)
        //their camera parameters and the number of binary files containing the point clouds data.
        QString jsonURL = returnValue["JsonUrl"].toString();
        downloadJsonData(jsonURL);
      }
      else
        SET_STATE(WRONG_COLLECTION_TYPE, true)
    }
    else
      SET_STATE(NEGATIVE_RESPONSE, true)
  else
    SET_STATE(UNEXPECTED_RESPONSE, true)
}

/*
 * Performs an HTTP request to download a json string containing data
 * about the synth coordinate systems (different clusters of points)
 * their camera parameters and the number of binary files containing
 * the point clouds data.
 */
void SynthData::downloadJsonData(QString jsonURL)
{
  QNetworkAccessManager *manager = new QNetworkAccessManager(this);
  connect(manager, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(parseJsonString(QNetworkReply*)));

  manager->get(QNetworkRequest(QUrl(jsonURL)));
  _progress = DOWNLOAD_JSON;
}

/*
 * Extracts from the given string informations about the coordinate
 * systems and their camera parameters.
 */
void SynthData::parseJsonString(QNetworkReply *httpResponse)
{
  _progress = PARSE_JSON;
  QByteArray payload = httpResponse->readAll();
  QString json(payload);
  QScriptEngine engine;
  QScriptValue jsonData = engine.evaluate("(" + json + ")");
  CHECK_DELETE(engine.hasUncaughtException(), JSON_PARSING, true)
  //the "l" property contains an object whose name is the synth cid
  //and whose value is an array containing the coordinate systems
  QScriptValue collections = jsonData.property("l");
  CHECK_DELETE(!collections.isValid(), JSON_PARSING, true)
  //use an iterator to retrieve the first collection
  QScriptValueIterator iterator(collections);
  QScriptValue collection;
  int coordSystemsCount = 0;
  if(iterator.hasNext())
  {
    iterator.next();
    collection = iterator.value();
    coordSystemsCount = collection.property("_num_coord_systems").toInt32();
    _numImages = collection.property("_num_images").toInt32();
  }
  else
    SET_STATE_DELETE(JSON_PARSING, true)
  if(coordSystemsCount > 0)
  {
    //the json object containing the images informations
    QScriptValue map = collection.property("image_map");
    parseImageMap(map);
    CoordinateSystem *coordSys;
    //the coordinate systems list
    QScriptValue coordSystems = collection.property("x");
    for(int i = 0; i <= coordSystemsCount; ++i)
    {
      coordSys = new CoordinateSystem(i,this);
      _coordinateSystems->append(coordSys);
      QScriptValue cs = coordSystems.property(QString::number(i));
      QScriptValue pointCloud = cs.property("k");
      if(pointCloud.isValid())
      {
        QScriptValueIterator it(pointCloud);
        if(it.hasNext())
        {
          //pointCloud[0]
          it.next();
          QString str = it.value().toString();
          if(!str.isNull() && !str.isEmpty())
          {
            //pointCloud[1]
            it.next();
            int binFileCount = it.value().toInt32();
            PointCloud *p = new PointCloud(i,binFileCount,coordSys);
            coordSys->_pointCloud = p;
          }
        }
      }
      //list of cameras
      QScriptValue cameras = cs.property("r");
      if(cameras.isValid() && !cameras.isNull())
      {
        QScriptValueIterator it(cameras);
        while(it.hasNext())
        {
          it.next();
          int id = it.name().toInt();
          QScriptValue camera = it.value();
          QScriptValue parameters = camera.property("j");
          CameraParameters params;
          params._camID = id;
          QScriptValueIterator paramIt(parameters);
          paramIt.next();
          params._imageID = paramIt.value().toInt32();
          for(int i = CameraParameters::FIRST; i <= CameraParameters::LAST; ++i)
          {
            paramIt.next();
            params[i] = paramIt.value().toNumber();
          }
          QScriptValue distortion = camera.property("f");
          if(distortion.isValid() && !distortion.isNull())
          {
            QScriptValueIterator distortionIt(distortion);
            distortionIt.next();
            params._distortionRadius1 = distortionIt.value().toNumber();
            distortionIt.next();
            params._distortionRadius2 = distortionIt.value().toNumber();
          }
          else
            params._distortionRadius1 = params._distortionRadius2 = 0;
          coordSys->_cameraParametersList.append(params);
        }
      }
    }
    downloadBinFiles();
  }
  else
    SET_STATE_DELETE(EMPTY, true)
  httpResponse->deleteLater();
}

/*
 * Parse the image list and, for each element of the list, creates an Image and puts it in the dictionary
 */
void SynthData::parseImageMap(QScriptValue &map)
{
  QScriptValueIterator imageIt(map);
  while(imageIt.hasNext())
  {
    imageIt.next();
    Image image;
    image._ID = imageIt.name().toInt();
    QScriptValue size = imageIt.value().property("d");
    QScriptValueIterator sizeIt(size);
    sizeIt.next();
    image._width = sizeIt.value().toInt32();
    sizeIt.next();
    image._height = sizeIt.value().toInt32();
    image._url = imageIt.value().property("u").toString();
    _imageMap->insert(image._ID,image);
  }
}

/*
 * Asks the server for bin files containing point clouds data
 * sending an http request for each bin file composing the synth
 */
void SynthData::downloadBinFiles()
{
  QNetworkAccessManager *manager = new QNetworkAccessManager(this);
  connect(manager, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(loadBinFile(QNetworkReply*)));
  _progress = DOWNLOAD_BIN;
  foreach(CoordinateSystem *sys, *_coordinateSystems)
  {
    if(sys->_pointCloud)
    {
      //As QNetworkAccessManager API is asynchronous, there is no mean, in the slot handling the response (loadBinFile),
      //to understand if the response received is the last one, and thus to know if all data have been received.
      //To let loadBinFile know when the processed response is the last one (allowing the _dataReady variable to be set to true)
      //the counter _semaphore is used
      _semaphore += sys->_pointCloud->_binFileCount;
      for(int i = 0; i < sys->_pointCloud->_binFileCount; ++i)
      {
        QString url = QString("%0points_%1_%2.bin").arg(_collectionRoot).arg(sys->_id).arg(i);
        QNetworkRequest *request = new QNetworkRequest(QUrl(url));
        PointCloud *p = (PointCloud *)sys->_pointCloud;
        //the slot handling the response (loadBinFile) is able to know which pointCloud the received data belong to
        //retrieving the originating object of the request whose response is being processed
        request->setOriginatingObject(p);
        manager->get(*request);
        delete request;
      }
    }
  }
}

/*
 * Reads point data from httpResponse payload and fills
 * the pointCloud of the coordinateSystem relative to the http request
 * originating httpResponse
 */
void SynthData::loadBinFile(QNetworkReply *httpResponse)
{
  if(_state == READING_BIN_DATA || _state == BIN_DATA_FORMAT)
  {
    httpResponse->deleteLater();
    return;
  }

  _progress = LOADING_BIN;

  bool error = false;
  unsigned short versionMajor = readBigEndianUInt16(httpResponse,error);
  CHECK_ERRORS(READING_BIN_DATA)
  unsigned short versionMinor = readBigEndianUInt16(httpResponse,error);
  CHECK_ERRORS(READING_BIN_DATA)
  CHECK_DELETE(versionMajor != 1 || versionMinor != 0, BIN_DATA_FORMAT, true)
  int n = readCompressedInt(httpResponse,error);
  CHECK_ERRORS(READING_BIN_DATA)
  //skip the header section
  for (int i = 0; i < n; i++)
  {
    int m = readCompressedInt(httpResponse,error);
    CHECK_ERRORS(READING_BIN_DATA)
    for (int j = 0; j < m; j++)
    {
      readCompressedInt(httpResponse,error);
      CHECK_ERRORS(READING_BIN_DATA)
      readCompressedInt(httpResponse,error);
      CHECK_ERRORS(READING_BIN_DATA)
    }
  }

  int nPoints = readCompressedInt(httpResponse,error);
  CHECK_ERRORS(READING_BIN_DATA)
  for (int i = 0; i < nPoints; i++)
  {
    Point point;

    point._x = readBigEndianSingle(httpResponse,error);
    CHECK_ERRORS(READING_BIN_DATA)
    point._y = readBigEndianSingle(httpResponse,error);
    CHECK_ERRORS(READING_BIN_DATA)
    point._z = readBigEndianSingle(httpResponse,error);
    CHECK_ERRORS(READING_BIN_DATA)

    ushort color = readBigEndianUInt16(httpResponse,error);
    CHECK_ERRORS(READING_BIN_DATA)

    point._r = (uchar)(((color >> 11) * 255) / 31);
    point._g = (uchar)((((color >> 5) & 63) * 255) / 63);
    point._b = (uchar)(((color & 31) * 255) / 31);
    //the cloud whose received points belong to
    PointCloud *cloud = (PointCloud *)httpResponse->request().originatingObject();
    cloud->_points.append(point);
  }

  --_semaphore;
  if(_semaphore == 0)
  {
    if(!_savePath.isEmpty())
      downloadImages();
    else
      SET_STATE_DELETE(NO_ERROR,true)
  }

  httpResponse->deleteLater();
}

/*
 * Asks the server for the images making up this synth.
 * One http request is made for each image.
 */
void SynthData::downloadImages()
{
  _progress = DOWNLOAD_IMG;
  QDir dir(_savePath);
  /*bool success = */dir.mkdir(_collectionID);
  //if(!success)
    //qWarning("fallimento creazione directory");
  ///CHECK(!success,CREATE_DIR,true)

  QNetworkAccessManager *manager = new QNetworkAccessManager(this);
  connect(manager, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(saveImages(QNetworkReply*)));
  foreach(Image img, *_imageMap)
  {
    QNetworkRequest *request = new QNetworkRequest(QUrl(img._url));
    request->setAttribute(QNetworkRequest::User,QVariant(img._ID));
    manager->get(*request);
    delete request;
  }
}

/*
 * Reads the http response and save the image to the filesystem
 */
void SynthData::saveImages(QNetworkReply *httpResponse)
{
  if(_state == SAVE_IMG)
  {
    httpResponse->deleteLater();
    return;
  }

  if(httpResponse->error() != QNetworkReply::NoError)
    qDebug() << httpResponse->errorString();

  QByteArray payload = httpResponse->readAll();
  QDir dir(_savePath);
  dir.cd(_collectionID);
  int id = httpResponse->request().attribute(QNetworkRequest::User).toInt();
  QString filename("IMG_%1.jpg");
  QFile file(dir.filePath(filename.arg(QString::number(id))));
  CHECK_DELETE(!file.open(QIODevice::WriteOnly),SAVE_IMG,true)
  CHECK_DELETE(file.write(payload) == -1,SAVE_IMG,true)
  file.close();

  ++_semaphore;
  CHECK_DELETE(_semaphore == _numImages, NO_ERROR, true)

  httpResponse->deleteLater();
}

/******************
 * ImportSettings *
 ******************/

ImportSettings::ImportSettings(ImportSource source, QString sourcePath, bool importPointClouds, bool importCameraParameters)
{
  _source = source;
  _sourcePath = sourcePath;
  _importPointClouds = importPointClouds;
  _importCameraParameters = importCameraParameters;
}

/*********************
 * Utility functions *
 *********************/

int readCompressedInt(QIODevice *device, bool &error)
{
  error = false;
  int i = 0;
  unsigned char byte;

  do
  {
    error = device->read((char *)&byte, sizeof(char)) == -1 ? true : false;
    if(error)
      return i;
    i = (i << 7) | (byte & 127);
  } while (byte < 128);
  return i;
}

float readBigEndianSingle(QIODevice *device, bool &error)
{
  error = false;
  unsigned char bytes[4];
  for(int i = 0; i < 4; ++i)
  {
    error = device->read((char *)(bytes + i), sizeof(char)) == -1 ? true : false;
    if(error)
      return -1;
  }
  char reversed[] = { bytes[3],bytes[2],bytes[1],bytes[0] };

  float *f = (float *)(&  reversed[0]);
  return*f;
}

unsigned short readBigEndianUInt16(QIODevice *device, bool &error)
{
  error = false;
  unsigned short byte1=0;
  error = device->read((char *)&byte1,sizeof(char)) == -1 ? true : false;
  if(error) return 0;
  unsigned short byte2=0;
  error = device->read((char *)&byte2,sizeof(char)) == -1 ? true : false;
  if(error)
    return 0;

  return byte2 | (byte1 << 8);
}

void printPoint(Point *p)
{
  qDebug() << "x =" << p->_x << "; y =" << p->_y << "; z =" << p->_z << "R: " << p->_r << " G: " << p->_g << " B: " << p->_b;
}
