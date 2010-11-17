/****************************************************************************
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include "filter_photosynth.h"
//#include "synthData.h"
#include <QtScript>
#include <unistd.h>
#include "jhead/jhead.h"

//#define CALIBRATED 1

using namespace vcg;

extern ImageInfo_t ImageInfo;

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly
FilterPhotosynthPlugin::FilterPhotosynthPlugin()
{
  typeList << FP_IMPORT_PHOTOSYNTH;
  foreach(FilterIDType tt , types())
    actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action
// (this string is used also to define the menu entry)
QString FilterPhotosynthPlugin::filterName(FilterIDType filterId) const
{
  switch(filterId)
  {
    case FP_IMPORT_PHOTOSYNTH:  return QString("Import Photosynth data");
    default:
        assert(0);
  }
  return QString();
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
QString FilterPhotosynthPlugin::filterInfo(FilterIDType filterId) const
{
  switch(filterId)
  {
    case FP_IMPORT_PHOTOSYNTH:
      return QString("Downloads the synth data from the given URL and creates a document with multiple layers, each containing a set of points");
    default:
      assert(0);
  }
  return QString("Unknown Filter");
}

// The FilterClass describes in which generic class of filters it fits.
// This choice affect the submenu in which each filter will be placed
// More than a single class can be choosen.
FilterPhotosynthPlugin::FilterClass FilterPhotosynthPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_IMPORT_PHOTOSYNTH:
      return MeshFilterInterface::MeshCreation;
    default:
      assert(0);
  }
  return MeshFilterInterface::Generic;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define,
// - the name of the parameter,
// - the default value
// - the string shown in the dialog
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterPhotosynthPlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst)
{
  switch(ID(action))
  {
    case FP_IMPORT_PHOTOSYNTH:
      parlst.addParam(new RichString("synthURL",
                                     "http://photosynth.net/view.aspx?cid=e8f476c5-ed00-4626-a86c-31d654e94109",
                                     "Synth URL",
                                     "Paste the synth URL from your browser."));
      parlst.addParam(new RichBool ("saveImages", true, "Download images", "Download images making up the specified synth."));
      //parlst.addParam(new RichSaveFile("savePath","./",".jpg","Save to","Select the path where images will be saved to"));
      parlst.addParam(new RichString("savePath",
                                     "./",
                                     "Save to",
                                     "Enter the path where images will be saved to"));
      break;
    default:
      assert(0);
  }
}

// The Real Core Function doing the actual mesh processing.
// Imports
bool FilterPhotosynthPlugin::applyFilter(QAction */*filter*/, MeshDocument &md, RichParameterSet &par, vcg::CallBackPos *cb)
{
  QString url = par.getString("synthURL");
  QString path("");
  if(par.getBool("saveImages"))
    //path = par.getSaveFileName("savePath");
    path = par.getString("savePath");
  SynthData *synthData = SynthData::downloadSynthInfo(url,path,cb);

  //Hangs on active wait until data are available from the server
  while(!synthData->_dataReady)
  {
    //allows qt main loop to process the events relative to the response from the server,
    //triggering the signals that cause the invocation of the slots that process the response
    //and set the control variable that stops this active wait.
    //Note that a call to the function usleep() causes an infinite loop, because when the process awakes,
    //the control remains inside this loop and doesn't reach qt main loop that this way can't process events.
    QApplication::processEvents();
    cb(synthData->progressInfo(),synthData->_info.toStdString().data());
  }

  if(!synthData->isValid())
  {
    this->errorMessage = SynthData::errors[synthData->_state];
    return false;
  }
  cb(0,"Finishing import...");
  QDir dir(path);
  QFile file(dir.filePath("Cam.txt"));
  bool success = true;
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    success = false;
    qWarning() << "Failed to create cam.txt";
  }
  QTextStream out(&file);
  //scan coordinate systems list and add a new layer for each one, containing its points
  const QList<CoordinateSystem*> *coordinateSystems = synthData->_coordinateSystems;
  CoordinateSystem *sys;
  int count = coordinateSystems->count();
  foreach(sys, *coordinateSystems)
  {
    cb((int)(sys->_id / count),"Finishing import...");
    if(sys->_pointCloud)
    {
      MeshModel *mm = md.addNewMesh("coordsys"); // After Adding a mesh to a MeshDocument the new mesh is the current one
      Point p;
      foreach(p, sys->_pointCloud->_points)
      {
        tri::Allocator<CMeshO>::AddVertices(mm->cm,1);
        mm->cm.vert.back().P() = Point3f(p._x,p._z,-p._y);
        mm->cm.vert.back().C() = Color4b(p._r,p._g,p._b,255);
      }
      //we consider cameras only if the user chooses to download images,
      //otherwise there is no mean to retrieve camera intrinsics (can't read images exif, without images)
      if(par.getBool("saveImages"))
      {
        //create a new layer where add points representing cameras to
        MeshModel *mm = md.addNewMesh("cameras");
        CameraParameters cam;
        //scan cameras list for this coordinate system and for each one add a raster with a shot matching the camera
        foreach(cam, sys->_cameraParametersList)
        {
          Shotf s;
          s.Extrinsics.SetRot(cam.getRotation());
          s.Extrinsics.SetTra(cam.getTranslation());
          //add a point to the cameras layer as a placeholder for the camera
          tri::Allocator<CMeshO>::AddVertices(mm->cm,1);
          mm->cm.vert.back().P() = cam.getTranslation();
          //find intrinsics
          Image img = synthData->_imageMap->value(cam._imageID);
          QDir imageDir(path);
          imageDir.cd(synthData->_collectionID);
          readExifData(img, cam, imageDir);
          s.Intrinsics.FocalMm = cam._focalLength;
          s.Intrinsics.PixelSizeMm = Point2f(cam._pixelSizeMm,cam._pixelSizeMm);
          s.Intrinsics.ViewportPx = Point2i(img._exifWidth,img._exifHeight);
          s.Intrinsics.CenterPx = Point2f(img._exifWidth/2,img._exifHeight/2);
#ifdef CALIBRATED
          setShotCalibratedExtrinsics(s,computeProjectionMatrix(img._exifWidth,img._exifHeight,cam));
#endif
          if(success)
            outputToFile(out, s, img, cam);
          //add a new raster
          //the same image can be added several times, one for each coordinate system it appears into
          //this way the user can choose which point cloud the raster has to align with
          RasterModel *rm = md.addNewRaster(imageDir.filePath(QString("IMG_%1.jpg").arg(img._ID) ));
          rm->shot = s;
          rm->setLabel(QString("IMG_%1_%2.jpg").arg(int(img._ID),3,10,QLatin1Char('0')).arg(sys->_id));
        }
      }
    }
  }
  file.close();

  return true;
}

QString FilterPhotosynthPlugin::filterScriptFunctionName(FilterIDType filterID)
{
  switch(filterID)
  {
    case FP_IMPORT_PHOTOSYNTH:
      return QString("importPhotosynth");
    default:
      assert(0);
  }
  return QString();
}

void FilterPhotosynthPlugin::matrixScale(int m, int n, float *A, float s, float *R)
{
  int i;
  int entries = m * n;

  for (i = 0; i < entries; i++)
  {
    R[i] = A[i] * s;
  }
}

void FilterPhotosynthPlugin::matrixProduct(int Am, int An, int Bm, int Bn, const float *A, const float *B, float *R)
{
  int r = Am;
  int c = Bn;
  int m = An;

  int i, j, k;
  for (i = 0; i < r; i++)
  {
    for (j = 0; j < c; j++)
    {
      R[i * c + j] = 0.0;
      for (k = 0; k < m; k++)
      {
        R[i * c + j] += A[i * An + k] * B[k * Bn + j];
      }
    }
  }
}

float *FilterPhotosynthPlugin::computeProjectionMatrix(int width, int height, CameraParameters &cam)
{
  float focal = cam[CameraParameters::FOCAL_LENGTH] * qMax(width,height);
  float R[9];
  float t[3];
  Matrix44f rot = cam.getRotation();
  Point3f tra = cam.getTranslation();
  for(int row = 0; row < 3; ++row)
    for(int col = 0; col < 3; ++col)
      R[row * 3 + col] = rot.ElementAt(row,col);
  for(int i = 0; i < 3; ++i)
    t[i] = tra[i];

  float K[9] = { -focal,  0.0,    0.5 * width - 0.5,
                 0.0,     focal,  0.5 * height - 0.5,
                 0.0,     0.0,    1.0 };
  float Ptmp[12] = { R[0], R[1], R[2], t[0],
                     R[3], R[4], R[5], t[1],
                     R[6], R[7], R[8], t[2] };
  float *P = (float*)calloc(12,sizeof(float));
  matrixProduct(3, 3, 3, 4, K, Ptmp, P);
  matrixScale(3, 4, P, -1.0, P);
  return P;
}

void FilterPhotosynthPlugin::setShotCalibratedExtrinsics(Shotf &shot, float *projectionMatrix)
{
  float r[] = {projectionMatrix[0], projectionMatrix[1], projectionMatrix[2],  0,
               projectionMatrix[4], projectionMatrix[5], projectionMatrix[6],  0,
               projectionMatrix[8], projectionMatrix[9], projectionMatrix[10], 0,
               0,                   0,                   0,                    1};
  Matrix44f rot(r);
  shot.Extrinsics.SetRot(rot);
  Point3f tra(projectionMatrix[3],projectionMatrix[7],projectionMatrix[11]);
  shot.Extrinsics.SetTra(tra);
}

void FilterPhotosynthPlugin::readExifData(Image &img, CameraParameters &cam, QDir &root)
{
  const char *FileName = root.filePath("IMG_%1.jpg").arg(img._ID).toStdString().data();
  if (strlen(FileName) >= JHEAD_PATH_MAX-1){
      // Protect against buffer overruns in strcpy / strcat's on filename
      ErrFatal("filename too long");
      qWarning() << "filename too long";
  }

  ResetJpgfile();

  // Start with an empty image information structure.
  memset(&ImageInfo, 0, sizeof(ImageInfo));
  ImageInfo.FlashUsed = -1;
  ImageInfo.MeteringMode = -1;
  ImageInfo.Whitebalance = -1;

  strncpy(ImageInfo.FileName, FileName, PATH_MAX);
  int res = ReadJpegFile(FileName,READ_METADATA);
  if(res)
  {
    img._exifWidth = ImageInfo.Width;
    img._exifHeight = ImageInfo.Height;
    cam._ccdWidth = ImageInfo.CCDWidth;
    cam._focalLength = cam._ccdWidth / cam[CameraParameters::FOCAL_LENGTH];
    cam._pixelSizeMm = cam._ccdWidth / qMax(img._exifWidth,img._exifHeight);
  }
  if(cam._ccdWidth == 0) //could not find ccdWidth in exif
  {
    int resUnit = ImageInfo.ResolutionUnit;
    float xResolution = ImageInfo.xResolution; //image dpi
    float yResolution = ImageInfo.yResolution; //image dpi
    cam._pixelSizeMm = 1 / qMax(xResolution,yResolution);
    switch(resUnit)
    {
    case 2: //inches
      cam._pixelSizeMm *= 25.4;
      break;
    case 3: //centimeters
      cam._pixelSizeMm *= 10;
      break;
    }
    cam._ccdWidth = cam._pixelSizeMm * qMax(img._exifWidth,img._exifHeight);
    cam._focalLength = cam._ccdWidth / cam[CameraParameters::FOCAL_LENGTH];
  }
}

void FilterPhotosynthPlugin::outputToFile(QTextStream &out, Shotf &s, Image &img, CameraParameters &cam)
{
  QString traVec = QString("TranslationVector=\"%1 %2 %3 1\"").arg(s.Extrinsics.Tra().X()).arg(s.Extrinsics.Tra().Y()).arg(s.Extrinsics.Tra().Z());
  QString lensDist("LensDistortion=\"0 0\"");
  QString viewPx = QString("ViewportPixel=\"%1 %2\"").arg(img._exifWidth).arg(img._exifHeight);
  QString pxSize = QString("PixelSizeMm=\"%1 %2\"").arg(cam._pixelSizeMm).arg(cam._pixelSizeMm);
  QString centerPx = QString("CenterPx=\"%1 %2\"").arg(img._exifWidth/2).arg(img._exifHeight/2);
  QString focalMm = QString("FocalMm=\"%1\"").arg(s.Intrinsics.FocalMm);
  out << QString("Camera %1 (Image %2: %3): ").arg(cam._camID).arg(img._ID).arg(img._url) << "\n\n";
  out << "<!DOCTYPE ViewState>\n<project>\n";
  out << " <VCGCamera ";
  out << traVec << " ";
  out << lensDist << " ";
  out << viewPx << " ";
  out << pxSize << " ";
  out << centerPx << " ";
  out << focalMm << " ";
  out << "RotationMatrix=\"";
  unsigned int i, j;
  for(i = 0; i < 4; i++)
    for(j = 0; j < 4; j++)
      out << s.Extrinsics.Rot().ElementAt(i,j) << " ";
  out << "\" />\n";
  out << " <ViewSettings NearPlane=\"0\" TrackScale=\"0.299015\" FarPlane=\"13.0311\"/>\n";
  out << " <Render Lighting=\"0\" DoubleSideLighting=\"0\" SelectedVert=\"0\" ColorMode=\"3\" SelectedFace=\"0\" BackFaceCull=\"0\" FancyLighting=\"0\" DrawMode=\"2\" TextureMode=\"0\"/>\n";
  out << "</project>\n\n\n\n";
}

Q_EXPORT_PLUGIN(FilterPhotosynthPlugin)
