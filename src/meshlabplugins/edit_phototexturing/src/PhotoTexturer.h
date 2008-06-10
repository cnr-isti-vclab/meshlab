#ifndef PHOTOTEXTURER_H_
#define PHOTOTEXTURER_H_

#include<QObject>
#include <QList>
#include <vector>
#include <meshlab/meshmodel.h>
#include <src/Camera.h>

#include <src/QuadTree/QuadTreeNode.h>
/*
 * The PhotoTexturer class can be used to calculate textures for a 
 * given mesh by using calibrated cameras. 
 */

class PhotoTexturer{
public:
	
	
	static const QString XML_PHOTOTEXTURING;
	
	static const std::string ORIGINALUVTEXTURECOORDS;
	static const std::string CAMERAUVTEXTURECOORDS;
	
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
	void calculateMeshTextureForAllCameras(MeshModel *m);

	/*
	 * Applies the uv coordinate set and the texture of camera to the
	 * MeshModel.
	 */
	void applyTextureToMesh(MeshModel *m,int camIdx);
	
	/*
	 * Combines the textures calculated be each camera to one texture.
	 * A new texture image will be created with the dimensions of (width x height).  
	 */
	void combineTextures(MeshModel *m,int width, int height, int ets);
	
private: 
	
	/*
	 * Calculates a set of new uv coordinates from one camres for the MeshModel. The uv coordinates will corresponde to the texture image of the 
	 * camera. The new sets of uv coordinates will be saved in the QMap, which is saved as perFaceAttribute 
	 * (key = PhotoTexturer::CAMERAUVTEXTURECOORDS ) of the MeshModel. 
	 */
	void calculateMeshTextureForCamera(Camera* cam,MeshModel *m);
	
	/*
	 * Unprojects a calculated texture of a camera to the original texture of the mesh to a new
	 * texture image (dimension res_x, res_y).
	 */
	void unprojectToOriginalTextureMap(MeshModel *m, Camera* camera,QuadTreeNode &qtree, QImage &image);

	
	void getSurrundingMeanColor(QImage &image, int x, int y, QColor &surcolor);
	
	void edgeTextureStretching(QImage &image, int pass);
};

#endif /*PHOTOTEXTURER_H_*/
