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

//#define FILTER_PHOTOSYNTH_DEBUG 1

using namespace vcg;

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
void FilterPhotosynthPlugin::initParameterSet(QAction *action, MeshModel &/*m*/, RichParameterSet &parlst)
{
  switch(ID(action))
  {
    case FP_IMPORT_PHOTOSYNTH:
      parlst.addParam(new RichString("synthURL",
                                     "http://photosynth.net/view.aspx?cid=e8f476c5-ed00-4626-a86c-31d654e94109",
                                     "Synth URL",
                                     "Paste the synth URL from your browser."));
      parlst.addParam(new RichInt("clusterID", -1, "Cluster ID", "The ID of the cluster to download, type '-1' to download all"));
      parlst.addParam(new RichBool ("saveImages", true, "Download images", "Download images making up the specified synth."));
      parlst.addParam(new RichString("savePath",
                                     "./",
                                     "Save to",
                                     "Enter the path where images will be saved to"));
      parlst.addParam(new RichBool ("addCameraLayer", true, "Show cameras", "Add a layer with points as cameras placeholders"));
      break;
    default:
      assert(0);
  }
}

// The Real Core Function doing the actual mesh processing.
bool FilterPhotosynthPlugin::applyFilter(QAction */*filter*/, MeshDocument &md, RichParameterSet &par, vcg::CallBackPos *cb)
{
	  ImportSettings settings(par.getString("synthURL"), par.getInt("clusterID"));
	  if(par.getBool("saveImages"))
	    settings._imageSavePath = par.getString("savePath");
		SynthData *synthData = new SynthData(settings);
		synthData->downloadSynthInfo(cb);
	  //Hangs on active wait until data are available from the server
    bool clearToProceed = false;
    bool success = synthData->_mutex.tryLock();
    if(success)
    {
      clearToProceed = synthData->_dataReady;
      synthData->_mutex.unlock();
    }
    //while(!synthData->_dataReady)
    while(!clearToProceed)
	  {
	    //allows qt main loop to process the events relative to the response from the server,
	    //triggering the signals that cause the invocation of the slots that process the response
	    //and set the control variable that stops this active wait.
	    //Note that a call to the function usleep() causes an infinite loop, because when the process awakes,
	    //the control remains inside this loop and doesn't reach qt main loop that this way can't process events.
      QApplication::processEvents();
	    cb(synthData->progressInfo(),synthData->_info.toStdString().data());
      success = synthData->_mutex.tryLock();
      if(success)
      {
        clearToProceed = synthData->_dataReady;
        synthData->_mutex.unlock();
      }
	  }
	
	  if(!synthData->isValid())
	  {
	    this->errorMessage = SynthData::errors[synthData->_state];
	    delete synthData;
	    return false;
	  }
	  cb(0,"Finishing import...");
	  QDir imageDir(settings._imageSavePath);
	  imageDir.cd(synthData->_collectionID);
	#ifdef FILTER_PHOTOSYNTH_DEBUG
	  QFile cameraXMLfile(imageDir.filePath("Cam.txt"));
	  bool success = true;
	  if (!cameraXMLfile.open(QIODevice::WriteOnly | QIODevice::Text))
	  {
	    success = false;
	    qWarning() << "Failed to create cam.txt";
	  }
	  QTextStream out(&cameraXMLfile);
	#endif
	  //scan coordinate systems list and add a new layer for each one, containing its points
	  const QList<CoordinateSystem*> *coordinateSystems = synthData->_coordinateSystems;
	  CoordinateSystem *sys;
	  int count = coordinateSystems->count();
	  foreach(sys, *coordinateSystems)
	  {
	    cb((int)(sys->_id / count),"Finishing import...");
	    if(sys->_pointCloud)
	    {
	      MeshModel *mm = md.addNewMesh("","coordsys"); // After Adding a mesh to a MeshDocument the new mesh is the current one
	      Point p;
	      foreach(p, sys->_pointCloud->_points)
	      {
	        tri::Allocator<CMeshO>::AddVertices(mm->cm,1);
	        mm->cm.vert.back().P() = Point3f(p._x,p._z,-p._y);
	        mm->cm.vert.back().C() = Color4b(p._r,p._g,p._b,255);
	      }
	      //we consider cameras only if the user chooses to download images
	      if(par.getBool("saveImages"))
	      {
          MeshModel *mm1 = 0;
	        if(par.getBool("addCameraLayer"))
	          //create a new layer where add points representing cameras to
            mm1 = md.addNewMesh("","cameras");
	        CameraParameters cam;
	        //scan cameras list for this coordinate system and for each one add a raster with a shot matching the camera
	        foreach(cam, sys->_cameraParametersList)
	        {
	          Shotf s;
	          s.Extrinsics.SetRot(cam.getRotation());
	          s.Extrinsics.SetTra(cam.getTranslation());
	          Image img = synthData->_imageMap->value(cam._imageID);
	          s.Intrinsics.FocalMm = cam[CameraParameters::FOCAL_LENGTH] * qMax(img._width,img._height);
	          s.Intrinsics.PixelSizeMm = Point2f(1,1);
	          s.Intrinsics.ViewportPx = Point2i(img._width,img._height);
	          s.Intrinsics.CenterPx = Point2f(img._width/2,img._height/2);
	#ifdef FILTER_PHOTOSYNTH_DEBUG
	          if(success)
	            outputToFile(out, s, img, cam);
	#endif
            //if(par.getBool("addCameraLayer"))
            if(mm1)
	          {
	            //add a point to the cameras layer as a placeholder for the camera
              tri::Allocator<CMeshO>::AddVertices(mm1->cm,1);
              mm1->cm.vert.back().P() = cam.getTranslation();
	          }
	          //add a new raster
	          //the same image can be added several times, one for each coordinate system it appears into
	          //this way the user can choose which point cloud the raster has to align with
	          RasterModel *rm = md.addNewRaster();
	          QString imgName = imageDir.filePath(QString("IMG_%1.jpg").arg(img._ID) );
	          rm->addPlane(new Plane(imgName,QString("")));
	          rm->shot = s;
	          rm->setLabel(QString("IMG_%1_%2.jpg").arg(int(img._ID),3,10,QLatin1Char('0')).arg(sys->_id));
	        }
	      }
	    }
	  }
	#ifdef DEGUG
	  cameraXMLfile.close();
	#endif
	  delete synthData;
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

/*
 * Writes on out the XML representing the camera described by s
 */
void FilterPhotosynthPlugin::outputToFile(QTextStream &out, Shotf &s, Image &img, CameraParameters &cam)
{
  QString traVec = QString("TranslationVector=\"%1 %2 %3 1\"").arg(s.Extrinsics.Tra().X()).arg(s.Extrinsics.Tra().Y()).arg(s.Extrinsics.Tra().Z());
  QString lensDist("LensDistortion=\"0 0\"");
  QString viewPx = QString("ViewportPixel=\"%1 %2\"").arg(img._width).arg(img._height);
  QString pxSize = QString("PixelSizeMm=\"%1 %2\"").arg(cam._pixelSizeMm).arg(cam._pixelSizeMm);
  QString centerPx = QString("CenterPx=\"%1 %2\"").arg(img._width/2).arg(img._height/2);
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
