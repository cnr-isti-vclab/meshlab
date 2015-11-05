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

void edit_referencingDialog::closeEvent(QCloseEvent * /*event*/)
{
    emit closing();
}

void edit_referencingDialog::updateTable()
{
    this->referencingPlugin->status_error = "";
    this->ui->tableWidget->clear();
    this->ui->tableWidget->setRowCount(int(this->referencingPlugin->usePoint.size()));

    this->ui->tableWidget->setHorizontalHeaderLabels(QString("Active; ID ;X (mov);Y (mov);Z (mov);X (ref);Y (ref);Z (ref);Error").split(";"));

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

		QTableWidgetItem* error;
		if (this->referencingPlugin->pointError[pindex] == -1.0)
			error = new QTableWidgetItem("--");
		else
			error = new QTableWidgetItem(QString::number(this->referencingPlugin->pointError[pindex]));

        error->setFlags(error->flags() ^ Qt::ItemIsEditable);
        error->setBackground(QBrush(QColor::fromRgbF(0.9, 0.7, 0.7)));
        this->ui->tableWidget->setItem(pindex, 8, error);
    }

    this->referencingPlugin->glArea->update();
    return;
}

void edit_referencingDialog::on_tabWidget_currentChanged(int index)
{
    this->referencingPlugin->referencingMode = (EditReferencingPlugin::refModeType)index;
	this->referencingPlugin->status_line1 = "";
	this->referencingPlugin->status_line2 = "";
	this->referencingPlugin->status_line3 = "";
	this->referencingPlugin->status_error = "";

	this->referencingPlugin->glArea->update();
}

void edit_referencingDialog::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    this->referencingPlugin->status_error = "";
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
    this->referencingPlugin->status_error = "";
    // only significative for first column, to toggle between active<->inactive
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

void edit_referencingDialog::on_tableWidget_currentCellChanged(int /*currentRow*/, int /*currentColumn*/, int /*previousRow*/, int /*previousColumn*/)
{
    this->referencingPlugin->status_error = "";
    this->referencingPlugin->glArea->update();
    return;
}

void edit_referencingDialog::updateTableDist()
{
    this->referencingPlugin->status_error = "";
    this->ui->tableWidgetDist->clear();
    this->ui->tableWidgetDist->setRowCount(int(this->referencingPlugin->useDistance.size()));

	this->ui->tableWidgetDist->setHorizontalHeaderLabels(QString("Active; ID ;X (a);Y (a);Z (a);X (b);Y (b);Z (b);Curr. D.;Targ. D.;Scale;Error").split(";"));

    this->ui->tableWidgetDist->setSelectionMode(QAbstractItemView::SingleSelection);
    this->ui->tableWidgetDist->setSelectionBehavior(QAbstractItemView::SelectRows);

    int pindex =0;
    for(pindex=0; pindex < this->referencingPlugin->useDistance.size(); pindex++)
    {
        QTableWidgetItem* useIt = new QTableWidgetItem();
        if(this->referencingPlugin->useDistance[pindex])
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
        this->ui->tableWidgetDist->setItem(pindex, 0, useIt);

        QTableWidgetItem* id = new QTableWidgetItem(this->referencingPlugin->distanceID[pindex]);
        this->ui->tableWidgetDist->setItem(pindex, 1, id);

        QTableWidgetItem* pickX = new QTableWidgetItem(QString::number(this->referencingPlugin->distPointA[pindex][0]));
		pickX->setBackground(QBrush(QColor::fromRgb(200, 233, 233)));
        this->ui->tableWidgetDist->setItem(pindex, 2, pickX);
        QTableWidgetItem* pickY = new QTableWidgetItem(QString::number(this->referencingPlugin->distPointA[pindex][1]));
		pickY->setBackground(QBrush(QColor::fromRgb(200, 233, 233)));
        this->ui->tableWidgetDist->setItem(pindex, 3, pickY);
        QTableWidgetItem* pickZ = new QTableWidgetItem(QString::number(this->referencingPlugin->distPointA[pindex][2]));
		pickZ->setBackground(QBrush(QColor::fromRgb(200, 233, 233)));
        this->ui->tableWidgetDist->setItem(pindex, 4, pickZ);

        QTableWidgetItem* refX = new QTableWidgetItem(QString::number(this->referencingPlugin->distPointB[pindex][0]));
		refX->setBackground(QBrush(QColor::fromRgb(233, 233, 200)));
        this->ui->tableWidgetDist->setItem(pindex, 5, refX);
        QTableWidgetItem* refY = new QTableWidgetItem(QString::number(this->referencingPlugin->distPointB[pindex][1]));
		refY->setBackground(QBrush(QColor::fromRgb(233, 233, 200)));
        this->ui->tableWidgetDist->setItem(pindex, 6, refY);
        QTableWidgetItem* refZ = new QTableWidgetItem(QString::number(this->referencingPlugin->distPointB[pindex][2]));
		refZ->setBackground(QBrush(QColor::fromRgb(233, 233, 200)));
        this->ui->tableWidgetDist->setItem(pindex, 7, refZ);

        QTableWidgetItem* cdist = new QTableWidgetItem(QString::number(this->referencingPlugin->currDist[pindex]));
        cdist->setFlags(cdist->flags() ^ Qt::ItemIsEditable);
        cdist->setBackground(QBrush(QColor::fromRgbF(0.9, 0.9, 0.9)));
        this->ui->tableWidgetDist->setItem(pindex, 8, cdist);
        QTableWidgetItem* tdist = new QTableWidgetItem(QString::number(this->referencingPlugin->targDist[pindex]));
        tdist->setBackground(QBrush(QColor::fromRgbF(0.9, 0.9, 0.9)));
        this->ui->tableWidgetDist->setItem(pindex, 9, tdist);
		QTableWidgetItem* scal = new QTableWidgetItem(QString::number(this->referencingPlugin->scaleFact[pindex]));
		scal->setFlags(scal->flags() ^ Qt::ItemIsEditable);
		if (this->referencingPlugin->scaleFact[pindex] == 0.0)
			scal->setBackground(QBrush(QColor::fromRgbF(0.9, 0.5, 0.5)));
		else
			scal->setBackground(QBrush(QColor::fromRgbF(1.0, 1.0, 1.0)));
		this->ui->tableWidgetDist->setItem(pindex, 10, scal);

        QTableWidgetItem* error;
		if (this->referencingPlugin->useDistance[pindex])
			error = new QTableWidgetItem(QString::number(this->referencingPlugin->distError[pindex]));
		else
			error = new QTableWidgetItem("--");
        error->setFlags(error->flags() ^ Qt::ItemIsEditable);
		error->setBackground(QBrush(QColor::fromRgbF(0.9, 0.8, 0.8)));
        this->ui->tableWidgetDist->setItem(pindex, 11, error);
    }

	if (this->referencingPlugin->globalScale == 0.0)
		this->ui->label_scale->setText("---");
	else
		this->ui->label_scale->setText(QString::number(this->referencingPlugin->globalScale));

    this->referencingPlugin->glArea->update();
    return;
}

void edit_referencingDialog::on_tableWidgetDist_itemChanged(QTableWidgetItem *item)
{
	this->referencingPlugin->status_error = "";

	int rowInd = this->ui->tableWidgetDist->currentRow();
	int colInd = this->ui->tableWidgetDist->currentColumn();

	if ((rowInd == -1) || (colInd == -1))
		return;

	if (colInd == 1) // id is just a text, just transfer it
	{
		this->referencingPlugin->distanceID[rowInd] = item->text();
		this->referencingPlugin->glArea->update();
		return;
	}

	if (colInd == 2) //check numerical value
	{
		bool convOK;
		double newDValue = item->text().toDouble(&convOK);

		if (convOK)
			this->referencingPlugin->distPointA[rowInd][0] = newDValue;
		else
			item->setText(QString::number(this->referencingPlugin->distPointA[rowInd][0]));

		this->referencingPlugin->updateDistances();
		this->referencingPlugin->glArea->update();
		return;
	}

	if (colInd == 3) //check numerical value
	{
		bool convOK;
		double newDValue = item->text().toDouble(&convOK);

		if (convOK)
			this->referencingPlugin->distPointA[rowInd][1] = newDValue;
		else
			item->setText(QString::number(this->referencingPlugin->distPointA[rowInd][1]));

		this->referencingPlugin->updateDistances();
		this->referencingPlugin->glArea->update();
		return;
	}

	if (colInd == 4) //check numerical value
	{
		bool convOK;
		double newDValue = item->text().toDouble(&convOK);

		if (convOK)
			this->referencingPlugin->distPointA[rowInd][2] = newDValue;
		else
			item->setText(QString::number(this->referencingPlugin->distPointA[rowInd][2]));

		this->referencingPlugin->updateDistances();
		this->referencingPlugin->glArea->update();
		return;
	}

	if (colInd == 5) //check numerical value
	{
		bool convOK;
		double newDValue = item->text().toDouble(&convOK);

		if (convOK)
			this->referencingPlugin->distPointB[rowInd][0] = newDValue;
		else
			item->setText(QString::number(this->referencingPlugin->distPointB[rowInd][0]));

		this->referencingPlugin->updateDistances();
		this->referencingPlugin->glArea->update();
		return;
	}

	if (colInd == 6) //check numerical value
	{
		bool convOK;
		double newDValue = item->text().toDouble(&convOK);

		if (convOK)
			this->referencingPlugin->distPointB[rowInd][1] = newDValue;
		else
			item->setText(QString::number(this->referencingPlugin->distPointB[rowInd][1]));

		this->referencingPlugin->updateDistances();
		this->referencingPlugin->glArea->update();
		return;
	}

	if (colInd == 7) //check numerical value
	{
		bool convOK;
		double newDValue = item->text().toDouble(&convOK);

		if (convOK)
			this->referencingPlugin->distPointB[rowInd][2] = newDValue;
		else
			item->setText(QString::number(this->referencingPlugin->distPointB[rowInd][2]));

		this->referencingPlugin->updateDistances();
		this->referencingPlugin->glArea->update();
		return;
	}

	if (colInd == 9) //check numerical value
	{
		bool convOK;
		double newDValue = item->text().toDouble(&convOK);

		if (convOK)
			this->referencingPlugin->targDist[rowInd] = newDValue;
		else
			item->setText(QString::number(this->referencingPlugin->targDist[rowInd]));

		this->referencingPlugin->updateDistances();
		this->referencingPlugin->glArea->update();
		return;
	}

    return;
}

void edit_referencingDialog::on_tableWidgetDist_cellDoubleClicked(int row, int column)
{
    this->referencingPlugin->status_error = "";
    // only significative for first column, to toggle between active<->inactive
    if(column == 0)
    {
        this->referencingPlugin->useDistance[row] = !(this->referencingPlugin->useDistance[row]);

        if(this->referencingPlugin->useDistance[row])
        {
            this->ui->tableWidgetDist->item(row,column)->setText("active");
            this->ui->tableWidgetDist->item(row,column)->setBackground(QBrush(QColor::fromRgbF(0.8, 0.9, 0.8)));
        }
        else
        {
            this->ui->tableWidgetDist->item(row,column)->setText("inactive");
            this->ui->tableWidgetDist->item(row,column)->setBackground(QBrush(QColor::fromRgbF(0.9, 0.8, 0.8)));
        }

		this->referencingPlugin->updateDistances();
    }

    this->referencingPlugin->glArea->update();
    return;
}

void edit_referencingDialog::on_tableWidgetDist_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    this->referencingPlugin->status_error = "";
    this->referencingPlugin->glArea->update();
    return;
}
