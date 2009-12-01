#include "filter_hqrender.h"

using namespace UtilitiesHQR;

bool FilterHighQualityRender::makeScene(MeshModel &m,
									   QStringList* textureList,
									   RichParameterSet &par,
									   QString templatePath,
									   QString destDirString,
									   QStringList* shaderDirs,
									   QStringList* textureDirs,
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
	
	}
	qDebug("Starting to write rib file into %s",qPrintable(destDirString + QDir::separator() + mainFileName()));
	
	FILE* fmain = fout; //if change the output file, save the main	
	convertedGeometry = false; //if the mesh is already converted
	int currentFrame = 0;
	int numOfFrames = par.getInt("frames");
	vcg::Matrix44f transfCamera = vcg::Matrix44f::Identity();
	bool stop = false;
	bool isFrameDeclaration = false;
	QStringList frameDeclaration = QStringList();
	bool currentDisplayTypeIsFile = false;
	bool anyOtherDisplayType = false;
	int numberOfDummies = 0; //the dummy object could be than one (e.g. for ambient occlusion passes)
	QString newFormat = "Format " + 
						QString::number(par.getInt("FormatX")) + " " +
						QString::number(par.getInt("FormatY")) + " " +
						QString::number(par.getFloat("PixelAspectRatio"));
	numOfWorldBegin = 0; //the number of world begin statement found (for progress bar)

	//reading cycle
	//int debugCount = 0;
	while(!files.isEmpty() && !stop) {
		//if((++debugCount)%200 == 0)	qDebug("Time after %i statement: %i",debugCount,tt.elapsed());

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
					*shaderDirs = dirList;
					break;
				case FilterHighQualityRender::TEXTURE:
					*textureDirs = dirList;
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
			//qDebug("check dir! line: %s\npath: %s",qPrintable(line),qPrintable(path));
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
			//qDebug("check dir! line: %s\npath: %s",qPrintable(line),qPrintable(path));
			checkDir(destDirString,path);

			//if there's more "Display" statement with one that's "file" is not considered a final image
			if(token[2].trimmed() != "\"framebuffer\"")
				if (!anyOtherDisplayType && token[2].trimmed() == "\"file\"") {
					currentDisplayTypeIsFile = true;
					QString img = token[1].trimmed().remove('\"');
					if(img.startsWith('+'))
						img = img.mid(1,img.size());
					*imagesRendered << img;
				}
				else
					anyOtherDisplayType = true;
			else
				line = "#" + line; //if there's a framebuffer will be open pqsl automatically
		}		

		//transformation camera
		if(numOfFrames > 0 && token[0].trimmed() == "Transform") {
			transfCamera = readMatrix(&files, line);
			writeMatrix(fout,transfCamera);
			writeLine = false;
		}

		//make another file if there is an animation
		if(token[0].trimmed() == "WorldBegin") {
			numOfWorldBegin++;
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
									convertObject(&files, fmesh, destDirString, m, par, textureList, cb);
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
	return true;
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
                                case FilterHighQualityRender::TOP:
					dx = (dummyX - m.cm.trBB().DimX() * scale) / 2;
					break;
                                case FilterHighQualityRender::BOTTOM:
					dx = -(dummyX - m.cm.trBB().DimX() * scale) / 2;
					break;
                                case FilterHighQualityRender::CENTER: break; //is already center
			}
			switch(par.getEnum("AlignY")) {
                                case FilterHighQualityRender::TOP:
					dy = (dummyY - m.cm.trBB().DimY() * scale) / 2;
					break;
                                case FilterHighQualityRender::BOTTOM:
					dy = -(dummyY - m.cm.trBB().DimY() * scale) / 2;
					break;
                                case FilterHighQualityRender::CENTER: break; //is already center
			}			
			switch(par.getEnum("AlignZ")) {
                                case FilterHighQualityRender::TOP:
					dz = (dummyZ - m.cm.trBB().DimZ() * scale) / 2;
					break;
                                case FilterHighQualityRender::BOTTOM:
					dz = -(dummyZ - m.cm.trBB().DimZ() * scale) / 2;
					break;
                                case FilterHighQualityRender::CENTER: break; //is already center
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

int FilterHighQualityRender::writeMatrix(FILE* fout, vcg::Matrix44f matrix, bool transposed) {
	fprintf(fout,"Transform [ ");
	for(int i = 0; i<4; i++)
		for(int j = 0; j<4; j++)
			fprintf(fout,"%f ",(transposed)? matrix.ElementAt(j,i) : matrix.ElementAt(i,j));
	fprintf(fout,"]\n");
	return 0;
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

//read array from the rib stack (an array can be contain \n character
QString FilterHighQualityRender::readArray(RibFileStack* files, QString arrayString) {
	QString str = arrayString;
	while(!(str.contains('[') && str.contains(']')))
		str += files->topNextLine();
	return str;
}
