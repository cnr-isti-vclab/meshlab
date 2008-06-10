#include <QtGui>
#include <QImage>
#include <QMap>
#include <QColor>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

#include <src/PhotoTexturer.h>
#include <src/UVFaceTexture.h>
#include <src/CameraCalibration.h>

#include<vcg/complex/trimesh/allocate.h>
#include <src/QuadTree/QuadTreeNode.h>


const QString PhotoTexturer::XML_PHOTOTEXTURING = "photoTexturing";

const std::string PhotoTexturer::ORIGINALUVTEXTURECOORDS = "OriginalUVTextureCoords";
const std::string PhotoTexturer::CAMERAUVTEXTURECOORDS = "CameraUVTextureCoords";


PhotoTexturer::PhotoTexturer(){
	
}
PhotoTexturer::~PhotoTexturer(){
	
}
	
void PhotoTexturer::loadConfigurationFile(QString cfgFile){
	
	QDomDocument doc;
	cameras.clear();
	QFile file(cfgFile);
	QString errorMessage;
	if (file.open(QIODevice::ReadOnly) && doc.setContent(&file, &errorMessage)){
		file.close();
		QDomElement root = doc.documentElement();
		if (root.nodeName() == XML_PHOTOTEXTURING){
			for(QDomElement element = root.firstChildElement(Camera::XML_CAMERA); !element.isNull(); element = element.nextSiblingElement(Camera::XML_CAMERA)){
				Camera* cam = new Camera();
				cam->loadFromXml(&element);
				cameras.push_back(cam);							
			}
		}
	}
}

void PhotoTexturer::saveConfigurationFile(QString cfgFile){
	QDomDocument doc(XML_PHOTOTEXTURING);
	QDomElement root = doc.createElement(XML_PHOTOTEXTURING);
	doc.appendChild(root);
	
	int i;
	for (i=0;i<cameras.size();i++){
	
		Camera *cam = cameras.at(i);
		cam->saveAsXml(&doc,&root);
	}
	
	
	QFile file(cfgFile);
	file.open(QIODevice::WriteOnly);
	QTextStream qstream(&file);
	doc.save(qstream,1);
	file.close();
}

void PhotoTexturer::addCamera(QString camFile){
	QDomDocument doc;
	QFile file(camFile);
	QString errorMessage;
	if (file.open(QIODevice::ReadOnly) && doc.setContent(&file, &errorMessage)){
		file.close();
		QDomElement root = doc.documentElement();
		if (root.nodeName() == Camera::XML_CAMERA){
			qDebug("root is camera \n");
			Camera* cam = new Camera();
			cam->loadFromXml(&root);
			cameras.push_back(cam);							
		}else{
			qDebug("root is not camera \n");
		}
	}
}

void PhotoTexturer::removeCamera(int i){
	//checks if i is a valid index of the cameras list
	//and deletes the camera on position i
	if (i>=0 && i < cameras.size()){
		cameras.removeAt(i);
	}	
}

void PhotoTexturer::storeOriginalTextureCoordinates(MeshModel *m){
	qDebug()<<"storeOriginalTextureCoordinates";
	// see http://vcg.sourceforge.net/index.php/Tutorial#User-defined_attributes
	//if (m->cm.HasPerWedgeTexCoord()){	//Problem HasPerWedgeTexCoord() returns true even if MeshModel
	//has no texture information
	
	//checks if the MeshModel has texture coordinates
	if(m->ioMask & MeshModel::IOM_WEDGTEXCOORD){
		//qDebug()<<"HasPerWedgeTexCoord";
		
		//checks if the original texture coordinates has been saved before
		if (!vcg::tri::HasPerFaceAttribute(m->cm,ORIGINALUVTEXTURECOORDS)){
			//qDebug()<<"has no OriginalTextureCoords";
			
			CMeshO::PerFaceAttributeHandle<UVFaceTexture*> ih = vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<UVFaceTexture*> (m->cm,ORIGINALUVTEXTURECOORDS);
			
			//saves the texture information for each face as perFaceAttribute
			CMeshO::FaceIterator fi; int i = 0;
			for(fi   = m->cm.face.begin(); fi != m->cm.face.end(); ++fi,i++){
				UVFaceTexture* uvft = new UVFaceTexture();
				uvft->u[0] = (*fi).WT(0).u();
				uvft->v[0] = (*fi).WT(0).v(); 
				
				uvft->u[1] = (*fi).WT(1).u();
				uvft->v[1] = (*fi).WT(1).v(); 
				
				uvft->u[2] = (*fi).WT(2).u();
				uvft->v[2] = (*fi).WT(2).v(); 
				
				uvft->textureindex = (*fi).WT(0).n();
				uvft->faceIndex = i;
						
				ih[i]  = uvft;   // [] operator takes a iterator
			}
		}else{
			//qDebug()<<"has OriginalTextureCoords";
		}
	}else{
		//qDebug()<<"!HasPerWedgeTexCoord";
	}
	
}

void PhotoTexturer::restoreOriginalTextureCoordinates(MeshModel *m){
	//qDebug() << "restoreOriginalTextureCoordinates";
	// see http://vcg.sourceforge.net/index.php/Tutorial#User-defined_attributes

	//checks if the original texture informationof the MeshModel were stored
	if (vcg::tri::HasPerFaceAttribute(m->cm,ORIGINALUVTEXTURECOORDS)){
		//qDebug()<<"has OriginalTextureCoords";
		CMeshO::PerFaceAttributeHandle<UVFaceTexture*> ih = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<UVFaceTexture*> (m->cm,ORIGINALUVTEXTURECOORDS);
		
		//overwrites the current texture information with the original texture information
		// from the perFaceAttribute "ORIGINALUVTEXTURECOORDS"
		CMeshO::FaceIterator fi; int i = 0;
		for(fi   = m->cm.face.begin(); fi != m->cm.face.end(); ++fi,++i){
			UVFaceTexture* uvft = ih[fi] ;
			
			(*fi).WT(0).u() = uvft->u[0];
			(*fi).WT(0).v() = uvft->v[0];
			(*fi).WT(0).n() = uvft->textureindex;
			
			(*fi).WT(1).u() = uvft->u[1];
			(*fi).WT(1).v() = uvft->v[1];
			(*fi).WT(1).n() = uvft->textureindex;
			
			(*fi).WT(2).u() = uvft->u[2];
			(*fi).WT(2).v() = uvft->v[2];
			(*fi).WT(2).n() = uvft->textureindex;

		}
	}else{
		//qDebug()<<"has no OriginalTextureCoords";
	}	
}



void PhotoTexturer::calculateMeshTextureForAllCameras(MeshModel *m){
	//checks if the MeshModel already has the perfaceAttribute PhotoTexturer::CAMERAUVTEXTURECOORDS
	//if not it creates one
	if (!vcg::tri::HasPerFaceAttribute(m->cm,CAMERAUVTEXTURECOORDS)){
		//qDebug()<<"has no PhotoTexturingUVCoords";
		vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<QMap<QString,UVFaceTexture*> > (m->cm,CAMERAUVTEXTURECOORDS);
	}
	
	//enables texture information for the MeshModel 
	m->updateDataMask(MeshModel::MM_WEDGTEXCOORD);
	//makes sure that the mesh model mask enabels texture coordinates (needed to save the uv coorinates later)
	m->ioMask |= MeshModel::IOM_WEDGTEXCOORD;

	//calculates the texture information (uv coordinates and texture index) for each camera
	int i;
	for (i=0;i<cameras.size();i++){
		Camera *cam = cameras.at(i);
		calculateMeshTextureForCamera(cam,m);
	}
}

void PhotoTexturer::calculateMeshTextureForCamera(Camera* cam,MeshModel *m){
	bool found = false;
	unsigned int size = static_cast<unsigned int>(m->cm.textures.size());
	unsigned j = 0;
	int tindx;
	
	//gets the perFaceAttributeHandler for the perFaceAttribute PhotoTexturer::CAMERAUVTEXTURECOORDS
	CMeshO::PerFaceAttributeHandle<QMap<QString,UVFaceTexture*> > ih = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<QMap<QString,UVFaceTexture*> > (m->cm,CAMERAUVTEXTURECOORDS);

	//loads the texture image and gets its dimensions
	QImage *img = new QImage(cam->textureImage);
	int imgw = img->width();
	int imgh = img->height();
	
	//looks if the texture image is allready loaded and stores the index of the texture
	//under tindx
	while (!found && (j < size))
	{
		if (cam->textureImage.toStdString().compare(m->cm.textures[j])==0)
		{
			tindx = (int)j;
			found = true;
		}
		++j;
	}
	
	if (!found)
	{
		m->cm.textures.push_back(cam->textureImage.toStdString());
		tindx = (int)size;
	}
	
	
	//cam->calibration->calibrateToTsai(m);
	
	//calculates the uv coordinates for each face and saves them as UVFaceTexture as
	//perFaceAttribute of the MeshModel
	CMeshO::FaceIterator fi;
	int count = 0;
	for(fi=m->cm.face.begin(); fi!=m->cm.face.end(); ++fi, count++) {
		int i;

		UVFaceTexture *ft = new UVFaceTexture();
		for (i=0;i<3;i++){

			double u,v;
			cam->calibration->getUVforPoint((*fi).V(i)->P()[0],(*fi).V(i)->P()[1],(*fi).V(i)->P()[2],&u,&v);

			ft->u[i] = u/imgw;
			ft->v[i] = 1.0-v/imgh;	
		}
		ft->textureindex =tindx;
		ft->faceIndex = count;
		ih[ft->faceIndex][cam->name]=ft;
	
	}

	cam->calculatedTextures = true;
}

void PhotoTexturer::applyTextureToMesh(MeshModel *m,int camIdx){
	
	if (vcg::tri::HasPerFaceAttribute(m->cm,CAMERAUVTEXTURECOORDS)){
	
		CMeshO::PerFaceAttributeHandle<QMap<QString,UVFaceTexture*> > ih = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<QMap<QString,UVFaceTexture*> > (m->cm,CAMERAUVTEXTURECOORDS);
		CMeshO::FaceIterator fi;
		int k =0;
		for(fi=m->cm.face.begin(); fi!=m->cm.face.end(); ++fi) {
			int i;
			UVFaceTexture* ft = ih[fi][cameras.at(camIdx)->name];
			for (i=0;i<3;i++){
				(*fi).WT(i).u() = ft->u[i];
				(*fi).WT(i).v() = ft->v[i];
				(*fi).WT(i).n() = ft->textureindex;
			}
			k++;
		}
	}
}

void PhotoTexturer::unprojectToOriginalTextureMap(MeshModel *m, Camera *camera, QuadTreeNode &qtree,int res_x, int res_y){
	//qDebug() <<"unprojectToOriginalTextureMap";
	
	//checks if the MeshModel has original uv coordinates and camera projected uv coordinates. 
	if (vcg::tri::HasPerFaceAttribute(m->cm,ORIGINALUVTEXTURECOORDS) && vcg::tri::HasPerFaceAttribute(m->cm,CAMERAUVTEXTURECOORDS)){
	
		CMeshO::PerFaceAttributeHandle<UVFaceTexture*> oth = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<UVFaceTexture*>(m->cm,ORIGINALUVTEXTURECOORDS);
		CMeshO::PerFaceAttributeHandle<QMap<QString,UVFaceTexture*> > cth = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<QMap<QString,UVFaceTexture*> > (m->cm,CAMERAUVTEXTURECOORDS);
		
		QString camname = camera->name;
		
		//creates a new RGBA image for saving the new texture
		QImage image(res_x, res_y, QImage::Format_ARGB32);
		
		//loading the texture corresponding to the camera
		QImage tmp_texture(camera->textureImage);
		
		QColor cpixel;
		
		int x;
		int y;
		CMeshO::FaceIterator fi;
		bool found = false;
		
		//goes pixelwise over the whole new texture image and looks if it lies inside
		//a texture face of the original texture coordinates. If the pixel lies inside
		//a textured face it looks in the corresponding camera texture for the color value 
		//of this pixel and stores it at the current pixel position in the new texture image.
		for (y=0;y<res_y;y++){
			for (x=0;x<res_x;x++){
				
				//sets the current pixel to black with an alpha value of 0
				cpixel = QColor(0, 0, 0, 0);
				image.setPixel(x,res_y-(y+1), cpixel.rgba());
				
				found = false;
				//searches the QuadTree for matching faces
				QList<QuadTreeLeaf*> list; 
				qtree.getLeafs(((double)x/(double)(res_x-1)),((double)y/(double)(res_y-1)),list);
				if (list.size()>0){
					
					UVFaceTexture* tmp;
					tmp = dynamic_cast<UVFaceTexture*>(list.first());
					
					double u,v;
					double a,b,c,d;
					UVFaceTexture* ct = cth[tmp->faceIndex][camname];
					
					tmp->getBarycentricCoordsForUV(((double)x/(double)(res_x-1)),((double)y/(double)(res_y-1)),a,b,c,d);
					ct->getUVatBarycentricCoords(u,v,a,b,c);
					
					int ix = (int)(((double)tmp_texture.width())*u);
					int iy = tmp_texture.height()-(int)(((double)tmp_texture.height())*v);
					if(ix>=0 && ix<tmp_texture.width() && iy>=0 && iy<tmp_texture.height()){
						cpixel = QColor(tmp_texture.pixel(ix,iy));
						cpixel.setAlpha(255);
						image.setPixel(x,res_y-(y+1), cpixel.rgba());
					}else{

					
					}			
					
				}
			}
		}
		QString filename = QString(m->fileName.c_str());
		filename = filename + "_" + camera->name + ".png";
		image.save(filename,"PNG");
	}
	//qDebug() <<"unprojectToOriginalTextureMap DONE";
}

void PhotoTexturer::combineTextures(MeshModel *m, int width, int height){
	
	QList<QuadTreeLeaf*> *list = new QList<QuadTreeLeaf*>();
	CMeshO::PerFaceAttributeHandle<UVFaceTexture*> oth = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<UVFaceTexture*>(m->cm,ORIGINALUVTEXTURECOORDS);
	CMeshO::FaceIterator fi;
	for(fi = m->cm.face.begin();fi!=m->cm.face.end();fi++) {
		list->push_back(oth[fi]);
	}
	
	QuadTreeNode qtree = QuadTreeNode(0.0,0.0,1.0,1.0);
	//qDebug() << "list->size(): "<<list->size();
	//qDebug()<< "buildQuadTree";
	qtree.buildQuadTree(list, 0.50/(double)width,0.50/(double)height);
	
	//qDebug()<< "buildQuadTree DONE";
	int i;
	for (i=0;i<cameras.size();i++){
		unprojectToOriginalTextureMap(m,cameras.at(i),qtree,width,height);
	}
	
	
}

