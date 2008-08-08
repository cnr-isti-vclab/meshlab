#include "edit_topodialog.h"



edit_topodialog::edit_topodialog(QWidget *parent) : QDockWidget(parent)
{
	ui.setupUi(this);
	this->setFloating(true);
	utensil = U_VTX_SEL_FREE;
	ui.toolBox->setCurrentIndex(0);
}

edit_topodialog::~edit_topodialog() 
{ }


bool edit_topodialog::isRadioButtonSimpleChecked()
{
	return ui.radioButtonSimple->isChecked();
}

int edit_topodialog::getIterations()
{
	return ui.TextEditIterations->value();
}

bool edit_topodialog::isCheckBoxTrColorChecked()
{
	return ui.checkBoxTrColor->isChecked();
}

void edit_topodialog::setBarMax(int val)
{
	ui.progressBar->setMaximum(val);
}

void edit_topodialog::setBarVal(int val)
{
	ui.progressBar->setValue(val);
}

int edit_topodialog::dist(int d)
{
	int toret =0;//ui.plainTextEdit->toPlainText().toInt();

	return toret;
}

void edit_topodialog::removeVertexInTable(QString vName)
{
	QTableWidget *w =ui.Vtable;

	int row=-1;
	for(int i = 0; i<(w->rowCount()); i++)
		if(w->item(i,0)->text() == vName)
			row = i;
	if(row!=-1)
		w->removeRow(row);
}

//
// Vertex list table
//
void edit_topodialog::removeVertexInTable(QString vx, QString vy, QString vz)
{
	QTableWidget *w =ui.Vtable;

	int row=-1;
	for(int i = 0; i<(w->rowCount()); i++)
		if((w->item(i,1)->text() == vx)&&(w->item(i,2)->text() == vy)&&(w->item(i,3)->text() == vz))
			row = i;
	if(row!=-1)
		w->removeRow(row);
}


void edit_topodialog::insertVertexInTable(QString c1, QString c2, QString c3, QString c4)
{
	QTableWidget *w =ui.Vtable;

	QTableWidgetItem *Item1 = new QTableWidgetItem((c1));
	QTableWidgetItem *Item2 = new QTableWidgetItem((c2));
	QTableWidgetItem *Item3 = new QTableWidgetItem((c3));
    QTableWidgetItem *Item4 = new QTableWidgetItem((c4));
    
	Item1->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    Item2->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    Item3->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	Item4->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);


    int row = w->rowCount();
    w->insertRow(row);
	w->setRowHeight(row, 17);
    w->setItem(row, 0, Item1);
    w->setItem(row, 1, Item2);	
    w->setItem(row, 2, Item3);	
    w->setItem(row, 3, Item4);	
}

//
// Vertex connections list table
//
void edit_topodialog::removeConnectionInTable(QString c1, QString c2)
{
	QTableWidget *w =ui.Etable;

	int row=-1;
	for(int i = 0; i<(w->rowCount()); i++)
		if((w->item(i,0)->text() == c1) && (w->item(i,1)->text() == c2))
			row = i;
	if(row!=-1)
		w->removeRow(row);
}

void edit_topodialog::insertConnectionInTable(QString c1, QString c2)
{
	QTableWidget *w =ui.Etable;

	QTableWidgetItem *Item1 = new QTableWidgetItem((c1));
	QTableWidgetItem *Item2 = new QTableWidgetItem((c2));
    
	Item1->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    Item2->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

	w->setColumnWidth(0,200);
	w->setColumnWidth(1,200);

    int row = w->rowCount();	
	w->insertRow(row);
	w->setRowHeight(row, 17);
    w->setItem(row, 0, Item1);
    w->setItem(row, 1, Item2);	
}




void edit_topodialog::insertFaceInTable(QString v1, QString v2, QString v3)
{
	QTableWidget *w =ui.Ftable;

	QTableWidgetItem *Item1 = new QTableWidgetItem((v1));
	QTableWidgetItem *Item2 = new QTableWidgetItem((v2));
	QTableWidgetItem *Item3 = new QTableWidgetItem((v3));
    
	Item1->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    Item2->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	Item3->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

	w->setColumnWidth(0,133);
	w->setColumnWidth(1,133);
	w->setColumnWidth(2,133);

    int row = w->rowCount();	
	w->insertRow(row);
	w->setRowHeight(row, 17);
    w->setItem(row, 0, Item1);
    w->setItem(row, 1, Item2);
    w->setItem(row, 2, Item3);
}

void edit_topodialog::removeFaceInTable(QString v1, QString v2, QString v3)
{}


void edit_topodialog::on_ButtonClose_clicked()
{
	this->close();
	utensil = U_NONE;		emit fuffa();
}

void edit_topodialog::on_ButtonSelectVertex_clicked()
{
	utensil = U_VTX_SEL;	emit fuffa();
}

void edit_topodialog::on_ButtonSelectVertexFree_clicked()
{
	utensil = U_VTX_SEL_FREE;	emit fuffa();
}


void edit_topodialog::on_ButtonDeSelectVertex_clicked()
{
	utensil = U_VTX_DEL;	emit fuffa();
}



void edit_topodialog::on_ButtonConnectVertex_clicked()
{
	utensil = U_VTX_CONNECT;	emit fuffa();
}

void edit_topodialog::on_ButtonDeConnectVertex_clicked()
{
	utensil = U_VTX_DE_CONNECT;	emit fuffa();
}


void edit_topodialog::on_ButtonSelectFace_clicked()
{
	utensil = U_FCE_SEL;
	emit fuffa();
}
void edit_topodialog::on_ButtonMeshCreate_clicked()
{
	emit mesh_create();
}



void edit_topodialog::on_toolBox_currentChanged(int i)
{
	switch(i) 
	{
		case 0:
			utensil = U_VTX_SEL_FREE;
			break;
		case 1:
			utensil = U_VTX_CONNECT;
			break;
		case 2:
			utensil = U_FCE_SEL;
			break;
	};
		emit fuffa();
}

void edit_topodialog::on_groupBox_2_toggled(bool)
{

}

void edit_topodialog::on_ButtonConnectVertex_2_clicked()
{
	utensil = U_DND;
	emit fuffa();
}
