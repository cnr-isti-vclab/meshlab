/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
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
#include <src/TextureFilter.h>
#include <src/TextureMerger.h>
#include <src/Tsai/TsaiCameraCalibration.h>

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
		//calculating angle between the camera direction and the face normal
		double angle = ((-1*cam->calibration->cameraDirection[0])*(*fi).N()[0])
						+((-1*cam->calibration->cameraDirection[1])*(*fi).N()[1])
						+((-1*cam->calibration->cameraDirection[2])*(*fi).N()[2]);
		angle = acos(angle);
		angle = (angle/M_PI)*180.0;

		//qDebug()<< "angle: " << angle;
		ft->faceAngleToCamera = angle;
		for (i=0;i<3;i++){

			double u,v;
			cam->calibration->getUVforPoint((*fi).V(i)->P()[0],(*fi).V(i)->P()[1],(*fi).V(i)->P()[2],&u,&v);

			ft->u[i] = u/cam->resolution[0];
			ft->v[i] = 1.0-v/cam->resolution[1];
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
			if(ft->faceAngleToCamera<=90.0){
				for (i=0;i<3;i++){
					(*fi).WT(i).u() = ft->u[i];
					(*fi).WT(i).v() = ft->v[i];
					(*fi).WT(i).n() = ft->textureindex;
				}
			}else{
				for (i=0;i<3;i++){
					(*fi).WT(i).u() = 0.0;
					(*fi).WT(i).v() = 0.0;
					(*fi).WT(i).n() = 0;
				}

			}

			k++;
		}
	}
}


void PhotoTexturer::unprojectToOriginalTextureMap(MeshModel *m, Camera* camera, QuadTreeNode &qtree, ImageFilterContainer *container ,bool use_distance_filter, int distance_weight, bool use_angle_filter, int angle_weight , int angle_map_sharpness, double min_angle, int imgResX, int imgResY){
	qDebug() <<"unprojectToOriginalTextureMap"<< min_angle;

	//checks if the MeshModel has original uv coordinates and camera projected uv coordinates.
	if (vcg::tri::HasPerFaceAttribute(m->cm,ORIGINALUVTEXTURECOORDS) && vcg::tri::HasPerFaceAttribute(m->cm,CAMERAUVTEXTURECOORDS)){

		CMeshO::PerFaceAttributeHandle<UVFaceTexture*> oth = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<UVFaceTexture*>(m->cm,ORIGINALUVTEXTURECOORDS);
		CMeshO::PerFaceAttributeHandle<QMap<QString,UVFaceTexture*> > cth = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<QMap<QString,UVFaceTexture*> > (m->cm,CAMERAUVTEXTURECOORDS);

		QString camname = camera->name;

		//creates a new RGBA image for saving the new texture
		//QImage image(res_x, res_y, QImage::Format_ARGB32);

		container->image = new QImage(imgResX,imgResY,QImage::Format_ARGB32);
		TextureFilter *distance_filter = NULL;
		TextureFilter *angle_filter = NULL;

		if(use_angle_filter){
			angle_filter = new TextureFilter(imgResX,imgResY,angle_weight);
		}
		if (use_distance_filter){
			distance_filter = new TextureFilter(imgResX,imgResY,distance_weight);
		}

		//loading the texture corresponding to the camera
		QImage tmp_texture(camera->textureImage);

		QColor cpixel;

		int x;
		int y;
		//CMeshO::FaceIterator fi;
		bool found = false;

		//goes pixelwise over the whole new texture image and looks if it lies inside
		//a texture face of the original texture coordinates. If the pixel lies inside
		//a textured face it looks in the corresponding camera texture for the color value
		//of this pixel and stores it at the current pixel position in the new texture image.
		for (y=0;y<imgResY;y++){
			for (x=0;x<imgResX;x++){

				//sets the current pixel to black with an alpha value of 0
				cpixel = QColor(0, 0, 0, 0);
				container->image->setPixel(x,imgResY-(y+1), cpixel.rgba());
				found = false;
				//searches the QuadTree for matching faces
				QList<QuadTreeLeaf*> list;
				qtree.getLeafs(((double)x/(double)(imgResX-1)),((double)y/(double)(imgResY-1)),list);
				int ns = list.size();

				if (ns>0){
					int idx = 0;
					while(!found && idx <ns){
						UVFaceTexture* tmp;
						tmp = dynamic_cast<UVFaceTexture*>(list.at(idx));

						double u,v;
						double a,b,c,d;
						UVFaceTexture* ct = cth[tmp->faceIndex][camname];

						tmp->getBarycentricCoordsForUV(((double)x/(double)(imgResX-1)),((double)y/(double)(imgResY-1)),a,b,c,d);

						ct->getUVatBarycentricCoords(u,v,a,b,c);
						int ix = (int)(((double)tmp_texture.width())*u);
						int iy = tmp_texture.height()-(int)(((double)tmp_texture.height())*v);

						if(ix>=0 && ix<tmp_texture.width() && iy>=0 && iy<tmp_texture.height()){

							found = true;
							//calculating alpha value of the pixel by using the angle

							cpixel = QColor(tmp_texture.pixel(ix,iy));
							cpixel.setAlpha(255);
							container->image->setPixel(x,imgResY-(y+1), cpixel.rgba());

							CFaceO f;
							f = m->cm.face.at(tmp->faceIndex);
							double angle = 0.0;
							if(use_angle_filter){
								//calculate normal vector for pixel
								double n1, n2,n3;
								n1 = a*f.V(0)->N()[0]+ b*f.V(1)->N()[0]+c*f.V(2)->N()[0];
								n2 = a*f.V(0)->N()[1]+ b*f.V(1)->N()[1]+c*f.V(2)->N()[1];
								n3 = a*f.V(0)->N()[2]+ b*f.V(1)->N()[2]+c*f.V(2)->N()[2];

								angle = ((-1*camera->calibration->cameraDirection[0])*n1)
												+((-1*camera->calibration->cameraDirection[1])*n2)
												+((-1*camera->calibration->cameraDirection[2])*n3);

								//qDebug() << "angle["<<ix<<"]["<<iy<<"]: " <<angle;


								angle = acos(angle);
								angle = (angle/M_PI)*180.0;

								if(angle<= min_angle){
									angle = (angle/180.0)*M_PI;
									angle = sin(angle);
									angle = pow(angle,angle_map_sharpness);

									angle_filter->setValue(x,imgResY-(y+1),angle);
								}
							}

							if(use_distance_filter){
								//calculate distance for pixel
								double p1, p2,p3;
								p1 = a*f.V(0)->P()[0]+ b*f.V(1)->P()[0]+c*f.V(2)->P()[0];
								p2 = a*f.V(0)->P()[1]+ b*f.V(1)->P()[1]+c*f.V(2)->P()[1];
								p3 = a*f.V(0)->P()[2]+ b*f.V(1)->P()[2]+c*f.V(2)->P()[2];

								double distance = sqrt(pow(p1-camera->calibration->cameraPosition[0],2)+pow(p2-camera->calibration->cameraPosition[1],2)+pow(p3-camera->calibration->cameraPosition[2],2));

								distance_filter->setValue(x,imgResY-(y+1),distance);
							}

							if (angle<= min_angle){
								cpixel = QColor(tmp_texture.pixel(ix,iy));
								cpixel.setAlpha(255);

							}else{
								cpixel = QColor(0,0,0,0);
								//cpixel.setAlpha(255);
							}
							container->image->setPixel(x,imgResY-(y+1), cpixel.rgba());
						}else{


						}
						//}
						idx++;
					}

				}
			}
		}
		if(use_angle_filter){
			container->addFilter(angle_filter);
		}
		if(use_distance_filter){
			container->addFilter(distance_filter);
		}
	}
}
void PhotoTexturer::getSurrundingMeanColor(QImage &image, int x, int y, QColor &surcolor){
	//qDebug()<<"getSurrundingMeanColor: "<<x<<y;
	int width = image.width();
	int height = image.height();
	if((x>=0) && (x< width) && (y>=0) && (y< height)){
		QColor c[8];
		c[0] = QColor(0,0,0,0);
		c[1] = QColor(0,0,0,0);
		c[2] = QColor(0,0,0,0);
		c[3] = QColor(0,0,0,0);
		c[4] = QColor(0,0,0,0);
		c[5] = QColor(0,0,0,0);
		c[6] = QColor(0,0,0,0);
		c[7] = QColor(0,0,0,0);
		if((x-1>=0) && (x-1< width) && (y-1>=0) && (y-1< height)){
			c[0]=QColor::fromRgba(image.pixel(x-1,y-1));
		}

		if((x>=0) && (x< width) && (y-1>=0) && (y-1< height)){
			c[1]=QColor::fromRgba(image.pixel(x,y-1));
		}
		if((x+1>=0) && (x+1< width) && (y-1>=0) && (y-1< height)){
			c[2]=QColor::fromRgba(image.pixel(x+1,y-1));
		}
		if((x+1>=0) && (x+1< width) && (y>=0) && (y< height)){
			c[3]=QColor::fromRgba(image.pixel(x+1,y));
		}
		if((x+1>=0) && (x+1< width) && (y+1>=0) && (y+1< height)){
			c[4]=QColor::fromRgba(image.pixel(x+1,y+1));
		}
		if((x>=0) && (x< width) && (y+1>=0) && (y+1< height)){
			c[5]=QColor::fromRgba(image.pixel(x,y+1));
		}
		if((x-1>=0) && (x-1< width) && (y+1>=0) && (y+1< height)){
			c[6]=QColor::fromRgba(image.pixel(x-1,y+1));
		}
		if((x-1>=0) && (x-1< width) && (y>=0) && (y< height)){
			c[7]=QColor::fromRgba(image.pixel(x-1,y));
		}
		int i;
		int r=0;
		int g=0;
		int b=0;
		int a=0;
		int count=0;
		for(i=0;i<8;i++){
			if(c[i].alpha()>0){
				r+=c[i].red();
				g+=c[i].green();
				b+=c[i].blue();
				a+= c[i].alpha();
				count++;
			}
		}
		if(count>0){
			surcolor.setRed(r/count);
			surcolor.setGreen(g/count);
			surcolor.setBlue(b/count);
			surcolor.setAlpha(a/count);
			surcolor.setAlpha(255);
		}else{
			surcolor.setRed(0);
			surcolor.setGreen(0);
			surcolor.setBlue(0);
			surcolor.setAlpha(0);
		}

	}


}


void PhotoTexturer::edgeTextureStretching(QImage &image, int pass){
	if(pass>0){
		int count = 0;
		while(pass>0){
			//qDebug()<< "edgeTextureStretching pass:" <<++count;
			QImage tmp_image = image.copy(0,0,image.width(),image.height());
			int x;
			int y;
			for(y=0;y<image.height();y++){
				for(x=0;x<image.width();x++){
					QColor test = QColor::fromRgba(image.pixel(x,y));

					if(test.alpha()==0){
						//qDebug()<< "alpha == 0";
						QColor surcolor;
						getSurrundingMeanColor(tmp_image,x,y,surcolor);
						image.setPixel(x,y,surcolor.rgba());
					}
				}
			}


			pass--;
		}
	}
}


void PhotoTexturer::combineTextures(MeshModel *m, int width, int height, int ets, bool enable_angle_map, int angle_weight,int angle_map_sharpness,double min_angle, bool enable_distance_map, int distance_weight){

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
	TextureMerger texMerger = TextureMerger();

	for (i=0;i<cameras.size();i++){
		ImageFilterContainer *ifc = new ImageFilterContainer();
		Camera *camera = cameras.at(i);
		qDebug()<<"unprojectToOriginalTextureMap";
		unprojectToOriginalTextureMap(m,camera,qtree,ifc, enable_distance_map, distance_weight, enable_angle_map,angle_weight,angle_map_sharpness,min_angle,width,height);
		texMerger.ifcList.push_back(ifc);
	}
	texMerger.normalizeFilterContainerList();
	QImage image = texMerger.mergeTextureImagesWinnerTakesAll(width,height);
	edgeTextureStretching(image,ets);
	QString filename = QString(m->fileName.c_str());
	filename = filename + "_merged.png";

	image.save(filename,"PNG");
	//delete image;
}

QImage PhotoTexturer::mergeTextureImagesWinnerTakesAll(int imgWidth, int imgHeight, QList<QImage> imgList){
	QImage image = QImage(imgWidth,imgHeight,QImage::Format_RGB32);
	int x;
	int y;
	for(x=0; x<imgWidth;x++){
		for(y=0;y<imgHeight;y++){
			QColor cpixel = QColor(0, 0, 0, 0);
			int i;
			for(i=0;i<imgList.size();i++){
				QImage tmpImg = imgList.at(i);
				QColor tmpPixel = QColor::fromRgba(tmpImg.pixel(x,y));
				if(cpixel.alpha()<tmpPixel.alpha()){
					cpixel = QColor(tmpPixel);
				}
			}
			image.setPixel(x,y,cpixel.rgba());

		}
	}
	return image;
}

void PhotoTexturer::convertToTsaiCamera(int camIdx, bool optimize, QString filename,MeshModel *mm){
	if(camIdx>=0 && camIdx< cameras.size()){
		Camera* newCam = new Camera();
		Camera* oldCam = cameras.at(camIdx);

		newCam->name = oldCam->name;
		newCam->resolution[0] = oldCam->resolution[0];
		newCam->resolution[1] = oldCam->resolution[1];
		newCam->textureImage = oldCam->textureImage;
		newCam->calibration = oldCam->calibration->calibrateToTsai(mm,optimize);

		QDomDocument doc(Camera::XML_CAMERA);

		newCam->saveAsXml(&doc,NULL);

		QFile file(filename);
		file.open(QIODevice::WriteOnly);
		QTextStream qstream(&file);
		doc.save(qstream,1);
		file.close();

	}
}

void PhotoTexturer::exportMaxScript(QString filename,MeshModel *mm){

	QFile* ms = new QFile(filename);
	if (ms->open(QIODevice::WriteOnly)){
	     QTextStream out(ms);
	     out << "(\n";
	     int i;
	     for (i=0;i<cameras.size();i++){
	    	 Camera* cam = cameras.at(i);
	    	 //TsaiCameraCalibration* tsai = dynamic_cast<TsaiCameraCalibration*>(cam->calibration);
	    	 TsaiCameraCalibration* tsai = dynamic_cast<TsaiCameraCalibration*>(cam->calibration->calibrateToTsai(mm,false));
	    	 if (tsai!=NULL){
	    		 out << "-- kappa1: "<< tsai->calib_const.kappa1<<"\n";
	    		 out << "-- Cx: "<< tsai->cam_para.Cx << "\tCy: "<<tsai->cam_para.Cy<<"\n";
	    		 out << "-- sx: "<< tsai->cam_para.sx<<"\n";
	    		 out << "-- p1: "<< tsai->calib_const.p1 << "\tp2: "<<tsai->calib_const.p2<<"\n";
	    		 out << "\n";
	    		 out << "r1 = [" << tsai->calib_const.r1<<",\t"<< -tsai->calib_const.r4<<",\t"<<-tsai->calib_const.r7<<"]\n";
	    		 out << "r2 = [" << tsai->calib_const.r2<<",\t"<< -tsai->calib_const.r5<<",\t"<<-tsai->calib_const.r8<<"]\n";
	    		 out << "r3 = [" << tsai->calib_const.r3<<",\t"<< -tsai->calib_const.r6<<",\t"<<-tsai->calib_const.r9<<"]\n";
	    		 out << "r4 = [" << tsai->calib_const.Tx<<",\t"<< -tsai->calib_const.Ty<<",\t"<<-tsai->calib_const.Tz<<"]\n";

	    		 out<< "m = matrix3 r1 r2 r3 r4\n";

	    		 out << "setRendApertureWidth ("<< tsai->cam_para.dpx * cam->resolution[0]<<")\n";//  -- horizontal size of sensor/filmback
	    		 out << "renderPixelAspect = " << tsai->cam_para.dpx/tsai->cam_para.dpy<<"\n"; //     -- non-square pixels?
	    		 out << "renderWidth = "<< cam->resolution[0] <<"\n";
	    		 out << "renderHeight = "<< cam->resolution[1] <<"\n";

	    		 out << "freecamera name:\""<< cam->name <<"\" fov:(cameraFOV.MMtoFOV "<<tsai->calib_const.f<<") transform:(inverse m)\n";
	    		 out << "\n\n";
	    	 }
	     }
	     out << ")\n";

	}else{
		qDebug()<< "Could not open max script file." <<filename;
	}


}

