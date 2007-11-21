#include "param.h"

Param::Param(QWidget *parent, Qt::WFlags flags)
	: QDialog(parent, flags)
{
	ui.setupUi(this);
}

Param::~Param()
{

}

void Param::on_ok_button_clicked()
{

}

void Param::on_tree_selector_itemClicked(QTreeWidgetItem* item,int col)
{
	ui.frame_container->changeFrame(item->text(col));
}