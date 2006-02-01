
/* History

$Log$
Revision 1.1  2006/02/01 14:28:24  mariolatronico
first try for a QEdgeLength widget


*/
#ifndef QEDGELENGTH_H
#define QEDGELENGTH_H

#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class QEdgeLength : public QWidget
{
Q_OBJECT
public:

	QEdgeLength(float diagValue, QWidget *parent = 0);
	~QEdgeLength();
	float getAbsoluteValue();
	float getPercentValue();
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
