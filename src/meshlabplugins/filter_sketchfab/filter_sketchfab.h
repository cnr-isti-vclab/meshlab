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
#ifndef FILTER_SKETCHFAB_H
#define FILTER_SKETCHFAB_H

#include <common/interfaces.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class FilterSketchFabPlugin : public MeshLabFilterInterface
{
    Q_OBJECT
    MESHLAB_PLUGIN_IID_EXPORTER(MESHLAB_FILTER_INTERFACE_IID)
    Q_INTERFACES(MeshLabFilterInterface)

public:
    bool applyFilter( const QString& filterName,MeshDocument& md,EnvWrap& env, vcg::CallBackPos * cb );
    bool upload();
    bool uploadCompleteFlag;
public slots:
    void finished();
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

private:
    QString apiToken;
    QString description;
    QString name;
    QString tags;
    QString zipFileName;
    QString sketchfabModelUrl;
    QString isPrivate;
    QString isPublished;
    vcg::CallBackPos * fcb;
};

#endif
