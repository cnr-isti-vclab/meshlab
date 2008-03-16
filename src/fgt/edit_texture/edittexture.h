#ifndef EDITTEXTUREPLUGIN_H
#define EDITTEXTUREPLUGIN_H

#include <QObject>
#include <QList>
#include <QDockWidget>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include "textureeditor.h"
#include "ui_textureeditor.h"

class EditTexturePlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
	QList <QAction *> actionList;
		
	public:
		EditTexturePlugin();
		virtual ~EditTexturePlugin();

		virtual const QString Info(QAction *);
		virtual const PluginInfo &Info();
		virtual void StartEdit(QAction * , MeshModel & , GLArea * );
		virtual void EndEdit(QAction * , MeshModel & , GLArea * );
	    virtual void Decorate(QAction * , MeshModel &m , GLArea * );
		virtual void mousePressEvent (QAction *, QMouseEvent *event, MeshModel & , GLArea * );
	    virtual void mouseMoveEvent (QAction *, QMouseEvent *event, MeshModel & , GLArea * );
		virtual void mouseReleaseEvent (QAction *, QMouseEvent *event, MeshModel & , GLArea * );
		virtual QList<QAction *> actions() const ;
	    
	private:
		typedef enum {	SMAdd,		// Enumeration for mouse actions
						SMClear,
						SMSub
					 } SelMode;
		SelMode selMode;
		bool isDragging;// Indicates if the user is selecting faces
		QPoint start;	// Point for the selection area
		QPoint cur;
	    QPoint prev;
		TextureEditor* widget;
		QDockWidget* dock;
		QFont qFont;
		vector<CMeshO::FacePointer> FaceSel;	// Set of selected faces

		Ui::TextureEditorClass ui;

		void DrawXORRect(GLArea * gla);
		void InitTexture(MeshModel &m);
	
		QString debug;

	signals:
		void setSelectionRendering(bool);

};

#endif
