#include <QDockWidget>
#include <meshlab/glarea.h>
#include "edit_mutualcorrsDialog.h"
#include "ui_edit_mutualcorrsDialog.h"
#include "edit_mutualcorrs.h"

edit_mutualcorrsDialog::edit_mutualcorrsDialog(QWidget *parent, EditMutualCorrsPlugin *plugin) : QDockWidget(parent), ui(new Ui::edit_mutualcorrsDialog)
{
    ui->setupUi(this);
    this->setWidget(ui->frame);
    this->setFeatures(QDockWidget::AllDockWidgetFeatures);
    this->setAllowedAreas(Qt::LeftDockWidgetArea);
    QPoint p = parent->mapToGlobal(QPoint(0,0));
    this->setFloating(true);
    this->setGeometry(p.x()+(parent->width() - this->width()), p.y() + 40, this->width(), this->height());

    this->mutualcorrsPlugin = plugin;
}

edit_mutualcorrsDialog::~edit_mutualcorrsDialog()
{
    delete ui;
}

void edit_mutualcorrsDialog::closeEvent(QCloseEvent * /*event*/)
{
    emit closing();
}

void edit_mutualcorrsDialog::updateTable()
{
    this->mutualcorrsPlugin->status_error = "";
    this->ui->tableWidget->clear();
    this->ui->tableWidget->setRowCount(int(this->mutualcorrsPlugin->usePoint.size()));

	this->ui->tableWidget->setHorizontalHeaderLabels(QString("Active; ID ;X (3D);Y (3D);Z (3D);X (2D);Y (2D);Error").split(";"));

    this->ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    this->ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    int pindex =0;
    for(pindex=0; pindex < this->mutualcorrsPlugin->usePoint.size(); pindex++)
    {
        QTableWidgetItem* useIt = new QTableWidgetItem();
        if(this->mutualcorrsPlugin->usePoint[pindex])
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

        QTableWidgetItem* id = new QTableWidgetItem(this->mutualcorrsPlugin->pointID[pindex]);
        this->ui->tableWidget->setItem(pindex, 1, id);

        QTableWidgetItem* pickX = new QTableWidgetItem(QString::number(this->mutualcorrsPlugin->modelPoints[pindex][0]));
        pickX->setBackground(QBrush(QColor::fromRgb(233, 233, 155)));
        this->ui->tableWidget->setItem(pindex, 2, pickX);
        QTableWidgetItem* pickY = new QTableWidgetItem(QString::number(this->mutualcorrsPlugin->modelPoints[pindex][1]));
        pickY->setBackground(QBrush(QColor::fromRgb(233, 233, 155)));
        this->ui->tableWidget->setItem(pindex, 3, pickY);
        QTableWidgetItem* pickZ = new QTableWidgetItem(QString::number(this->mutualcorrsPlugin->modelPoints[pindex][2]));
        pickZ->setBackground(QBrush(QColor::fromRgb(233, 233, 155)));
        this->ui->tableWidget->setItem(pindex, 4, pickZ);

        QTableWidgetItem* refX = new QTableWidgetItem(QString::number(this->mutualcorrsPlugin->imagePoints[pindex][0]));
        refX->setBackground(QBrush(QColor::fromRgb(155, 233, 233)));
        this->ui->tableWidget->setItem(pindex, 5, refX);
        QTableWidgetItem* refY = new QTableWidgetItem(QString::number(this->mutualcorrsPlugin->imagePoints[pindex][1]));
        refY->setBackground(QBrush(QColor::fromRgb(155, 233, 233)));
        this->ui->tableWidget->setItem(pindex, 6, refY);
       /* QTableWidgetItem* refZ = new QTableWidgetItem(QString::number(this->mutualcorrsPlugin->imagePoints[pindex][2]));
        refZ->setBackground(QBrush(QColor::fromRgb(155, 233, 233)));
        this->ui->tableWidget->setItem(pindex, 7, refZ);*/

        QTableWidgetItem* error = new QTableWidgetItem(QString::number(this->mutualcorrsPlugin->pointError[pindex]));
        error->setFlags(error->flags() ^ Qt::ItemIsEditable);
        error->setBackground(QBrush(QColor::fromRgbF(0.9, 0.7, 0.7)));
        this->ui->tableWidget->setItem(pindex, 7, error);
    }

    this->mutualcorrsPlugin->glArea->update();
    return;
}

void edit_mutualcorrsDialog::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    this->mutualcorrsPlugin->status_error = "";
    // an item in the table has changed
    int rowInd = this->ui->tableWidget->currentRow();
    int colInd = this->ui->tableWidget->currentColumn();

    if((rowInd == -1)||(colInd == -1))
        return;

    if(colInd == 1) // id is just a text, just transfer it
    {
        this->mutualcorrsPlugin->pointID[rowInd] = item->text();
        this->mutualcorrsPlugin->glArea->update();
        return;
    }

    if(colInd == 2) //check numerical value
    {
        bool convOK;
        double newDValue = item->text().toDouble(&convOK);

        if(convOK)
            this->mutualcorrsPlugin->modelPoints[rowInd][0] = newDValue;
        else
            item->setText(QString::number(this->mutualcorrsPlugin->modelPoints[rowInd][0]));

        this->mutualcorrsPlugin->glArea->update();
        return;
    }

    if(colInd == 3) //check numerical value
    {
        bool convOK;
        double newDValue = item->text().toDouble(&convOK);

        if(convOK)
            this->mutualcorrsPlugin->modelPoints[rowInd][1] = newDValue;
        else
            item->setText(QString::number(this->mutualcorrsPlugin->modelPoints[rowInd][1]));

        this->mutualcorrsPlugin->glArea->update();
        return;
    }

    if(colInd == 4) //check numerical value
    {
        bool convOK;
        double newDValue = item->text().toDouble(&convOK);

        if(convOK)
            this->mutualcorrsPlugin->modelPoints[rowInd][2] = newDValue;
        else
            item->setText(QString::number(this->mutualcorrsPlugin->modelPoints[rowInd][2]));

        this->mutualcorrsPlugin->glArea->update();
        return;
    }

    if(colInd == 5) //check numerical value
    {
        bool convOK;
        double newDValue = item->text().toDouble(&convOK);

        if(convOK)
            this->mutualcorrsPlugin->imagePoints[rowInd][0] = newDValue;
        else
            item->setText(QString::number(this->mutualcorrsPlugin->imagePoints[rowInd][0]));

        this->mutualcorrsPlugin->glArea->update();
        return;
    }

    if(colInd == 6) //check numerical value
    {
        bool convOK;
        double newDValue = item->text().toDouble(&convOK);

        if(convOK)
            this->mutualcorrsPlugin->imagePoints[rowInd][1] = newDValue;
        else
            item->setText(QString::number(this->mutualcorrsPlugin->imagePoints[rowInd][1]));

        this->mutualcorrsPlugin->glArea->update();
        return;
    }

    //if(colInd == 7) //check numerical value
    //{
    //    bool convOK;
    //    double newDValue = item->text().toDouble(&convOK);

    //    if(convOK)
    //        this->mutualcorrsPlugin->imagePoints[rowInd][2] = newDValue;
    //    else
    //        item->setText(QString::number(this->mutualcorrsPlugin->imagePoints[rowInd][2]));

    //    this->mutualcorrsPlugin->glArea->update();
    //    return;
    //}

}

void edit_mutualcorrsDialog::on_tableWidget_cellDoubleClicked(int row, int column)
{
    this->mutualcorrsPlugin->status_error = "";
    // only significative for forst column, to toggle between active<->inactive
    if(column == 0)
    {
        this->mutualcorrsPlugin->usePoint[row] = !(this->mutualcorrsPlugin->usePoint[row]);

        if(this->mutualcorrsPlugin->usePoint[row])
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

    this->mutualcorrsPlugin->glArea->update();
    return;
}

void edit_mutualcorrsDialog::on_tableWidget_currentCellChanged(int /*currentRow*/, int /*currentColumn*/, int /*previousRow*/, int /*previousColumn*/)
{
    this->mutualcorrsPlugin->status_error = "";
    this->mutualcorrsPlugin->glArea->update();
    return;
}
