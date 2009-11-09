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

#define NO_RENDERING

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterHighQualityRender::FilterHighQualityRender() 
{
  typeList << FP_HIGHQUALITY_RENDER;
	
  foreach(FilterIDType tt , types())
    actionList << new QAction(filterName(tt), this);

  templateDir = qApp->applicationDirPath();
#if defined(Q_OS_WIN)
  if (templateDir.dirName() == "debug" || templateDir.dirName() == "release" || templateDir.dirName() == "plugins")
	templateDir.cdUp();
#elif defined(Q_OS_MAC)
  if (templateDir.dirName() == "MacOS") {
    for (int i = 0; i < 6; ++i) {
	  templateDir.cdUp();
	  if (templateDir.exists("render_template"))
	    break;
	}
  }
#endif
  if(!templateDir.cd("render_template")) {
	qDebug("Error. I was expecting to find the render_template dir. Now i am in dir %s",qPrintable(templateDir.absolutePath()));
    ;//this->errorMessage = "\"render_template\" folder not found";
  }

  alignValue = QStringList();
  alignValue << "center" << "bottom" << "top";

}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
const QString FilterHighQualityRender::filterName(FilterIDType filterId) const
{
  switch(filterId) {
    case FP_HIGHQUALITY_RENDER :  return QString("Render high quality image");
    default : assert(0); 
  }
  return QString();
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
const QString FilterHighQualityRender::filterInfo(FilterIDType filterId) const
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
const FilterHighQualityRender::FilterClass FilterHighQualityRender::getClass(QAction *a)
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
			parlst.addParam(new RichEnum("AlignX",0,alignValue,"Align X"));
			parlst.addParam(new RichEnum("AlignY",0,alignValue,"Align Y"));
			parlst.addParam(new RichEnum("AlignZ",0,alignValue,"Align Z"));

			//update the template list
			templates = QStringList();
			foreach(QString subDir, templateDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
				QString temp(templateDir.absolutePath() + QDir::separator() + subDir + QDir::separator() + subDir + ".rib");
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
			// ******Ë il nome dell'immagine, ma poi va copiata nella cartella della mesh...******
			parlst.addParam(new RichString("ImageName", "default.tiff", "Name of output image"));
			
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

	
 	//read a template file e make a new file rib
			
	//QString templatePath = par.getOpenFileName("TemplateName");
	QString templateName = templates.at(par.getEnum("scene"));
	QString templatePath = templateDir.absolutePath() + QDir::separator() + templateName + QDir::separator() + templateName + ".rib";
	//QString templatePath("e:\\fgt\\meshlab\\src\\meshlab\\render_template\\default\\default.rib");


	QString templateDir = getDirFromPath(&templatePath);

 	RibFileStack files(&templateName, &templateDir); //constructor
	//open file and stream
	if(!files.pushFile(&templatePath)) {
		this->errorMessage = "Template path is wrong: " + templatePath;
		return false;
	}

	//destination diretory
	QString dest("");
	//QString dest = par.getSaveFileName("SceneName");
	if(dest == "") { //default value: temporany directory
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
		dest = temp.absolutePath() + QDir::separator() + "scene.rib";
		delRibFiles = true;
	}
	else {
		delRibFiles = false;
	}

	QString destDir = getDirFromPath(&dest);
	QString destFile = getFileNameFromPath(&dest);
	qDebug("Starting to write rib file into %s",qPrintable(dest));
	//output file
	QFile outFile(dest);
	if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		return false;
	}	
	FILE* fout;
	fout = fopen(qPrintable(dest),"wb");
	if(fout==NULL)	{
	
	}
	
	/////controlli di coordinate, ecc.... per ora ignoro
	//if(m.cm.textures.size()>1 && m.cm.HasPerWedgeTexCoord() || m.cm.HasPerVertexTexCoord())

	//TEXTURE:
	//1. Take the list of texture mesh
	QStringList textureList;
	for(int i=0; i<m.cm.textures.size(); i++) {
		textureList << QString(m.cm.textures[i].c_str());
	}
	QDir tmp(destDir);
	if(!(tmp.entryList(QDir::Dirs | QDir::NoDotAndDotDot)).contains("Maps"))
	{
		if(!tmp.mkdir("Maps"))
			return false;
		else
			tmp.cd("Maps");
	}
	else
		tmp.cd("Maps");

	//2. Copy all texture in outDir (magari in una cartella a parte)	
	foreach(QString textureName, textureList) {
		QString str(m.fileName.c_str()); //mesh directory
		str = getDirFromPath(&str);
		QFile srcFile(str + QDir::separator() + textureName);

		QString newTex = tmp.absolutePath() + QDir::separator() + getFileNameFromPath(&textureName,false);
		if(srcFile.exists())
		{
			QImage image;
			image.load(srcFile.fileName());
			image.save(newTex+".tiff","Tiff");
		}
		else
			return false;
	}
	//3. se attive, prima del primo "frame begin" va messo il MAKETEXTURE ;)

	convertedGeometry = false; //if the mesh is already converted
	int currentFrame = 0;
	int numOfFrames = par.getInt("frames");
	vcg::Matrix44f transfCamera = vcg::Matrix44f::Identity();
	bool stop = false;
	FILE* fmain = fout;
	QStringList frameDeclaration = QStringList();
	bool isFrameDeclaration = false;

	//reading cycle
	while(!files.isEmpty() && !stop) {
		bool writeLine = true;
		QString line = files.topNextLine();
		//the ReadArchive and Option SearchPath "String Archive" statement are already handled
		QStringList token = line.split(' ');


		//add "MakeTexture" statement to create the texure, but only before frame one
		// ********* forse aqsis ha un exe che crea le texture..... ********** (pixie sì)
		if(token[0].trimmed() == "FrameBegin") {
			currentFrame = token[1].trimmed().toInt(); //no check on cast
			if(currentFrame == 1) {
				foreach(QString textureName, textureList) {
					QString makeTexture("MakeTexture \"" + getFileNameFromPath(&textureName,false) + ".tiff" + "\" \"" + getFileNameFromPath(&textureName, false) + ".tx\" \"periodic\" \"periodic\" \"gaussian\" 1 1");
					fprintf(fout,"%s\n",qPrintable(makeTexture));
				}
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


		//change the output image file name
		if(token[0].trimmed() == "Display") {
			line = token[0] + " \"";
			if(token[2].trimmed() == "\"file\"")
				line += "+";			
			if(currentFrame == 1)
				line += par.getString("ImageName");
			else {
				QString imageName = par.getString("ImageName");
				imageName = getFileNameFromPath(&imageName,false);
				line += imageName + QString::number(currentFrame) + ".tiff";
			}
			line += "\" " + token[2] + " " + token[3];
			for(int i = 4; i<token.size(); i++) {
				line += token[i];
			}
		}

		//change the output image format
		if(token[0].trimmed() == "Format") {
			line = token[0] + " " + QString::number(par.getInt("FormatX")) + " " + QString::number(par.getInt("FormatY")) + " " + QString::number(par.getFloat("PixelAspectRatio"));
		}

		//transformation camera
		if(numOfFrames > 0 && token[0].trimmed() == "Transform") {
			transfCamera = readMatrix(&files, line);
			writeMatrix(fout,transfCamera);
			writeLine = false;
		}

		//make another file if there is an animation
		if(numOfFrames > 0 && token[0].trimmed() == "WorldBegin") {
			isFrameDeclaration = false;
			//it's certainly the first WorldBegin
			QString filename = destDir + QDir::separator() + "world.rib";
			fprintf(fout,"ReadArchive \"world.rib\"\n");
			fout = fopen(qPrintable(filename),"wb");
			if(fout==NULL)	{
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
		if(token[0].trimmed() == "AttributeBegin") {
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
						token[3].trimmed() == "name\"") { //"identifier" "string name"
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
									QString filename = "mesh" + QString::number(currentFrame) + ".rib";

									QString meshDest = destDir + QDir::separator() + filename;
									FILE *fmesh = fopen(qPrintable(meshDest),"wb");
									if(fmesh==NULL) {							
									}

									fprintf(fout,"%s\n## HO TROVATO UN OGGETTO DUMMY\n",qPrintable(line));
									fprintf(fout,"ReadArchive \"%s\"\n", qPrintable(filename));									
									convertObject(&files, fmesh, destDir, m, par, &textureList);
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
	
	//va copiata una quantita indefinita di file dal template e organizzata in directory ;)
	
	//run the aqsis rendering
	QStringList env = QProcess::systemEnvironment();
	QString aqsisDir;
	foreach(QString envElem, env) {
		if(envElem.contains("AQSISHOME")) {
			aqsisDir = envElem.remove("AQSISHOME=");
			break;
		}
	}
#if defined(Q_OS_WIN)
	//if os is win and a dir contains a space, it must be wrapped in quotes (..\"Program files"\..)
	QStringList dirs = aqsisDir.split(QDir::separator());
	aqsisDir.clear();
	for(int i = 0; i < dirs.size(); i++) {
		if(!dirs[i].contains(" "))
			aqsisDir += dirs[i];
		else
			aqsisDir = aqsisDir + "\"" + dirs[i] + "\"";
		aqsisDir += QDir::separator();
	}
	dirs.clear();
	dirs = dest.split(QDir::separator());
	dest.clear();
	for(int i = 0; i < dirs.size(); i++) {
		if(!dirs[i].contains(" "))
			dest += dirs[i];
		else
			dest = dest + "\"" + dirs[i] + "\"";
		
		if(!dirs[i].contains("."))
			dest += QDir::separator();
	}
#endif
	QProcess renderProcess;
	renderProcess.setWorkingDirectory(destDir); //for the shaders/maps reference
	QString toRun = aqsisDir+"bin"+QDir::separator()+"aqsis.exe " + destFile;
#if !defined(NO_RENDERING)
	renderProcess.start(toRun);
	if (!renderProcess.waitForFinished(-1))
         return false; //devo?
#endif
	//the image is copied in mesh folder
	QString meshDir(m.fileName.c_str());
	meshDir = getDirFromPath(&meshDir);
	QString finalImage = meshDir + QDir::separator() + par.getString("ImageName");
	if(QFile::exists(finalImage)) {
		//delete without control?
		QFile::remove(finalImage);
	}
	QFile::copy(destDir + QDir::separator() + par.getString("ImageName"), finalImage);
    
	//delete all files (if it's required)
	

	return true;
}
/*
int FilterHighQualityRender::ignoreObject(RibFileStack* files) {
	int c = 1; //the number of AttributeBegin statement readed
	bool exit = false;
	while(!files->isEmpty() && c!=0) {
		QString line = files->topNextLine();
		QStringList token = line.split(' ');
		
		if(token[0].trimmed() == "AttributeBegin")
			++c;
		if(token[0].trimmed() == "AttributeEnd")
			--c;		
	}
	return 0; //errors...
}*/


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
int FilterHighQualityRender::convertObject(RibFileStack* files, FILE* fout, QString destDir, MeshModel &m, RichParameterSet &par, QStringList* textureList)
{	
	float scale = 1.0;
	vcg::Matrix44f templateMatrix = vcg::Matrix44f::Identity();
	
	int c = 1; //the number of AttributeBegin statement readed
	bool exit = false;
	while(!files->isEmpty() && !exit) {
		QString line = files->topNextLine();
		QStringList token = line.split(' ');
		
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
				///////////////////////////////////////
				//CONTROLLARE Y e Z (se vanno scambiate
				///////////////////////////////////////
			//autoscale
			if(par.getBool("Autoscale")) {
				float meshX = m.cm.trBB().DimX();
				float meshY = m.cm.trBB().DimY();
				float meshZ = m.cm.trBB().DimZ();

				float ratioX = dummyX / meshX;
				float ratioY = dummyY / meshY;
				float ratioZ = dummyZ / meshZ;
				scale = std::min<float>(ratioX, ratioY);
				scale = std::min<float>(scale, ratioZ);
				scaleMatrix.SetScale(scale,scale,scale);
			}
			
			//center mesh
			vcg::Point3f c = m.cm.trBB().Center();
			vcg::Matrix44f translateBBMatrix;
			translateBBMatrix.SetTranslate(-c[0],-c[1],-c[2]);
			
			//align
			float dx = 0.0, dy = 0.0, dz = 0.0;
			QString x = alignValue.at(par.getEnum("AlignX"));
			if(x != "center") {
				dx = (dummyX - m.cm.trBB().DimX() * scale) / 2;
				if(x == "bottom")
					dx = -dx;
			}
			QString y = alignValue.at(par.getEnum("AlignY"));
			if(y != "center") {
				dy = (dummyY - m.cm.trBB().DimY() * scale) / 2;
				if(y == "bottom")
					dy = -dy;
			}
			QString z = alignValue.at(par.getEnum("AlignZ"));
			if(z != "center") {
				dz = (dummyZ - m.cm.trBB().DimZ() * scale) / 2;
				if(z == "bottom")
					dz = -dz;
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

		//texture mapping
		if(token[0].trimmed() == "Surface") {
			if(m.cm.textures.size()>1 && m.cm.HasPerWedgeTexCoord() || m.cm.HasPerVertexTexCoord()) {
				foreach(QString textureName, *textureList) {
					fprintf(fout,"Surface \"paintedplastic\" \"Kd\" 1.0 \"Ks\" 1.0 \"texturename\" [\"%s.tx\"]\n", qPrintable(getFileNameFromPath(&textureName,false)));
					//fprintf(fout,"Surface \"sticky_texture\" \"texturename\" [\"%s.tx\"]\n", qPrintable(getFileNameFromPath(&textureName,false)));
					//fprintf(fout,"Surface \"mytexmap\" \"name\" \"%s.tx\"\n", qPrintable(getFileNameFromPath(&textureName,false)));
					
				}
			}
		}

		if(token[0].trimmed() == "PointsPolygons") {
			QString filename = "geometry.rib";
			fprintf(fout,"ReadArchive \"%s\"", qPrintable(filename));
			if(!convertedGeometry) {
				convertedGeometry = true;
				QString geometryDest = destDir + QDir::separator() + filename;			
				vcg::tri::io::ExporterRIB<CMeshO>::Save(m.cm, qPrintable(geometryDest), vcg::tri::io::Mask::IOM_ALL, scale, false);
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

//read array???

//read a matrix from the rib stack and transpose it
vcg::Matrix44f FilterHighQualityRender::readMatrix(RibFileStack* files, QString line){
	float t[16];
	//an array in renderman can contains the char '\n' :(
	QString matrixString = line;
	while(!line.contains(']')) {
		line = files->topNextLine();
		matrixString += line;
	}
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

QString FilterHighQualityRender::getDirFromPath(QString* path) {
	//return path->left(path->lastIndexOf(QDir::separator())); //don't work :/
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

Q_EXPORT_PLUGIN(FilterHighQualityRender)