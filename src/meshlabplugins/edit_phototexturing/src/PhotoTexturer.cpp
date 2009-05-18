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

#include <cfloat>
#include <iostream>
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
#include <vcg/math/matrix44.h>
#include <src/QuadTree/QuadTreeNode.h>

#include "photo_texture_tools.h"

#include <src/WinnerTakesAllTextureMerger.h>
#include <src/SmartBlendTextureMerger.h>


#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/bounding.h>



const QString PhotoTexturer::XML_PHOTOTEXTURING = "photoTexturing";

//const std::string PhotoTexturer::ORIGINALUVTEXTURECOORDS = "OriginalUVTextureCoords";
const std::string PhotoTexturer::UVTEXTURECOORDS = "UVTextureCoords";

const QString PhotoTexturer::TEXTURE_SIZE_WIDTH = "pt_texture_width";
const QString PhotoTexturer::TEXTURE_SIZE_HEIGHT = "pt_texture_height";

const QString PhotoTexturer::UNPROJECT_ENABLE_ANGLE = "pt_enable_angle";
const QString PhotoTexturer::UNPROJECT_ANGLE = "pt_angle";
const QString PhotoTexturer::UNPROJECT_ANGLE_WEIGHT = "pt_angle_weight";
const QString PhotoTexturer::UNPROJECT_ANGLE_SHARPNESS = "pt_angle_sharpness";

const QString PhotoTexturer::UNPROJECT_ENABLE_DISTANCE = "pt_enable_distance";
const QString PhotoTexturer::UNPROJECT_DISTANCE_WEIGHT = "pt_distance_weight";
const QString PhotoTexturer::UNPROJECT_DISTANCE_SHARPNESS = "pt_distance_shjarpness";

const QString PhotoTexturer::UNPROJECT_ENABLE_EDGE_STRETCHING = "pt_enable_edge_stretching";
const QString PhotoTexturer::UNPROJECT_EDGE_STRETCHING_PASS = "pt_edge_stretching_pass";

const QString PhotoTexturer::UNPROJECT_TEXTURE_FILENAME = "pt_unproject_texture_name";


const QString PhotoTexturer::BAKE_SAVE_UNPROJECT = "pt_save_unproject";

const QString PhotoTexturer::BAKE_MERGE_TEXTURES = "pt_merge_textures";
const QString PhotoTexturer::BAKE_MERGE_TYPE = "pt_merge_type";
const QString PhotoTexturer::BAKE_MERGED_TEXTURE = "pt_merged_texture_file";
const QString PhotoTexturer::BAKE_SMARTBLEND = "pt_smartblend_command";

#define ZB_EPSILON 1e-2


PhotoTexturer::PhotoTexturer(){
	origTextureID = -1;
	nextTextId = 0;
	bakeCounter = 0;
	combineCounter = 0;
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

int PhotoTexturer::generateTextureId(){
	return ++nextTextId;
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
		qDebug() << root.nodeName() ;
		if (root.nodeName() == Camera::XML_CAMERADOCUMENT){
			
			QDomElement xml_cam = root.firstChildElement(Camera::XML_CAMERA);
			if (!xml_cam.isNull()){
						Camera* cam = new Camera();
						cam->loadFromXml(&xml_cam);
						cameras.push_back(cam);
			}
		}else{
			qDebug("root is not camera \n");
		}
	}else{
		qDebug()<< "errorMessage: " << errorMessage;
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
	if(m->hasDataMask(MeshModel::MM_WEDGTEXCOORD) ){
		//qDebug()<<"HasPerWedgeTexCoord";
		CMeshO::PerFaceAttributeHandle<QMap<int,UVFaceTexture*> >ih;
		if (!vcg::tri::HasPerFaceAttribute(m->cm,UVTEXTURECOORDS)){
			//qDebug()<<"has no PhotoTexturingUVCoords";
			ih = vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<QMap<int,UVFaceTexture*> >(m->cm,UVTEXTURECOORDS);		
		}else{
			ih = vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<QMap<int,UVFaceTexture*> >(m->cm,UVTEXTURECOORDS);
		}
		//checks if the original texture coordinates has been saved before
	
		if (origTextureID == -1){
			//qDebug()<<"has no OriginalTextureCoords";
			origTextureID = generateTextureId();
			//CMeshO::PerFaceAttributeHandle<QMap<int,UVFaceTexture*> >ih = vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<QMap<int,UVFaceTexture*> >(m->cm,UVTEXTURECOORDS);

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

				ih[i][origTextureID]  = uvft;   // [] operator takes a iterator
			}
			textureList[origTextureID]="original";
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
	if (origTextureID!= -1 && vcg::tri::HasPerFaceAttribute(m->cm,UVTEXTURECOORDS)){
		applyTextureToMesh(m,origTextureID);

	}else{
		//qDebug()<<"has no OriginalTextureCoords";
	}
}



void PhotoTexturer::calculateMeshTextureForAllCameras(MeshModel *m, bool calcZBuffer){
	//checks if the MeshModel already has the perfaceAttribute PhotoTexturer::CAMERAUVTEXTURECOORDS
	//if not it creates one


	//enables texture information for the MeshModel
	//m->updateDataMask(MeshModel::MM_WEDGTEXCOORD);
	//makes sure that the mesh model mask enabels texture coordinates (needed to save the uv coorinates later)
	//m->ioMask |= MeshModel::IOM_WEDGTEXCOORD;

	
	//checks if special transformation data is stored asMeshData

	
	
	//calculates the texture information (uv coordinates and texture index) for each camera
	int i;
	for (i=0;i<cameras.size();i++){
		Camera *cam = cameras.at(i);
		calculateMeshTextureForCamera(m,cam, calcZBuffer);
	}
}

void PhotoTexturer::calculateMeshTextureForCamera(MeshModel *m, Camera* cam,bool calcZBuffer){
	bool found = false;
	unsigned int size = static_cast<unsigned int>(m->cm.textures.size());
	unsigned j = 0;
	int tindx;

	//gets the perFaceAttributeHandler for the perFaceAttribute PhotoTexturer::CAMERAUVTEXTURECOORDS
	CMeshO::PerFaceAttributeHandle<QMap<int ,UVFaceTexture*> > ih;
	
	if (!vcg::tri::HasPerFaceAttribute(m->cm,UVTEXTURECOORDS)){
		//qDebug()<<"has no PhotoTexturingUVCoords";
		ih = vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<QMap<int,UVFaceTexture*> > (m->cm,UVTEXTURECOORDS);
	}else{
		ih = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<QMap<int,UVFaceTexture*> > (m->cm,UVTEXTURECOORDS);
	}
	
	vcg::Matrix44f matrixTr = m->cm.Tr;
	vcg::Matrix44f matrix;
	vcg::Matrix44f matrixInv;
	if(vcg::tri::HasPerMeshAttribute(m->cm, PhotoTextureTools::TransformForPhoto)){
		CMeshO::PerMeshAttributeHandle<vcg::Matrix44f> transformHandle = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<vcg::Matrix44f> (m->cm, PhotoTextureTools::TransformForPhoto);
		matrix = transformHandle();
		matrixInv =vcg::Inverse(matrix);
		m->cm.Tr = matrix;
		vcg::tri::UpdatePosition<CMeshO>::Matrix(m->cm, m->cm.Tr,true);
		//vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m->cm);
		vcg::tri::UpdateBounding<CMeshO>::Box(m->cm);
		
		qDebug()<< "transformHandle";
	}/*else{
		qDebug()<<"Identity";
		matrix = vcg::Matrix44f();
		matrix.SetIdentity();
	}
	
	
	m->cm.Tr = matrix;
	*/
	//stdout << "matrix" << matrix;
	qDebug() << "matrix2: " << matrix[0][0]<< matrix[0][1]<< matrix[0][2]<< matrix[0][3]<< matrix[1][0]<< matrix[1][1]<< matrix[1][2]<< matrix[1][3]<< matrix[2][0]<< matrix[2][1]<< matrix[2][2]<< matrix[2][3]<< matrix[3][0]<< matrix[3][1]<< matrix[3][2]<< matrix[3][3];
	
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
	int textureId = generateTextureId();
	cam->textureId = textureId;
	QList<QuadTreeLeaf*> buildQuadTree;
	//calculates the uv coordinates for each face and saves them as UVFaceTexture as
	//perFaceAttribute of the MeshModel
	CMeshO::FaceIterator fi;
	int count = 0;
	for(fi=m->cm.face.begin(); fi!=m->cm.face.end(); ++fi, count++) {
		int i;
		UVFaceTexture *ft = new UVFaceTexture();
		ft->type = 0;
		//calculating angle between the camera direction and the face normal
		vcg::Matrix33f rMatrix = vcg::Matrix33f(matrix,3);
		//vcg::Point3f tmpN = rMatrix*(*fi).N();
		vcg::Point3f tmpN = (*fi).N();
		tmpN.Normalize();
		double angle = ((-1*cam->calibration->cameraDirection[0])*tmpN[0])
						+((-1*cam->calibration->cameraDirection[1])*tmpN[1])
						+((-1*cam->calibration->cameraDirection[2])*tmpN[2]);
		angle = acos(angle);
		angle = (angle/M_PI)*180.0;

		//qDebug()<< "angle: " << angle;
		ft->faceAngleToCamera = angle;

		for (i=0;i<3;i++){

			double u,v;
			//vcg::Point3f tmpVector = matrix*(*fi).V(i)->cP();
			vcg::Point3f tmpVector = (*fi).V(i)->cP();
			cam->calibration->getUVforPoint(tmpVector[0],tmpVector[1],tmpVector[2],&u,&v);

			ft->u[i] = u/cam->resolution[0];
			ft->v[i] = 1.0-v/cam->resolution[1];

		}
		ft->textureindex =tindx;
		ft->faceIndex = count;

		ih[ft->faceIndex][textureId] = ft;
		buildQuadTree.push_back(ft);

	}
	
	if (cam->zBuffer!= NULL){
		delete cam->zBuffer;
		cam->zBuffer = NULL;
	}
	if(calcZBuffer){
		QuadTreeNode zBufferTree = QuadTreeNode(0.0,0.0,1.0,1.0);
		zBufferTree.buildQuadTree(&buildQuadTree,1.0/imgw,1.0/imgh);
		cam->zBuffer = new TextureFilterZB(imgw,imgh,1);
		calculateZBuffer(m,cam,&zBufferTree,cam->zBuffer);
		cam->zBuffer->normalize();
	}
	//save Z-Buffer as image
	//cam->zBuffer->SaveAsImage("zbuffer_",cam->name);
	cam->calculatedTextures = true;
	textureList[textureId]= cam->name;
	
	
	if(vcg::tri::HasPerMeshAttribute(m->cm, PhotoTextureTools::TransformForPhoto)){
			m->cm.Tr = matrixInv;
			vcg::tri::UpdatePosition<CMeshO>::Matrix(m->cm, m->cm.Tr);
			//vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m->cm);
			vcg::tri::UpdateBounding<CMeshO>::Box(m->cm);
			m->cm.Tr = matrixTr;
			qDebug()<< "transformHandle";
	}
}

void PhotoTexturer::applyTextureToMesh(MeshModel *m,int textureIdx, bool use_different_tidx, int tidx){
	
	if(!m->hasDataMask(MeshModel::MM_WEDGTEXCOORD)){
		m->updateDataMask(MeshModel::MM_WEDGTEXCOORD);
	}
	QMap<int, QString>::const_iterator i = textureList.find(textureIdx);
	 if (i != textureList.end()){
	
		if (vcg::tri::HasPerFaceAttribute(m->cm,UVTEXTURECOORDS)){
	
			CMeshO::PerFaceAttributeHandle<QMap<int,UVFaceTexture*> > ih = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<QMap<int,UVFaceTexture*> > (m->cm,UVTEXTURECOORDS);
			CMeshO::FaceIterator fi;
			int k =0;
			for(fi=m->cm.face.begin(); fi!=m->cm.face.end(); ++fi) {
				int i;
				UVFaceTexture* ft = ih[fi][textureIdx];
				if(ft!=NULL){
					for (i=0;i<3;i++){
						(*fi).WT(i).u() = ft->u[i];
						(*fi).WT(i).v() = ft->v[i];
						if (use_different_tidx && tidx >-1){
							(*fi).WT(i).n() = tidx;
						}else{
							(*fi).WT(i).n() = ft->textureindex;
						}
					}
				}
				k++;
			}
			//m->cm
		}
	}
}


void PhotoTexturer::unprojectToOriginalTextureMap(MeshModel *m, Camera* camera, QuadTreeNode &qtree, ImageFilterContainer *container ,bool use_distance_filter, int distance_weight, bool use_angle_filter, int angle_weight , int angle_map_sharpness, double min_angle, int imgResX, int imgResY){
	qDebug() <<"unprojectToOriginalTextureMap"<< min_angle;

	//checks if the MeshModel has original uv coordinates and camera projected uv coordinates.
	if (origTextureID != -1 && vcg::tri::HasPerFaceAttribute(m->cm,UVTEXTURECOORDS)){

		//CMeshO::PerFaceAttributeHandle<UVFaceTexture*> oth = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<UVFaceTexture*>(m->cm,ORIGINALUVTEXTURECOORDS);
		CMeshO::PerFaceAttributeHandle<QMap<int,UVFaceTexture*> > cth = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<QMap<int,UVFaceTexture*> > (m->cm,UVTEXTURECOORDS);
		
		
		
		vcg::Matrix44f matrixTr = m->cm.Tr;
		vcg::Matrix44f matrix;
		vcg::Matrix44f matrixInv;
		if(vcg::tri::HasPerMeshAttribute(m->cm, PhotoTextureTools::TransformForPhoto)){
			CMeshO::PerMeshAttributeHandle<vcg::Matrix44f> transformHandle = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<vcg::Matrix44f> (m->cm, PhotoTextureTools::TransformForPhoto);
			matrix = transformHandle();
			matrixInv =vcg::Inverse(matrix);
			m->cm.Tr = matrix;
			vcg::tri::UpdatePosition<CMeshO>::Matrix(m->cm, m->cm.Tr,true);
			//vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m->cm);
			vcg::tri::UpdateBounding<CMeshO>::Box(m->cm);
			
			qDebug()<< "transformHandle";
		}
		
		QString camname = camera->name;

		//creates a new RGBA image for saving the new texture
		//QImage image(res_x, res_y, QImage::Format_ARGB32);

		container->image = new QImage(imgResX,imgResY,QImage::Format_ARGB32);
		TextureFilterSTD *distance_filter = NULL;
		TextureFilterSTD *angle_filter = NULL;

		if(use_angle_filter){
			angle_filter = new TextureFilterSTD(imgResX,imgResY,angle_weight);
		}
		if (use_distance_filter){
			distance_filter = new TextureFilterSTD(imgResX,imgResY,distance_weight);
		}

		//loading the texture corresponding to the camera
		QImage tmp_texture(camera->textureImage);
		QRgb* utimg = (QRgb*)tmp_texture.bits();
		int twidth = tmp_texture.width();
		int theight = tmp_texture.height();
		
		QColor cpixel;

		int x;
		int y;
		//CMeshO::FaceIterator fi;
		bool found = false;

		
		//goes pixelwise over the whole new texture image and looks if it lies inside
		//a texture face of the original texture coordinates. If the pixel lies inside
		//a textured face it looks in the corresponding camera texture for the color value
		//of this pixel and stores it at the current pixel position in the new texture image.

		QRgb* ucimg = (QRgb*)container->image->bits();
		
		for (y=0;y<imgResY;y++){
			for (x=0;x<imgResX;x++){

				//sets the current pixel to black with an alpha value of 0
				cpixel = QColor(0, 0, 0, 0);
				//container->image->setPixel(x,imgResY-(y+1), cpixel.rgba());
				ucimg[(imgResY-(y+1))*imgResX+x] =  cpixel.rgba();
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
						
						UVFaceTexture* ct = cth[tmp->faceIndex][camera->textureId];
						tmp->getBarycentricCoordsForUV(((double)x/(double)(imgResX-1)),((double)y/(double)(imgResY-1)),a,b,c,d);
						
						ct->getUVatBarycentricCoords(u,v,a,b,c);
						int ix = (int)(((double)twidth-1)*u);
						int iy = theight-(int)((((double)theight-1)*v)+1);

						if(ix>=0 && ix<twidth && iy>=0 && iy<theight){


							//calculating alpha value of the pixel by using the angle

							CFaceO f;
							f = m->cm.face.at(tmp->faceIndex);
							
							vcg::Point3f p;
							p =  f.V(0)->cP()*a+ f.V(1)->cP()*b+f.V(2)->cP()*c;
							double distance = sqrt(pow(p[0]-camera->calibration->cameraPosition[0],2)+pow(p[1]-camera->calibration->cameraPosition[1],2)+pow(p[2]-camera->calibration->cameraPosition[2],2));

							if((camera->zBuffer!= 0 && (camera->zBuffer->normalizeValue(distance)-ZB_EPSILON)<=camera->zBuffer->getValue(ix,tmp_texture.height()-(iy+1)))|| (camera->zBuffer== 0 )){
								
								found = true;
								
								//cpixel = QColor(tmp_texture.pixel(ix,iy));
								cpixel = QColor(utimg[iy*twidth+ix]);
								cpixel.setAlpha(255);
								//container->image->setPixel(x,imgResY-(y+1), cpixel.rgba());
								ucimg[(imgResY-(y+1))*imgResX+x] = cpixel.rgba();
	
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
										double wangle = (angle/180.0)*M_PI;
										wangle = sin(wangle);
										wangle = pow(wangle,angle_map_sharpness);
	
										angle_filter->setValue(x,imgResY-(y+1),wangle);
									}
								}
	
								if(use_distance_filter){
									//calculate distance for pixel
									distance_filter->setValue(x,imgResY-(y+1),distance);
								}
									
								
								if (angle<= min_angle){
									//cpixel = QColor(tmp_texture.pixel(ix,iy));
									cpixel = QColor(utimg[iy*twidth+ix]);
									//int rgb = (int)((angle/90.0*255.0));
									//cpixel = QColor(rgb,rgb,rgb);
									cpixel.setAlpha((int)((angle/90.0*255.0)));
									//cpixel.setAlpha(255);
	
								}else{
									cpixel = QColor(0,0,0,0);

								}
								//container->image->setPixel(x,imgResY-(y+1), cpixel.rgba());
								ucimg[(imgResY-(y+1))*imgResX+x] = cpixel.rgba();
							}
						}else{


						}
						//}
						idx++;
					}if(!found){
						//qDebug() << "not found ";
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
		
		
		if(vcg::tri::HasPerMeshAttribute(m->cm, PhotoTextureTools::TransformForPhoto)){
				m->cm.Tr = matrixInv;
				vcg::tri::UpdatePosition<CMeshO>::Matrix(m->cm, m->cm.Tr);
				//vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m->cm);
				vcg::tri::UpdateBounding<CMeshO>::Box(m->cm);
				m->cm.Tr = matrixTr;
				qDebug()<< "transformHandle";
		}
	}
}
void PhotoTexturer::getSurrundingMeanColor(QRgb* uimg, int iwidth, int iheight, int x, int y, QColor &surcolor){
	//qDebug()<<"getSurrundingMeanColor: "<<x<<y;
	
	if((x>=0) && (x< iwidth) && (y>=0) && (y< iheight)){
		QColor c[8];
		c[0] = QColor(0,0,0,0);
		c[1] = QColor(0,0,0,0);
		c[2] = QColor(0,0,0,0);
		c[3] = QColor(0,0,0,0);
		c[4] = QColor(0,0,0,0);
		c[5] = QColor(0,0,0,0);
		c[6] = QColor(0,0,0,0);
		c[7] = QColor(0,0,0,0);
		if((x-1>=0) && (x-1< iwidth) && (y-1>=0) && (y-1< iheight)){
			//c[0]=QColor::fromRgba(image.pixel(x-1,y-1));
			c[0]=QColor::fromRgba(uimg[(y-1)*iwidth+(x-1)]);
		}

		if((x>=0) && (x< iwidth) && (y-1>=0) && (y-1< iheight)){
			//c[1]=QColor::fromRgba(image.pixel(x,y-1));
			c[1]=QColor::fromRgba(uimg[(y-1)*iwidth+(x)]);
		}
		if((x+1>=0) && (x+1< iwidth) && (y-1>=0) && (y-1< iheight)){
			//c[2]=QColor::fromRgba(image.pixel(x+1,y-1));
			c[2]=QColor::fromRgba(uimg[(y-1)*iwidth+(x+1)]);
		}
		if((x+1>=0) && (x+1< iwidth) && (y>=0) && (y< iheight)){
			//c[3]=QColor::fromRgba(image.pixel(x+1,y));
			c[3]=QColor::fromRgba(uimg[(y)*iwidth+(x+1)]);
		}
		if((x+1>=0) && (x+1< iwidth) && (y+1>=0) && (y+1< iheight)){
			//c[4]=QColor::fromRgba(image.pixel(x+1,y+1));
			c[4]=QColor::fromRgba(uimg[(y+1)*iwidth+(x+1)]);
		}
		if((x>=0) && (x< iwidth) && (y+1>=0) && (y+1< iheight)){
			//c[5]=QColor::fromRgba(image.pixel(x,y+1));
			c[5]=QColor::fromRgba(uimg[(y+1)*iwidth+(x)]);
		}
		if((x-1>=0) && (x-1< iwidth) && (y+1>=0) && (y+1< iheight)){
			//c[6]=QColor::fromRgba(image.pixel(x-1,y+1));
			c[6]=QColor::fromRgba(uimg[(y+1)*iwidth+(x-1)]);
		}
		if((x-1>=0) && (x-1< iwidth) && (y>=0) && (y< iheight)){
			//c[7]=QColor::fromRgba(image.pixel(x-1,y));
			c[7]=QColor::fromRgba(uimg[(y)*iwidth+(x-1)]);
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


void PhotoTexturer::edgeTextureStretching(QImage *image, int pass){
	QRgb* uimg = (QRgb*) image->bits();
	int width = image->width();
	int height = image->height();
	if(pass>0){
		int count = 0;
		while(pass>0){
			//qDebug()<< "edgeTextureStretching pass:" <<++count;
			QImage tmp_image = image->copy(0,0,width,height);
			QRgb* utimg = (QRgb*) tmp_image.bits();
			int x;
			int y;
			for(y=0;y<height;y++){
				for(x=0;x<width;x++){
					//QColor test = QColor::fromRgba(image->pixel(x,y));
					QColor test = QColor::fromRgba(uimg[y*width+x]);
					if(test.alpha()==0){
						//qDebug()<< "alpha == 0";
						QColor surcolor;
						getSurrundingMeanColor(utimg,width,height,x,y,surcolor);
						//image->setPixel(x,y,surcolor.rgba());
						uimg[y*width+x] = surcolor.rgba();
					}
				}
			}


			pass--;
		}
	}
}


int PhotoTexturer::unprojectTextures(MeshModel *m, int textureID, FilterParameterSet *paraSet){
	int width = paraSet->getInt(TEXTURE_SIZE_WIDTH);
	int height = paraSet->getInt(TEXTURE_SIZE_HEIGHT);
	int ets = paraSet->getInt(UNPROJECT_EDGE_STRETCHING_PASS);
	//bool enable_angle_map = paraSet->getBool(UNPROJECT_ENABLE_ANGLE);
	//int angle_weight = paraSet->getInt(UNPROJECT_ANGLE_WEIGHT);
	//int angle_map_sharpness = paraSet->getInt(UNPROJECT_ANGLE_SHARPNESS);
	//double min_angle = paraSet->getFloat(UNPROJECT_ANGLE);
	//bool enable_distance_map = paraSet->getBool(UNPROJECT_ENABLE_DISTANCE);
	//int distance_weight = paraSet->getInt(UNPROJECT_DISTANCE_WEIGHT);
	QString smartblend = paraSet->getString(BAKE_SMARTBLEND);
	
	QList<QuadTreeLeaf*> *list = new QList<QuadTreeLeaf*>();
	CMeshO::PerFaceAttributeHandle<QMap<int,UVFaceTexture*> >oth = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<QMap<int,UVFaceTexture*> >(m->cm,UVTEXTURECOORDS);
	CMeshO::FaceIterator fi;
	for(fi = m->cm.face.begin();fi!=m->cm.face.end();fi++) {
		list->push_back(oth[fi][origTextureID]);
	}

	QuadTreeNode qtree = QuadTreeNode(0.0,0.0,1.0,1.0);
	//qDebug() << "list->size(): "<<list->size();
	//qDebug()<< "buildQuadTree";
	qtree.buildQuadTree(list, 0.50/(double)width,0.50/(double)height);
/*
	ImageFilterContainer *ifc = new ImageFilterContainer();
	unprojectToOriginalTextureMap(m,camera,qtree,ifc, enable_distance_map, distance_weight, enable_angle_map,angle_weight,angle_map_sharpness,min_angle,width,height);
		
	ifc->image->save(unprojectTextureName,"PNG");
*/	
	return 0;
	//delete image;
}

int PhotoTexturer::bakeTextures(MeshModel *m, FilterParameterSet *paraSet){
	int width = paraSet->getInt(TEXTURE_SIZE_WIDTH);
	int height = paraSet->getInt(TEXTURE_SIZE_HEIGHT);
	bool enable_ets = paraSet->getBool(UNPROJECT_ENABLE_EDGE_STRETCHING);
	int ets = paraSet->getInt(UNPROJECT_EDGE_STRETCHING_PASS);
	bool enable_angle_map = paraSet->getBool(UNPROJECT_ENABLE_ANGLE);
	int angle_weight = paraSet->getInt(UNPROJECT_ANGLE_WEIGHT);
	int angle_map_sharpness = paraSet->getInt(UNPROJECT_ANGLE_SHARPNESS);
	double min_angle = paraSet->getFloat(UNPROJECT_ANGLE);
	bool enable_distance_map = paraSet->getBool(UNPROJECT_ENABLE_DISTANCE);
	int distance_weight = paraSet->getInt(UNPROJECT_DISTANCE_WEIGHT);
	QString smartblend = paraSet->getString(BAKE_SMARTBLEND);
	int merger_type = paraSet->getEnum(BAKE_MERGE_TYPE);
	bool saveUnprojected = paraSet->getBool(BAKE_SAVE_UNPROJECT);
	//creating a list of all UVFaceTexture
	QList<QuadTreeLeaf*> list;
	CMeshO::PerFaceAttributeHandle<QMap<int,UVFaceTexture*> >oth = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<QMap<int,UVFaceTexture*> >(m->cm,UVTEXTURECOORDS);
	CMeshO::FaceIterator fi;
	for(fi = m->cm.face.begin();fi!=m->cm.face.end();fi++) {
		list.push_back(oth[fi][origTextureID]);
	}

	//creating a quadtree from list UVFaceTexture
	QuadTreeNode qtree = QuadTreeNode(0.0,0.0,1.0,1.0);
	qtree.buildQuadTree(&list, 0.50/(double)width,0.50/(double)height);
	TextureMerger *texMerger = NULL;
	//deciedes which TextureMerger to use
	if (merger_type == 0){
		texMerger = new WinnerTakesAllTextureMerger();
	}else if(merger_type == 1){
		texMerger = new SmartBlendTextureMerger(smartblend);
	}
	if (texMerger != NULL){
		for (int i=0;i<cameras.size();i++){
			Camera *camera = cameras.at(i);
			if(camera->textureId >-1){
				ImageFilterContainer *ifc = new ImageFilterContainer();
				ifc->tag = camera->name;
				unprojectToOriginalTextureMap(m,camera,qtree,ifc, enable_distance_map, distance_weight, enable_angle_map,angle_weight,angle_map_sharpness,min_angle,width,height);
				texMerger->ifcList.push_back(ifc);
			}
		}
		texMerger->normalizeFilterContainerList();
		
		if (saveUnprojected){
			ImageFilterContainer *ifc;
			QFileInfo fi = QFileInfo(m->fileName.c_str());
			for (int i=0;i<texMerger->ifcList.size();i++){
				ifc = texMerger->ifcList.at(i);
				QString upTextureName = fi.baseName()+"_unproject_"+ifc->tag+".png";
				ifc->image->save(upTextureName,"PNG");
			}
		}
		
		QImage *image = texMerger->merge(width,height);
		if(image !=NULL){
			bakeCounter++;
			
			if(enable_ets){
				edgeTextureStretching(image,ets);
			}
			QString filename = paraSet->getString(BAKE_MERGED_TEXTURE);
			
			QImage final_image = image->convertToFormat(QImage::Format_RGB32);

			final_image.save(filename,"PNG");
			
			//create new UVTexture set
			qDebug()<<"filename:"<<filename;
			int textureId = generateTextureId();
			bool found = false;
			unsigned int size = static_cast<unsigned int>(m->cm.textures.size());
			unsigned j = 0;
			int tindx;
			while (!found && (j < size)){
				
				if (filename.toStdString().compare(m->cm.textures[j])==0)
				{
					tindx = (int)j;
					found = true;
				}
				++j;
			}
		
			if (!found)
			{
				m->cm.textures.push_back(filename.toStdString());
				tindx = (int)size;
			}
			
			qDebug()<<"tindx:"<<tindx;
			
			if (origTextureID>-1){
					qDebug()<<"has OriginalTextureCoords";
					//CMeshO::PerFaceAttributeHandle<UVFaceTexture*> ihot = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<UVFaceTexture*> (m->cm,ORIGINALUVTEXTURECOORDS);
					CMeshO::PerFaceAttributeHandle<QMap<int,UVFaceTexture*> > cth = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<QMap<int,UVFaceTexture*> > (m->cm,UVTEXTURECOORDS);
		
					//overwrites the current texture information with the original texture information
					// from the perFaceAttribute "ORIGINALUVTEXTURECOORDS"
					CMeshO::FaceIterator fi; int i = 0;
					for(fi   = m->cm.face.begin(); fi != m->cm.face.end(); ++fi,++i){
						
						UVFaceTexture* uvft = cth[fi][origTextureID] ;
						UVFaceTexture* tmp = new UVFaceTexture(*uvft);
						tmp->textureindex = tindx;
						tmp->type = 1;
						cth[fi][textureId] = tmp;
		
		
					}
				}else{
					//qDebug()<<"has no OriginalTextureCoords";
				}
			qDebug()<<"textureId:"<<textureId;
			if(merger_type==0){
				textureList[textureId]="baked_win_"+QString::number(bakeCounter);
			}else if(merger_type==1){
				textureList[textureId]="baked_sb_"+QString::number(bakeCounter);
			}
			
			delete texMerger;
			return textureId;
			delete image;
			
		}
	}
	return 0;
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

		QDomDocument doc(Camera::XML_CAMERADOCUMENT);
		QDomElement root = doc.createElement(Camera::XML_CAMERADOCUMENT);
		newCam->saveAsXml(&doc,&root);
		doc.appendChild(root);
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
	     ms->close();
	}else{
		qDebug()<< "Could not open max script file." <<filename;
	}


}



int PhotoTexturer::combineTextures(MeshModel* m){
	int textureId  = -1;
	if (vcg::tri::HasPerFaceAttribute(m->cm,UVTEXTURECOORDS)){
		
		CMeshO::PerFaceAttributeHandle<QMap<int,UVFaceTexture*> > ih = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<QMap<int,UVFaceTexture*> > (m->cm,UVTEXTURECOORDS);
		CMeshO::FaceIterator fi;
		textureId = generateTextureId();
		
		int k =0;
		for(fi=m->cm.face.begin(); fi!=m->cm.face.end(); ++fi) {
			UVFaceTexture* ft =0;
			int j;
			for(j=0;j<cameras.size();j++){
				UVFaceTexture* tmp = ih[fi][cameras.at(j)->textureId];
				if (tmp!=NULL){
					if(tmp->type == 0){
						if (ft==0 || ft->faceAngleToCamera>tmp->faceAngleToCamera){
							
							if(((tmp->u[0]>=0.0 && tmp->u[0] <=1.0)&&(tmp->v[0]>=0.0 && tmp->v[0] <=1.0))&&
								((tmp->u[1]>=0.0 && tmp->u[1] <=1.0)&&(tmp->v[1]>=0.0 && tmp->v[1] <=1.0))&&
								((tmp->u[2]>=0.0 && tmp->u[2] <=1.0)&&(tmp->v[2]>=0.0 && tmp->v[2] <=1.0))){
								ft = tmp;
							}
						}
					}
				}
			}
			
			UVFaceTexture* tmp2 = NULL;
			if(ft!=NULL){
				tmp2 = new UVFaceTexture(*ft);
				tmp2->type = 1;
			}
			ih[fi][textureId]=tmp2;
				
			
			k++;
		}
	}
	textureList[textureId] = "combined_"+QString::number(combineCounter);
	combineCounter++;
	return textureId;
}

void PhotoTexturer::calculateZBuffer(MeshModel *mm,Camera* camera,QuadTreeNode *qtree, TextureFilterZB *zbuffer){
	
	qDebug()<< "zbuffer->vm_height:"<<zbuffer->vm_height<<"zbuffer->vm_width:" <<zbuffer->vm_width;
	int x,y;
	for (y=0;y<zbuffer->vm_height;y++){
		for (x=0;x<zbuffer->vm_width;x++){
			double dx = ((double)x/(double)(zbuffer->vm_width-1));
			double dy = ((double)y/(double)(zbuffer->vm_height-1));
			QList<QuadTreeLeaf*>list;
			qtree->getLeafs(dx,dy,list);
			if(list.size()>0){
				int i;
				for (i=0;i<list.size();i++){
					QuadTreeLeaf* leaf = list.at(i);
					UVFaceTexture *uvft = dynamic_cast<UVFaceTexture*>(leaf);
					if (uvft != 0){
						double a,b,c,d;
						uvft->getBarycentricCoordsForUV(dx,dy,a,b,c,d);
						//qDebug() << "d:" << d;
						if(d == 0.0){
							CFaceO face = mm->cm.face.at(uvft->faceIndex);
							vcg::Point3f point = face.V(0)->cP()*a+face.V(1)->cP()*b+face.V(2)->cP()*c;
							double distance = sqrt(pow(camera->calibration->cameraPosition[0]-point[0],2)+pow(camera->calibration->cameraPosition[1]-point[1],2)+pow(camera->calibration->cameraPosition[2]-point[2],2));
							//qDebug()<< "distance: " << distance;
							if(zbuffer->getValue(x,y)== DBL_MIN||zbuffer->getValue(x,y)>distance){
								zbuffer->setValue(x,y,distance);
								//qDebug()<< "found z";
							}
						}else{
				
						}
					}
					
				}
			}
		}
	}
	
}

void PhotoTexturer::reset(MeshModel *mm){
	if (origTextureID > -1){
		restoreOriginalTextureCoordinates(mm);
	}else{
		mm->clearDataMask(MeshModel::MM_WEDGTEXCOORD);
	}
	
	
	QList<int> keys = textureList.keys(); 
	
	
	for(int i =0; i< keys.size();i++){
		int key = keys[i];
		if(key != origTextureID){
			QMap<int, QString>::const_iterator it = textureList.find(key);
			if (it != textureList.end()){
				if (vcg::tri::HasPerFaceAttribute(mm->cm,UVTEXTURECOORDS)){
				
					CMeshO::PerFaceAttributeHandle<QMap<int,UVFaceTexture*> > ih = vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<QMap<int,UVFaceTexture*> > (mm->cm,UVTEXTURECOORDS);
					CMeshO::FaceIterator fi;
					for(fi=mm->cm.face.begin(); fi!=mm->cm.face.end(); ++fi) {
						int i;
						UVFaceTexture* ft = ih[fi][key];
						delete ft;
						ih[fi].remove(key);
					}
					//m->cm
				}
			}
			textureList.remove(key);
		}
	}
	
	for(int i = 0;i<cameras.size();i++){
		Camera *c = cameras.at(i);
		delete c;
		cameras.clear();
	}
}

