#ifndef RMSHADERDIALOG_H
#define RMSHADERDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

#include <QList>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QSignalMapper>

#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>

#include <GL/glew.h>
#include <QGLWidget>
#include <meshlab/meshmodel.h>

#include "parser/RmXmlParser.h"
#include "parser/RmEffect.h"
#include "parser/RmPass.h"
#include "ui_rmShadowDialog.h"
#include "glstateholder.h"

class RmShaderDialog : public QDialog
{
    Q_OBJECT

	public:
		RmShaderDialog( GLStateHolder * holder, RmXmlParser * parser, QGLWidget* gla, RenderMode &rm, QWidget *parent = NULL );
		~RmShaderDialog();

	private:
		Ui_RmShaderDialogClass ui;
		QGLWidget* glarea;
		RenderMode * rendMode;
		RmXmlParser * parser;

		RmEffect * eff_selected;
		RmPass * pass_selected;

		QList<QWidget*> shown;
		GLStateHolder * holder;

		QSignalMapper * signaler;

	public slots:
		void fillDialogWithEffect( int index );
		void fillTabsWithPass( int index );
		void clearTabs();

		void valuesChanged(const QString & varNameAndIndex );

};

#endif

