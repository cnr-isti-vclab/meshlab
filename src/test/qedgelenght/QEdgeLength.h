
/* History

$Log$
Revision 1.2  2006/02/05 11:16:58  mariolatronico
some changes to adapt the widget for Designer integration

Revision 1.1  2006/02/01 14:28:24  mariolatronico
first try for a QEdgeLength widget


*/
#ifndef QEDGELENGTH_H
#define QEDGELENGTH_H

#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtDesigner/QDesignerExportWidget>

class QDESIGNER_WIDGET_EXPORT QEdgeLength : public QWidget
{
    Q_OBJECT
public:

    QEdgeLength(QWidget *parent = 0);
  	~QEdgeLength();
    float getAbsoluteValue();
    float getPercentValue();
    void setDiagonal( float diagValue );
private slots:

    void setAbsoluteValue(const QString &);
    void setPercentValue(const QString &);

private:
	float diagValue, absValue, percValue;
	QLineEdit *absLE, *percLE;
	QLabel *diagValueLBL, *diagLBL, *absLBL, *percLBL  ;
	QHBoxLayout *layoutAbs, *layoutPerc, *layoutDiag;
	QVBoxLayout *layout;
};


#endif
