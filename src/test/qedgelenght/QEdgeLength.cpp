/* History

$Log$
Revision 1.2  2006/02/05 11:16:58  mariolatronico
some changes to adapt the widget for Designer integration

Revision 1.1  2006/02/01 14:28:24  mariolatronico
first try for a QEdgeLength widget


*/

#include "QEdgeLength.h"


QEdgeLength::QEdgeLength(QWidget *parent) : QWidget(parent)
 {

	diagValue = 1;
	percValue = 100.0f;
	absValue = 0.0f;
	// Widget and layout creation

	absLE = new QLineEdit(this);
	percLE = new QLineEdit(this);
  absLE->setMaxLength( 15 );
  percLE->setMaxLength( 15 );

	diagValueLBL = new QLabel(QString::number((double)diagValue),this);
	diagLBL = new QLabel("Diagonal", this);
	absLBL =  new QLabel("Absolute",this);
	percLBL = new QLabel("Percent ",this);

	layout = new QVBoxLayout;
	layoutAbs = new QHBoxLayout;
	layoutPerc = new QHBoxLayout;
	layoutDiag = new QHBoxLayout;

	// Layout settings

	layoutAbs -> addWidget(absLBL) ; 	layoutAbs -> addWidget(absLE, 0, Qt::AlignLeft);
	layoutPerc-> addWidget(percLBL); 	layoutPerc-> addWidget(percLE,0, Qt::AlignLeft);
	layoutDiag-> addWidget(diagLBL); 	layoutDiag-> addWidget(diagValueLBL, 0, Qt::AlignLeft);

	layout->addLayout(layoutAbs);
	layout->addLayout(layoutPerc);
	layout->addLayout(layoutDiag);

	setLayout(layout);

	// Connections

	connect(absLE,  SIGNAL( textChanged( const QString & ) ),
					this, SLOT(  setPercentValue( const QString & ) )  );
	connect(percLE, SIGNAL( textChanged( const QString & ) ),
					this,  SLOT( setAbsoluteValue( const QString & ) )  );

}

float QEdgeLength::getAbsoluteValue() {
	return absValue;
}

float QEdgeLength::getPercentValue() {
	return percValue;
}

void QEdgeLength::setAbsoluteValue(const QString & percString) {

	bool ok;
	percValue = percString.toFloat(&ok);
	if (! ok ) // it's not a number
		return;
	absValue = percValue / 100.0f * diagValue;
	absLE->setText(QString::number((double) absValue));
}
void QEdgeLength::setPercentValue(const QString & absString) {

	bool ok;
	absValue = absString.toFloat(&ok);
	if (! ok ) // it's not a number
		return;
	percValue = absValue / diagValue * 100.0f;;
	percLE->setText(QString::number((double) percValue));

}

QEdgeLength::~QEdgeLength() {

    // It's layout responsability to delete their own widgets
	// added with addWidget
//     if ( layout )
//         delete layout;
// 	if ( layoutAbs )
//       delete layoutAbs;
// 	if ( layoutDiag )
//       delete layoutDiag;
//   if ( layoutPerc )
//       delete layoutPerc;

}

void QEdgeLength::setDiagonal( float diagValue )
{
    this->diagValue = diagValue;
    diagValueLBL->setText( QString::number((double)diagValue));

}





