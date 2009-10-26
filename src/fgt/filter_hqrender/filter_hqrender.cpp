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
			parlst.addParam(new RichInt("FormatX", 800, "Format X"));
			parlst.addParam(new RichInt("FormatY", 600, "Format Y"));
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
	//3. se attive, prima di "world begin" va messo il MAKETEXTURE ;)

	

	//reading cycle
	while(!files.isEmpty()) {
		bool writeLine = true;
		QString line = files.topNextLine();
		//the ReadArchive and Option SearchPath "String Archive" statement are already handled
		QStringList token = line.split(' ');

		//add "MakeTexture" statement to create the texure
		// ********* forse aqsis ha un exe che crea le texture..... ********** (pixie sì)
		if(token[0].trimmed() == "FrameBegin" && token[1].trimmed() == "1") {
			foreach(QString textureName, textureList) {
				QString makeTexture("MakeTexture \"" + getFileNameFromPath(&textureName,false) + ".tiff" + "\" \"" + getFileNameFromPath(&textureName, false) + ".tx\" \"periodic\" \"periodic\" \"gaussian\" 1 1");
				fprintf(fout,"%s\n",qPrintable(makeTexture));
			}
		}

		//change the output image file name
		if(token[0].trimmed() == "Display") {
			//if is there more frame?
			//////il + viene messo soltanto su framebuffer e non su file (o viceversa :D)
			line = token[0] + " \"+" + par.getString("ImageName") + "\" " + token[2] + " " + token[3];
			for(int i = 4; i<token.size(); i++) {
				line += token[i];
			}
		}

		//change the output image format
		if(token[0].trimmed() == "Format") {
			line = token[0] + " " + QString::number(par.getInt("FormatX")) + " " + QString::number(par.getInt("FormatY")) + " " + QString::number(par.getFloat("PixelAspectRatio"));
		}

		

		//looking for a dummy object...
		if(token[0].trimmed() == "Attribute") {
			if(token[1].trimmed() == "\"identifier\"") {
				if(token[2].trimmed() == "\"string" && token[3].trimmed() == "name\"") { //"string name"
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
								fprintf(fout,"%s\n## HO TROVATO UN OGGETTO DUMMY\n",qPrintable(line));
								convertGeometry(&files, fout, destDir, m, par, &textureList);
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
		}
		if(writeLine) {
			//copy the same line of in file
			//(out)<<line<<endl;
			fprintf(fout,"%s\n",qPrintable(line));
		}
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

//write on a opened file the geometry of the mesh (faces topology, index of vertex per face, coordinates, normal and color per vertex
int FilterHighQualityRender::convertGeometry(RibFileStack* files, FILE* fout, QString destDir, MeshModel &m, RichParameterSet &par, QStringList* textureList)
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
			templateMatrix = vcg::Matrix44f(t);
			templateMatrix = templateMatrix.transpose();
			
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
				float ratioY = dummyY / meshZ;
				float ratioZ = dummyZ / meshY;
				scale = std::min<float>(ratioX, ratioY);
				scale = std::min<float>(scale, ratioZ);
				scaleMatrix.SetScale(scale,scale,scale);
			}
			
			//center mesh
			vcg::Point3f c = m.cm.trBB().Center();
			vcg::Matrix44f translateBBMatrix;
			translateBBMatrix.SetTranslate(-c[0],-c[2],-c[1]);
			
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
			alignMatrix = alignMatrix.SetTranslate(dx,dz,dy);

			vcg::Matrix44f result = templateMatrix * alignMatrix * scaleMatrix * translateBBMatrix;			
			//write transformation matrix
			fprintf(fout,"Transform [ ");
			for(int i = 0; i<4; i++)
				for(int j = 0; j<4; j++)
					fprintf(fout,"%f ",result.ElementAt(j,i)); //traspose
			//write bounding box (not modified)
			fprintf(fout,"]\n%s\n",qPrintable(line));
			
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
					//fprintf(fout,"Surface \"paintedplastic\" \"texturename\" [\"%s.tx\"]\n", qPrintable(getFileNameFromPath(&textureName,false)));
					//fprintf(fout,"Surface \"sticky_texture\" \"texturename\" [\"%s.tx\"]\n", qPrintable(getFileNameFromPath(&textureName,false)));
					fprintf(fout,"Surface \"mytexmap\" \"name\" \"%s.tx\"\n", qPrintable(getFileNameFromPath(&textureName,false)));
					
				}
			}
		}

		if(token[0].trimmed() == "PointsPolygons") {
			QString filename = "geometry.rib";
			fprintf(fout,"ReadArchive \"%s\"", qPrintable(filename));
			QString geometryDest = destDir + QDir::separator() + filename;
			
			vcg::tri::io::ExporterRIB<CMeshO>::Save(m.cm, qPrintable(geometryDest), vcg::tri::io::Mask::IOM_ALL, scale, false);
		} 

		
		if(c == 0) {
			exit = true;
			fprintf(fout,"%s\n",qPrintable(line));
		}
	}
	//ignore the geometry of dummy object defined in the template
	//until end of file or number of statement "AttributeEnd" is equal to c (?)
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