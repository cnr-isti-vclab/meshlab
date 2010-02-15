#ifndef OCMEBOX_H
#define OCMEBOX_H


#include <vector>
#include <vcg/math/base.h>
#include <GL/glew.h>
#include "ui_ocme.h"


/**
 */
class OcmeBox : public QWidget, private  Ui::OcmeQFrame
{
	Q_OBJECT

private:

public:
	OcmeBox(QWidget * parent = 0, Qt::WindowFlags flags = 0);

};

#endif // OCMEBOX_H
