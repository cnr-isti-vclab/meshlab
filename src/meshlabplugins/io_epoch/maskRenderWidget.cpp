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

#include "maskRenderWidget.h"
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
  namespace priv
  {
    template <class T>
    inline void unwind(std::stack<T> &stack)
    {
      while (!stack.empty())
				stack.pop();
    }
  };


  struct maskRenderWidget::Impl
  {
    enum Shape { Nothing, Polyline, Rect, Rubber, Point } shape_;

    QPen pen_;
    QPolygon polygon_;

    QPoint start_point_, last_point_, end_point_;
    QRect rubber_band_;

		QImage foreground_, band_buffer_;
		std::stack<QImage> undo_, redo_;

    Impl();

    void paintOnDevice(QPaintDevice *);
  };


  maskRenderWidget::Impl::Impl() : pen_(Qt::black)
  {
    shape_ = Nothing;
    pen_.setWidth(16);
    pen_.setCapStyle(Qt::RoundCap);
  }


  void maskRenderWidget::Impl::paintOnDevice(QPaintDevice *device)
  {
		assert(device);
    QPainter painter(device);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    switch (shape_)
    {
    case Impl::Polyline:
      {
				painter.setPen(pen_);
				painter.drawPolyline(polygon_);
      }
      break;
    case Impl::Point:
      {
				painter.setPen(pen_);
				QPoint p2(end_point_.x() + 1, end_point_.y() + 1);
				painter.drawLine(end_point_, p2);
      }
      break;
    case Impl::Rect:
      {
				QPen pen;
				pen.setColor(pen_.color());
				painter.setPen(pen);
				const int x(rubber_band_.x()), y(rubber_band_.y());
				const int w(rubber_band_.width()), h(rubber_band_.height());
				for (int i = 0; i < w; ++i)
					for (int j = 0; j < h; ++j)
						painter.drawPoint(QPoint(x + i, y + j));
				rubber_band_ = QRect(0, 0, 0, 0);
      }
      break;
    case Impl::Rubber:
      {
				QPen pen(Qt::gray);
				pen.setWidth(1);
				painter.setPen(pen);
				painter.drawRect(rubber_band_);
      }
      break;
    default:
      break;
    }
  }


  maskRenderWidget::maskRenderWidget(QWidget *parent) : QWidget(parent), pimpl_(new Impl)
  {
    setAttribute(Qt::WA_StaticContents);
    setBackgroundRole(QPalette::Base);
    QImage image(640, 480, QImage::Format_ARGB32);
    image.fill(Qt::white);
    setImage(image);
		setFocusPolicy(Qt::StrongFocus);
  }

  maskRenderWidget::maskRenderWidget(const QImage& image, QWidget *parent) : QWidget(parent), pimpl_(new Impl)
  {
	  qDebug("MaskRenderWidget started with an image %i x %i",image.width(),image.height());
    setAttribute(Qt::WA_StaticContents);
    setBackgroundRole(QPalette::Base);
    setImage(image);
		setFocusPolicy(Qt::StrongFocus);
  }

  maskRenderWidget::~maskRenderWidget() throw()
  {
		delete pimpl_;
  }

  void maskRenderWidget::keyPressEvent(QKeyEvent *e)
  {
    if (e->key() == Qt::Key_Z && (e->modifiers() & Qt::ControlModifier))
    {
			undo();
    }
  }


  void maskRenderWidget::mousePressEvent(QMouseEvent *e)
  {
    if (e->button() == Qt::LeftButton)
    {
      if (e->modifiers() & Qt::ShiftModifier)
      {
				emit pointSelected(e->pos());
      }
      else
      {
				pimpl_->undo_.push(pimpl_->foreground_);
				pimpl_->end_point_ = e->pos();
				pimpl_->polygon_ = QPolygon();
				pimpl_->polygon_ << e->pos();
				priv::unwind(pimpl_->redo_);
				pimpl_->shape_ = Impl::Point;
				update();
      }
    }
    else if (e->button() == Qt::RightButton)
    {
      pimpl_->undo_.push(pimpl_->foreground_);
      QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
      pimpl_->start_point_ = e->pos();
      pimpl_->shape_ = Impl::Rubber;
    }
  }


  void maskRenderWidget::mouseMoveEvent(QMouseEvent *e)
  {
    if (Impl::Rubber == pimpl_->shape_)
    {
      pimpl_->band_buffer_ = pimpl_->foreground_;
      int x(std::min(e->pos().x(), pimpl_->start_point_.x()));
      int y(std::min(e->pos().y(), pimpl_->start_point_.y()));
      int w(std::abs((float)e->pos().x() - pimpl_->start_point_.x()));
      int h(std::abs((float)e->pos().y() - pimpl_->start_point_.y()));

      pimpl_->rubber_band_ = QRect(x, y, w, h);
      update();
    }
    else if (Impl::Point == pimpl_->shape_)
    {
      pimpl_->shape_ = Impl::Polyline;
    }
    else if (Impl::Polyline == pimpl_->shape_)
    {
      pimpl_->last_point_ = pimpl_->end_point_;
      pimpl_->end_point_ = e->pos();
      pimpl_->polygon_ << e->pos();
      update();
    }
  }


  void maskRenderWidget::mouseReleaseEvent(QMouseEvent *e)
  {
    if (Impl::Rubber == pimpl_->shape_)
    {
      QApplication::restoreOverrideCursor();
      pimpl_->shape_ = Impl::Rect;
      update();
      return;
    }
    else if (Impl::Polyline == pimpl_->shape_)
    {
      pimpl_->last_point_ = pimpl_->end_point_;
      pimpl_->end_point_ = e->pos();
      update();      
    }
    pimpl_->shape_ = Impl::Nothing;
  }


  void maskRenderWidget::paintEvent(QPaintEvent *e)
  {
		QImage * device = &pimpl_->foreground_;

    if (Impl::Rubber == pimpl_->shape_)
      device = &pimpl_->band_buffer_;

    pimpl_->paintOnDevice(device);

    QPainter painter(this);
    QVector<QRect> rects(e->region().rects());
    for (int i = 0; i < rects.count(); ++i)
    {
      QRect r = rects[i];
			painter.drawImage(r, *device, r);
    }
  }


  QSize maskRenderWidget::sizeHint() const
  {
    return minimumSizeHint();
  }


  QSize maskRenderWidget::minimumSizeHint() const
  {
    return pimpl_->foreground_.isNull()? QSize(400, 400) : pimpl_->foreground_.size();
  }


  void maskRenderWidget::setPen(const QPen &pen)
  {
    pimpl_->pen_ = pen;
  }


  QPen maskRenderWidget::pen() const
  {
    return pimpl_->pen_;
  }


  void maskRenderWidget::setImage(const QImage &image)
  {
    QPalette palette;
#if (QT_VERSION >= 0x040100)
		setAutoFillBackground(true);
#endif
		palette.setBrush(backgroundRole(), QBrush(QPixmap::fromImage(image)));
    setPalette(palette);
    pimpl_->foreground_ = image;
		QImage alpha(image.width(), image.height(),QImage::Format_Mono);
		alpha.fill(0);
		
		pimpl_->foreground_.setAlphaChannel(alpha);

    priv::unwind(pimpl_->undo_);
    priv::unwind(pimpl_->redo_);

    update();
  }


  void maskRenderWidget::load(const QString &filename)
  {
    QImage alpha(filename);
		// I would have liked to use KeepAspectRatio but if someone loads a
		// bogus mask with a different ratio, the rest will crash. The output
		// is now undefined but it won't crash.
		alpha = alpha.scaled(pimpl_->foreground_.width(), pimpl_->foreground_.height(), Qt::IgnoreAspectRatio);
		QImage temp(pimpl_->foreground_);
    const int width(temp.width()), height(temp.height());
    for (int i = 0; i < width; ++i)
      for (int j = 0; j < height; ++j)
      {
				QRgb rgb = temp.pixel(i, j);
				temp.setPixel(i, j, QColor(qRed(rgb), qGreen(rgb), qBlue(rgb), qGray(alpha.pixel(i, j))).rgba());
      }
    setAlphaMask(temp);
  }


  void maskRenderWidget::save(const QString &filename, int w, int h)
  {
    pimpl_->foreground_.alphaChannel().scaled(w, h, Qt::KeepAspectRatio).save(filename, "PGM");
  }

  QImage maskRenderWidget::getMask(int w, int h) const
  {
//    return pimpl_->foreground_.alphaChannel().scaled(w, h, Qt::KeepAspectRatio);
    return pimpl_->foreground_.alphaChannel().scaled(w, h); // changed to this becouse sometimes for rounding error did not create the original size.

  }


  void maskRenderWidget::setAlphaMask(const QImage &image)
  {
    pimpl_->undo_.push(pimpl_->foreground_);
    pimpl_->foreground_ = image;
    update();
  }


  QImage maskRenderWidget::alphaMask() const
  {
    return pimpl_->foreground_;
  }


  void maskRenderWidget::undo()
  {
    if (!pimpl_->undo_.empty())
    {
      pimpl_->redo_.push(pimpl_->foreground_);
      pimpl_->foreground_ = pimpl_->undo_.top();
      pimpl_->undo_.pop();
      update();
    }
  }


  void maskRenderWidget::redo()
  {
    if (!pimpl_->redo_.empty())
    {
      pimpl_->undo_.push(pimpl_->foreground_);
      pimpl_->foreground_ = pimpl_->redo_.top();
      pimpl_->redo_.pop();
      update();
    }
  }


  void maskRenderWidget::clear()
  {
    pimpl_->undo_.push(pimpl_->foreground_);
    priv::unwind(pimpl_->redo_);
    pimpl_->foreground_.fill(QColor(Qt::transparent).rgba());
    update();
  }
};
