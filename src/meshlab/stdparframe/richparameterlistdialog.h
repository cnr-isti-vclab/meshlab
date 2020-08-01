#ifndef RICHPARAMETERLISTDIALOG_H
#define RICHPARAMETERLISTDIALOG_H

#include <QDialog>

#include "../../common/filter_parameter/rich_parameter_list.h"
#include "richparameterlistframe.h"

// This class provide a modal dialog box for asking a generic parameter set
// It can be used by anyone needing for some values in a structured form and having some integrated help
class RichParameterListDialog: public QDialog
{
	Q_OBJECT
public:
	RichParameterListDialog(QWidget *p, RichParameterList *_curParSet, QString title=QString(), MeshDocument *_meshDocument = 0);
	~RichParameterListDialog();

	RichParameterList *curParSet;
	RichParameterListFrame *stdParFrame;

	void createFrame();

	public slots:
	void getAccept();
	void toggleHelp();

	//reset the values on the gui back to the ones originally given to the dialog
	void resetValues();

private:
	MeshDocument *meshDocument;

};

#endif // RICHPARAMETERLISTDIALOG_H
