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

#include "synthData.h"
#include "filter_photosynth.h"
#include <QtScript>
#include <unistd.h>
//#include "qmeta/exif.h"
//#include "qmeta/jpeg.h"
//#include "exif.h"
//#include "jpeg.h"
#include "jhead.h"
//#include "myexif.h"

using namespace vcg;

extern ImageInfo_t ImageInfo;

/*
Matrix44f FilterPhotosynthPlugin::getRotation(CameraParameters cam)
{
  qreal a = cam[CameraParameters::ROT_X] * cam[CameraParameters::ROT_X] +
            cam[CameraParameters::ROT_Y] * cam[CameraParameters::ROT_Y] +
            cam[CameraParameters::ROT_Z] * cam[CameraParameters::ROT_Z];
  qreal w2 = 1 - a;
  qreal w = sqrt(w2);
  Quaternion<float> q((float)w,(float)cam[CameraParameters::ROT_X],(float)cam[CameraParameters::ROT_Y],(float)cam[CameraParameters::ROT_Z]);
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
/*
Point3f FilterPhotosynthPlugin::getTranslation(CameraParameters cam)
{
  Point4f translation(cam[CameraParameters::POS_X], cam[CameraParameters::POS_Y], cam[CameraParameters::POS_Z], 1);
  Point4f t = getRotation(cam) * translation;
  return Point3f(-t[0],-t[1],-t[2]);
}
*/

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
      parlst.addParam(new RichBool ("saveImages", false, "Download images", "Download images making up the specified synth."));
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
  SynthData *synthData = SynthData::downloadSynthInfo(url,path);

  //Hangs on active wait until data are available from the server
  while(!synthData->_dataReady)
  {
    int progress = 0;
    switch(synthData->_progress)
    {
      case SynthData::WEB_SERVICE:
        progress = 0;
        break;
      case SynthData::DOWNLOAD_JSON:
        progress = 20;
        break;
      case SynthData::PARSE_JSON:
        progress = 40;
        break;
      case SynthData::DOWNLOAD_BIN:
        progress = 60;
        break;
      case SynthData::LOADING_BIN:
        progress = 80;
        break;
      case SynthData::DOWNLOAD_IMG:
        progress = 100;
    }

    cb(progress,SynthData::progress[synthData->_progress]);
    //allows qt main loop to process the events relative to the response from the server,
    //triggering the signals that cause the invocation of the slots that process the response
    //and set the control variable that stops this active wait.
    //Note that a call to the function usleep() causes an infinite loop, because when the process awakes,
    //the control remains inside this loop and doesn't reach qt main loop that this way can't process events.
    QApplication::processEvents();
  }

  if(!synthData->isValid())
  {
    this->errorMessage = SynthData::errors[synthData->_state];
    return false;
  }

  QDir dir(QDir::currentPath());
  QFile file(dir.filePath("Cam.txt"));
  bool success = true;
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
      success = false;
  QTextStream out(&file);
  const QList<CoordinateSystem*> *coordinateSystems = synthData->_coordinateSystems;
  CoordinateSystem *sys;
  foreach(sys, *coordinateSystems)
  {
    if(sys->_pointCloud)
    {
      MeshModel *mm = md.addNewMesh("coordsys"); // After Adding a mesh to a MeshDocument the new mesh is the current one
      Point p;
      foreach(p, sys->_pointCloud->_points)
      {
        tri::Allocator<CMeshO>::AddVertices(mm->cm,1);
        //mm->cm.vert.back().P() = Point3f(p._x,p._z,p._y);
        //mm->cm.vert.back().P() = Point3f(p._x,p._z,-p._y);
        //mm->cm.vert.back().P() = Point3f(p._y,p._z,p._x);// <---
        mm->cm.vert.back().P() = Point3f(p._x,p._y,p._z);
        mm->cm.vert.back().C() = Color4b(p._r,p._g,p._b,255);
      }

    }
    MeshModel *mm = md.addNewMesh("cameras");
    CameraParameters cam;
    foreach(cam, sys->_cameraParametersList)
    {

      qreal a = cam[CameraParameters::ROT_X] * cam[CameraParameters::ROT_X] +
                cam[CameraParameters::ROT_Y] * cam[CameraParameters::ROT_Y] +
                cam[CameraParameters::ROT_Z] * cam[CameraParameters::ROT_Z];
      qreal w2 = 1 - a;
      qreal w = sqrt(w2);
      Quaternion<float> q((float)w,(float)cam[CameraParameters::ROT_X],(float)cam[CameraParameters::ROT_Y],(float)cam[CameraParameters::ROT_Z]);
      //Quaternion<float> q((float)w,(float)cam[CameraParameters::ROT_Y],(float)cam[CameraParameters::ROT_Z],(float)cam[CameraParameters::ROT_X]);
      Matrix44f rot;
      q.ToMatrix(rot);
      //permutation matrix;
      const float _p[] = {0,1,0,0, 0,0,1,0, 1,0,0,0, 0,0,0,1};
      Matrix44f p((float *)(&_p[0]));
      Matrix44f permRot = p * rot;

      Matrix44f flip;
      flip.SetRotateDeg(180,Point3f(1,0,0));
      // (rot * flip)^T = flip^T * rot^T
      Matrix44f transposedFlippedRot = flip.transpose() * rot.transpose();

      Shotf s;
      //s.Extrinsics.SetRot(rot);
      s.Extrinsics.SetRot(transposedFlippedRot);

      s.Extrinsics.SetTra(Point3f(-cam[CameraParameters::POS_X], -cam[CameraParameters::POS_Y], -cam[CameraParameters::POS_Z]));
      //s.Extrinsics.SetTra(getTranslation(cam));
      //s.Extrinsics.SetTra(Point3f(cam[CameraParameters::POS_Y], cam[CameraParameters::POS_Z], cam[CameraParameters::POS_X]));


      //MeshModel *mm = md.addNewMesh("camera");
      tri::Allocator<CMeshO>::AddVertices(mm->cm,1);
      //mm->cm.vert.back().P() = getTranslation(cam);
      mm->cm.vert.back().P() = Point3f(cam[CameraParameters::POS_X], cam[CameraParameters::POS_Y], cam[CameraParameters::POS_Z]);


      //qreal focal = cam[CameraParameters::FOCAL_LENGTH];
      //s.Intrinsics.FocalMm = focal * 10;
      //s.Intrinsics.PixelSizeMm = Point2f(0.1,0.1);
      Image img = synthData->_imageMap->value(cam._imageID);
      QDir imageDir(path);
      imageDir.cd(synthData->_collectionID);
      //qmeta::Jpeg jpeg(imageDir.filePath("IMG_%1.jpg").arg(img._ID));
      int width = 0;
      int height = 0;
      float focalLength = 0;
      float ccdWidth = 0;
      float pixelSizeMm = 0;
      const char *FileName = imageDir.filePath("IMG_%1.jpg").arg(img._ID).toStdString().data();
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
          //width = jpeg.exif()->Value(qmeta::Exif::kPixelXDimension).ToUInt();
          //height = jpeg.exif()->Value(qmeta::Exif::kPixelYDimension).ToUInt();
        width = ImageInfo.Width;
        height = ImageInfo.Height;
        focalLength = ImageInfo.FocalLength;
        ccdWidth = ImageInfo.CCDWidth;
        pixelSizeMm = ccdWidth / qMax(width,height);
        qDebug() << "Width:" << width;
        qDebug() << "Heigth:" << height;
        qDebug() << "CCDwidth / focalLength = " << ccdWidth / focalLength << " Photosynth value = " << cam[CameraParameters::FOCAL_LENGTH];
      }
      s.Intrinsics.FocalMm = focalLength;
      s.Intrinsics.PixelSizeMm = Point2f(pixelSizeMm,pixelSizeMm);
      s.Intrinsics.ViewportPx = Point2i(width,height);
      s.Intrinsics.CenterPx = Point2f(width/2,height/2);
      if(success)
      {
        QString traVec = QString("TranslationVector=\"%1 %2 %3 1\"").arg(s.Extrinsics.Tra().X()).arg(s.Extrinsics.Tra().Y()).arg(s.Extrinsics.Tra().Z());
        QString lensDist("LensDistortion=\"0 0\"");
        QString viewPx = QString("ViewportPixel=\"%1 %2\"").arg(width).arg(height);
        QString pxSize = QString("PixelSizeMm=\"%1 %2\"").arg(pixelSizeMm).arg(pixelSizeMm);
        QString centerPx = QString("CenterPx=\"%1 %2\"").arg(width/2).arg(height/2);
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
            //out << rot.ElementAt(i,j) << " ";
            out << transposedFlippedRot.ElementAt(i,j) << " ";
        out << "\" ";
        out << "PhotosynthQuaternion=\"" << cam[CameraParameters::ROT_X] << " " << cam[CameraParameters::ROT_Y] << " " << cam[CameraParameters::ROT_Z] << "\" ";
        out << "w=\"" << w << "\" />\n";
        out << " <ViewSettings NearPlane=\"0\" TrackScale=\"0.299015\" FarPlane=\"13.0311\"/>\n";
        out << " <Render Lighting=\"0\" DoubleSideLighting=\"0\" SelectedVert=\"0\" ColorMode=\"3\" SelectedFace=\"0\" BackFaceCull=\"0\" FancyLighting=\"0\" DrawMode=\"2\" TextureMode=\"0\"/>\n";
        out << "</project>\n\n\n\n";
      }
    }
  }
  file.close();
  if(par.getBool("saveImages"))
  {
    QDir dir(path);
    dir.cd(synthData->_collectionID);
    QHashIterator<int, Image> it(*(synthData->_imageMap));
    QString rasterBase("IMG_%1.jpg");
    while (it.hasNext())
    {
      it.next();
      md.addNewRaster(dir.filePath(rasterBase.arg(it.value()._ID)).toStdString().data());
    }
  }

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

Q_EXPORT_PLUGIN(FilterPhotosynthPlugin)
