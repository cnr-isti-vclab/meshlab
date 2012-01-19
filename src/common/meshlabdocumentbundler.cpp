#include <QString>
#include <QtGlobal>
#include <QString>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QtXml>

#include "meshmodel.h"
#include "meshlabdocumentbundler.h"
#include <wrap/io_trimesh/import_out.h>

#include <wrap/qt/shot_qt.h>


bool MeshDocumentFromBundler(MeshDocument &md, QString filename_out,QString image_list_filename, QString model_filename)
{
    
	md.addNewMesh(model_filename,QString("model"));
	std::vector<vcg::Shotf> shots;
	const QString path = QFileInfo(filename_out).absolutePath();
        const QString path_im = QFileInfo(image_list_filename).absolutePath()+QString("/");

	std::vector<std::string>   image_filenames;
        vcg::tri::io::ImporterOUT<CMeshO>::Open(md.mm()->cm,shots,image_filenames,qPrintable(filename_out), qPrintable(image_list_filename),qPrintable(path_im));

        QString curr_path = QDir::currentPath();
        QFileInfo imi(image_list_filename);

        QDir::setCurrent(imi.absoluteDir().absolutePath());
	QStringList image_filenames_q;
	for(unsigned int i  = 0; i < image_filenames.size(); ++i)
                image_filenames_q.push_back(QString::fromStdString(image_filenames[i]));

	for(int i=0 ; i<shots.size() ; i++){
			md.addNewRaster();
                        const QString fullpath_image_filename = path_im + image_filenames_q[i];
			md.rm()->addPlane(new Plane(md.rm(),fullpath_image_filename,QString("")));
			md.rm()->setLabel(image_filenames_q[i].section('/',1,2));
			md.rm()->shot = shots[i];
	}
        QDir::setCurrent(curr_path);

    return true;
}



