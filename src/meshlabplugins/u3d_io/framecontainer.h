#ifndef FRAME_CONTAINER_H
#define FRAME_CONTAINER_H

#include <QFrame>
#include "ConverterDirectory.h"
#include "cameraparametergui.h"


class FrameContainer : public QFrame
{
public:

	FrameContainer(QWidget* parent)
		:QFrame(parent)
	{
		_framewin = new ConverterDirectory(this);
		_framewin->show();
	}

	bool changeFrame(const QString& objtxt)
	{
		if ((objtxt == "Converter Directory") && (_framewin->objectName() != "ConverterDirectoryClass"))
		{
			delete _framewin;
			_framewin = new ConverterDirectory(this);
			_framewin->show();
			return true;
		}
	
		if ((objtxt == "Camera Parameters") && (_framewin->objectName() != "CameraparameterguiClass"))
		{
			delete _framewin;
			_framewin = new Cameraparametergui(this);
			_framewin->show();
			return true;
		}

		return false;
	}
	
	QFrame* _framewin;
};

#endif