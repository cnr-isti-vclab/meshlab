#include "synthData.h"
#include <QtSoapMessage>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QScriptEngine>
#include <QScriptValue>
#include <QScriptValueIterator>

#define CHECK_ERRORS(errorCode) { if(error) { _state = (errorCode); _dataReady = true; return; } }

/**************
 * PointCloud *
 **************/

PointCloud::PointCloud(int coordSysID, int binFileCount, QObject *parent)
  : QObject(parent)
{
  _coordinateSystem = coordSysID;
  _binFileCount = binFileCount;
}


int PointCloud::binFileCount() const
{
  return _binFileCount;
}


/********************
 * CoordinateSystem *
 ********************/

CoordinateSystem::CoordinateSystem(int id, QObject *parent)
  : QObject(parent)
{
  _id = id;
  _shouldBeExported = true;
  _pointCloud = 0;
}

int CoordinateSystem::id()
{
  return _id;
}

const PointCloud *CoordinateSystem::pointCloud()
{
  return _pointCloud;
}

void CoordinateSystem::setPointCloud(PointCloud *pointCloud)
{
  _pointCloud = pointCloud;
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
  "The web service returned an error",
  "The requested Synth is unavailable",
  "Could not parse web service response: unexpected response",
  "This filter is compatible with photosynths belonging to \"Synth\" category only",
  "Error parsing collection data",
  "Error reading binary data, file may be corrupted",
  "The point cloud is stored in an incompatible format and cannot be loaded"
};

const char *SynthData::progress[] =
{
  "Contacting web service...",
  "Downloading json data...",
  "Parsing json data...",
  "Downloading point cloud bin files...",
  "Loading point cloud data..."
};

SynthData::SynthData(QObject *parent)
  : QObject(parent)
{
  _coordinateSystems = new QList<CoordinateSystem*>();
  _state = PENDING;
  _progress = WEB_SERVICE;
  _dataReady = false;
  _semaphore = 0;
}

SynthData::SynthData(const SynthData &other)
{
  _collectionID = other._collectionID;
  _collectionRoot = other._collectionRoot;
  _coordinateSystems = new QList<CoordinateSystem*>(*other._coordinateSystems);
  _state = other._state;
  _progress = other._progress;
  _dataReady = other._dataReady;
  _semaphore = other._semaphore;
}

SynthData::~SynthData()
{
  delete _coordinateSystems;
}

void SynthData::setCollectionID(QString id)
{
  _collectionID = id;
}

void SynthData::setCollectionRoot(QString collectionRoot)
{
  _collectionRoot = collectionRoot;
}

const QList<CoordinateSystem*> *SynthData::coordinateSystems()
{
  return _coordinateSystems;
}

/*
 * Returns true if this SynthData represents a Synth downloaded from photosynth.net.
 * Upon a successful download this function returns true if called on the download operation result.
 * Use this function to determine if the data were successfully downloaded.
 */
bool SynthData::isValid()
{
  bool valid = false;
  if(_state == NO_ERROR)
      valid = true;
  return valid;
}

/*
 * Returns true if and only if the download process is finished.
 * Use isValid() to know if errors occurred during the operation.
 */
bool SynthData::dataReady()
{
    return _dataReady;
}

int SynthData::state()
{
  return _state;
}

int SynthData::step()
{
  return _progress;
}

const char *SynthData::progressInfo()
{
  return progress[_progress];
}

QtSoapHttpTransport SynthData::transport;

/*
 * Contacts the photosynth web service to retrieve informations about
 * the synth whose identifier is contained within the given url.
 */
SynthData *SynthData::downloadSynthInfo(QString url)
{
  SynthData *synthData = new SynthData();

  if(url.isNull() || url.isEmpty())
  {
    synthData->_state = WRONG_URL;
    synthData->_dataReady = true;
    return synthData;
  }

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
  synthData->setCollectionID(cid);

  if(settings.source() == ImportSettings::WEB_SITE)
  {
    QtSoapMessage message;
    message.setMethod("GetCollectionData", "http://labs.live.com/");
    message.addMethodArgument("collectionId", "", settings.sourcePath());
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
  if(response.isFault())
  {
    qWarning("ERROR: %s\n",response.faultString().toString().toLatin1().constData());
    _state = WEBSERVICE_ERROR;
    _dataReady = true;
    return;
  }

#ifdef FILTER_PHOTOSYNTH_DEBUG_SOAP
  qWarning("%s\n", response.toXmlString().toLatin1().constData());
#endif

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
      {
        qWarning("This filter is compatible with photosynths only, the provided url belongs to the category: %s\n",returnValue["CollectionType"].toString().toLatin1().constData());
        _state = WRONG_COLLECTION_TYPE;
        _dataReady = true;
        return;
      }
    }
    else
    {
      qWarning("The web service returned: %s\n",returnValue["Result"].toString().toLatin1().constData());
      _state = NEGATIVE_RESPONSE;
      _dataReady = true;
      return;
    }
  else
  {
    qWarning("Cannot read the response\n");
    _state = UNEXPECTED_RESPONSE;
    _dataReady = true;
  }
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
#ifdef PRINT_JSON
  qWarning("json string:\n%s",json.toLatin1().constData());
#endif
  QScriptEngine engine;
  QScriptValue jsonData = engine.evaluate("(" + json + ")");
  if(engine.hasUncaughtException())
  {
    qWarning("Uncaught exception\n");
    if(jsonData.isError())
    {
      qWarning("%s %d\n",jsonData.toString().toLatin1().constData(),json.length());
    }
    _state = JSON_PARSING;
    return;
  }
  //the "l" property contains an object whose name is the synth cid
  //and whose value is an array containing the coordinate systems
  QScriptValue collections = jsonData.property("l");
  if(!collections.isValid())
  {
    qWarning("Invalid property\n");
    _state = JSON_PARSING;
    return;
  }
  //use an iterator to retrieve the first collection
  QScriptValueIterator iterator(collections);
  QScriptValue collection;
  int coordSystemsCount = 0;
  if(iterator.hasNext())
  {
    iterator.next();
    collection = iterator.value();
    coordSystemsCount = collection.property("_num_coord_systems").toInt32();
  }
  else
  {
    qWarning("Error\n");
    _state = JSON_PARSING;
    return;
  }
  if(coordSystemsCount > 0)
  {
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
            coordSys->setPointCloud(p);
          }
        }
        else
          qWarning("Niente point cloud\n");
      }
    }
    downloadBinFiles();
  }
  else
  {
    qWarning() << "Empty collection\n";
    _dataReady = true;
    return;
  }
  httpResponse->deleteLater();
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
    if(sys->pointCloud() != 0)
    {
      //As QNetworkAccessManager API is asynchronous, there is no mean, in the slot handling the response (loadBinFile),
      //to understand if the response received is the last one, and thus to know if all data have been received.
      //To let loadBinFile know when the processed response is the last one (allowing the _dataReady variable to be set to true)
      //the counter _semaphore is used
      _semaphore += sys->pointCloud()->binFileCount();
      for(int i = 0; i < sys->pointCloud()->binFileCount(); ++i)
      {
        QString url = QString("%0points_%1_%2.bin").arg(_collectionRoot).arg(sys->id()).arg(i);
        QNetworkRequest *request = new QNetworkRequest(QUrl(url));
        PointCloud *p = (PointCloud *)sys->pointCloud();
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
    return;

  _progress = LOADING_BIN;

  bool error = false;
  unsigned short versionMajor = readBigEndianUInt16(httpResponse,error);
  CHECK_ERRORS(READING_BIN_DATA)
  unsigned short versionMinor = readBigEndianUInt16(httpResponse,error);
  CHECK_ERRORS(READING_BIN_DATA)
  if (versionMajor != 1 || versionMinor != 0)
  {
    _state = BIN_DATA_FORMAT;
    _dataReady = true;
    return;
  }

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
  qDebug("Reading %i points",nPoints);
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
    //qDebug("Read (%15f %15f %15f) - R:%3i G:%3i B:%3i",point._x,point._y,point._z,point._r,point._g,point._b);
    //the cloud whose received points belong to
    PointCloud *cloud = (PointCloud *)httpResponse->request().originatingObject();
    cloud->points.append(point);
  }

  --_semaphore;
  if(_semaphore == 0)
  {
    //all data have been received
    _state = NO_ERROR;
    _dataReady = true;
  }
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

ImportSettings::ImportSource ImportSettings::source()
{
  return _source;
}

QString ImportSettings::sourcePath()
{
  return _sourcePath;
}

bool ImportSettings::importPointClouds()
{
  return _importPointClouds;
}

bool ImportSettings::importCameraParameters()
{
  return _importCameraParameters;
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
  //qDebug() << (int) bytes[3] << (int) bytes[2] << (int) bytes[1] << (int) bytes[0];
 // qDebug("%3i %3i %3i %3i",(int)bytes[0], (int)bytes[1], (int)bytes[2], (int)bytes[3]);
  char reversed[] = { bytes[3],bytes[2],bytes[1],bytes[0] };
  //char reversed[] = { bytes[0],bytes[1],bytes[2],bytes[3] };

  float *f = (float *)(&  reversed[0]);
  return*f;
  //qDebug() << QByteArray(reversed).toFloat() << QByteArray(bytes).toFloat();
  //return QByteArray(reversed).toFloat();
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
