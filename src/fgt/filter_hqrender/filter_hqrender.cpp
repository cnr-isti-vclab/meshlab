#include <QtGui>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "filter_hqrender.h"

//#define NO_RENDERING

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly
using namespace UtilitiesHQR;

FilterHighQualityRender::FilterHighQualityRender() 
{
  typeList << FP_HIGHQUALITY_RENDER;
	
  foreach(FilterIDType tt , types())
    actionList << new QAction(filterName(tt), this);

  templatesDir = qApp->applicationDirPath();
#if defined(Q_OS_WIN)
  if (templatesDir.dirName() == "debug" || templatesDir.dirName() == "release" || templatesDir.dirName() == "plugins")
	templatesDir.cdUp();
#elif defined(Q_OS_MAC)
  if (templatesDir.dirName() == "MacOS") {
    for (int i = 0; i < 6; ++i) {
	  templatesDir.cdUp();
	  if (templatesDir.exists("render_template"))
	    break;
	}
  }
#endif
  if(!templatesDir.cd("render_template")) {
	qDebug("Error. I was expecting to find the render_template dir. Now i am in dir %s",qPrintable(templatesDir.absolutePath()));
    ;//this->errorMessage = "\"render_template\" folder not found";
  }

  QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
  for(int i=0; i<imageFormats.count(); i++) {
	  imageFormatsSupported << imageFormats.at(i).data();
  }
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
QString FilterHighQualityRender::filterName(FilterIDType filterId) const{
  switch(filterId) {
    case FP_HIGHQUALITY_RENDER :  return QString("Render high quality image");
    default : assert(0); 
  }
  return QString();
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
QString FilterHighQualityRender::filterInfo(FilterIDType filterId) const{
  switch(filterId) {
    case FP_HIGHQUALITY_RENDER :  return QString("Make an high quality image of current mesh on a choosen template scene.");
    default : assert(0); 
  }
  return QString("Unknown Filter");
}

// The FilterClass describes in which generic class of filters it fits. 
// This choice affect the submenu in which each filter will be placed 
// More than a single class can be choosen.
FilterHighQualityRender::FilterClass FilterHighQualityRender::getClass(QAction *a){
  switch(ID(a))
	{
		case FP_HIGHQUALITY_RENDER :  return MeshFilterInterface::Generic;
		default : assert(0); 
	}
	return MeshFilterInterface::Generic;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterHighQualityRender::initParameterSet(QAction *action, MeshModel &m, RichParameterSet & parlst) 
{
	 switch(ID(action))	 {
		case FP_HIGHQUALITY_RENDER :  
		{
			//update the template list
			templates = QStringList();
			foreach(QString subDir, templatesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
				QString temp(templatesDir.absolutePath() + QDir::separator() + subDir + QDir::separator() + subDir + ".rib");
				if(QFile::exists(temp))
					templates << subDir;
			}
			if(templates.isEmpty())
			{
				this->errorMessage = "No template scene has been found in \"render_template\" directory";
				qDebug(qPrintable(this->errorMessage));
			}
			parlst.addParam(new RichEnum("scene",0,templates,"Select scene"));
      //parlst.addParam(new RichInt("frames",0, "Number of frames for animation (0 for no animation)"));			
			parlst.addParam(new RichString("ImageName", "default", "Name of output image"));
			parlst.addParam(new RichEnum("ImageFormat", 0, imageFormatsSupported, "Output image format"));
			parlst.addParam(new RichBool("ShowResult",true,"Show the images created?","If checked a window with the created images will be showed at finish"));
			//parlst.addParam(new RichInt("FormatX", 800, "Format X"));
			//parlst.addParam(new RichInt("FormatY", 600, "Format Y"));
			parlst.addParam(new RichInt("FormatX", 320, "Format X"));
			parlst.addParam(new RichInt("FormatY", 200, "Format Y"));
			parlst.addParam(new RichFloat("PixelAspectRatio", 1.0, "Pixel aspect ratio"));
			parlst.addParam(new RichBool("Autoscale",true,"Auto-scale mesh","Check if the object will be scaled on render scene"));			
			QStringList alignValueList = (QStringList() << "Center" << "Top" << "Bottom");
			parlst.addParam(new RichEnum("AlignX",0,alignValueList,"Align X"));
			parlst.addParam(new RichEnum("AlignY",0,alignValueList,"Align Y"));
			parlst.addParam(new RichEnum("AlignZ",0,alignValueList,"Align Z"));			
			parlst.addParam(new RichBool("SaveScene",false,"Save the files created for rendering?",
				"If checked the scene will be created in the same directory of mesh, else in the temporary system folder and then removed"));
			break;
		}
		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
bool FilterHighQualityRender::applyFilter(QAction *filter, MeshModel &m, RichParameterSet & par, vcg::CallBackPos *cb)
{
	this->cb = cb;
	QTime tt; tt.start(); //time for debuging
	qDebug("Starting apply filter");

	QString templateName = templates.at(par.getEnum("scene"));
	QString templatePath = templatesDir.absolutePath() + QDir::separator() + templateName + QDir::separator() + templateName + ".rib";
    QDir templateDir(templatesDir);
	templateDir.cd(templateName);
	QString templateDirString = getDirFromPath(&templatePath);

	//directory of current mesh
	QString meshDirString = QString(m.fileName.c_str());
	meshDirString = getDirFromPath(&meshDirString);

	//name and format of final image
	QString imageName = par.getString("ImageName");
	int imageFormat = par.getEnum("ImageFormat");

	//creating of destination directory
	bool delRibFiles = !par.getBool("SaveScene");
	//if SaveScene is true create in same mesh directory, otherwise in system temporary directry
	QDir destDir = QDir::temp(); //system temporary directry
	if(!delRibFiles) {
		//destDir = QDir(par.getSaveFileName("SceneName"));
		destDir = QDir(meshDirString);
	}
	//create scene directory
	if(!destDir.cd("scene")) {
		if(destDir.mkdir("scene")) {
			if(!destDir.cd("scene")) {
				this->errorMessage = "Creating scene directory at " + destDir.absolutePath();
				return false;
			}
		}
		else {
			this->errorMessage = "Creating scene directory at " + destDir.absolutePath();
			return false;
		}
	}
	//create a new directory with template name
	QString newDir = templateName;
	int k = 0;
	while(destDir.cd(newDir)) {
		destDir.cdUp();
		newDir = templateName + QString::number(++k); //dir templateName+k
	}
	if(!destDir.mkdir(newDir)) {
		this->errorMessage = "Creating scene directory at " + destDir.absolutePath();
		return false;
	}
	if(!destDir.cd(newDir)) {
		this->errorMessage = "Creating scene directory at " + destDir.absolutePath();
		return false;
	}

	//destination diretory + main file	
	QString destDirString = destDir.absolutePath();

	//TEXTURE: take the list of texture mesh
	QStringList textureList = QStringList();
	for(int i=0; i<m.cm.textures.size(); i++) {
		textureList << QString(m.cm.textures[i].c_str());
	}
	
	QStringList shaderDirs, textureDirs, proceduralDirs, imagesRendered;
	
	//read the template files and create the new scenes files
	qDebug("Starting reading cycle %i",tt.elapsed());
	if(!makeScene(m, &textureList, par, templatePath, destDirString, &shaderDirs, &textureDirs, &proceduralDirs, &imagesRendered))
		return false; //message already set
	qDebug("Cycle ending at %i",tt.elapsed());
	Log(GLLogStream::FILTER,"Successfully created scene");
	
	//check if the final rib file will render any image
	if(imagesRendered.size() == 0) {
		this->errorMessage = "The template hasn't a statement to render any image";
		return false;
	}

	//copy the rest of template (shaders, textures, procedural..)
	copyFiles(&templateDir, &destDir, &textureDirs);
	copyFiles(&templateDir, &destDir, &shaderDirs);
	copyFiles(&templateDir, &destDir, &proceduralDirs);
	qDebug("Copied needed file at %i",tt.elapsed());
	QStringList aqsisEnv = QProcess::systemEnvironment();

	//looking for aqsis installation directory:
	//take the system environment variables
	#if defined(Q_OS_MAC)
 		 QDir macPath("/Applications/Aqsis.app");
		 if(macPath.exists())
		 {
			qDebug("a bit of hope");
			QProcess process;
			aqsisEnv << "AQSISHOME=/Applications/Aqsis.app"; // Add an environment variable
			aqsisEnv.replaceInStrings(QRegExp("^PATH=(.*)", Qt::CaseInsensitive), "PATH=\\1:/Applications/Aqsis.app/Contents/Resources/bin");
		 }
	#endif
	
	QString aqsisDir;
	bool found = false;
	foreach(QString envElem, aqsisEnv) { //looking for AQSISHOME variable
		if(envElem.contains("AQSISHOME")) {
			qDebug("founded environment variable value: %s", qPrintable(envElem));
			aqsisDir = envElem.remove("AQSISHOME="); //the string is "AQSISHOME='path'"
			qDebug("aqsis directory: %s", qPrintable(aqsisDir));
			found = true;
			break;
		}
	}
	if(!found) {
		this->errorMessage = "Aqsis is not installed correctly";
		return false;
	}
#if defined(Q_OS_WIN)
	//if os is win and a dir contains a space, it must be wrapped in quotes (..\"Program files"\..)
	aqsisDir = quotesPath(&aqsisDir);
#endif

	//compile the shaders with current aqsis shader compiler version
	foreach(QString dirStr, shaderDirs) {
		if(destDir.exists(dirStr)) {
			QProcess compileProcess; //compile all shaders together foreach folder
			//set working directory the location of shaders
			compileProcess.setWorkingDirectory(destDirString + QDir::separator() + dirStr);
			compileProcess.setEnvironment(aqsisEnv);
			qDebug("compiling shader working directory: %s",qPrintable(destDirString + QDir::separator() + dirStr));
			QString toRun = aqsisDir + aqsisBinPath() + QDir::separator() + aqslName()+" *.sl";
			qDebug("compiling command: %s",qPrintable(toRun));
			compileProcess.start(toRun);
			if (!compileProcess.waitForFinished(-1)) { //wait the finish of process
				//if there's an arror of compiling the process exits normally!!
				QString out = QString::fromLocal8Bit(compileProcess.readAllStandardError().data());
				this->errorMessage = "Unable to compile the shaders of template" + out;
				qDebug("compiling msg err: %s",qPrintable(out));
				out = QString::fromLocal8Bit(compileProcess.readAllStandardOutput().data());
				qDebug("compiling msg out: %s",qPrintable(out));
				return false;
			}
		}
	}
	qDebug("Compiled shaders at %i",tt.elapsed());
	
	//Copy and convert to tiff format, all mesh textures, in dest dir and convert the to renderman format
	//multi-texture not supported!Copy and convert only the first texture
	if(textureList.count() > 0) {
	//foreach(QString textureName, textureList) {
		QString textureName = textureList.first();

		QFile srcFile(meshDirString + QDir::separator() + textureName);

		//position in the first readable/writable between textures directories
		QString newImageDir = ".";
		foreach(QString dir, textureDirs) {
			if(dir!="." && destDir.cd(dir)) {
				newImageDir = dir;
				destDir.cdUp();
				break;
			}
		}
		qDebug("source texture directory: %s", qPrintable(srcFile.fileName()));
		QString newTex = destDirString + QDir::separator() + newImageDir + QDir::separator() + getFileNameFromPath(&textureName,false);
		qDebug("destination texture directory: %s", qPrintable(newTex + ".tiff"));
		if(srcFile.exists())
		{
			//convert image to tiff format (the one readable in aqsis)
			QImage image;
			image.load(srcFile.fileName());
			image.save(newTex + ".tiff", "Tiff");

			//convert the texture to renderman format
			QProcess convertTextureProcess;
			convertTextureProcess.setEnvironment(aqsisEnv);
			//set working directory the location of texture
			convertTextureProcess.setWorkingDirectory(destDirString + QDir::separator() + newImageDir);
			qDebug("convert texture working directory: %s",qPrintable(destDirString + QDir::separator() + newImageDir));
			QString toRun = aqsisDir + aqsisBinPath() + QDir::separator() + teqserName() + " " + getFileNameFromPath(&textureName,false) +".tiff " + getFileNameFromPath(&textureName,false) + ".tx";
			qDebug("convert command: %s",qPrintable(toRun));
			convertTextureProcess.start(toRun);				
			if (!convertTextureProcess.waitForFinished(-1)) { //wait the finish of process
				QByteArray err = convertTextureProcess.readAllStandardError();
				this->errorMessage = "Is impossible to convert the texture " + textureName + "\n" + QString(err);				
				return false;
			}
		}
		else {
			this->errorMessage = "Not founded the texture file: " + srcFile.fileName();
			return false; //the mesh has a texture not existing
		}
	}
	qDebug("Converted image at %i",tt.elapsed());
	
	//run the aqsis rendering
	renderProcess.setWorkingDirectory(destDirString); //for the shaders/maps reference
	renderProcess.setEnvironment(aqsisEnv);
	QString toRun = aqsisDir + aqsisBinPath() + QDir::separator() + aqsisName()+ " -progress -progressformat=%p "+ mainFileName();
	qDebug("Runnig aqsis command: %s", qPrintable(toRun));
	//every time the render process write a message, receive a signal
	worldBeginRendered = 1; lastCb = 0;
	qDebug("number of world begin found: %i",numOfWorldBegin);
	connect(&renderProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(updateOutputProcess()));
	connect(&renderProcess, SIGNAL(readyReadStandardError()) , this, SLOT(errSgn()));
	cb(0, "Rendering image with Aqsis");
#if !defined(NO_RENDERING)
	renderProcess.start(toRun);
	if (!renderProcess.waitForFinished(-1)) {
		this->errorMessage = "An error occured in Aqsis";
    return false;
	}
#endif
	disconnect(&renderProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(updateOutputProcess()));	
	//if process Aqsis is stopped, no image are created
	qDebug("aqsis process finished");
	
	//the rendering result image is copied in mesh folder (maybe a set of file)
	QString finalImage = meshDirString + QDir::separator() + imageName;
	qDebug("final image position: %s", qPrintable(finalImage));
	QString imageFormatString = imageFormatsSupported.at(imageFormat);	
	int n = numberOfCiphers(imagesRendered.size());//n is the ciphers number of imagesRendered.size()
	for(int i = 0; i < imagesRendered.size(); i++) {
		QString currentImage = destDir.absolutePath() + QDir::separator() + imagesRendered.at(i);
		qDebug("rendering result image position: %s", qPrintable(currentImage));
		QImage image;
		if(!image.load(currentImage)) {
			this->errorMessage = "An error occured with Aqsis or rendered image are be removing";
			return false;
		}
		
		if(imagesRendered.size() == 1) {
			image.save(finalImage + "." + imageFormatString, qPrintable(imageFormatString));
			qDebug("saved image in: %s", qPrintable(finalImage + "." + imageFormatString));
		}
		else {
			//add zero for correct image name
			QString num = QString::number(i);
			int j = numberOfCiphers(i) + 1;
			for(;j <= n; j++)
				num = "0" + num;
			image.save(finalImage + num + "." + imageFormatString, qPrintable(imageFormatString));
			qDebug("saved image in: %s", qPrintable(finalImage + num + "." + imageFormatString));
			//unique file gif?
		}		
	}

	qDebug("end: %i",tt.elapsed());
	cb(100, qPrintable("Created " + QString::number(imagesRendered.size())+" images"));
  Log(GLLogStream::FILTER,"Successfully created high quality image");
	//Log(GLLogStream::FILTER,"Aqsis has rendered image successfully");
	
	//run piqls with rendered image
	if(par.getBool("ShowResult")) {
		QProcess piqslProcess;
		piqslProcess.setWorkingDirectory(destDirString);
		piqslProcess.setEnvironment(aqsisEnv);
		toRun = aqsisDir + aqsisBinPath() + QDir::separator() + piqslName();
		//if there'isnt image, it stops before...
		foreach(QString img, imagesRendered) {
			toRun += " " + img;
		}
		qDebug("Runnig piqsl command: %s", qPrintable(toRun));
		piqslProcess.start(toRun);
		
		piqslProcess.waitForFinished(-1); //no check error...
		piqslProcess.terminate();
	}
	//only now we can delete recursively all created files (if it's required)
	if(delRibFiles) {
		QString dirName = destDir.dirName();
		QDir temp = destDir;
		temp.cdUp();
		delDir(&temp, &dirName);
		delRibFiles = false;
	}
	qDebug("finsish to apply filter at %i",tt.elapsed());

	return true;
}

void FilterHighQualityRender::updateOutputProcess() {
	//a thread for each signal => working with signal disabled
	disconnect(&renderProcess, SIGNAL(readyReadStandardOutput()),this, SLOT(updateOutputProcess()));
	//the format is a number which say the percentage (maybe more that one)
	QString out = QString::fromLocal8Bit(renderProcess.readAllStandardOutput().data());
	//qDebug("aqsis.exe output: %s",qPrintable(out));
	out = QStringList(out.trimmed().split(' ')).last(); //take only the last
	//qDebug("aqsis output taken: %s",qPrintable(out));
	int currentCb = int(out.toFloat());
	if(currentCb < lastCb)
		worldBeginRendered++;
	QString msg = "Rendering image with Aqsis (pass: " +
		QString::number(worldBeginRendered) + "/" + QString::number(numOfWorldBegin) + ")";
	int value = int( (100 * (worldBeginRendered - 1) + currentCb ) / numOfWorldBegin );
	cb(value, qPrintable(msg)); //update progress bar
	//qDebug("cb value: worldBeginRendered %i last %i current %i effective %i" ,worldBeginRendered,lastCb,currentCb,value);
	lastCb = currentCb;
	//restore the signal handling
	connect(&renderProcess, SIGNAL(readyReadStandardOutput()),this, SLOT(updateOutputProcess()));
}

void FilterHighQualityRender::errSgn() {
	QString out = QString::fromLocal8Bit(renderProcess.readAllStandardError().data());
	qDebug("aqsis error signal: %s",qPrintable(out));
}

Q_EXPORT_PLUGIN(FilterHighQualityRender)
