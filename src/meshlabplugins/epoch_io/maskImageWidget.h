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
