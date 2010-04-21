#include "io_renderman.h"
#include <limits>

using namespace UtilitiesHQR;

//reset the current bound to infinite (default RIS)
bool IORenderman::resetBound() {
	// xmin, xmax, ymin, ymax, zmin, zmax
  for(int i=0; i< 6; i=i+2)
		objectBound[i] = std::numeric_limits<float>::min();
	for(int i=1; i<6; i=i+2)
		objectBound[i] = std::numeric_limits<float>::max();
  return true;
}

//reset graphics state (transformation matrix, bound, surface shader) to default value
bool IORenderman::resetGraphicsState() {
  //graphics state initialization (only interesting things)
  transfMatrixStack = QStack<vcg::Matrix44f>();
  transfMatrixStack << vcg::Matrix44f::Identity();
  surfaceShaderStack = QStack<QString>();
  surfaceShaderStack << ""; //nothing to set implementation-dependent default surface shader
	resetBound();
  return true;
}

//read source files and write to destination file
//if it's found the attribute name with value "dummy", the following geometry statement
//are replaced with a current mesh conversion
bool IORenderman::makeScene(MeshModel* m,
									   QStringList* textureList,
									   const RichParameterSet &par,
                     QFileInfo* templateFile,
									   QString destDirString,
									   QStringList* shaderDirs,
									   QStringList* textureDirs,
									   QStringList* proceduralDirs,
									   QStringList* imagesRendered)
{
  
  //rib file structure
  RibFileStack files(templateFile->absolutePath()); //constructor
	//open file and stream
  if(!files.pushFile(templateFile->absoluteFilePath())) {
		this->errorMessage = "Template path is wrong: " + templateFile->absoluteFilePath();
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
  QQueue<Procedure> procedures = QQueue<Procedure>(); //every time it's found a procedural call it's stored here
	
  //reading cycle
	while(files.hasNext() && !stop) {
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
						case IORenderman::ARCHIVE:
							files.addSubDirs(dirList);
							break;
						case IORenderman::SHADER:
							*shaderDirs = dirList;
							break;
						case IORenderman::TEXTURE:
							*textureDirs = dirList;
							break;
						case IORenderman::PROCEDURAL:
							files.addSubDirs(dirList);
							*proceduralDirs = dirList;					
							break;
						case IORenderman::ERR:
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
				path = QFileInfo(path).path();
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
        if(path.startsWith('+.'))
				  path = path.mid(2,path.size());
				path = QFileInfo(path).path();
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
				//else
					//line = "#" + line; //if there's a framebuffer will be open pqsl automatically									 
				break;
			}
      case ribParser::SCREENWINDOW:
      {
        writeLine = false;
        break;
      }
      //a new world description (reset graphics state)
			case ribParser::WORLDBEGIN:
			{
        //make the conversion of texture mesh before the first WorldBegin statement
        if(numOfWorldBegin == 0 && !textureList->empty() && (m->cm.HasPerWedgeTexCoord() || m->cm.HasPerVertexTexCoord())) {
          QString textureName = QFileInfo(textureList->first()).completeBaseName();
          fprintf(fout,"MakeTexture \"%s.tiff\" \"%s.tx\" \"periodic\" \"periodic\" \"box\" 1 1\n", qPrintable(textureName),qPrintable(textureName));
        }
  
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
        transfMatrixStack.pop();
        transfMatrixStack.push(getMatrix(&line));
				break;
      }
      //set surface in graphics state
      case ribParser::SURFACE: 
			{
				//the surface shader remain the same of template
				surfaceShaderStack.pop();
        surfaceShaderStack.push(line);
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
        if(solidBegin) { //if and only if foundDummy is true
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
        transfMatrixStack.push(transfMatrixStack.top());
        surfaceShaderStack.push(surfaceShaderStack.top());
        break;
			}
      //remove a "level" to graphics state stack
      case ribParser::ATTRIBUTEEND:
      {
        transfMatrixStack.pop();
        surfaceShaderStack.pop();
        break;
      }
      //a procedural statement: managed at the end of cycle
      case ribParser::PROCEDURAL: 
      {
        //manage only dealayedreadarchive
        //0: Procedural 
        //1: " 
        //2: DelayedReadArchive
        //3: " 
        //4: [ 
        //5: " 
        //6: filename 
        //7: " 
        //8: ]
        //9: [
        //10-15: bound element
        //11: ]
        QStringList token = ribParser::splitStatement(&line);
        if(token[2] == "DelayedReadArchive") {
          qDebug("found a procedural: %s",qPrintable(token[6]));
          Procedure p = Procedure();
          p.name = token[6];
          p.matrix = transfMatrixStack.top();
          p.surfaceShader = surfaceShaderStack.top();
          //have i to read bound from actual graphics state or from procedural call?
          for(int i = 0; i < 6; i++)
            p.bound[i] = token[10 + i].toFloat(); //from procedural call (don't check if it's a number)
            //p.bound[i] = objectBound[i]; //from actual graphics state
          procedures.enqueue(p);
        }
        break;				
      }
      //the end of scene is reached
			case ribParser::NOMORESTATEMENT:
			{
				qDebug("Stack empty");
        stop = true;
				writeLine = false;        
			}
		} //end of switch

		if(writeLine) {
			//copy the same line in file
			fprintf(fout,"%s\n",qPrintable(line));
		}
    
    if((!files.hasNext() || stop) && !procedures.isEmpty()) {
      qDebug("There's a procedural to manage");
      //continue the cycle over procedure files..
      Procedure p = procedures.dequeue();
      //add procedure to rib file stack if exist
      bool noProc = false;
      while(!files.searchFile(p.name) && !noProc)
        if(procedures.isEmpty())
          noProc = true;
        else
          p = procedures.dequeue();
      
      if(!noProc) { //it's true only if all procedures elements don't exist
        fclose(fout);
        fout = fopen(qPrintable(destDirString + QDir::separator() + p.name),"wb");
	      if(fout==NULL)	{
          this->errorMessage = "Impossible to create file: " + destDirString + QDir::separator() + p.name;
          return false;
	      }
	      qDebug("Starting to write rib file into %s",qPrintable(destDirString + QDir::separator() + p.name));
	
        //restore the graphics state to the procedure call state
        transfMatrixStack << p.matrix;
        surfaceShaderStack << p.surfaceShader;
        for(int i = 0; i < 6; i++)
          objectBound[i] = p.bound[i];
        //continue cycle
        writeLine = true;
        stop = false;
      }
    }

	} //end of cycle
	fclose(fout);
  
	return true;
}

//write to an opened file the attribute of object entity
QString IORenderman::convertObject(int currentFrame, QString destDir, MeshModel* m,const RichParameterSet &par, QStringList* textureList)//, ObjValues* dummyValues)
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
		case IORenderman::TOP:
			dx = (dummyX - m->cm.trBB().DimX() * scale) / 2;	break;
        case IORenderman::BOTTOM:
			dx = -(dummyX - m->cm.trBB().DimX() * scale) / 2; break;
        case IORenderman::CENTER: break; //is already center
	}
	switch(par.getEnum("AlignY")) {
		case IORenderman::TOP:
			dy = (dummyY - m->cm.trBB().DimY() * scale) / 2; break;
        case IORenderman::BOTTOM:
			dy = -(dummyY - m->cm.trBB().DimY() * scale) / 2; break;
        case IORenderman::CENTER: break; //is already center
	}			
	switch(par.getEnum("AlignZ")) {
		case IORenderman::TOP:
			dz = (dummyZ - m->cm.trBB().DimZ() * scale) / 2; break;
        case IORenderman::BOTTOM:
			dz = -(dummyZ - m->cm.trBB().DimZ() * scale) / 2; break;
        case IORenderman::CENTER: break; //is already center
	}
	vcg::Matrix44f alignMatrix;
	alignMatrix = alignMatrix.SetTranslate(dx,dy,dz);
	vcg::Matrix44f templateMatrix = transfMatrixStack.top(); //by default is identity
	
	vcg::Matrix44f result = templateMatrix * alignMatrix * scaleMatrix * translateBBMatrix;
	//write transformation matrix (after transpose it)
	writeMatrix(fout, &result);
	//write bounding box
	fprintf(fout,"Bound %g %g %g %g %g %g\n",
		m->cm.trBB().min.X(), m->cm.trBB().max.X(),
		m->cm.trBB().min.Y(), m->cm.trBB().max.Y(),
		m->cm.trBB().min.Z(), m->cm.trBB().max.Z());

	//force the shading interpolation to smooth
  fprintf(fout,"ShadingInterpolation \"smooth\"\n");
	//shader
	fprintf(fout,"%s\n",qPrintable(surfaceShaderStack.top()));
	//texture mapping (are TexCoord needed for texture mapping?)
	if(!textureList->empty() > 0 && (m->cm.HasPerWedgeTexCoord() || m->cm.HasPerVertexTexCoord())) {
		//multi-texture don't work!I need ad-hoc shader and to read the texture index for vertex..
		//foreach(QString textureName, *textureList) {

		//read only the first texture
		QString textureName = QFileInfo(textureList->first()).completeBaseName();
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
QStringList IORenderman::readSearchPath(const QStringList* token, int* type) {
	//the line maybe: Option "searchpath" "string type" [ ..values..]
	//            or: Option "searchpath" "type"        [ ..values..]
	int index = 5;
	*type = IORenderman::ERR;
	if((*token)[index] == "archive" || (*token)[index] == "string archive")
		*type = IORenderman::ARCHIVE;
	if((*token)[index] == "shader" || (*token)[index] == "string shader")
		*type = IORenderman::SHADER;
	if((*token)[index] == "texture" || (*token)[index] == "string texture")
		*type = IORenderman::TEXTURE;
	if((*token)[index] == "procedural" || (*token)[index] == "string procedural")
		*type = IORenderman::PROCEDURAL;
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
int IORenderman::writeMatrix(FILE* fout, const vcg::Matrix44f* matrix, bool transposed) {
	fprintf(fout,"Transform [ ");
	for(int i = 0; i<4; i++)
		for(int j = 0; j<4; j++)
			fprintf(fout,"%f ",(transposed)? matrix->ElementAt(j,i) : matrix->ElementAt(i,j));
	fprintf(fout,"]\n");
	return 0;
}

//get a vcg::Matrix44f from line (and transpose it)
vcg::Matrix44f IORenderman::getMatrix(const QString* matrixString) const {
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
