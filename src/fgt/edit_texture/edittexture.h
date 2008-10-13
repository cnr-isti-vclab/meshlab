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
	
	public:
		EditTexturePlugin();
		virtual ~EditTexturePlugin();

		static const QString Info();
		virtual void StartEdit(MeshModel & , GLArea * );
		virtual void EndEdit(MeshModel & , GLArea * );
	    virtual void Decorate(MeshModel &m , GLArea * );
		virtual void mousePressEvent(QMouseEvent *event, MeshModel & , GLArea * );
	    virtual void mouseMoveEvent(QMouseEvent *event, MeshModel & , GLArea * );
		virtual void mouseReleaseEvent (QMouseEvent *event, MeshModel & , GLArea * );
	    
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
		QString pluginName;
		vector<CMeshO::FacePointer> FaceSel;	// Set of selected faces

		Ui::TextureEditorClass ui;

		void DrawXORRect(GLArea * gla);
		void InitTexture(MeshModel &m);
		bool HasCollapsedTextCoords(MeshModel &m);

		bool degenerate;
	
	signals:
		void setSelectionRendering(bool);

};

#endif
