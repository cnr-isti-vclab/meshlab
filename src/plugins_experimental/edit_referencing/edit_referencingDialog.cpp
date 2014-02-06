#include <QDockWidget>
#include <meshlab/glarea.h>
#include "edit_referencingDialog.h"
#include "ui_edit_referencingDialog.h"
#include "edit_referencing.h"

edit_referencingDialog::edit_referencingDialog(QWidget *parent, EditReferencingPlugin *plugin): QDockWidget(parent), ui(new Ui::edit_referencingDialog)
{
    ui->setupUi(this);
    this->setWidget(ui->frame);
    this->setFeatures(QDockWidget::AllDockWidgetFeatures);
    this->setAllowedAreas(Qt::LeftDockWidgetArea);
    QPoint p = parent->mapToGlobal(QPoint(0,0));
    this->setFloating(true);
    this->setGeometry(p.x()+(parent->width() - this->width()), p.y() + 40, this->width(), this->height());

    this->referencingPlugin = plugin;
}

edit_referencingDialog::~edit_referencingDialog()
{
    delete ui;
}

void edit_referencingDialog::closeEvent(QCloseEvent *event)
{
    emit closing();
}

void edit_referencingDialog::updateTable()
{
    this->ui->tableWidget->clear();
    this->ui->tableWidget->setRowCount(this->referencingPlugin->usePoint.size());

    this->ui->tableWidget->setHorizontalHeaderLabels(QString("Active; ID ;X (picked);Y (picked);Z (picked);X (ref);Y (ref);Z (ref);Error").split(";"));

    this->ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    this->ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    int pindex =0;
    for(pindex=0; pindex < this->referencingPlugin->usePoint.size(); pindex++)
    {
        QTableWidgetItem* useIt = new QTableWidgetItem();
        if(this->referencingPlugin->usePoint[pindex])
        {
            useIt->setText("active");
            useIt->setBackground(QBrush(QColor::fromRgbF(0.8, 0.9, 0.8)));
        }
        else
        {
            useIt->setText("inactive");
            useIt->setBackground(QBrush(QColor::fromRgbF(0.9, 0.8, 0.8)));
        }
        useIt->setFlags(useIt->flags() ^ Qt::ItemIsEditable);
        this->ui->tableWidget->setItem(pindex, 0, useIt);

        QTableWidgetItem* id = new QTableWidgetItem(this->referencingPlugin->pointID[pindex]);
        this->ui->tableWidget->setItem(pindex, 1, id);

        QTableWidgetItem* pickX = new QTableWidgetItem(QString::number(this->referencingPlugin->pickedPoints[pindex][0]));
        pickX->setBackground(QBrush(QColor::fromRgb(233, 233, 155)));
        this->ui->tableWidget->setItem(pindex, 2, pickX);
        QTableWidgetItem* pickY = new QTableWidgetItem(QString::number(this->referencingPlugin->pickedPoints[pindex][1]));
        pickY->setBackground(QBrush(QColor::fromRgb(233, 233, 155)));
        this->ui->tableWidget->setItem(pindex, 3, pickY);
        QTableWidgetItem* pickZ = new QTableWidgetItem(QString::number(this->referencingPlugin->pickedPoints[pindex][2]));
        pickZ->setBackground(QBrush(QColor::fromRgb(233, 233, 155)));
        this->ui->tableWidget->setItem(pindex, 4, pickZ);

        QTableWidgetItem* refX = new QTableWidgetItem(QString::number(this->referencingPlugin->refPoints[pindex][0]));
        refX->setBackground(QBrush(QColor::fromRgb(155, 233, 233)));
        this->ui->tableWidget->setItem(pindex, 5, refX);
        QTableWidgetItem* refY = new QTableWidgetItem(QString::number(this->referencingPlugin->refPoints[pindex][1]));
        refY->setBackground(QBrush(QColor::fromRgb(155, 233, 233)));
        this->ui->tableWidget->setItem(pindex, 6, refY);
        QTableWidgetItem* refZ = new QTableWidgetItem(QString::number(this->referencingPlugin->refPoints[pindex][2]));
        refZ->setBackground(QBrush(QColor::fromRgb(155, 233, 233)));
        this->ui->tableWidget->setItem(pindex, 7, refZ);

        QTableWidgetItem* error = new QTableWidgetItem(QString::number(this->referencingPlugin->pointError[pindex]));
        error->setFlags(error->flags() ^ Qt::ItemIsEditable);
        error->setBackground(QBrush(QColor::fromRgbF(0.9, 0.7, 0.7)));
        this->ui->tableWidget->setItem(pindex, 8, error);
    }

    this->referencingPlugin->glArea->update();
    return;
}

void edit_referencingDialog::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    // an item in the table has changed
    int rowInd = this->ui->tableWidget->currentRow();
    int colInd = this->ui->tableWidget->currentColumn();

    if((rowInd == -1)||(colInd == -1))
        return;

    if(colInd == 1) // id is just a text, just transfer it
    {
        this->referencingPlugin->pointID[rowInd] = item->text();
        this->referencingPlugin->glArea->update();
        return;
    }

    if(colInd == 2) //check numerical value
    {
        bool convOK;
        double newDValue = item->text().toDouble(&convOK);

        if(convOK)
            this->referencingPlugin->pickedPoints[rowInd][0] = newDValue;
        else
            item->setText(QString::number(this->referencingPlugin->pickedPoints[rowInd][0]));

        this->referencingPlugin->glArea->update();
        return;
    }

    if(colInd == 3) //check numerical value
    {
        bool convOK;
        double newDValue = item->text().toDouble(&convOK);

        if(convOK)
            this->referencingPlugin->pickedPoints[rowInd][1] = newDValue;
        else
            item->setText(QString::number(this->referencingPlugin->pickedPoints[rowInd][1]));

        this->referencingPlugin->glArea->update();
        return;
    }

    if(colInd == 4) //check numerical value
    {
        bool convOK;
        double newDValue = item->text().toDouble(&convOK);

        if(convOK)
            this->referencingPlugin->pickedPoints[rowInd][2] = newDValue;
        else
            item->setText(QString::number(this->referencingPlugin->pickedPoints[rowInd][2]));

        this->referencingPlugin->glArea->update();
        return;
    }

    if(colInd == 5) //check numerical value
    {
        bool convOK;
        double newDValue = item->text().toDouble(&convOK);

        if(convOK)
            this->referencingPlugin->refPoints[rowInd][0] = newDValue;
        else
            item->setText(QString::number(this->referencingPlugin->refPoints[rowInd][0]));

        this->referencingPlugin->glArea->update();
        return;
    }

    if(colInd == 6) //check numerical value
    {
        bool convOK;
        double newDValue = item->text().toDouble(&convOK);

        if(convOK)
            this->referencingPlugin->refPoints[rowInd][1] = newDValue;
        else
            item->setText(QString::number(this->referencingPlugin->refPoints[rowInd][1]));

        this->referencingPlugin->glArea->update();
        return;
    }

    if(colInd == 7) //check numerical value
    {
        bool convOK;
        double newDValue = item->text().toDouble(&convOK);

        if(convOK)
            this->referencingPlugin->refPoints[rowInd][2] = newDValue;
        else
            item->setText(QString::number(this->referencingPlugin->refPoints[rowInd][2]));

        this->referencingPlugin->glArea->update();
        return;
    }

}

void edit_referencingDialog::on_tableWidget_cellDoubleClicked(int row, int column)
{
    // only significative for forst column, to toggle between active<->inactive
    if(column == 0)
    {
        this->referencingPlugin->usePoint[row] = !(this->referencingPlugin->usePoint[row]);

        if(this->referencingPlugin->usePoint[row])
        {
            this->ui->tableWidget->item(row,column)->setText("active");
            this->ui->tableWidget->item(row,column)->setBackground(QBrush(QColor::fromRgbF(0.8, 0.9, 0.8)));
        }
        else
        {
            this->ui->tableWidget->item(row,column)->setText("inactive");
            this->ui->tableWidget->item(row,column)->setBackground(QBrush(QColor::fromRgbF(0.9, 0.8, 0.8)));
        }
    }

    this->referencingPlugin->glArea->update();
    return;
}

void edit_referencingDialog::on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    this->referencingPlugin->glArea->update();
    return;
}
