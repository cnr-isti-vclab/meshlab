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
#include <common/GLExtensionsManager.h>
#include <QMessageBox>
#include "mainwindow.h"
#include <QGLFormat>
#include <QString>
#include <clocale>

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif //_WIN32

void handleCriticalError(const MLException& exc);

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

	GLExtensionsManager::init();
	std::unique_ptr<MainWindow> window;
	try {
		window = std::unique_ptr<MainWindow>(new MainWindow());
	}
	catch (const MLException& exc) {
		handleCriticalError(exc);
		return -1;
	}
	window->showMaximized();

	// This event filter is installed to intercept the open events sent directly by the Operative System.
	FileOpenEater *filterObj=new FileOpenEater(window.get());
	app.installEventFilter(filterObj);
	app.processEvents();

	// Can load multiple meshes and projects, and also a camera view
	if(argc>1) {
		QString helpOpt1="-h";
		QString helpOpt2="--help";
		if( (helpOpt1==argv[1]) || (helpOpt2==argv[1]) ) {
			printf(
						"Usage:\n"
						"meshlab <meshfile>\n"
						"Look at http://www.meshlab.net\n"
						"for a longer documentation\n"
						);
			return 0;
		}

		std::vector<QString> cameraViews;
		for (int i = 1; i < argc; ++i) {
			QString arg = QString::fromLocal8Bit(argv[i]);
			if(arg.endsWith("mlp",Qt::CaseInsensitive) || arg.endsWith("mlb",Qt::CaseInsensitive) || arg.endsWith("aln",Qt::CaseInsensitive) || arg.endsWith("out",Qt::CaseInsensitive) || arg.endsWith("nvm",Qt::CaseInsensitive))
				window->openProject(arg);
			else if(arg.endsWith("xml",Qt::CaseInsensitive))
				cameraViews.push_back(arg);
			else
				window->importMeshWithLayerManagement(arg);
		}

		// Load the view after everything else
		if (!cameraViews.empty()) {
			if (cameraViews.size() > 1)
				printf("Multiple views specified. Loading the last one.\n");
			window->readViewFromFile(cameraViews.back());
		}
	}
	//else 	if(filterObj->noEvent) window.open();
	return app.exec();
}

void handleCriticalError(const MLException& exc){

	QString message = "MeshLab was not able to start.\n";
	if (QString::fromStdString(exc.what()).contains("GLEW initialization failed")){
		message.append("Please check your Graphics drivers.\n");
	}
	message.append("\nError: " + QString::fromStdString(exc.what()));

	QMessageBox messageBox(
				QMessageBox::Critical,
				"Critical Error",
				message);
	messageBox.addButton(QMessageBox::Ok);

#ifdef _WIN32
	bool openGLProblem = QString::fromStdString(exc.what()).contains("GLEW initialization failed");
	QCheckBox *cb = nullptr;
	if (openGLProblem) {
		cb = new QCheckBox("Use CPU OpenGL and restart MeshLab");
		messageBox.setCheckBox(cb);
	}
#endif //_WIN32

	messageBox.exec();

#ifdef _WIN32
	if (openGLProblem && cb->isChecked()) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(
					nullptr, "Are you really sure?",
					"By accepting, you are disabling hardware acceleration for MeshLab.\n\n"
					"Before proceeding, you should check the default graphic card you are "
					"using (if a dedicated graphic card is available, be sure you are using "
					"it instead of the integrated one) and if the drivers could be updated. "
					"If none of there options are available, you can proceed disabling "
					"hardware acceleration for MeshLab.\n\n"
					"You can undo this operation by renaming the file 'opengl32.dll' to "
					"'opengl32sw.dll' inside the MeshLab folder.",
					QMessageBox::No|QMessageBox::Yes, QMessageBox::No);
		if (reply == QMessageBox::Yes) {
			//start a new process "UseCPUOpenGL.exe" to copy opengl32.dll
			SHELLEXECUTEINFO sei;

			ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));

			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.lpVerb = _T("runas");
			sei.lpFile = _T("UseCPUOpenGL.exe"); //obviously not the actual file name
			//but it can be substituted and will work just fine in windows 7
			sei.lpParameters = _T("1");

			ShellExecuteEx(&sei);
		}
	}
#endif //_WIN32
}
