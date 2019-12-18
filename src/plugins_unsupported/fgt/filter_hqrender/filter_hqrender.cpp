#include <QtGui>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "filter_hqrender.h"
#include <common/pluginmanager.h>

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

  templatesDir = PluginManager::getBaseDirPath();

  if(!templatesDir.cd("render_template")) {
	  qDebug("Error. I was expecting to find the render_template dir. Now i am in dir %s",qPrintable(templatesDir.absolutePath()));
    ;//this->errorMessage = "\"render_template\" folder not found";
  }

  //create the list of image formats supported by QT
  QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
  for(int i=0; i<imageFormats.count(); i++) {
	  imageFormatsSupported << imageFormats.at(i).data();
  }

  //set the system dipendent file name for Aqsis
  setAqsisFileName();
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

//add global parameter to set aqsis bin path correctly
void FilterHighQualityRender::initGlobalParameterSet(QAction */*action*/, RichParameterSet &parset) {
  //on windows i could use PATH system environment to find directory..on mac os?
  QStringList aqsisEnv = QProcess::systemEnvironment();
  QString aqsisDir = defaultAqsisBinPath();
	bool found = false;
	foreach(QString envElem, aqsisEnv) { //looking for (AQSISHOME|PATH) variable
		if(envElem.contains("AQSISHOME", Qt::CaseInsensitive)) { //old version of aqsis? (1.4)
			qDebug("founded environment variable value: %s", qPrintable(envElem));
			aqsisDir = envElem.remove("AQSISHOME=", Qt::CaseInsensitive); //the string is "AQSISHOME='path'"
			qDebug("aqsis directory: %s", qPrintable(aqsisDir));
			found = true;
			break;
		}
		if(envElem.startsWith("PATH=", Qt::CaseInsensitive)) { //aqsis 1.6
			qDebug("founded environment variable value: %s", qPrintable(envElem));
			QStringList pathlist = envElem.remove("PATH=", Qt::CaseInsensitive).split(';');
			foreach(QString str, pathlist) {
				if(str.contains("aqsis", Qt::CaseInsensitive)) { //is it enough?
					qDebug("founded environment variable value: %s", qPrintable(str));
					aqsisDir = str;
					qDebug("aqsis directory: %s", qPrintable(aqsisDir));
					found = true;
					break;
				}
			}
			if(found) break;
		}
	}
  assert(!parset.hasParameter(AqsisBinPathParam()));
	parset.addParam(new RichString(AqsisBinPathParam(), aqsisDir, "", ""));
  //parset.addParam(new RichString(AqsisBinPathParam(), defaultAqsisBinPath(), "", "")); //no search, use default
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterHighQualityRender::initParameterSet(QAction *action, MeshModel &/*m*/, RichParameterSet & parlst)
{
	 switch(ID(action))	 {
		case FP_HIGHQUALITY_RENDER :  
		{
			//update the template list
			templates = QStringList();
			foreach(QString subDir, templatesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        //foreach directory, search a file with the same name
				QString temp(templatesDir.absolutePath() + QDir::separator() + subDir + QDir::separator() + subDir + ".rib");
				if(QFile::exists(temp))
					templates << subDir;
			}
			if(templates.isEmpty())
			{
				this->errorMessage = "No template scene has been found in \"render_template\" directory";
        qDebug("%s",qPrintable(this->errorMessage));
			}
			parlst.addParam(new RichEnum("scene",0,templates,"Select scene",
        "Select the scene where the loaded mesh will be drawed in."));			
			parlst.addParam(new RichString("ImageName", "default", "Name of output image",
        "The name of final result. If the images is more than one will be numbered."));
			parlst.addParam(new RichEnum("ImageFormat", 0, imageFormatsSupported, "Output image format",
        "The format of the final image (or images, if it's more than one)"));
			parlst.addParam(new RichBool("ShowResult",true,"Show the images created?",
        "If checked a window, with the created images, will be showed at finish."));
			parlst.addParam(new RichInt("FormatX", 640, "Format X", "Final image/s lenght size."));
			parlst.addParam(new RichInt("FormatY", 480, "Format Y", "Final image/s width size."));
			parlst.addParam(new RichFloat("PixelAspectRatio", 1.0, "Pixel aspect ratio", "Final image/s pixel aspect ratio."));
			parlst.addParam(new RichBool("Autoscale",true,"Auto-scale mesh","Check if the mesh will be scaled on render scene"));			
			QStringList alignValueList = (QStringList() << "Center" << "Top" << "Bottom");
			parlst.addParam(new RichEnum("AlignX",0,alignValueList,"Align X",
        "If it's checked the mesh will be aligned with scene x axis"));
			parlst.addParam(new RichEnum("AlignY",0,alignValueList,"Align Y",
			  "If it's checked the mesh will be aligned with scene y axis"));
      parlst.addParam(new RichEnum("AlignZ",0,alignValueList,"Align Z",			
			  "If it's checked the mesh will be aligned with scene z axis"));
      parlst.addParam(new RichBool("SaveScene",false,"Save the files created for rendering?",
				"If it's checked the scene will be created in the same directory of mesh, else in the temporary system folder and then removed"));
			break;
		}
		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
bool FilterHighQualityRender::applyFilter(QAction */*filter*/, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
  //***check if the AqsisBinPathParam() parameter it's correct***
  QString aqsisBinDirString = par.getString(AqsisBinPathParam());
  QDir aqsisBinDir = QDir(aqsisBinDirString);
  qDebug("Presumed Aqsis directory: %s", qPrintable(aqsisBinDir.absolutePath()));
  if(!aqsisBinDir.exists()) {
    this->errorMessage = "Please, set correct Aqsis directory in Meshlab options";
    return false;
  }
  foreach(QString file, aqsisFileName) {
    qDebug("Check if the file %s exist", qPrintable(aqsisBinDir.filePath(file)));
    if(!aqsisBinDir.exists(file)) {
      this->errorMessage = QString("Verify if Aqsis it's installed correctly. Not found file: ") + aqsisBinDir.filePath(file);
      return false;
    }
  }
  #if defined(Q_OS_WIN)
	//if OS is WIN and a dir contains a space, it must be wrapped in quotes (..\"Program files"\..)
	aqsisBinDirString = quotesPath(&aqsisBinDirString);
  #endif
  //aqsis it's installed correctly!
  qDebug("Aqsis it's installed correctly!");

  //***get the filter parameters and create destination directory***
	MeshModel* m = md.mm();
	this->cb = cb;
	QTime tt; tt.start(); //time for debuging
	qDebug("Starting apply filter");

  if(templates.isEmpty()) //there aren't any template
	{
	  this->errorMessage = "No template scene has been found in \"render_template\" directory";
		return false;
	}
	QString templateName = templates.at(par.getEnum("scene")); //name of selected template
	QFileInfo templateFile = QFileInfo(templatesDir.absolutePath() + QDir::separator() + templateName + QDir::separator() + templateName + ".rib");
  
	//directory of current mesh
	QString meshDirString = m->pathName();
	
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
	//create scene directory if don't exists
	if(!destDir.cd("scene")) {
    if(!destDir.mkdir("scene") || !destDir.cd("scene")) {
			this->errorMessage = "Creating scene directory at " + destDir.absolutePath();
			return false;
		}		
	}
	//create a new directory with template name
	QString newDir = templateName + " - " + QFileInfo(m->fullName()).completeBaseName();
	int k = 0;
	while(destDir.cd(newDir)) {
		destDir.cdUp();
		newDir = templateName + " - " + QFileInfo(m->fullName()).completeBaseName() + "(" + QString::number(++k) + ")"; //dir templateName+k
	}
	if(!destDir.mkdir(newDir) || !destDir.cd(newDir)) {
		this->errorMessage = "Creating scene directory at " + destDir.absolutePath();
		return false;
	}

	//destination diretory
	QString destDirString = destDir.absolutePath();

	//***Texture: take the list of texture mesh
	QStringList textureListPath = QStringList();
  for(size_t i=0; i<m->cm.textures.size(); i++) {
    QString path = QString(m->cm.textures[i].c_str());
		textureListPath << path;
	}
	
	//***read the template files and create the new scenes files
	QStringList shaderDirs, textureDirs, proceduralDirs, imagesRendered;
	qDebug("Starting reading cycle %i",tt.elapsed());
  if(!makeScene(m, &textureListPath, par, &templateFile, destDirString, &shaderDirs, &textureDirs, &proceduralDirs, &imagesRendered))
		return false; //message already set
	qDebug("Cycle ending at %i",tt.elapsed());
	Log(GLLogStream::FILTER,"Successfully created scene");

	//check if the final rib file will render any image
	if(imagesRendered.size() == 0) {
		this->errorMessage = "The template description hasn't a statement to render any image";
		return false;
	}

	//***copy the rest of template files (shaders, textures, procedural..)
  copyFiles(templateFile.dir(), destDir, textureDirs);
  copyFiles(templateFile.dir(), destDir, shaderDirs);
  copyFiles(templateFile.dir(), destDir, proceduralDirs);
	qDebug("Copied needed file at %i",tt.elapsed());
	
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	//looking for aqsis installation directory:
	//take the system environment variables
  #if defined(Q_OS_MAC)
     if(aqsisBinDir.exists())
		 {
			qDebug("a bit of hope");
      env.insert("AQSISHOME", "/Applications/Aqsis.app/"); // Add an environment variable
      //env.insert("PATH", env.value("Path") + ":"+aqsisBinDir.absolutePath());
      env.insert("PATH", "/Applications/Aqsis.app/Contents/Resources/bin:"+env.value("PATH"));
		 }
  #endif

	//***compile the shaders with current aqsis shader compiler version
	foreach(QString dirStr, shaderDirs) {
		if(destDir.exists(dirStr)) {
			QProcess compileProcess; //compile all shaders together foreach folder
			//set working directory the location of shaders
			compileProcess.setWorkingDirectory(destDirString + QDir::separator() + dirStr);
      compileProcess.setProcessEnvironment(env);
			qDebug("compiling shader working directory: %s",qPrintable(destDirString + QDir::separator() + dirStr));
      foreach(QString shaderName, QDir(compileProcess.workingDirectory()).entryList(QStringList("*.sl")))
        {
          QString toRun = aqsisBinDirString +"/" + aqsisFileName[AQSL]+ " "+shaderName;
          qDebug("compiling command: %s",qPrintable(toRun));
          qDebug("Path is %s",qPrintable(compileProcess.processEnvironment().value("PATH")));
          compileProcess.start(toRun);
          if (!compileProcess.waitForFinished(-1)) { //wait the finish of process
            //if there's an arror of compiling the process exits normally!!
            QString out = QString::fromLocal8Bit(compileProcess.readAllStandardError().data());
            this->errorMessage = "Unable to compile the shaders of template" + out;
            qDebug("compiling msg err: %s",qPrintable(out));
            QProcess::ProcessError errr= compileProcess.error();
            if(errr==QProcess::FailedToStart) qDebug("QProcess::FailedToStart");
            if(errr==QProcess::ReadError    ) qDebug("QProcess::ReadError");
            if(errr==QProcess::FailedToStart) qDebug("QProcess::FailedToStart");
            return false;
          }
          qDebug("compiling msg out: %s",compileProcess.readAllStandardOutput().data());
			}
				
		}
	}
	qDebug("Compiled shaders at %i",tt.elapsed());
	
	//***all mesh textures are copied in a textures directory and are converted to tiff format, then to renderman format
	if(textureListPath.count() > 0) {
	//(IMPORTANT: multi-texture not supported!The plugin copies and converts only the first texture)
		QString textureName = QFileInfo(textureListPath.first()).completeBaseName(); //just texture name
    QFile srcFile(meshDirString + QDir::separator() + textureListPath.first());

		//destination directory it's the first readable/writable between textures directories
		QString newImageDir = ".";
		foreach(QString dir, textureDirs) {
			if(dir!="." && destDir.cd(dir)) {
				newImageDir = dir;
				destDir.cdUp();
				break;
			}
		}
		qDebug("source texture directory: %s", qPrintable(srcFile.fileName()));
		QString newTex = destDirString + QDir::separator() + newImageDir + QDir::separator() + textureName;
		qDebug("destination texture directory: %s", qPrintable(newTex + ".tiff"));
		if(srcFile.exists()) {
			//convert image to tiff format (the one readable in aqsis)
			QImage image;
			image.load(srcFile.fileName());
			image.save(newTex + ".tiff", "Tiff");

			//convert the texture to renderman format
			QProcess convertTextureProcess;
			//convertTextureProcess.setEnvironment(aqsisEnv);
			//set working directory to the location of texture
			convertTextureProcess.setWorkingDirectory(destDirString + QDir::separator() + newImageDir);
			qDebug("convert texture working directory: %s",qPrintable(destDirString + QDir::separator() + newImageDir));
      QString toRun = aqsisBinDirString +"/" + aqsisFileName[TEQSER] + " -wrap=periodic " + textureName + ".tiff " + textureName + ".tx";
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
	  //IMPORTANT: we don't convert the template textures, because we expect them already in renderman format
	  qDebug("Converted image at %i",tt.elapsed());
	}
  
	//***run the aqsis rendering
	renderProcess.setWorkingDirectory(destDirString); //for the shaders/maps reference
  renderProcess.setProcessEnvironment(env);
  QString toRun = aqsisBinDirString +"/"+aqsisFileName[AQSIS] + " -progress " /*+"\"-progressformat=%p\" "*/+ mainFileName();
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
	
	//***copy the rendering result image in mesh folder (maybe a set of file)
	QString finalImage = meshDirString + QDir::separator() + imageName;
	qDebug("final image position: %s", qPrintable(finalImage));
	QString imageFormatString = imageFormatsSupported.at(imageFormat);	
	int n = numberOfCiphers(imagesRendered.size());//n is the ciphers number of imagesRendered.size()
	for(int i = 0; i < imagesRendered.size(); i++) {
		QString currentImage = destDirString + QDir::separator() + imagesRendered.at(i);
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
	
	//***run piqls with rendered image
	if(par.getBool("ShowResult")) {
		QProcess piqslProcess;
		piqslProcess.setWorkingDirectory(destDirString);
    piqslProcess.setProcessEnvironment(env);
    toRun = aqsisBinDirString +"/" + aqsisFileName[PIQSL];
		//if there'isnt image, it stops before...
		foreach(QString img, imagesRendered) {
			toRun += " " + img;
		}
		qDebug("Runnig piqsl command: %s", qPrintable(toRun));
		piqslProcess.start(toRun);
		
		piqslProcess.waitForFinished(-1); //no check error...
		piqslProcess.terminate();
	}
	//***only now we can delete recursively all created files (if it's required)
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
  //disconnect(&renderProcess, SIGNAL(readyReadStandardOutput()),this, SLOT(updateOutputProcess()));
	//the format is a number which say the percentage (maybe more that one)
	QString out = QString::fromLocal8Bit(renderProcess.readAllStandardOutput().data());
  //qDebug("aqsis.exe output: %s",qPrintable(out));

  QStringList TokenList(out.trimmed().split(' '));
  QStringList TokenList2 = TokenList.filter(QRegExp("[0-9][0-9]"));
  if(TokenList2.size()>0) out = TokenList2.at(0);//take only the first
 // if(out.right(1) =="%")
  out.chop(1);
  //qDebug("aqsis output taken: %s",qPrintable(out));

  int currentCb = int(out.toFloat());
	if(currentCb < lastCb)
		worldBeginRendered++;
	QString msg = "Rendering image with Aqsis (pass: " +
		QString::number(worldBeginRendered) + "/" + QString::number(numOfWorldBegin) + ")";
	int value = int( (100 * (worldBeginRendered - 1) + currentCb ) / numOfWorldBegin );
  cb(value, qPrintable(msg)); //update progress bar
  //qDebug("cb %i, '%s'",value,qPrintable(msg));
	//qDebug("cb value: worldBeginRendered %i last %i current %i effective %i" ,worldBeginRendered,lastCb,currentCb,value);
	lastCb = currentCb;
	//restore the signal handling
  //connect(&renderProcess, SIGNAL(readyReadStandardOutput()),this, SLOT(updateOutputProcess()));
}

void FilterHighQualityRender::errSgn() {
	QString out = QString::fromLocal8Bit(renderProcess.readAllStandardError().data());
	qDebug("aqsis error signal: %s",qPrintable(out));
}

Q_EXPORT_PLUGIN(FilterHighQualityRender)
