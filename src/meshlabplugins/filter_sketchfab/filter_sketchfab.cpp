/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2013                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
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

#include <iostream>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QHttpMultiPart>
#include <QProgressDialog>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QScriptEngine>

#include "filter_sketchfab.h"
#include "miniz.c"
#include <wrap/io_trimesh/export_ply.h>
using namespace std;
using namespace vcg;
void Uploader();

int saveMeshZip(string fileName, string internalName, string zipName) {
    qDebug("Trying to add %s to %s", fileName.c_str(), zipName.c_str());
    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof (zip_archive)); //Saving memory for the zip archive
    if (!mz_zip_writer_init_file(&zip_archive, zipName.c_str(), 65537)) {
        qDebug("Failed creating zip archive");
        mz_zip_writer_end(&zip_archive);
        return 0;
    }

    const char *pTestComment = "test comment";
    
    //MZ_BEST_COMPRESSION = 9
    if (!mz_zip_writer_add_file(&zip_archive, internalName.c_str(), fileName.c_str(), pTestComment, strlen(pTestComment), MZ_UBER_COMPRESSION)) {
        qDebug("failed adding %s to %s", fileName.c_str(), zipName.c_str());
        mz_zip_writer_end(&zip_archive);
        return 0;
    }
    
    mz_zip_writer_finalize_archive(&zip_archive);
    qDebug("Compressed %i",zip_archive.m_archive_size);
    return 1;
}


// Core Function doing the actual mesh processing.
bool FilterSketchFabPlugin::applyFilter( const QString& filterName, MeshDocument& md, EnvWrap& env, vcg::CallBackPos * cb )
{
    if (filterName == "Export to Sketchfab")
    {
      qDebug("Export to SketchFab start ");
      this->fcb=cb;
      QString APIToken=env.evalString(Env::convertToAMLScriptValidName("MeshLab::Plugins::sketchFabKeyCode"));
      this->fcb(1,"Compressing Mesh");
      qDebug("APIToken = '%s' ",qPrintable(APIToken));
      Matrix44m rot; rot.SetRotateDeg(-90,Point3m(1,0,0));
      Matrix44m rotI; rot.SetRotateDeg(90,Point3m(1,0,0));
      
      if(APIToken.isEmpty() || APIToken=="0000000")
      {
        this->errorMessage = QString("Please set in the MeshLab preferences your private API Token string that you can find on the<a href=\"https://sketchfab.com/settings/password\">Sketchfab Password Settings.");
        return false;
      }
      this->apiToken = APIToken;
      this->name = env.evalString("title");
      this->description = env.evalString("description");
      this->tags = env.evalString("tags");
      this->isPrivate = (env.evalBool("isPrivate"))?"1":"0";
      this->isPublished = (env.evalBool("isPublished"))?"1":"0";
      QString tmpObjFileName = QDir::tempPath() + "/xxxx.ply";
      QString tmpZipFileName = QDir::tempPath() + "/xxxx.zip";
      
      int mask=0;
      if(md.mm()->hasDataMask(MeshModel::MM_VERTCOLOR)) mask+=tri::io::Mask::IOM_VERTCOLOR;
      tri::UpdatePosition<CMeshO>::Matrix(md.mm()->cm,rot);
      vcg::tri::io::ExporterPLY<CMeshO>::Save(md.mm()->cm,qPrintable(tmpObjFileName),mask,true);
      tri::UpdatePosition<CMeshO>::Matrix(md.mm()->cm,rotI);
      qDebug("Saved %20s",qPrintable(tmpObjFileName));
      qDebug("Compressed %20s",qPrintable(tmpZipFileName));
      saveMeshZip(qPrintable(tmpObjFileName),"xxxx.ply",qPrintable(tmpZipFileName));
      this->zipFileName = tmpZipFileName;
      
      qDebug("Model Title %s %s %s\n",qPrintable(this->name),qPrintable(this->description),qPrintable(this->tags));
      qDebug("Starting Upload");
      this->fcb(10,"Starting Upload");
      bool ret = this->upload();
      if(!ret){
        qDebug("Upload FAILED");
        return false;
      }
      
      this->Log("Upload Completed; you can access the uploaded model at the following URL:\n"); 
      this->Log("<a href=\"%s\">%s</a>\n",qPrintable(this->sketchfabModelUrl),qPrintable(this->sketchfabModelUrl));
      return true;
    }
      return false;
}

void FilterSketchFabPlugin::uploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
  qDebug("Upload progress %i on %i",int(bytesSent),int(bytesTotal));
  char buf[1024]; sprintf(buf,"Upload progress %i on %i",int(bytesSent),int(bytesTotal));
  if(bytesTotal) this->fcb(100*int(bytesSent)/int(bytesTotal),buf);
}

void FilterSketchFabPlugin::finished()
{
  qDebug("FilterSketchFabPlugin::finished()");
  
  uploadCompleteFlag = true;
}

QHttpPart part_parameter(QString key, QString value) {
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"" + key + "\""));
    part.setBody(value.toLatin1());
    return part;
}



bool FilterSketchFabPlugin::upload()
{
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // upload parameters data
    multiPart->append(part_parameter("token", this->apiToken));
    multiPart->append(part_parameter("name", this->name));
    multiPart->append(part_parameter("description", this->description));
    multiPart->append(part_parameter("tags", this->tags));
    multiPart->append(part_parameter("private", this->isPrivate));
    multiPart->append(part_parameter("isPublished", this->isPublished));
    multiPart->append(part_parameter("source", "meshlab-exporter"));

    // upload file data
    QHttpPart modelPart;
    modelPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"modelFile\"; filename=\""+this->zipFileName+"\""));
    modelPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    QFile *zipfileToUpload = new QFile(this->zipFileName);

    zipfileToUpload->open(QIODevice::ReadOnly);
    modelPart.setBodyDevice(zipfileToUpload);
    zipfileToUpload->setParent(multiPart);
    multiPart->append(modelPart);

    QUrl url("https://api.sketchfab.com/v2/models");
    QNetworkRequest request(url);

    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.post(request, multiPart);
    multiPart->setParent(reply);
    qDebug() << "Transmitting" << zipfileToUpload->size() << "bytes file.";

    QObject::connect(reply, SIGNAL(finished()), this, SLOT(finished()));
    QObject::connect(reply, SIGNAL(uploadProgress(qint64,qint64)),this,SLOT(uploadProgress(qint64,qint64)));
    uploadCompleteFlag=false;
    while(!uploadCompleteFlag)
    {
      QApplication::processEvents();
    }
    qDebug("got it!");
    // test for network error
    QNetworkReply::NetworkError err = reply->error();
    if (err != QNetworkReply::NoError) {
        qDebug() << reply->errorString();
        return false;
    }

    // get the api anwser
    QByteArray result = reply->readAll();
    QScriptValue sc;
    QScriptEngine engine;
    qDebug() << "Result:" << result;
    sc = engine.evaluate("(" + QString(result) + ")");
    QString uid = sc.property("uid").toString();
    if(uid.isEmpty())         return false;
    qDebug() << "Model uploaded with id" << uid;
    this->sketchfabModelUrl="https://sketchfab.com/models/"+uid;
    return true;
}
/* *********** PER GUIDO ****************
 * Ovviamente mi aspetto anche che uno debba aggiungere la propria funzione di inizializzazione una volta per tutte come per le altre globali...
 *

void FilterSketchFabPlugin::initGlobalParameterSet(QAction *action, RichParameterSet &parset)
{
  parset.addParam(new RichString(SketchFabKeyCode(),"0000000","SketchFab KeyCode",""));
}
*/


MESHLAB_PLUGIN_NAME_EXPORTER(FilterMeasurePlugin)
