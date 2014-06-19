#include <QString>
#include <QtGlobal>
#include <QString>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QtXml>

#include "meshmodel.h"
#include<QImageReader>
#include "meshlabdocumentbundler.h"

#include <wrap/qt/shot_qt.h>


bool MeshDocumentFromBundler(MeshDocument &md, QString filename_out,QString image_list_filename, QString model_filename)
{
  md.addNewMesh(model_filename,QString("model"));
  std::vector<Shotm> shots;
  const QString path = QFileInfo(filename_out).absolutePath();
  const QString path_im = QFileInfo(image_list_filename).absolutePath()+QString("/");

  std::vector<std::string>   image_filenames;
  vcg::tri::io::ImporterOUT<CMeshO>::Open(md.mm()->cm,shots,image_filenames,qPrintable(filename_out), qPrintable(image_list_filename));
  md.mm()->updateDataMask(MeshModel::MM_VERTCOLOR);

  QString curr_path = QDir::currentPath();
  QFileInfo imi(image_list_filename);

  //
  QStringList image_filenames_q;
  QSize size;
  for(unsigned int i  = 0; i < image_filenames.size(); ++i)
  {
    QImageReader sizeImg(QString::fromStdString(image_filenames[i]));
    if(sizeImg.size()==QSize(-1,-1))
        image_filenames_q.push_back(path_im+QString::fromStdString(image_filenames[i]));
    else
        image_filenames_q.push_back(QString::fromStdString(image_filenames[i]));
  }
  QDir::setCurrent(imi.absoluteDir().absolutePath());

    for(size_t i=0 ; i<shots.size() ; i++){
            md.addNewRaster();
                        const QString fullpath_image_filename = image_filenames_q[i];
            md.rm()->addPlane(new Plane(fullpath_image_filename,Plane::RGBA));
            int count=fullpath_image_filename.count('\\');
            if (count==0)
            {
                count=fullpath_image_filename.count('/');
                md.rm()->setLabel(fullpath_image_filename.section('/',count,1));
            }
            else
                md.rm()->setLabel(fullpath_image_filename.section('\\',count,1));
            md.rm()->shot = shots[i];
    }
        QDir::setCurrent(curr_path);

  return true;
}

bool MeshDocumentFromNvm(MeshDocument &md, QString filename_nvm, QString model_filename)
{
  md.addNewMesh(model_filename,QString("model"));
  std::vector<Shotm> shots;
  const QString path = QFileInfo(filename_nvm).absolutePath();
  //const QString path_im = QFileInfo(image_list_filename).absolutePath()+QString("/");

  std::vector<std::string>   image_filenames;
  vcg::tri::io::ImporterNVM<CMeshO>::Open(md.mm()->cm,shots,image_filenames,qPrintable(filename_nvm));
  md.mm()->updateDataMask(MeshModel::MM_VERTCOLOR);

  QString curr_path = QDir::currentPath();
  //QFileInfo imi(image_list_filename);

  //QDir::setCurrent(imi.absoluteDir().absolutePath());
  QStringList image_filenames_q;
  for(unsigned int i  = 0; i < image_filenames.size(); ++i)
    image_filenames_q.push_back(QString::fromStdString(image_filenames[i]));

    for(size_t i=0 ; i<shots.size() ; i++){
            md.addNewRaster();
                        const QString fullpath_image_filename = image_filenames_q[i];
            md.rm()->addPlane(new Plane(fullpath_image_filename,Plane::RGBA));
            md.rm()->setLabel(image_filenames_q[i].section('/',1,2));
            md.rm()->shot = shots[i];
            /*md.rm()->shot.Intrinsics.ViewportPx[0]=md.rm()->currentPlane->image.width();
            md.rm()->shot.Intrinsics.ViewportPx[1]=md.rm()->currentPlane->image.height();
            md.rm()->shot.Intrinsics.CenterPx[0]=(int)((double)md.rm()->shot.Intrinsics.ViewportPx[0]/2.0f);
            md.rm()->shot.Intrinsics.CenterPx[1]=(int)((double)md.rm()->shot.Intrinsics.ViewportPx[1]/2.0f);*/

    }
        QDir::setCurrent(curr_path);

  return true;
}



