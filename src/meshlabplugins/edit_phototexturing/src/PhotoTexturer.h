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
#ifndef PHOTOTEXTURER_H_
#define PHOTOTEXTURER_H_

#include<QObject>
#include <QList>
#include <vector>
#include <meshlab/meshmodel.h>
#include <src/Camera.h>
#include <src/TextureFilter.h>
#include <src/QuadTree/QuadTreeNode.h>
#include <src/TextureMerger.h>
#include <meshlab/stdpardialog.h>
/*
 * The PhotoTexturer class can be used to calculate textures for a
 * given mesh by using calibrated cameras.
 */

class PhotoTexturer{
public:


	static const QString XML_PHOTOTEXTURING;

	static const std::string UVTEXTURECOORDS;
//	static const std::string CAMERAUVTEXTURECOORDS;
	
	//constant strings for tool settings
	static const QString TEXTURE_SIZE_WIDTH;
	static const QString TEXTURE_SIZE_HEIGHT;
	
	static const QString UNPROJECT_ENABLE_ANGLE;
	static const QString UNPROJECT_ANGLE;
	static const QString UNPROJECT_ANGLE_WEIGHT;
	static const QString UNPROJECT_ANGLE_SHARPNESS;
	
	static const QString UNPROJECT_ENABLE_DISTANCE;
	static const QString UNPROJECT_DISTANCE_WEIGHT;
	static const QString UNPROJECT_DISTANCE_SHARPNESS;
	
	static const QString UNPROJECT_ENABLE_EDGE_STRETCHING;
	static const QString UNPROJECT_EDGE_STRETCHING_PASS;
	static const QString UNPROJECT_TEXTURE_FILENAME;
	
	static const QString BAKE_SAVE_UNPROJECT;
	
	static const QString BAKE_MERGE_TEXTURES;
	static const QString BAKE_MERGE_TYPE;
	static const QString BAKE_MERGED_TEXTURE;
	static const QString BAKE_SMARTBLEND;
	
	QMap<int,QString> textureList;
	
	int origTextureID;
	
	int bakeCounter;
	int combineCounter;
	/*
	 * list of cameras used for the texturing of the meshes
	 */

	QList<Camera*> cameras;
	PhotoTexturer();
	~PhotoTexturer();

	/*
	 * loads the configuration file. The file contains a list of calibrated cameras and
	 * the corresponding images.
	 */
	void loadConfigurationFile(QString cfgFile);

	/*
	 * saves all cameras and their calibraton to a xml file.
	 */
	void saveConfigurationFile(QString cfgFile);

	/*
	 * Adds a camrea to the camera list of the PhotoTexturer.
	 * It loads the camera and its calibration from a xml file.
	 */
	void addCamera(QString camFile);

	/*
	 * removes a camera (at position i) from the camera list.
	 */
	void removeCamera(int);

	/*
	 * If the MeshModel allready has texture coordinates, these texture coordinates
	 * can be saved using the this function. The original coordinates will be saved
	 * inside the MeshModel as perFaceAttribute (key = PhotoTexturer::ORIGINALUVTEXTURECOORDS).
	 */
	void storeOriginalTextureCoordinates(MeshModel *m);

	/*
	 * Restores the original uv coordinates and textures of the mesh if they were
	 * stored as perFaceAttribute (key = PhotoTexturer::ORIGINALUVTEXTURECOORDS).
	 */
	void restoreOriginalTextureCoordinates(MeshModel *m);

	/*
	 * Calculates new sets of uv coordinates for the MeshModel using the calibrated cameras
	 * from the camera list. The uv coordinates will corresponde to the texture image of the
	 * camera. The new sets of uv coordinates will be saved as QMap as perFaceAttribute
	 * (key = PhotoTexturer::CAMERAUVTEXTURECOORDS ).
	 */
	void calculateMeshTextureForAllCameras(MeshModel *m, bool calcZBuffer);

	
	/*
	 * Calculates a set of new uv coordinates from one camres for the MeshModel. The uv coordinates will corresponde to the texture image of the
	 * camera. The new sets of uv coordinates will be saved in the QMap, which is saved as perFaceAttribute
	 * (key = PhotoTexturer::CAMERAUVTEXTURECOORDS ) of the MeshModel.
	 */
	void calculateMeshTextureForCamera(MeshModel *m, Camera* cam,bool calcZBuffer);
	
	/*
	 * Applies the uv coordinate set and the texture of camera to the
	 * MeshModel.
	 */
	void applyTextureToMesh(MeshModel *m,int textureIdx, bool use_different_tidx=false, int tidx = -1);

	/*
	 * Combines the textures calculated be each camera to one texture.
	 * A new texture image will be created with the dimensions of (width x height).
	 */
	int unprojectTextures(MeshModel *m, int textureID, FilterParameterSet *paraSet);

	int bakeTextures(MeshModel *m, FilterParameterSet *paraSet);
	
	int combineTextures(MeshModel *m);
	
	void exportMaxScript(QString filename,MeshModel *mm);

	void convertToTsaiCamera(int camIdx, bool optimize, QString filename,MeshModel *mm);

	QImage mergeTextureImagesWinnerTakesAll(int imgWidth, int imgHeight, QList<QImage> imgList);

	void reset(MeshModel *mm);
	
	void calculateZBuffer(MeshModel *mm, Camera* camera, QuadTreeNode *qtree, TextureFilterZB *zbuffer);
private:
	int nextTextId;
	
	int generateTextureId();
	


	/*
	 * Unprojects a calculated texture of a camera to the original texture of the mesh to a new
	 * texture image (dimension res_x, res_y).
	 */
	void unprojectToOriginalTextureMap(MeshModel *m, Camera* camera, QuadTreeNode &qtree, ImageFilterContainer *container ,bool use_distance_filter, int distance_weight, bool use_angle_filter, int angle_weight , int angle_map_sharpness, double min_angle, int imgResX, int imgResY);

	void getSurrundingMeanColor(QRgb* uimg, int iwidth, int iheight, int x, int y, QColor &surcolor);

	void edgeTextureStretching(QImage *image, int pass);
	
};

#endif /*PHOTOTEXTURER_H_*/
