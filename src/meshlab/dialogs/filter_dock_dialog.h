#ifndef FILTER_DOCK_DIALOG_H
#define FILTER_DOCK_DIALOG_H

#include <QDockWidget>

#include <common/ml_document/mesh_model_state.h>
#include <common/plugins/interfaces/filter_plugin.h>

class GLArea;
class MainWindow;

namespace Ui {
class FilterDockDialog;
}

class FilterDockDialog : public QDockWidget
{
	Q_OBJECT

public:
	explicit FilterDockDialog(
		const RichParameterList& rpl,
		FilterPlugin*            plugin,
		const QAction*           filter,
		QWidget*                 parent = nullptr,
		GLArea*                  glArea = nullptr);
	~FilterDockDialog();

signals:
	void applyButtonClicked(const QAction*, RichParameterList, bool, bool);

private slots:
	void on_previewCheckBox_stateChanged(int state);
	void on_applyPushButton_clicked();
	void on_helpPushButton_clicked();
	void on_closePushButton_clicked();
	void on_defaultPushButton_clicked();

	// preview slots
	void applyDynamic();
	void changeCurrentMesh(int meshId);

private:
	bool isPreviewable() const;

	static bool isFilterPreviewable(FilterPlugin* plugin, const QAction* filter);
	static void updateRenderingData(MainWindow* mw, MeshModel* mesh);

	Ui::FilterDockDialog* ui;

	FilterPlugin*     plugin;
	const QAction*    filter;
	RichParameterList parameters;
	unsigned int      mask;
	GLArea*           currentGLArea;

	// preview
	bool              isPreviewMeshStateValid;
	MeshModelState    noPreviewMeshState;
	MeshModelState    previewMeshState;
	RichParameterList prevParams;
	MainWindow*       mw;
	MeshDocument*     md;
	MeshModel*        mesh;
};

#endif // FILTER_DOCK_DIALOG_H
