#include "filter_hqrender.h"
#include <limits>

using namespace UtilitiesHQR;

bool FilterHighQualityRender::resetBound() {
	// xmin, xmax, ymin, ymax, zmin, zmax
  //default RIS bound is infinite
	for(int i=0; i< 6; i=i+2)
		objectBound[i] = std::numeric_limits<float>::min();
	for(int i=1; i<6; i=i+2)
		objectBound[i] = std::numeric_limits<float>::max();
  return true;
}

bool FilterHighQualityRender::resetGraphicsState() {
  //graphics state initialization (only interesting things)
  transfMatrix = QStack<vcg::Matrix44f>();
  transfMatrix << vcg::Matrix44f::Identity();
  surfaceShader = QStack<QString>();
  surfaceShader << ""; //nothing to set implementation-dependent default surface shader
	resetBound();
  return true;
}


//read source files and write to destination file
//if it's found the attribute name with value "dummy", the following geometry statement
//are replaced with a current mesh conversion
bool FilterHighQualityRender::makeScene(MeshModel* m,
									   QStringList* textureList,
									   RichParameterSet &par,
									   QString templatePath,
									   QString destDirString,
									   QStringList* shaderDirs,
									   QStringList* textureDirs,
									   QStringList* proceduralDirs,
									   QStringList* imagesRendered)
{
  
  //rib file structure
  RibFileStack files(getDirFromPath(&templatePath)); //constructor
	//open file and stream
	if(!files.pushFile(&templatePath)) {
		this->errorMessage = "Template path is wrong: " + templatePath;
		return false;
	}
	
	//output file
	FILE* fout;
	fout = fopen(qPrintable(destDirString + QDir::separator() + mainFileName()),"wb");
	if(fout==NULL)	{
    this->errorMessage = "Impossible to create file: " + destDirString + QDir::separator() + mainFileName();
    return false;
	}
	qDebug("Starting to write rib file into %s",qPrintable(destDirString + QDir::separator() + mainFileName()));
	
	FILE* fmain = fout; //if change the output file, the main is saved here
  convertedGeometry = false; //if the mesh is already converted
	int currentFrame = 0; //frame counter
	
  bool stop = false;
	bool currentDisplayTypeIsFile = false;
	bool anyOtherDisplayType = false;
	numberOfDummies = 0; //the dummy object could be than one (e.g. for ambient occlusion passes)
	QString newFormat = "Format " + 
						QString::number(par.getInt("FormatX")) + " " +
						QString::number(par.getInt("FormatY")) + " " +
						QString::number(par.getFloat("PixelAspectRatio"));
	numOfWorldBegin = 0; //the number of world begin statement found (for progress bar)
	numOfObject = 0;
  bool foundDummy = false;
  bool solidBegin = false; //true only if define a dummy object
	bool writeLine = true; //true if the line has to be write to final file
  resetGraphicsState(); //transformation matrix, bound, surface shader
	//reading cycle
	while(!files.hasNext() && !stop) {
		if(!solidBegin)
      writeLine = true;
    int statementType = 0; //type of statement
		QString line = files.nextStatement(&statementType); //current line
    
    switch(statementType) {
      //declare other directory for the template 
			case ribParser::OPTION: {
				QStringList token = ribParser::splitStatement(&line);
				if(token[2] == "searchpath") {
					int type = 0;
					QStringList dirList = readSearchPath(&token,&type);
					switch(type) {
						case FilterHighQualityRender::ARCHIVE:
							files.addSubDirs(dirList);
							break;
						case FilterHighQualityRender::SHADER:
							*shaderDirs = dirList;
							break;
						case FilterHighQualityRender::TEXTURE:
							*textureDirs = dirList;
							break;
						case FilterHighQualityRender::PROCEDURAL:
							*proceduralDirs = dirList;					
							break;
						case FilterHighQualityRender::ERR:
							//ignore: maybe an error or another searchpath type (not in RISpec3.2)
							break;
					}
				}
				break;
			}
      //make a map (create the path if needed)
			case ribParser::MAKE:
			case ribParser::MAKECUBEFACEENVIRONMENT:
			{
				QStringList token = ribParser::splitStatement(&line);				
				QString path = token[2]; //for MakeTexture, MakeShadow, MakeLatLongEnvironment
				if(statementType == ribParser::MAKECUBEFACEENVIRONMENT)
					path = token[7];
				path = getDirFromPath(&path);
				//qDebug("check dir! line: %s\npath: %s",qPrintable(line),qPrintable(path));
				checkDir(&destDirString,&path);
				break;
			}
      //begin a new frame
      case ribParser::FRAMEBEGIN:
			{
				QStringList token = ribParser::splitStatement(&line);				
        bool isNum;
        int i = token[1].toInt(&isNum);
				if(isNum)
          currentFrame = i;
				break;
			}
      //set output type (create the path if needed)
      case ribParser::DISPLAY: 
			{
				//if output is not a file the format must be the same!! framebuffer is ignored and commented
				QStringList token = ribParser::splitStatement(&line);							
				//create the path if needed
				QString path = token[2];
				path = getDirFromPath(&path);
				//qDebug("check dir! line: %s\npath: %s",qPrintable(line),qPrintable(path));
				checkDir(&destDirString,&path);

				//if there's more "Display" statement with one that's "file" is not considered a final image
				if(token[5] != "framebuffer")
					if (!anyOtherDisplayType && token[5] == "file") {
						currentDisplayTypeIsFile = true;
						QString img = token[2];
						if(img.startsWith('+'))
							img = img.mid(1,img.size());
						*imagesRendered << img;
					}
					else
						anyOtherDisplayType = true;
				else
					line = "#" + line; //if there's a framebuffer will be open pqsl automatically									 
				break;
			}
      //a new world description (reset graphics state)
			case ribParser::WORLDBEGIN:
			{
				numOfWorldBegin++;
				//if there's another type of display the format is not change
				if(!anyOtherDisplayType && currentDisplayTypeIsFile) {
					fprintf(fout,"%s\n", qPrintable(newFormat));					
				}
				currentDisplayTypeIsFile = false;
				anyOtherDisplayType = false;
				//is right?yes,because before the next WorldBegin will there be a new Display statement
        resetGraphicsState(); //reset the graphics state
				break;
			}
      //set transform in graphics state
      case ribParser::TRANSFORM:
			{
        transfMatrix.pop();
        transfMatrix.push(getMatrix(&line));
				break;
      }
      //set surface in graphics state
      case ribParser::SURFACE: 
			{
				//the surface shader remain the same of template
				surfaceShader.pop();
        surfaceShader.push(line);				
				break;
			}
      //set bound in graphics state
      case ribParser::BOUND: 
			{
				//take the transformation bound
				QStringList token = ribParser::splitStatement(&line);
				int index = 1;
				if(token[index] == "[")
					index++;
				for(int i=0; i<6; i++) {
					bool isNumber;
					float number = token[index+i].toFloat(&isNumber);
					if(isNumber)
						objectBound[i] = number;
				}
				break;
			}
      //looking for a dummy
      case ribParser::ATTRIBUTE: 
			{
				QStringList token = ribParser::splitStatement(&line);				
				//RISpec3.2 not specify what and how many attributes are there in renderman
				//we take care of name only
				if(token[2] == "identifier") {
					//Attribute "identifier" "string name" [ "object name" ]
					if(token[5] == "string name" || token[5] == "name") {
						int index = 7;
						if(token[index] == "[")
							index++;
						if(token[index] == "\"")
							index++;
						if(token[index].trimmed().toLower() == "dummy") {//found a dummy object?
							foundDummy = true;
						}
					}
				}
				break;
			}
      //the begin of a set of statement to define a solid
      case ribParser::SOLIDBEGIN: {
        if(foundDummy) {
          solidBegin = true;
          writeLine = false;
        }
      }
      //the end of solid definition
      case ribParser::SOLIDEND: {
        if(solidBegin) {
          QString filename = convertObject(currentFrame, destDirString, m, par, textureList);
          qDebug("dummy conversion, filename: %s",qPrintable(filename));
				  if(filename == "") { //error in parseObject
            fclose(fmain);
            return false;
          }
				  fprintf(fout,"ReadArchive \"%s\"\n",qPrintable(filename));
          
          solidBegin = false;
          foundDummy = false;
        }
        //reset bound (and surface?)
        resetBound();
        break;
      }
      //there's a geometric statement...if there was a dummy too, replace geometry
      case ribParser::GEOMETRIC: {
        if(foundDummy) {
          QString filename = convertObject(currentFrame, destDirString, m, par, textureList);
          qDebug("dummy conversion, filename: %s",qPrintable(filename));
				  if(filename == "") { //error in parseObject
            fclose(fmain);
            return false;
          }
				  fprintf(fout,"ReadArchive \"%s\"\n",qPrintable(filename));
          
          writeLine = false;
          foundDummy = false;
        }
        //reset bound (and surface?)
        resetBound();
        break;
      }
      //add a new graphics state "level"
      case ribParser::ATTRIBUTEBEGIN: 
			{
        transfMatrix.push(transfMatrix.top());
        surfaceShader.push(surfaceShader.top());
        break;
			}
      //remove a "level" to graphics state stack
      case ribParser::ATTRIBUTEEND:
      {
        transfMatrix.pop();
        surfaceShader.pop();
        break;
      }
      //the end of scene is reached
			case ribParser::NOMORESTATEMENT:
			{
				qDebug("Stack empty");
				stop = true;
				writeLine = false;
			}
		}

		if(writeLine) {
			//copy the same line in file
			fprintf(fout,"%s\n",qPrintable(line));
		}
	}
	fclose(fout);
	return true;
}


/*bool FilterHighQualityRender::makeScene(MeshModel* m,
									   QStringList* textureList,
									   RichParameterSet &par,
									   QString templatePath,
									   QString destDirString,
									   QStringList* shaderDirs,
									   QStringList* textureDirs,
									   QStringList* proceduralDirs,
									   QStringList* imagesRendered) {
	
	RibFileStack files(getDirFromPath(&templatePath)); //constructor
	//open file and stream
	if(!files.pushFile(&templatePath)) {
		this->errorMessage = "Template path is wrong: " + templatePath;
		return false;
	}
	
	//output file
	FILE* fout;
	fout = fopen(qPrintable(destDirString + QDir::separator() + mainFileName()),"wb");
	if(fout==NULL)	{
    this->errorMessage = "Impossible to create file: " + destDirString + QDir::separator() + mainFileName();
    return false;
	}
	qDebug("Starting to write rib file into %s",qPrintable(destDirString + QDir::separator() + mainFileName()));
	
	FILE* fmain = fout; //if change the output file, save the main	
	convertedGeometry = false; //if the mesh is already converted
	int currentFrame = 0;
	vcg::Matrix44f transfObject = vcg::Matrix44f::Identity();
	bool stop = false;
	bool currentDisplayTypeIsFile = false;
	bool anyOtherDisplayType = false;
	numberOfDummies = 0; //the dummy object could be than one (e.g. for ambient occlusion passes)
	QString newFormat = "Format " + 
						QString::number(par.getInt("FormatX")) + " " +
						QString::number(par.getInt("FormatY")) + " " +
						QString::number(par.getFloat("PixelAspectRatio"));
	numOfWorldBegin = 0; //the number of world begin statement found (for progress bar)
	numOfObject = 0;
	//reading cycle
	//int debugCount = 0;
	while(!files.hasNext() && !stop) {
		//if((++debugCount)%200 == 0)	qDebug("Time after %i statement: %i",debugCount,tt.elapsed());

		bool writeLine = true;
		int statementType = 0;
		QString line = files.nextStatement(&statementType);		
		
		switch(statementType) {
			case ribParser::OPTION: {
				QStringList token = ribParser::splitStatement(&line);
				//statement to declare other directory for the template
				if(token[2] == "searchpath") {
					int type = 0;
					QStringList dirList = readSearchPath(&token,&type);
					switch(type) {
						case FilterHighQualityRender::ARCHIVE:
							files.addSubDirs(dirList);
							break;
						case FilterHighQualityRender::SHADER:
							*shaderDirs = dirList;
							break;
						case FilterHighQualityRender::TEXTURE:
							*textureDirs = dirList;
							break;
						case FilterHighQualityRender::PROCEDURAL:
							*proceduralDirs = dirList;					
							break;
						case FilterHighQualityRender::ERR:
							//ignore: maybe an error or another searchpath type (not in RISpec3.2)
							break;
					}
				}
				break;
			}
			case ribParser::MAKE:
			case ribParser::MAKECUBEFACEENVIRONMENT:
			{
				QStringList token = ribParser::splitStatement(&line);				
				QString path = token[2]; //for MakeTexture, MakeShadow, MakeLatLongEnvironment
				if(statementType == ribParser::MAKECUBEFACEENVIRONMENT)
					path = token[7];
				path = getDirFromPath(&path);
				//qDebug("check dir! line: %s\npath: %s",qPrintable(line),qPrintable(path));
				checkDir(&destDirString,&path);
				break;
			}
			case ribParser::FRAMEBEGIN:
			{
				QStringList token = ribParser::splitStatement(&line);				
        bool isNum;
        int i = token[1].toInt(&isNum);
				if(isNum)
          currentFrame = i;
				break;
			}
			//case ribParser::FRAMEEND:
			//{	
			//	break;
			//}
			case ribParser::DISPLAY: 
			{
				//if output is not a file the format must be the same!! framebuffer is ignored and commented
				QStringList token = ribParser::splitStatement(&line);							
				//create the path if needed
				QString path = token[2];
				path = getDirFromPath(&path);
				//qDebug("check dir! line: %s\npath: %s",qPrintable(line),qPrintable(path));
				checkDir(&destDirString,&path);

				//if there's more "Display" statement with one that's "file" is not considered a final image
				if(token[5] != "framebuffer")
					if (!anyOtherDisplayType && token[5] == "file") {
						currentDisplayTypeIsFile = true;
						QString img = token[2];
						if(img.startsWith('+'))
							img = img.mid(1,img.size());
						*imagesRendered << img;
					}
					else
						anyOtherDisplayType = true;
				else
					line = "#" + line; //if there's a framebuffer will be open pqsl automatically									 
				break;
			}
			case ribParser::TRANSFORM:
			{
        transfObject = getMatrix(&line);
				break;
      }
			case ribParser::WORLDBEGIN:
			{
				numOfWorldBegin++;
				//if there's another type of display the format is not change
				if(!anyOtherDisplayType && currentDisplayTypeIsFile) {
					fprintf(fout,"%s\n", qPrintable(newFormat));					
				}
				currentDisplayTypeIsFile = false;
				anyOtherDisplayType = false;
				//is right?yes,because before the next WorldBegin will there be a new Display statement
        transfObject = vcg::Matrix44f::Identity(); //reset the graphics state
				break;
			}
			//case ribParser::WORLDEND:
			//{
			//	break;
			//}
			case ribParser::ATTRIBUTEBEGIN:
			{
				//is an object (peraphs)
				//write it (o convert it if it's named dummy) to another file
				QString filename = parseObject(&files, destDirString, currentFrame, m, par, textureList);
				qDebug("fuori: %s",qPrintable(filename));
				writeLine = false;
        if(filename == "") { //error in parseObject
          fclose(fmain);
          return false;
        }
				fprintf(fout,"ReadArchive \"%s\"\n",qPrintable(filename));
        break;
			}
			case ribParser::NOMORESTATEMENT:
			{
				qDebug("Stack empty");
				stop = true;
				writeLine = false;
			}
		}

		if(writeLine) {
			//copy the same line in file
			fprintf(fout,"%s\n",qPrintable(line));
		}
	}
	fclose(fout);
	return true;
}*/

//object is a sequence beetwen the statement AttributeBegin and AttributeEnd
/*
QString FilterHighQualityRender::parseObject(RibFileStack* files, QString destDirString, int currentFrame, MeshModel* m, RichParameterSet &par, QStringList* textureList) {
	QString name = "object" + QString::number(numOfObject) + ".rib";
	FILE* fout = fopen(qPrintable(destDirString + QDir::separator() + name),"wb");
	if(fout == NULL) {
    this->errorMessage = "Impossible to create file: " + destDirString + QDir::separator() + name;
    return "";
	}
	qDebug("parse object");
	//if it's a dummy object, i need the the follow value:
	ObjValues* current = new ObjValues();
	current->objectMatrix = vcg::Matrix44f::Identity();
	//default RIS bound is infinite
	for(int i=0; i< 6; i=i+2)
		current->objectBound[i] = std::numeric_limits<float>::min();
	for(int i=1; i<6; i=i+2)
		current->objectBound[i] = std::numeric_limits<float>::max();

	//write AttributeBegin statement (already readed)
	fprintf(fout,"AttributeBegin\n");

	//write next statement and looking for an object called "dummy"...
	int c = 1; //number of nestled "AttributeBegin"
	bool isDummy = false; //if is dummy stops to write
	while(c != 0 && !files->hasNext()) {		
		int statementType = 0;
		QString line = files->nextStatement(&statementType);		
		
		switch(statementType) {
			case ribParser::ATTRIBUTEBEGIN:
				++c;
				break;
			case ribParser::ATTRIBUTEEND:
				--c;
				break;
			case ribParser::SURFACE:
			{
				//the surface shader remain the same of template
				current->objectShader << line;				
				break;
			}
			case ribParser::TRANSFORM:
			{
				current->objectMatrix = getMatrix(&line);
				break;
			}
			case ribParser::BOUND:
			{
				//take the transformation bound
				QStringList token = ribParser::splitStatement(&line);
				int index = 1;
				if(token[index] == "[")
					index++;
				for(int i=0; i<6; i++) {
					bool isNumber;
					float number = token[index+i].toFloat(&isNumber);
					if(isNumber)
						current->objectBound[i] = number;
				}
				break;
			}
			case ribParser::ATTRIBUTE:
			{
				QStringList token = ribParser::splitStatement(&line);
				QString deb = "";
				foreach(QString s,token)
					deb += "str: " + s + "\n";
				//qDebug(qPrintable(deb));
				//RISpec3.2 not specify what and how many attributes are there in renderman
				//"user id" and "displacemetbound" don't be needed (perhaps)
				if(token[2] == "user" && current->objectId == "")
					current->objectId = line;	//only first time (can be nestled id attributes)
			
				if(token[2] == "displacementbound")
					current->objectDisplacementbound = line;

				if(token[2] == "identifier") {
					//Attribute "identifier" "string name" [ "object name" ]
					if(token[5] == "string name" || token[5] == "name") {
						int index = 7;
						if(token[index] == "[")
							index++;
						if(token[index] == "\"")
							index++;
						if(token[index].trimmed().toLower() == "dummy") {//found a dummy object?
							qDebug("object name is dummy");
							isDummy = true;
						}
					}
				}
				break;
			}
			case ribParser::NOMORESTATEMENT:
			{
				c = 0;
				break;
			}
		}
		
		if(!isDummy) {
			fprintf(fout,"%s\n",qPrintable(line));		
		}
	}
	fclose(fout);

	if(!isDummy)
		numOfObject++;
	else {
		qDebug("Found dummy object");
		//delete the previous file
		QDir tmp = QDir(destDirString);
		tmp.remove(name);
		//and create another one
		name = "meshF" + QString::number(currentFrame) + "O" + QString::number(numberOfDummies) + ".rib";
		numberOfDummies++;
		FILE *fmesh = fopen(qPrintable(destDirString + QDir::separator() + name),"wb");
		if(fmesh==NULL) {
      this->errorMessage = "Impossible to create the file: " + destDirString + QDir::separator() + name;
      return "";
		}
    bool res = convertObject(fmesh, destDirString, m, par, textureList, current);
    fclose(fmesh);
    if(!res)
      return "";		
	}
	delete current;
	return name;
}
*/
//write to an opened file the attribute of object entity

QString FilterHighQualityRender::convertObject(int currentFrame, QString destDir, MeshModel* m, RichParameterSet &par, QStringList* textureList)//, ObjValues* dummyValues)
{	
	QString name = "meshF" + QString::number(currentFrame) + "O" + QString::number(numberOfDummies) + ".rib";
  numberOfDummies++;
  FILE *fout = fopen(qPrintable(destDir + QDir::separator() + name),"wb");
  if(fout == NULL) {
    this->errorMessage = "Impossible to create the file: " + destDir + QDir::separator() + name;
    return "";
  }
  fprintf(fout,"AttributeBegin\n");
	//name
	fprintf(fout,"Attribute \"identifier\" \"string name\" [ \"meshlabMesh\" ]\n");
	//id
	/*if(dummyValues->objectId != "")
		fprintf(fout,"%s\n",qPrintable(dummyValues->objectId.trimmed()));
		*/
	//modify the transformation matrix
	vcg::Matrix44f scaleMatrix = vcg::Matrix44f::Identity();
	float dummyX = objectBound[1] - objectBound[0];
	float dummyY = objectBound[3] - objectBound[2];
	float dummyZ = objectBound[5] - objectBound[4];
				
	//autoscale
	float scale = 1.0;
	if(par.getBool("Autoscale")) {
		float ratioX = dummyX / m->cm.trBB().DimX();
		float ratioY = dummyY / m->cm.trBB().DimY();
		float ratioZ = dummyZ / m->cm.trBB().DimZ();
		scale = std::min<float>(ratioX, std::min<float>(ratioY, ratioZ)); //scale factor is min ratio
		scaleMatrix.SetScale(scale,scale,scale);
	}
	
	//center mesh
	vcg::Point3f c = m->cm.trBB().Center();
	vcg::Matrix44f translateBBMatrix;
	translateBBMatrix.SetTranslate(-c[0],-c[1],-c[2]);
			
	//align
	float dx = 0.0, dy = 0.0, dz = 0.0;				
	switch(par.getEnum("AlignX")) {
		case FilterHighQualityRender::TOP:
			dx = (dummyX - m->cm.trBB().DimX() * scale) / 2;	break;
        case FilterHighQualityRender::BOTTOM:
			dx = -(dummyX - m->cm.trBB().DimX() * scale) / 2; break;
        case FilterHighQualityRender::CENTER: break; //is already center
	}
	switch(par.getEnum("AlignY")) {
		case FilterHighQualityRender::TOP:
			dy = (dummyY - m->cm.trBB().DimY() * scale) / 2; break;
        case FilterHighQualityRender::BOTTOM:
			dy = -(dummyY - m->cm.trBB().DimY() * scale) / 2; break;
        case FilterHighQualityRender::CENTER: break; //is already center
	}			
	switch(par.getEnum("AlignZ")) {
		case FilterHighQualityRender::TOP:
			dz = (dummyZ - m->cm.trBB().DimZ() * scale) / 2; break;
        case FilterHighQualityRender::BOTTOM:
			dz = -(dummyZ - m->cm.trBB().DimZ() * scale) / 2; break;
        case FilterHighQualityRender::CENTER: break; //is already center
	}
	vcg::Matrix44f alignMatrix;
	alignMatrix = alignMatrix.SetTranslate(dx,dy,dz);
	vcg::Matrix44f templateMatrix = transfMatrix.top(); //by default is identity
	
	vcg::Matrix44f result = templateMatrix * alignMatrix * scaleMatrix * translateBBMatrix;
	//write transformation matrix (after transpose it)
	writeMatrix(fout, &result);
	QString bound = "Bound";
	for(int i=0; i<6; i++)
		bound += " " + QString::number(objectBound[i]);
	/*fprintf(fout,"Bound %g %g %g %g %g %g",
		m.cm.trBB().min.X(), m.cm.trBB().max.X(),
		m.cm.trBB().min.Y(), m.cm.trBB().max.Y(),
		m.cm.trBB().min.Z(), m.cm.trBB().max.Z());*/
	
	//write bounding box (not modified) /////VA MODIFICATO IL BB?????
	fprintf(fout,"%s\n",qPrintable(bound));
	
	//force the shading interpolation to smooth
    fprintf(fout,"ShadingInterpolation \"smooth\"\n");
	//displacementBound
	/*if(dummyValues->objectDisplacementbound != "")
		fprintf(fout,"%s\n",qPrintable(dummyValues->objectDisplacementbound.trimmed()));*/
	//shader
	fprintf(fout,"%s\n",qPrintable(surfaceShader.top()));
	//texture mapping (are TexCoord needed for texture mapping?)
	if(!textureList->empty() > 0 && (m->cm.HasPerWedgeTexCoord() || m->cm.HasPerVertexTexCoord())) {
		//multi-texture don't work!I need ad-hoc shader and to read the texture index for vertex..
		//foreach(QString textureName, *textureList) {

		//read only the first texture
		QString textureName = textureList->first();
    fprintf(fout,"Surface \"paintedplastic\" \"Kd\" 1.0 \"Ks\" 0.0 \"texturename\" [\"%s.tx\"]\n", qPrintable(textureName));								
	}
	//geometry
	QString filename = "geometry.rib";
	fprintf(fout,"ReadArchive \"%s\"\n", qPrintable(filename));
	if(!convertedGeometry) {
		//make the conversion only once
		convertedGeometry = true;
		QString geometryDest = destDir + QDir::separator() + filename;			
		int res = vcg::tri::io::ExporterRIB<CMeshO>::Save(m->cm, qPrintable(geometryDest), vcg::tri::io::Mask::IOM_ALL, false, cb);
    if(res != vcg::tri::io::ExporterRIB<CMeshO>::E_NOERROR) {
      fclose(fout);
	    this->errorMessage = QString(vcg::tri::io::ExporterRIB<CMeshO>::ErrorMsg(res));
      return "";
    }
    else
		  Log(GLLogStream::FILTER,"Successfully converted mesh");
	}
	fprintf(fout,"AttributeEnd\n");
  fclose(fout);
	return name;
}

//return an a list of directory (separated by ':' character)
QStringList FilterHighQualityRender::readSearchPath(const QStringList* token, int* type) {
	//the line maybe: Option "searchpath" "string type" [ ..values..]
	//            or: Option "searchpath" "type"        [ ..values..]
	int index = 5;
	*type = FilterHighQualityRender::ERR;
	if((*token)[index] == "archive" || (*token)[index] == "string archive")
		*type = FilterHighQualityRender::ARCHIVE;
	if((*token)[index] == "shader" || (*token)[index] == "string shader")
		*type = FilterHighQualityRender::SHADER;
	if((*token)[index] == "texture" || (*token)[index] == "string texture")
		*type = FilterHighQualityRender::TEXTURE;
	if((*token)[index] == "procedural" || (*token)[index] == "string procedural")
		*type = FilterHighQualityRender::PROCEDURAL;
	index = 7;
	if((*token)[index] == "[")
		index++;
	if((*token)[index] == "\"")
		index++;
	//else err?
	QStringList dirs = (*token)[index].split(':'); //is it the standard method divide dirs with character ':' ?
	return dirs;		
}

//write a vcg::Matrix44f to file
int FilterHighQualityRender::writeMatrix(FILE* fout, const vcg::Matrix44f* matrix, bool transposed) {
	fprintf(fout,"Transform [ ");
	for(int i = 0; i<4; i++)
		for(int j = 0; j<4; j++)
			fprintf(fout,"%f ",(transposed)? matrix->ElementAt(j,i) : matrix->ElementAt(i,j));
	fprintf(fout,"]\n");
	return 0;
}

//get a vcg::Matrix44f from line (and transpose it)
vcg::Matrix44f FilterHighQualityRender::getMatrix(const QString* matrixString) const {
	float t[16];
	int k=0;
	QStringList list = matrixString->split(' ');
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