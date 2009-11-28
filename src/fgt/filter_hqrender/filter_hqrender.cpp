#include <filter_hqrender.h>
#include <QtGui>
#include <QDir>
#include <QProcess>
//#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>

//#define NO_RENDERING

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

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

  /*alignValue = QStringList();
  alignValue << "center" << "bottom" << "top";
*/
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
 QString FilterHighQualityRender::filterName(FilterIDType filterId) const
{
  switch(filterId) {
    case FP_HIGHQUALITY_RENDER :  return QString("Render high quality image");
    default : assert(0); 
  }
  return QString();
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
 QString FilterHighQualityRender::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
    case FP_HIGHQUALITY_RENDER :  return QString("Make an high quality image of current mesh on a choosen template scene.");
    default : assert(0); 
  }
  return QString("Unknown Filter");
}

// The FilterClass describes in which generic class of filters it fits. 
// This choice affect the submenu in which each filter will be placed 
// More than a single class can be choosen.
 FilterHighQualityRender::FilterClass FilterHighQualityRender::getClass(QAction *a)
{
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
			parlst.addParam(new RichInt("frames",0, "Number of frames for animation (0 for no animation)"));
			//format is output is only tiff for now
			parlst.addParam(new RichString("ImageName", "default", "Name of output image"));
			
			//DON'T WORK!!
			//delRibFiles = true;
			//FileValue fv("");
			//parlst.addParam(new RichSaveFile("SceneName",&fv,&FileDecoration(&fv,".rib","Name of file rib to save","If not specified, the files will be removed")));
			
			
			break;
			}
		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
bool FilterHighQualityRender::applyFilter(QAction *filter, MeshModel &m, RichParameterSet & par, vcg::CallBackPos *cb)
{
	// Typical usage of the callback for showing a nice progress bar in the bottom. 
	// First parameter is a 0..100 number indicating percentage of completion, the second is an info string.
	//cb(100*i/m.cm.vert.size(), "Randomly Displacing...");
	// Log function dump textual info in the lower part of the MeshLab screen. 
	//Log(GLLogStream::FILTER,"Successfully displaced %i vertices",m.cm.vn);
	this->cb = cb;
	QTime tt; tt.start(); //debug
	qDebug("Starting apply filter");
 	//read a template file e make a new file rib
			
	//QString templatePath = par.getOpenFileName("TemplateName");
	QString templateName = templates.at(par.getEnum("scene"));
	QString templatePath = templatesDir.absolutePath() + QDir::separator() + templateName + QDir::separator() + templateName + ".rib";
    QDir templateDir(templatesDir);
	templateDir.cd(templateName);
	QString templateDirString = getDirFromPath(&templatePath);

 	RibFileStack files(&templateName, &templateDirString); //constructor
	//open file and stream
	if(!files.pushFile(&templatePath)) {
		this->errorMessage = "Template path is wrong: " + templatePath;
		return false;
	}

	//destination diretory + main file
	QString destPathFileString("");
	//QString destPathFileString = par.getSaveFileName("SceneName");
	if(destPathFileString == "") { //default value: temporany directory
		QDir temp = QDir::temp();
		if(!temp.cd("scene")) {
			if(temp.mkdir("scene")) {
				if(!temp.cd("scene"))
					return false;
			}
			else {
				return false;
			}
		}		
		destPathFileString = temp.absolutePath() + QDir::separator() + "scene.rib";
		delRibFiles = true;
	}
	else {
		delRibFiles = false;
	}


	QString destDirString = getDirFromPath(&destPathFileString);
	QString destFile = getFileNameFromPath(&destPathFileString);
	qDebug("Starting to write rib file into %s",qPrintable(destPathFileString));
	//output file		
	FILE* fout;
	fout = fopen(qPrintable(destPathFileString),"wb");
	if(fout==NULL)	{
	
	}

	//TEXTURE: take the list of texture mesh
	QStringList textureList = QStringList();
	for(int i=0; i<m.cm.textures.size(); i++) {
		textureList << QString(m.cm.textures[i].c_str());
	}
	
	convertedGeometry = false; //if the mesh is already converted
	int currentFrame = 0;
	int numOfFrames = par.getInt("frames");
	vcg::Matrix44f transfCamera = vcg::Matrix44f::Identity();
	bool stop = false;
	FILE* fmain = fout; //if change the output file
	QStringList frameDeclaration = QStringList();
	bool isFrameDeclaration = false;
	QStringList shaderDirs, textureDirs;
	bool currentDisplayTypeIsFile = false;
	bool anyOtherDisplayType = false;
	QString newFormat = "Format " + 
						QString::number(par.getInt("FormatX")) + " " +
						QString::number(par.getInt("FormatY")) + " " +
						QString::number(par.getFloat("PixelAspectRatio"));
	QStringList renderedImage = QStringList();
	int numberOfDummies = 0; //the dummy object could be than one (e.g. for ambient occlusion passes)
	
	qDebug("Starting reading cycle %i",tt.elapsed());
	//reading cycle
	int debugCount = 0;
	while(!files.isEmpty() && !stop) {
		if((++debugCount)%200 == 0)	qDebug("Time after %i statement: %i",debugCount,tt.elapsed());

		bool writeLine = true;
		QString line = files.topNextLine();
		
		//the ReadArchive statement is already handled
		QStringList token = line.split(' ');

		//statement to declare other directory for the template
		if(token[0].trimmed() == "Option" && token[1].trimmed() == "\"searchpath\"") {			
			int type = 0;
			QStringList dirList = readSearchPath(&files,line,&type);
			switch(type) {
				case FilterHighQualityRender::ARCHIVE:
					files.addSubDirs(dirList);
					break;
				case FilterHighQualityRender::SHADER:
					shaderDirs = dirList;
					break;
				case FilterHighQualityRender::TEXTURE:
					textureDirs = dirList;
					break;
				case FilterHighQualityRender::ERR:
					//????????????????COSA FACCIO??????
					break;
			}
		}
		
		//there are some statement that make an output. It's need to create the path
		if(token[0].trimmed().startsWith("Make")) {
			QString path = token[2]; //for MakeTexture, MakeShadow, MakeLatLongEnvironment
			if(token[0].trimmed() == "MakeCubeFaceEnvironment")
				path = token[7];
			path = getDirFromPath(&path);
			qDebug("dir! line: %s\npath: %s",qPrintable(line),qPrintable(path));
			checkDir(destDirString,path);
		}

		

		if(token[0].trimmed() == "FrameBegin") {
			currentFrame = token[1].trimmed().toInt(); //no check on cast
			if(currentFrame == 1) {		//questo è casino con animazioni		
				if(numOfFrames > 0)
					isFrameDeclaration = true;
			}
		}

		//if there's an animation, stop the processing of other possible frame
		if(numOfFrames > 0 && currentFrame == 1 && token[0].trimmed() == "FrameEnd") {
			fprintf(fout,"%s\n",qPrintable(line));
			writeLine = false;
			makeAnimation(fout,numOfFrames,transfCamera,frameDeclaration,par.getString("ImageName"));
			stop = true;
		}

		//if output is not a file the format must be the same!!framebuffer is ignored
		if(token[0].trimmed() == "Display") {
			//create the path if needed
			QString path = token[1].trimmed();
			path = getDirFromPath(&path);
			qDebug("dir! line: %s\npath: %s",qPrintable(line),qPrintable(path));
			checkDir(destDirString,path);

			if(token[2].trimmed() != "\"framebuffer\"")
				if (!anyOtherDisplayType && token[2].trimmed() == "\"file\"") {
					currentDisplayTypeIsFile = true;
					renderedImage << token[1].trimmed();
				}
				else
					anyOtherDisplayType = true;
		}		

		//transformation camera
		if(numOfFrames > 0 && token[0].trimmed() == "Transform") {
			transfCamera = readMatrix(&files, line);
			writeMatrix(fout,transfCamera);
			writeLine = false;
		}

		//make another file if there is an animation
		if(token[0].trimmed() == "WorldBegin") {
			//if there's another type of display the format is not change
			if(!anyOtherDisplayType && currentDisplayTypeIsFile) {
				fprintf(fout,"%s\n", qPrintable(newFormat));
				frameDeclaration << newFormat;
			}
			currentDisplayTypeIsFile = false;
			anyOtherDisplayType = false;
			//is right?yes,because before the next WorldBegin will there be a new Display statement
				
			if(numOfFrames > 0) {
				isFrameDeclaration = false;
				//it's certainly the first WorldBegin
				QString filename = destDirString + QDir::separator() + "world.rib";
				fprintf(fout,"ReadArchive \"world.rib\"\n");
				fout = fopen(qPrintable(filename),"wb");
				if(fout==NULL)	{
				}
			}
		}
		if(numOfFrames > 0 && token[0].trimmed() == "WorldEnd") {
			//it's certainly the first WorldEnd
			fprintf(fout,"%s\n",qPrintable(line));
			fclose(fout);
			fout = fmain;
			writeLine = false;
		}

		//is an object
		if(token[0].trimmed() == "AttributeBegin") {//rewrite this part
			fprintf(fout,"%s\n",qPrintable(line));
			//looking for an object called dummy...
			int c = 1;
			while(!files.isEmpty() && c != 0) {
				line = files.topNextLine();
				token = line.split(' ');
				
				if(token[0].trimmed() == "AttributeBegin")
					++c;
				if(token[0].trimmed() == "AttributeEnd")
					--c;
				
				if(token[0].trimmed() == "Attribute") {
					if(token[1].trimmed() == "\"identifier\"" &&
						token[2].trimmed() == "\"string" &&
						token[3].trimmed() == "name\"") { //"identifier" "string name" !!!"identifier" "name" TOO!!!
						QString str = token[4];
						if(token.size()>5) {
							for(int i= 5; i<token.size(); i++)
								str += " " + token[i]; //the remainig tokens are joined together
						}
						str = str.simplified();
						//remove the character [ ] "
						if(str.startsWith('[')) {
							if(str.endsWith(']')) {					
								//remove the character [ ] "
								str.remove('[');
								str.remove(']');
								str.remove('\"');
								str = str.simplified();
								if(str.toLower() == "dummy") {									
									QString filename = "meshF" + QString::number(currentFrame) + "O" + QString::number(numberOfDummies) + ".rib";
									numberOfDummies++;
									QString meshDest = destDirString + QDir::separator() + filename;
									FILE *fmesh = fopen(qPrintable(meshDest),"wb");
									if(fmesh==NULL) {							
									}

									fprintf(fout,"%s\n## HO TROVATO UN OGGETTO DUMMY\n",qPrintable(line));
									fprintf(fout,"ReadArchive \"%s\"\n", qPrintable(filename));									
									convertObject(&files, fmesh, destDirString, m, par, &textureList, cb);
									fclose(fmesh);
									fprintf(fout,"AttributeEnd\n");
									--c;
									writeLine = false; //line is already writed...jump the next statement
								}
							} else {
								//an array can be contains the \n character.... :/
							}
						} else {
							//the statement is: 'Attribute "identifier" "string name"' without an array =>do nothing
						}
					}
				}
				if(writeLine)
					fprintf(fout,"%s\n",qPrintable(line));
			}
			writeLine = false;
		}
		if(writeLine) {
			//copy the same line in file
			fprintf(fout,"%s\n",qPrintable(line));
		}
		if(isFrameDeclaration && token[0].trimmed() != "FrameBegin" && token[0].trimmed() != "Transform")
			frameDeclaration << line;
	}
	fclose(fout);
	Log(GLLogStream::FILTER,"Successfully created scene");
	qDebug("Cycle ending at %i",tt.elapsed());
	//copy the rest of template (shaders, textures..)
	QDir destDir(destDirString);
	copyFiles(templateDir, destDir, textureDirs);
	copyFiles(templateDir, destDir, shaderDirs);
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
				QByteArray err = compileProcess.readAllStandardError();
				this->errorMessage = "Is impossible to compile the shaders of template" + err;
				qDebug("compiling msg err: %s",err.data());
				qDebug("compiling msg out: %s",compileProcess.readAllStandardOutput().data());
				return false;
			}

		}
	}
	qDebug("Compiled shaders at %i",tt.elapsed());
	Log(GLLogStream::FILTER,"Successfully compiled scene shaders");
	//Copy and convert to tiff format, all mesh textures, in dest dir and convert the to renderman format
	//multi-texture not supported!Copy and convert only the first texture
	if(textureList.count() > 0) {
	//foreach(QString textureName, textureList) {
		QString textureName = textureList.first();
		QString str(m.fileName.c_str()); //mesh directory
		str = getDirFromPath(&str);
		QFile srcFile(str + QDir::separator() + textureName);

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
		Log(GLLogStream::FILTER,"Successfully converted mesh texture");
	}
	qDebug("Converted image at %i",tt.elapsed());
	
	//run the aqsis rendering
	//QProcess renderProcess;
	renderProcess.setWorkingDirectory(destDirString); //for the shaders/maps reference
	renderProcess.setEnvironment(aqsisEnv);
	QString toRun = aqsisDir + aqsisBinPath() + QDir::separator() + aqsisName()+ " -progress -progressformat=%p "+ destFile;
	qDebug("Runnig aqsis command: %s", qPrintable(toRun));
	//every time the render process write a message, receive a signal
	connect(&renderProcess, SIGNAL(readyReadStandardOutput()),this, SLOT(updateOutputProcess()));
	connect(&renderProcess, SIGNAL(readyReadStandardError()),this, SLOT(errSgn()));
	connect(&renderProcess, SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finish(int,QProcess::ExitStatus)));
	cb(0, "Rendering image with Aqsis");
#if !defined(NO_RENDERING)
	renderProcess.start(toRun);	
	//if (!renderProcess.waitForFinished(-1)) //wait the finish of process
         //return false; //devo?quando si verifica?se la finestra viene chiusa?	
#endif
	//qDebug("end rendering at %i",tt.elapsed());
/*	
	//the rendering result image is copied in mesh folder
	QString meshDir(m.fileName.c_str());
	meshDir = getDirFromPath(&meshDir);
	QString finalImage = meshDir + QDir::separator() + par.getString("ImageName") + ".tiff";
	qDebug("final image position: %s", qPrintable(finalImage));
	if(QFile::exists(finalImage)) {
		//delete without control?
		QFile::remove(finalImage);
	}
	for(int i = 0; i < renderedImage.size(); i++)
		qDebug("rendering result image position: %s", qPrintable(destDirString + QDir::separator() + renderedImage.at(i)));
	//maybe a set of file
	//QFile::copy(destDirString + QDir::separator() + par.getString("ImageName"), finalImage);

    
	//delete all created files (if it's required)
	qDebug("end: %i",tt.elapsed());
	Log(GLLogStream::FILTER,"Successfully created high quality image");
*/
	return true;
}

void FilterHighQualityRender::updateOutputProcess() {
	QString out = QString::fromLocal8Bit(renderProcess.readAllStandardOutput().data());
	//the format is a number which say the percentage (maybe more that one)
	out = QStringList(out.trimmed().split(' ')).last(); //take only last
	//qDebug("aqsis.exe output: %s",qPrintable(out));
	cb(int(out.toFloat()), "Rendering image with Aqsis"); //update progress bar
}
void FilterHighQualityRender::finish(int a,QProcess::ExitStatus b){
	qDebug("aqsis.exe finished");
	Log(GLLogStream::FILTER,"Aqsis has rendered image successfully");
	cb(0, "");
	//ho bisogno di alcune info per questi:
	//copia l'immagine creata
	//cancella file temporanei
}

void FilterHighQualityRender::errSgn() {
	QString out = QString::fromLocal8Bit(renderProcess.readAllStandardError().data());
	qDebug("aqsis.exe error: %s",qPrintable(out));
}

//rivedere....nome file
int FilterHighQualityRender::makeAnimation(FILE* fout, int numOfFrame,vcg::Matrix44f transfCamera, QStringList frameDeclaration, QString imageName) {
	//with numOfFrame+2 the last image is the same of first
	for(int frame=2; frame<numOfFrame+1; frame++) {
		fprintf(fout,"FrameBegin %i\n",frame);
		foreach(QString statement, frameDeclaration) {
			QStringList token = statement.split(' ');
			if(token[0].trimmed() == "Display") {
				statement = token[0] + " \"";
				if(token[2].trimmed() == "\"file\"")
					statement += "+";			
					if(frame == 1)
						statement += imageName;
					else {
						imageName = getFileNameFromPath(&imageName,false);
						statement += imageName + QString::number(frame) + ".tiff";
				}
				statement += "\" " + token[2] + " " + token[3];
				for(int i = 4; i<token.size(); i++) {
					statement += token[i];
				}
			}
			fprintf(fout,"%s\n",qPrintable(statement));
		}
		vcg::Matrix44f result;
		float rot = float(360*(frame-1)/numOfFrame);
		result = result.SetRotateDeg(rot,vcg::Point3f(0.0,0.0,1.0));
		result = transfCamera * result;
		writeMatrix(fout,result);
		fprintf(fout,"ReadArchive \"world.rib\"\nFrameEnd\n");
	}
	return 0; //errors
}

//write on a opened file the attribute of object entity
int FilterHighQualityRender::convertObject(RibFileStack* files, FILE* fout, QString destDir, MeshModel &m, RichParameterSet &par, QStringList* textureList, vcg::CallBackPos * cb)
{	
	float scale = 1.0;
	vcg::Matrix44f templateMatrix = vcg::Matrix44f::Identity();
	
	int c = 1; //the number of AttributeBegin statement readed
	bool exit = false;
	while(!files->isEmpty() && !exit) {
		QString line = files->topNextLine();
		QStringList token = line.split(' ');
		
		//the surface shader remain the same of template (maybe many line)
		if(token[0].trimmed() == "Surface") {
			fprintf(fout,"%s\n",qPrintable(line));
			line = files->topNextLine();
			while(line.trimmed().startsWith('\"'))
			{				
				fprintf(fout,"%s\n",qPrintable(line));
				line = files->topNextLine();
			}
		}

		if(token[0].trimmed() == "AttributeBegin")
			++c;
		if(token[0].trimmed() == "AttributeEnd")
			--c;

		//take the transformation matrix of dummy
		if(token[0].trimmed() == "Transform") {			
			templateMatrix = readMatrix(files,line);
		}

		//modify the transformation matrix
		if(token[0].trimmed() == "Bound") {
			vcg::Matrix44f scaleMatrix = vcg::Matrix44f::Identity();
			float dummyX = token[2].toFloat() - token[1].toFloat();
			float dummyY = token[4].toFloat() - token[3].toFloat();
			float dummyZ = token[6].toFloat() - token[5].toFloat();
				
			//autoscale
			if(par.getBool("Autoscale")) {				
				float ratioX = dummyX / m.cm.trBB().DimX();
				float ratioY = dummyY / m.cm.trBB().DimY();
				float ratioZ = dummyZ / m.cm.trBB().DimZ();
				scale = std::min<float>(ratioX, std::min<float>(ratioY, ratioZ)); //scale factor is min ratio
				scaleMatrix.SetScale(scale,scale,scale);
			}
			
			//center mesh
			vcg::Point3f c = m.cm.trBB().Center();
			vcg::Matrix44f translateBBMatrix;
			translateBBMatrix.SetTranslate(-c[0],-c[1],-c[2]);
			
			//align
			float dx = 0.0, dy = 0.0, dz = 0.0;				
			switch(par.getEnum("AlignX")) {
				case alignValue::TOP:
					dx = (dummyX - m.cm.trBB().DimX() * scale) / 2;
					break;
				case alignValue::BOTTOM:
					dx = -(dummyX - m.cm.trBB().DimX() * scale) / 2;
					break;
				case alignValue::CENTER: break; //is already center
			}
			switch(par.getEnum("AlignY")) {
				case alignValue::TOP:
					dy = (dummyY - m.cm.trBB().DimY() * scale) / 2;
					break;
				case alignValue::BOTTOM:
					dy = -(dummyY - m.cm.trBB().DimY() * scale) / 2;
					break;
				case alignValue::CENTER: break; //is already center
			}			
			switch(par.getEnum("AlignZ")) {
				case alignValue::TOP:
					dz = (dummyZ - m.cm.trBB().DimZ() * scale) / 2;
					break;
				case alignValue::BOTTOM:
					dz = -(dummyX - m.cm.trBB().DimZ() * scale) / 2;
					break;
				case alignValue::CENTER: break; //is already center
			}
			vcg::Matrix44f alignMatrix;
			alignMatrix = alignMatrix.SetTranslate(dx,dy,dz);

			vcg::Matrix44f result = templateMatrix * alignMatrix * scaleMatrix * translateBBMatrix;
			//write transformation matrix (after transpose it)
			writeMatrix(fout,result);
			//write bounding box (not modified) /////VA MODIFICATO IL BB SE NON SCALO?????
			fprintf(fout,"%s\n",qPrintable(line));
			
		}
		
		if(token[0].trimmed() == "ShadingInterpolation") {
			fprintf(fout,"%s\n",qPrintable(line)); //forzare smooth???
		}
		
		//some like switch???
		if(token[0].trimmed() == "Attribute")
		{
//			if(token[1].trimmed() == "\"identifier\"" 
			if(token[1].trimmed() == "\"user\"" || token[1].trimmed() == "\"displacementbound\"")
			{
				fprintf(fout,"%s\n",qPrintable(line));
				if(line.contains('[') && !line.contains(']')) //array maybe contains '\n'
				{
					while(!line.contains(']'))
					{
						line = files->topNextLine();
						fprintf(fout,"%s\n",qPrintable(line));
					}
				}
			}
		}

		if(token[0].trimmed() == "PointsPolygons") {
			//texture mapping (are TexCoord needed for texture mapping?)
			if(!textureList->empty() > 0 && (m.cm.HasPerWedgeTexCoord() || m.cm.HasPerVertexTexCoord())) {
				//multi-texture don't work!I need ad-hoc shader and to read the texture index for vertex..
				//foreach(QString textureName, *textureList) {

				//read only the one texture
				QString textureName = textureList->first();
				fprintf(fout,"Surface \"paintedplastic\" \"Kd\" 1.0 \"Ks\" 0.0 \"texturename\" [\"%s.tx\"]\n", qPrintable(getFileNameFromPath(&textureName,false)));								
			}
			
			QString filename = "geometry.rib";
			fprintf(fout,"ReadArchive \"%s\"", qPrintable(filename));
			if(!convertedGeometry) {
				//make the conversion only once
				convertedGeometry = true;
				QString geometryDest = destDir + QDir::separator() + filename;			
				vcg::tri::io::ExporterRIB<CMeshO>::Save(m.cm, qPrintable(geometryDest), vcg::tri::io::Mask::IOM_ALL, false, cb);
				Log(GLLogStream::FILTER,"Successfully converted mesh");
			}
		}
		
		if(c == 0) {
			exit = true; //sposta questo nel while!!
			//fprintf(fout,"%s\n",qPrintable(line));
		}
	}
	//ignore the geometry of dummy object defined in the template
	//until end of file or number of statement "AttributeEnd" is equal to c
	return 0;  //errors...
}

//read array from the rib stack (an array can be contain \n character
QString FilterHighQualityRender::readArray(RibFileStack* files, QString arrayString) {
	QString str = arrayString;
	while(!(str.contains('[') && str.contains(']')))
		str += files->topNextLine();
	return str;
}

//read a matrix from the rib stack and transpose it
vcg::Matrix44f FilterHighQualityRender::readMatrix(RibFileStack* files, QString line){
	float t[16];
	//an array in renderman can contains the char '\n' :(
	QString matrixString = readArray(files, line);	
	int k=0;
	QStringList list = matrixString.split(' ');
	for(int i=0; i<list.size(); i++) {
		if(list[i].trimmed().contains('[') || list[i].trimmed().contains(']')) {
			list[i] = list[i].remove('[');
			list[i] = list[i].remove(']');
		}
		bool isNumber;
		float number = list[i].toFloat(&isNumber);
		if(isNumber)
			t[k++]=number;
	}
	vcg::Matrix44f tempMatrix(t);
	return tempMatrix.transpose();
}

int FilterHighQualityRender::writeMatrix(FILE* fout, vcg::Matrix44f matrix, bool transposed) {
	fprintf(fout,"Transform [ ");
	for(int i = 0; i<4; i++)
		for(int j = 0; j<4; j++)
			fprintf(fout,"%f ",(transposed)? matrix.ElementAt(j,i) : matrix.ElementAt(i,j));
	fprintf(fout,"]\n");
	return 0;
}

//return an a list of directory (separated by ':' character)
QStringList FilterHighQualityRender::readSearchPath(RibFileStack* files,QString line, int* type) {
	QStringList dirs;
	QStringList token = line.split(" ");
	if(!(token[0].trimmed() == "Option" && token[1].trimmed() == "\"searchpath\"")) {
		*type = FilterHighQualityRender::ERR;
		return dirs;
	}
	//the line maybe: Option "searchpath" "string type" [ ..values..]
	//            or: Option "searchpath" "type"        [ ..values..]
	int index = 2;
	if(token[2].trimmed() == "\"string")
		index++; //is "string type"..
	if(token[index].trimmed() == "\"archive\"" || token[index].trimmed() == "archive\"")
		*type = FilterHighQualityRender::ARCHIVE;
	if(token[index].trimmed() == "\"shader\"" || token[index].trimmed() == "shader\"")
		*type = FilterHighQualityRender::SHADER;
	if(token[index].trimmed() == "\"texture\"" || token[index].trimmed() == "texture\"")
		*type = FilterHighQualityRender::TEXTURE;

	QString str = token[++index];
	if(token.size()>(index+1)) {
		for(int i = index + 1; i < token.size(); i++) {
			str += " " + token[i]; //the remainig token are joined together
		}
	}
	//maybe that the array is defined over more line
	str = readArray(files,str);
	str = str.simplified();
	if(str.startsWith('[') && str.endsWith(']')) {
		//remove the character [ ] "
		str.remove('[');
		str.remove(']');
		str.remove('\"');
		str = str.simplified();
		dirs = str.split(':');
	} else {
		*type = FilterHighQualityRender::ERR;		
	}
	return dirs;		
}

//path must have the filename
QString FilterHighQualityRender::getDirFromPath(QString* path) {
	//return path->left(path->lastIndexOf(QDir::separator())); //don't work :/
	if(path->lastIndexOf('\\') == -1 && path->lastIndexOf('/') == -1)
		return ".";
	return path->left(std::max<int>(path->lastIndexOf('\\'),path->lastIndexOf('/')));
}

QString FilterHighQualityRender::getFileNameFromPath(QString* path, bool type) {
	//return path->right(path->size() - 1 - path->lastIndexOf(QDir::separator())); //don't work :/
	QString temp = path->right(path->size() - 1 - std::max<int>(path->lastIndexOf('\\'),path->lastIndexOf('/')));
	if(type)
		return temp;
	else
		return temp.left(temp.lastIndexOf('.'));
}


QString FilterHighQualityRender::quotesPath(QString* path) {
	//if path contains a space, is wrapped in quotes (e.g. ..\"Program files"\..)
	QStringList dirs = path->split(QDir::separator());
	QString temp("");
	for(int i = 0; i < dirs.size(); i++) {
		if(!dirs[i].contains(" "))
			temp += dirs[i];
		else
			temp = temp + "\"" + dirs[i] + "\"";
		temp += QDir::separator();
	}
	//the final of path is separator!!!
	return temp;
}

bool FilterHighQualityRender::checkDir(QString destDirString, QString path) {
	QDir destDir(destDirString);
	QStringList pathDirs = path.split('/');
	foreach(QString dir, pathDirs) {
		if(!destDir.cd(dir)) {
			destDir.mkdir(dir);
			destDir.cd(dir);
		}
	}
	qDebug(qPrintable(destDir.absolutePath()));
	return true;
}

//take all files in templateDir/[dirs] directories and copy them in dest/[dirs]
bool FilterHighQualityRender::copyFiles(QDir templateDir,QDir destDir,QStringList dirs) {
	QDir src = templateDir, dest = destDir;
	foreach(QString dir, dirs) {
		if(dir != "." && src.cd(dir)) {
			bool a;
			if(!dest.mkdir(dir)) {
				if(!dest.cd(dir))
					return false;
			}
			else
				a = dest.cd(dir);

			QStringList filesList = src.entryList(QDir::Files);
			foreach(QString file, filesList) {
				qDebug("copy file from %s to %s", 
					qPrintable(src.absolutePath() + QDir::separator() + file),
					qPrintable(dest.absolutePath() + QDir::separator() + file));	
				QFile::copy(src.absolutePath() + QDir::separator() + file,dest.absolutePath() + QDir::separator() + file);
			}
		}
	}
	return true;
}
Q_EXPORT_PLUGIN(FilterHighQualityRender)