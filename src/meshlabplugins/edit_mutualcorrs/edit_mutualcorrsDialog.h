#ifndef edit_mutualcorrsDialog_H
#define edit_mutualcorrsDialog_H

#include <QDockWidget>
#include "ui_edit_mutualcorrsDialog.h"

class GLArea;
class EditMutualCorrsPlugin;
class MeshModel;

namespace Ui {
class edit_mutualcorrsDialog;
}

class edit_mutualcorrsDialog : public QDockWidget
{
    Q_OBJECT

public:
    edit_mutualcorrsDialog(QWidget *parent, EditMutualCorrsPlugin * plugin);
    ~edit_mutualcorrsDialog();

    Ui::edit_mutualcorrsDialog *ui;
    GLArea *gla;

    virtual void closeEvent(QCloseEvent *event);

    void updateTable();     // update the referencing data displayed in the dialog table

signals:
    void closing();

private slots:
    void on_tableWidget_itemChanged(QTableWidgetItem *item);

    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

private:
    EditMutualCorrsPlugin *mutualcorrsPlugin;
};

#endif // edit_mutualcorrsDialog_H
