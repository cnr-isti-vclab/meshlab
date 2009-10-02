#include <filter_hqrender.h>
#include <QtGui>
#include <QDir>
//#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>



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
			//parlst.addParam(new RichSaveFile("SceneName",&fv,&FileDecoration(&fv,".rib","Name of file rib to save","If not specified, the file will be removed")));
			
			
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
	QTextStream out(&outFile);

	/////controlli di coordinate, ecc.... per ora non servono... credo
	//if(m.cm.textures.size()>1 && m.cm.HasPerWedgeTexCoord() || m.cm.HasPerVertexTexCoord())

	//TEXTURE:
	//1. Take the list of texture mesh
	QStringList textureList;
	for(int i=0; i<m.cm.textures.size(); i++) {
		textureList << QString(m.cm.textures[i].c_str());
	}
	QDir tmp(destDir);
	if(!(tmp.entryList(QDir::Dirs | QDir::NoDotAndDotDot)).contains("Maps"))
		if(!tmp.mkdir("Maps"))
			return false;
		else
			tmp.cd("Maps");

	//2. Copy all texture in outDir (magari in una cartella a parte)	
	foreach(QString textureName, textureList) {
		QString str(m.fileName.c_str());
		str = getDirFromPath(&str);
		QFile srcFile(str + QDir::separator() + textureName);
		
		QString newTex = tmp.absolutePath() + QDir::separator() + getFileNameFromPath(&textureName);
		if(srcFile.exists())
			srcFile.copy(newTex);
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
		// ********* forse pixie ha un exe che crea le texture..... **********
		if(token[0].trimmed() == "FrameBegin" && token[1].trimmed() == "1") {
			foreach(QString textureName, textureList) {
				QString makeTexture("MakeTexture \"" + getFileNameFromPath(&textureName) + "\" \"" + getFileNameFromPath(&textureName, false) + ".tx\" \"periodic\" \"periodic\" \"gaussian\" 2 2");
				out<<makeTexture<<endl;
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
								(out)<<line<<endl<<"## HO TROVATO UN OGGETTO DUMMY"<<endl;
								convertGeometry(&files, out, m, par, &textureList);
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
			(out)<<line<<endl;
		}
	}

	//va copiata una quantita indefinita di file dal template e organizzata in directory ;)
	//va copiata l'immagine alla fine È_Ë
	return true;
}

//write on a opened file the geometry of the mesh (faces topology, index of vertex per face, coordinates, normal and color per vertex
int FilterHighQualityRender::convertGeometry(RibFileStack* files, QTextStream &out, MeshModel &m, RichParameterSet &par, QStringList* textureList) {
		
	float scale = 1.0;
	
	int c = 1; //the number of AttributeBegin statement readed
	bool exit = false;
	while(!files->isEmpty() && !exit) {
		QString line = files->topNextLine();
		QStringList token = line.split(' ');
		
		if(token[0].trimmed() == "AttributeBegin")
			++c;
		if(token[0].trimmed() == "AttributeEnd")
			--c;

		//autoscale
		if(token[0].trimmed() == "Bound") {
			if(par.getBool("Autoscale")) {
				float meshX = m.cm.trBB().DimX();
				float meshY = m.cm.trBB().DimY();
				float meshZ = m.cm.trBB().DimZ();
				float dummyX = token[2].toFloat() - token[1].toFloat();
				float dummyY = token[4].toFloat() - token[3].toFloat();
				float dummyZ = token[6].toFloat() - token[5].toFloat();
				float ratioX = dummyX / meshX;
				float ratioY = dummyY / meshZ;
				float ratioZ = dummyZ / meshY;
				scale = std::min<float>(ratioX, ratioY);
				scale = std::min<float>(scale, ratioZ);
			}
			out<<line<<endl; //will the bound be modify?
		}

		//BAD CODE!!!!
		if(token[0].trimmed() == "Transform" || token[0].trimmed() == "ShadingInterpolation") {
			out<<line<<endl;
			if(line.contains('[') && !line.contains(']')) { //an array maybe contains '\n'
				while(!line.contains(']')) { //se trovo la fine del file? O_O
					line = files->topNextLine();
					out<<line<<endl;
				}
			}
		}
		
		//some like switch???
		if(token[0].trimmed() == "Attribute") {
//			if(token[1].trimmed() == "\"identifier\"" || token[1].trimmed() == "\"user\"" || token[1].trimmed() == "\"displacementbound\"")
			if(token[1].trimmed() == "\"user\"" || token[1].trimmed() == "\"displacementbound\"") {
				out<<line<<endl;
				if(line.contains('[') && !line.contains(']')) { //array maybe contains '\n'
					while(!line.contains(']')) {
						line = files->topNextLine();
						out<<line<<endl;
					}
				}
			}
		}

		//texture mapping
		if(token[0].trimmed() == "Surface") {
			if(m.cm.textures.size()>1 && m.cm.HasPerWedgeTexCoord() || m.cm.HasPerVertexTexCoord()) {
				foreach(QString textureName, *textureList) {
					out<<"Surface \"paintedplastic\" \"texturename\" [\"" + getFileNameFromPath(&textureName,false) + ".tx\"]"<<endl;
				}
			}
		}

		if(token[0].trimmed() == "PointsPolygons") {
			m.updateDataMask(m.MM_ALL); //VA TOLTO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			vcg::tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
			vcg::tri::Allocator<CMeshO>::CompactFaceVector(m.cm);
		
			QTime tt; tt.start();
		
			//replace with opened mesh
			out<<"Declare \"Cs\" \"facevarying color\""<<endl;
			out<<"Declare \"st\" \"facevarying float[2]\""<<endl;
			out<<"Declare \"N\" \"facevarying normal\""<<endl;
			//first: faces topology
			out<<"PointsPolygons"<<endl<<"["<<endl;
			for(int i=0; i<m.cm.fn; i++) {
					out<<"3 ";//<<endl;
			}
			out<<"]"<<endl<<"["<<endl;
			qDebug("PointsPolygons %i",tt.elapsed());
			vcg::tri::UpdateFlags<CMeshO>::VertexClearV(m.cm);
			
			//second: index of vertex for face
			for(CMeshO::FaceIterator fi=m.cm.face.begin(); fi!=m.cm.face.end(); ++fi) {
					for(int j=0; j<3; ++j) {						
						int indexOfVertex = (*fi).V(j) - &(m.cm.vert[0]);
						out<<indexOfVertex<<" ";
						//if it's the first visit, set visited bit
						if(!(*fi).V(j)->IsV()) {
							(*fi).V(j)->SetV();
						}
//					out<<endl;
				}
			}
			out<<"]"<<endl;
			qDebug("coords %i",tt.elapsed());

			//third: vertex coordinates
			out<<"\"P\""<<endl<<"["<<endl;			
			vcg::Point3f c = m.cm.trBB().Center();
			for(CMeshO::VertexIterator vi=m.cm.vert.begin(); vi!=m.cm.vert.end(); ++vi) {
				if(vi->IsV()) {
					vcg::Point3f p;
					//p.Import(vi->P());
					p = vi->P();
					if(scale!= 1.0) {
						p[0] = (p[0] + c[0])*scale - c[0];
						p[1] = (p[1] + c[1])*scale - c[1];
						p[2] = (p[2] + c[2])*scale - c[2];
					}
					out<<p[0]<<" "<<p[2]<<" "<<p[1];//<<endl;
				}
			}
			out<<"]"<<endl;
						qDebug("coords %i",tt.elapsed());

			//fourth: vertex normal
			out<<"\"N\""<<endl<<"["<<endl;
			for(CMeshO::FaceIterator fi=m.cm.face.begin(); fi!=m.cm.face.end(); ++fi) {
				//for each face, foreach vertex write normal
					for(int j=0; j<3; ++j) {						
						vcg::Point3f &n=(*fi).V(j)->N();
						out<<float(n[0])<<" "<<float(n[1])<<" "<<float(n[2]);// << endl;						
				}
			}
			out<<"]"<<endl;
						qDebug("normal %i",tt.elapsed());

			//fifth: vertex color	
			out<<"\"Cs\""<<endl<<"["<<endl;
			for(CMeshO::FaceIterator fi=m.cm.face.begin(); fi!=m.cm.face.end(); ++fi) {
				//for each face, foreach vertex write color
					for(int j=0; j<3; ++j) {						
						vcg::Color4b &c=(*fi).V(j)->C();
						out<<float(c[0])/255<<" "<<float(c[1])/255<<" "<<float(c[2])/255<< " ";//<<endl;						
					}
			}
			out<<"]"<<endl;
						qDebug("color %i",tt.elapsed());

			//sixth: texture coordinates (for edge)
			out<<"\"st\""<<endl<<"["<<endl;
			for(CMeshO::FaceIterator fi=m.cm.face.begin(); fi!=m.cm.face.end(); ++fi) {
				//for each face, foreach vertex write uv coord
					for(int j=0; j<3; ++j) {
						//CVertexO* vertex = ((*fi).V(j));
						//vcg::TexCoord2<float,1> tc(((*fi).V(j))->T().U(),((*fi).V(j))->T().V()); ;
						//out<<tc.U()<<" "<<tc.V()<<endl;
						out<< (*fi).WT(j).U() <<" "<< (*fi).WT(j).V() << " ";// <<endl;
					}
			}
			out<<"]"<<endl;
						qDebug("texcoords %i",tt.elapsed());

		} 

		
		if(c == 0) {
			exit = true;
			out<<line<<endl; //AttributeEnd
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