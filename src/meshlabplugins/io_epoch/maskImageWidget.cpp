/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include "maskImageWidget.h"
#include "maskRenderWidget.h"
#include "fillImage.h"
#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtGui/QPolygon>
#include <QtGui/QPixmap>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QAction>
#include <QtGui/QVBoxLayout>
#include <QtGui/QToolBar>
#include <QtGui/QSpinBox>
#include <QtGui/QFileDialog>
#include <QtGui/QDesktopWidget>

#include <stack>
#include <cmath>
#include <sstream>
#include <cassert>
#include <iostream>

#ifdef WIN32
#undef min
#undef max
#endif


namespace ui
{

  struct maskImageWidget::Impl
  {
		enum DrawMode { Pen, Eraser } mode_;

		maskRenderWidget *render_area_;
		int threshold_gradient_, threshold_fixed_;
		int realwidth_, realheight_;
    Impl();
  };


  maskImageWidget::Impl::Impl()
  {
		mode_ = Pen;
		threshold_gradient_ = 100;
		threshold_fixed_ = 30;
  }


  maskImageWidget::maskImageWidget(const QImage& image, QWidget *parent) : QDialog(parent), pimpl_(new Impl)
  {
		init(image);
  }


  maskImageWidget::~maskImageWidget() throw()
  {
		delete pimpl_;
  }

	QImage maskImageWidget::getMask() const
	{
		return pimpl_->render_area_->getMask(pimpl_->realwidth_, pimpl_->realheight_);
	}

	void maskImageWidget::loadMask(const QString& filename)
	{
		pimpl_->render_area_->load(filename);
	}

	void maskImageWidget::init(const QImage& image)
	{
		setWindowTitle(tr("Mask Editor"));

		QPixmap load("coral_open32x32.png");
		QPixmap save("coral_save32x32.png");
		QPixmap undo("coral_undo32x32.png");
		QPixmap redo("coral_redo32x32.png");
		QPixmap pen("coral_pencil32x32.png");
		QPixmap eraser("coral_eraser32x32.png");			

		QAction *canvasloadmask = new QAction(this);
		canvasloadmask->setIcon(load);
		canvasloadmask->setText(tr("&Load Mask"));
		QAction *canvassavemask = new QAction(this);
		canvassavemask->setIcon(QIcon(save));
		canvassavemask->setText(tr("&Save Mask"));
		QAction *canvasundo = new QAction(this);
		canvasundo->setIcon(QIcon(undo));
		canvasundo->setText(tr("&Undo"));
		canvasundo->setShortcut(QKeySequence("Ctrl+Z"));
		QAction *canvasredo = new QAction(this);
		canvasredo->setIcon(QIcon(redo));
		canvasredo->setText(tr("&Redo"));
		canvasredo->setShortcut(QKeySequence("Ctrl+Shift+Z"));
		QAction *canvasclear = new QAction(tr("&Clear"), this);
		canvasclear->setShortcut(QKeySequence("Ctrl+C"));

		QAction *canvaspen = new QAction(this);
		canvaspen->setIcon(QIcon(pen));
		canvaspen->setText(tr("&Pen"));
		QAction *canvaseraser = new QAction(this);
		canvaseraser->setIcon(QIcon(eraser));
		canvaseraser->setText(tr("&Eraser"));

		QActionGroup *actions(new QActionGroup(this));
		actions->addAction(canvaspen);
		actions->addAction(canvaseraser);
		canvaspen->setCheckable(true);
		canvaseraser->setCheckable(true);
		canvaspen->setChecked(true);
		actions->setExclusive(true);

		QAction *canvasOK = new QAction(this);
		canvasOK->setText("OK");
		QAction *canvasCancel = new QAction(this);
		canvasCancel->setText("Cancel");

		QBoxLayout *layout(new QVBoxLayout(this));

		// We don't want a real-size image. We will downscale it!
		QImage image_to_use = image;
		pimpl_->realwidth_ = image.width();
		pimpl_->realheight_ = image.height();
		qDebug("maskImageWidget::Init real wxh %i x%i",pimpl_->realwidth_,pimpl_->realheight_);
		QDesktopWidget *desktop(QApplication::desktop());
		if (image.width() > (desktop->width() * .8) ||
				image.height() > (desktop->height() * .8))
		{
			int width(desktop->width()), height(desktop->height());
			image_to_use = image.scaled((int)std::floor(width * .75),
																	(int)std::floor(height * .75), Qt::KeepAspectRatio);
		}
		pimpl_->render_area_ = new maskRenderWidget(image_to_use, this);

		QToolBar *canvas_toolbar(new QToolBar(this));
		canvas_toolbar->addSeparator();
		canvas_toolbar->addAction(canvasloadmask);
		canvas_toolbar->addAction(canvassavemask);
		canvas_toolbar->addSeparator();
		
		canvas_toolbar->addAction(canvasundo);
		canvas_toolbar->addAction(canvasredo);
		canvas_toolbar->addSeparator();
		
		QSpinBox *pen_width(new QSpinBox(canvas_toolbar));
		pen_width->setToolTip(tr("Pen Width"));
		pen_width->setRange(0, 80);
		pen_width->setSingleStep(2);
		pen_width->setValue(16);
		connect(pen_width, SIGNAL(valueChanged(int)), SLOT(setCanvasPenWidth(int)));
		canvas_toolbar->addWidget(pen_width);
		canvas_toolbar->addAction(canvaspen);
		canvas_toolbar->addAction(canvaseraser);
		canvas_toolbar->addSeparator();

		QSpinBox *gradient(new QSpinBox(canvas_toolbar));
		gradient->setToolTip("Gradient Threshold");
		gradient->setRange(0, 255);
		gradient->setValue(pimpl_->threshold_gradient_);
		connect(gradient, SIGNAL(valueChanged(int)), SLOT(setGradientThreshold(int)));
		
		QSpinBox *fixed(new QSpinBox(canvas_toolbar));
		fixed->setToolTip("Fixed Threshold");
		fixed->setRange(0, 255);
		fixed->setValue(pimpl_->threshold_fixed_);
		connect(fixed, SIGNAL(valueChanged(int)), SLOT(setFixedThreshold(int)));
		
		canvas_toolbar->addWidget(gradient);
		canvas_toolbar->addWidget(fixed);
		canvas_toolbar->addSeparator();

		canvas_toolbar->addAction(canvasOK);
		canvas_toolbar->addAction(canvasCancel);

		layout->addWidget(canvas_toolbar);
		layout->addWidget(pimpl_->render_area_);
		layout->setSizeConstraint(QLayout::SetFixedSize);

		connect(canvasloadmask, SIGNAL(activated()), SLOT(loadMask()));
		connect(canvassavemask, SIGNAL(activated()), SLOT(saveMask()));
		connect(canvasundo, SIGNAL(activated()), pimpl_->render_area_, SLOT(undo()));
		connect(canvasredo, SIGNAL(activated()), pimpl_->render_area_, SLOT(redo()));
		connect(canvasclear, SIGNAL(activated()), pimpl_->render_area_, SLOT(clear()));
		connect(canvaspen, SIGNAL(activated()), SLOT(setCanvasPen()));
		connect(canvaseraser, SIGNAL(activated()), SLOT(setCanvasEraser()));

		connect(pimpl_->render_area_, SIGNAL(pointSelected(const QPoint &)), SLOT(automaticMask(const QPoint &)));

		connect(canvasOK, SIGNAL(activated()), SLOT(accept()));
		connect(canvasCancel, SIGNAL(activated()), SLOT(reject()));
	}

	void maskImageWidget::setCanvasPenWidth(int width)
	{
		QPen pen(pimpl_->render_area_->pen());
		pen.setWidth(width);
		pimpl_->render_area_->setPen(pen);
	}


	void maskImageWidget::setCanvasPen()
	{
		QPen pen(pimpl_->render_area_->pen());
		pen.setColor(QColor(Qt::black));
		pen.setJoinStyle(Qt::RoundJoin);
		pimpl_->render_area_->setPen(pen);
	}


	void maskImageWidget::setCanvasEraser()
	{
		QPen pen(pimpl_->render_area_->pen());
		pen.setColor(QColor(Qt::transparent));
		pen.setJoinStyle(Qt::RoundJoin);
		pimpl_->render_area_->setPen(pen);
	}


	void maskImageWidget::setGradientThreshold(int threshold_gradient)
	{
		pimpl_->threshold_gradient_ = threshold_gradient;
	}


	void maskImageWidget::setFixedThreshold(int threshold_fixed)
	{
		pimpl_->threshold_fixed_ = threshold_fixed;
	}

	void maskImageWidget::loadMask()
	{
		try
		{
			QString filename(QFileDialog::getOpenFileName(this, QString("Open mask file"), QString(), QString("*.png")));
			if (QString::null != filename)
				pimpl_->render_area_->load(filename);
		}
		catch (std::exception &e)
		{
			QMessageBox::warning(this, tr("Problem"), e.what());
		}
	}

	namespace
	{
		bool check_extension(QString &filename, const QString &ext)
		{
			bool ret(false);
			if (ext != filename.section('.', -1))
			{
				int index(filename.lastIndexOf('.'));
				if (-1 == index)
				{
					filename += '.';
					index += filename.size();
				}
				filename.replace(index + 1, ext.size(), ext);
				filename.resize(index + 1 + ext.size());
				ret = true;
			}
			return ret;
		}
	};

	void maskImageWidget::saveMask()
	{
		try
		{
			QString filename(QFileDialog::getSaveFileName(this, QString("Save mask file"), QString(), QString("*.png")));
			if (QString::null != filename)
			{
				check_extension(filename, QString("png"));
				pimpl_->render_area_->save(filename, pimpl_->realwidth_, pimpl_->realheight_);
			}
		}
		catch (std::exception &e)
		{
			QMessageBox::warning(this, tr("Epoch 3D Webservice"), e.what());
		}
	}


	void maskImageWidget::automaticMask(const QPoint &p)
	{
		QImage image = (pimpl_->render_area_->palette().base().texture()).toImage();
		QImage out;
		fillImage fi;
		fi.Compute(image, p.x(), p.y(), pimpl_->threshold_gradient_, pimpl_->threshold_fixed_, out);
		
		const size_t width(image.width()), height(image.height());
		QImage temp(pimpl_->render_area_->alphaMask());
		for (size_t i = 0; i < width; ++i)
		{
			for (size_t j = 0; j < height; ++j)
			{
				if (out.pixelIndex(i, j) > 0)
					temp.setPixel(i, j, QColor(Qt::black).rgba());
			}
		}
		//temp.save("temp.jpg","jpg");
		pimpl_->render_area_->setAlphaMask(temp);
	}
};
