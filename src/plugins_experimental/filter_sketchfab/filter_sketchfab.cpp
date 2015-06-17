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

using namespace std;
using namespace vcg;
void Uploader();
// Core Function doing the actual mesh processing.
bool FilterSketchFabPlugin::applyFilter( const QString& filterName, MeshDocument& md, EnvWrap& env, vcg::CallBackPos * cb )
{
    if (filterName == "Export to SketchFab")
    {

      qDebug("Export to SketchFab start ");

      // ********** PER GUIDO *************
      // Qui mi aspetto che nell'ambiente ci sia gia' le cose globali (Quelle che uno definisce dalle preferences).

      QString APIToken=env.evalString(SketchFabKeyCode());

      qDebug("APIToken = '%s' ",qPrintable(APIToken));

      if(APIToken.isEmpty())
      {
        this->errorMessage = QString("Please set APIToken");
        return false;
      }
      qDebug("Starting Upload");
      bool ret = this->upload();
      if(!ret){
        qDebug("Upload FAILED");
        return false;
      }

      return true;
    }
      return false;
}

void FilterSketchFabPlugin::uploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
  qDebug("Upload progress %i on %i",int(bytesSent),int(bytesTotal));
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
    multiPart->append(part_parameter("token", "f7252b3a4f784323a7554923b48cccda"));
    multiPart->append(part_parameter("name", "oh bunny bunny"));
    multiPart->append(part_parameter("description", "Test of the api with a simple model"));
    multiPart->append(part_parameter("tags", "bunnyhedron"));
    multiPart->append(part_parameter("private", "1"));
    multiPart->append(part_parameter("isPublished", "0"));
    multiPart->append(part_parameter("source", "meshlab-exporter"));

    // upload file data
    QHttpPart modelPart;
    modelPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    modelPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"modelFile\"; filename=\"./bunny5k_bin.ply\""));
    QFile *file = new QFile("/Users/cignoni/devel/meshlab/src/plugins_experimental/filter_sketchfab/bunny5k_bin.ply");

    file->open(QIODevice::ReadOnly);
    modelPart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(modelPart);

    QUrl url("https://api.sketchfab.com/v2/models");
    QNetworkRequest request(url);

    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.post(request, multiPart);
    multiPart->setParent(reply);
    qDebug() << "Transmitting" << file->size() << "bytes file.";

    QObject::connect(reply, SIGNAL(finished()), this, SLOT(finished()));
    QObject::connect(reply, SIGNAL(uploadProgress(qint64,qint64)),this,SLOT(uploadProgress));
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
    return true;
}
/* *********** PER GUIDO ****************
 * Ovviamente mi aspetto anche che uno debba aggiungere la propria funzione di inizializzazione una volta per tutte come per le altre globali...
 *
 *

void FilterSketchFabPlugin::initGlobalParameterSet(QAction *action, RichParameterSet &parset)
{
  parset.addParam(new RichString(SketchFabKeyCode(),"0000000","SketchFab KeyCode",""));
}
*/


MESHLAB_PLUGIN_NAME_EXPORTER(FilterMeasurePlugin)
