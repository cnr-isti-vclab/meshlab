/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
#include <common/mlapplication.h>
#include <QMessageBox>
#include "mainwindow.h"
#include <QGLFormat>
#include <QString>
#include <clocale>

int main(int argc, char *argv[])
{

    MeshLabApplication app(argc, argv);
	std::setlocale(LC_ALL, "C");
    QLocale::setDefault(QLocale::C);
    QCoreApplication::setOrganizationName(MeshLabApplication::organization());
#if QT_VERSION >= 0x050100
    // Enable support for highres images (added in Qt 5.1, but off by default)
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QString tmp = MeshLabApplication::appArchitecturalName(MeshLabApplication::HW_ARCHITECTURE(QSysInfo::WordSize));
    QCoreApplication::setApplicationName(MeshLabApplication::appArchitecturalName(MeshLabApplication::HW_ARCHITECTURE(QSysInfo::WordSize)));

    QGLFormat fmt = QGLFormat::defaultFormat();
    fmt.setAlphaBufferSize(8);
    QGLFormat::setDefaultFormat(fmt);

    MainWindow window;
    window.showMaximized();

    // This event filter is installed to intercept the open events sent directly by the Operative System.
    FileOpenEater *filterObj=new FileOpenEater(&window);
    app.installEventFilter(filterObj);
    app.processEvents();

    if(argc>1)
    {
        QString helpOpt1="-h";
        QString helpOpt2="--help";
        if( (helpOpt1==argv[1]) || (helpOpt2==argv[1]) )
            printf(
            "usage:\n"
            "meshlab <meshfile>\n"
            "Look at http://www.meshlab.net\n"
            "for a longer documentation\n"
            );

        for (int i = 1; i < argc; ++i)
        {
            QString arg = QString::fromLocal8Bit(argv[i]);
            if(arg.endsWith("mlp",Qt::CaseInsensitive) || arg.endsWith("mlb",Qt::CaseInsensitive) || arg.endsWith("aln",Qt::CaseInsensitive) || arg.endsWith("out",Qt::CaseInsensitive) || arg.endsWith("nvm",Qt::CaseInsensitive))
                window.openProject(arg);
            else
                window.importMeshWithLayerManagement(arg);
        }
    }
    //else 	if(filterObj->noEvent) window.open();
    return app.exec();
}
