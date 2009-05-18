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

#ifndef __IO_MASK_IMAGE_WIDGET_INC__
#define __IO_MASK_IMAGE_WIDGET_INC__


#include <QtGui/QDialog>


namespace ui
{
  /*! \class maskImageWidget
    \brief A brief description
    \author Maarten Vergauwen
	*/
  class maskImageWidget : public QDialog
  {
    Q_OBJECT

    struct Impl;
    Impl* pimpl_;

		void init (const QImage &);

  public:
    /*! \brief Constructor
			\param The image.
    */
    explicit maskImageWidget(const QImage &, QWidget *parent = 0);
;
    /*! \brief Destructor
    */
    virtual ~maskImageWidget() throw();

		QImage getMask() const;

  public slots:
		
		void loadMask(const QString& filename);

  private slots:
								
    void setCanvasPenWidth(int);
    void setCanvasPen();
    void setCanvasEraser();
    void setGradientThreshold(int);
    void setFixedThreshold(int);
    void automaticMask(const QPoint &);
    void loadMask();
    void saveMask();
  };
};


#endif
