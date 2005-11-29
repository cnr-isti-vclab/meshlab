#include "ui/ui_custom.h"
#include <QColor>
#include <QDialog>
#include <QColorDialog>
#include <wrap/gl/trimesh.h>

using namespace std;
using namespace vcg;

class CustomDialog : public QDialog
{
Q_OBJECT
public:
	CustomDialog(QWidget *parent = 0);

	void	 LoadCurrentSetting(const Color4b& bb,const Color4b& bt,const Color4b& l);
  Color4b GetBackgroundBottomColor(){ return cBackgroundBottom; };
	Color4b GetBackgroundTopColor(){ return cBackgroundTop; };
	Color4b GetLogColor(){ return cLog; };

private slots:
	void SetBackgroundBottomColor();
	void SetLogColor();
	void SetBackgroundTopColor();
		
private:
	Ui::CDialogCustom ui;
	Color4b cBackgroundBottom;
	Color4b cBackgroundTop;
	Color4b cLog;
};
