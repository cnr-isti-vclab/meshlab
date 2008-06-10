#ifndef PHOTOTEXTURINGDIALOG_H_
#define PHOTOTEXTURINGDIALOG_H_

#include <QDialog>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <wrap/gl/trimesh.h>
//#include <photoTexturing.h>
#include <src/PhotoTexturer.h>
#include "ui_photoTexturingDialog.h"

class PhotoTexturingDialog: public QDialog, Ui::photoTexturingDialog{
	
Q_OBJECT

private:
	MeshModel *mesh;
	GLArea *glarea;
	Ui::photoTexturingDialog ui;
	MeshEditInterface *ptPlugin;
	PhotoTexturer *photoTexturer;
	
	signals:
		void updateGLAreaTextures();
		void setGLAreaDrawMode(vcg::GLW::DrawMode mode);
		void setGLAreaColorMode(vcg::GLW::ColorMode mode);
		void setGLAreaTextureMode(vcg::GLW::TextureMode mode);
		
		void updateMainWindowMenus();
		
private slots:
	//void browseCalibrationFile();
	void loadConfigurationFile();
	void saveConfigurationFile();
	void addCamera();
	void removeCamera();
	
	void update();
	
	void assignImage();
	void calculateTextures();
	void selectCurrentTexture(int index);
	void combineTextures();
	void apply();
	void close();
	void cancel();
	
public:
	PhotoTexturingDialog(MeshEditInterface*, PhotoTexturer*,MeshModel &m, GLArea *gla);
	~PhotoTexturingDialog();
	
};

#endif /*PHOTOTEXTURINGDIALOG_H_*/
