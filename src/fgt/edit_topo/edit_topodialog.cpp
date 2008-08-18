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


int edit_topodialog::getIterations()
{
	return ui.TextEditIterations->value();
}

void edit_topodialog::setBarMax(int val)
{
	ui.progressBar->setMaximum(val);
}

void edit_topodialog::setBarVal(int val)
{
	ui.progressBar->setValue(val);
}


// DEBUG
double edit_topodialog::dist()
{
	double toret = ui.TextEditIterations_2->value() / 10;

	return toret;
}

bool edit_topodialog::isDEBUG()
{
//	int toret =0;//ui.plainTextEdit->toPlainText().toInt();

	return ui.checkBox->isChecked();
}

// END DEBUG


void edit_topodialog::updateVtxTable(QList<Vtx> list)
{
	QTableWidget *w =ui.Vtable;

	int n=w->rowCount();
	for(int i=0; i<n; i++)
		w->removeRow(0);

	for(int i=0; i<list.count(); i++)
	{
		Vtx v = list.at(i);
		QString c1 = v.vName;
		QString c2 = QString("%1").arg(v.V.X());
		QString c3 = QString("%1").arg(v.V.Y());
		QString c4 = QString("%1").arg(v.V.Z());

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
}


void edit_topodialog::updateEdgTable(QList<Edg> list)
{
	QTableWidget *w =ui.Etable;

	int n=w->rowCount();
	for(int i=0; i<n; i++)
		w->removeRow(0);

	for(int i=0; i<list.count(); i++)
	{
		Edg e = list.at(i);
		QString c1=e.v[0].vName;
		QString c2=e.v[1].vName;
		QTableWidgetItem *Item1 = new QTableWidgetItem((c1));
		QTableWidgetItem *Item2 = new QTableWidgetItem((c2));
    
		Item1->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		Item2->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

		w->setColumnWidth(0,150);
		w->setColumnWidth(1,150);

	    int row = w->rowCount();	
		w->insertRow(row);
		w->setRowHeight(row, 17);
		w->setItem(row, 0, Item1);
		w->setItem(row, 1, Item2);	
	}
}


void edit_topodialog::updateFceTable(QList<Fce> list)
{
	QTableWidget *w =ui.Ftable;
	int n=w->rowCount();
	for(int i=0; i<n; i++)
		w->removeRow(0);

	for(int i=0; i<list.count(); i++)
	{
		Fce f = list.at(i);

		QList<Vtx> allv;
		for(int e=0; e<3; e++)
			for(int v=0; v<2; v++)
				if(!allv.contains(f.e[e].v[v]))
					allv.push_back(f.e[e].v[v]);

		QString v1 = allv.at(0).vName;
		QString v2 = allv.at(1).vName;
		QString v3 = allv.at(2).vName;

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
}

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

void edit_topodialog::on_ButtonConnectVertex_2_clicked()
{
	utensil = U_DND;
	emit fuffa();
}

void edit_topodialog::on_ButtonConnectVertex_4_clicked()
{
	utensil = U_EDG_COLLAPSE;
	emit fuffa();
}

void edit_topodialog::on_ButtonConnectVertex_5_clicked()
{
	utensil = U_EDG_SPLIT;
	emit fuffa();
}