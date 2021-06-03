#include <QString>
#include <QtGlobal>
#include <QString>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QtXml>

#include "ml_document/mesh_document.h"
#include<QImageReader>
#include "meshlabdocumentbundler.h"

#include <wrap/qt/shot_qt.h>

#include <wrap/io_trimesh/import_out.h>
#include <wrap/io_trimesh/import_nvm.h>

bool MeshDocumentFromNvm(MeshDocument &md, QString filename_nvm, QString model_filename)
{
    md.addNewMesh(model_filename,QString("model"));
    std::vector<Shotm> shots;
    const QString path = QFileInfo(filename_nvm).absolutePath();
    //const QString path_im = QFileInfo(image_list_filename).absolutePath()+QString("/");

    std::vector<std::string>   image_filenames;
    vcg::tri::io::ImporterNVM<CMeshO>::Open(md.mm()->cm,shots,image_filenames, qUtf8Printable(filename_nvm));
    md.mm()->updateDataMask(MeshModel::MM_VERTCOLOR);

    QString curr_path = QDir::currentPath();
    //QFileInfo imi(image_list_filename);

    //QDir::setCurrent(imi.absoluteDir().absolutePath());
    QStringList image_filenames_q;
    for(size_t i  = 0; i < image_filenames.size(); ++i)
        image_filenames_q.push_back(QString::fromStdString(image_filenames[int(i)]));

    for(size_t i=0 ; i<shots.size() ; i++){
        md.addNewRaster();
        const QString fullpath_image_filename = image_filenames_q[int(i)];
        md.rm()->addPlane(new RasterPlane(fullpath_image_filename,RasterPlane::RGBA));
        md.rm()->setLabel(image_filenames_q[int(i)].section('/',1,2));
        md.rm()->shot = shots[int(i)];
        /*md.rm()->shot.Intrinsics.ViewportPx[0]=md.rm()->currentPlane->image.width();
        md.rm()->shot.Intrinsics.ViewportPx[1]=md.rm()->currentPlane->image.height();
        md.rm()->shot.Intrinsics.CenterPx[0]=(int)((double)md.rm()->shot.Intrinsics.ViewportPx[0]/2.0f);
        md.rm()->shot.Intrinsics.CenterPx[1]=(int)((double)md.rm()->shot.Intrinsics.ViewportPx[1]/2.0f);*/

    }
    QDir::setCurrent(curr_path);

    return true;
}
