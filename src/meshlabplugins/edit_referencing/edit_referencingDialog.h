#ifndef EDIT_REFERENCINGDIALOG_H
#define EDIT_REFERENCINGDIALOG_H

#include <QDockWidget>
#include "ui_edit_referencingDialog.h"

class GLArea;
class EditReferencingPlugin;
class MeshModel;

namespace Ui {
class edit_referencingDialog;
}

class edit_referencingDialog : public QDockWidget
{
    Q_OBJECT

public:
    edit_referencingDialog(QWidget *parent, EditReferencingPlugin * plugin);
    ~edit_referencingDialog();

    Ui::edit_referencingDialog *ui;
    GLArea *gla;

    virtual void closeEvent(QCloseEvent *event);

    void updateTable();      // update the absolute referencing data displayed in the dialog table
    void updateTableDist();  // update the scale referencing data displayed in the dialog table

signals:
    void closing();

private slots:
    void on_tabWidget_currentChanged(int index);

    void on_tableWidget_itemChanged(QTableWidgetItem *item);

    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void on_tableWidgetDist_itemChanged(QTableWidgetItem *item);

    void on_tableWidgetDist_cellDoubleClicked(int row, int column);

    void on_tableWidgetDist_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

private:
    EditReferencingPlugin *referencingPlugin;
};

#endif // EDIT_REFERENCINGDIALOG_H
