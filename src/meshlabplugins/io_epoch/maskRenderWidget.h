#ifndef __IO_MASK_RENDER_WIDGET_INC__
#define __IO_MASK_RENDER_WIDGET_INC__


#include <QtGui/QWidget>


namespace ui
{
  /*! \class maskRenderWidget
    \brief A brief description
    \author gmatthew
  */
  class maskRenderWidget : public QWidget
  {
    Q_OBJECT

    struct Impl;
    Impl* pimpl_;

    virtual void keyPressEvent(QKeyEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);

  public:
    /*! \brief Constructor
    */
    explicit maskRenderWidget(QWidget *parent = 0);
    /*! \brief Constructor
    */
    explicit maskRenderWidget(const QImage &, QWidget *parent = 0);
    /*! \brief Destructor
    */
    virtual ~maskRenderWidget() throw();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
#endif

    /*! \brief Set the drawable pen.
      \param The pen object.
    */
    void setPen(const QPen &pen);
    /*! \brief Returns the drawable pen.
      \return pen The pen object.
    */
    QPen pen() const;
    /*! \brief Set the background Image.
      \param image The image to be set as the background.
    */
    void setImage(const QImage &);
    /*! \brief Load the alpha mask.
      \param filename The path to the image to be used as the mask.
    */
    void load(const QString &filename);
    /*! \brief Save the alpha mask.
      \param filename The path to the image to be used as the mask.
			\param w The width of the image to save to.
			\param h The height of the image to save to.
    */
    void save(const QString &filename, int w, int h);
    /*! \brief Get the alpha mask.
			\param w The width of the image to return.
			\param h The height of the image to return.
    */
    QImage getMask(int w, int h) const;
    /*! \brief Set the alpha mask.
      \param image The image to be set as the mask.
    */
    void setAlphaMask(const QImage &image);
    /*! \brief Returns the alpha mask.
      \return An qimage object.
    */
    QImage alphaMask() const;

  public slots:
    /*! \brief Undoes the last action and adds the current action to the redo stack and updates the display.  If no more actions could be undone, does nothing.

    The number of times this can be done is limited only by the resources available.
    */
    void undo();
    /*! \brief Redoes the last action and adds the current action to the undo stack and updates the display.  If no more actions could be redone, does nothing.

    The number of times this can be done is limited only by the resources available.
    */
    void redo();
    /*! \brief Clears the display.

    This action is also added to the undo actions.
    */
    void clear();

  signals:
    void pointSelected(const QPoint &);
  };
};


#endif
