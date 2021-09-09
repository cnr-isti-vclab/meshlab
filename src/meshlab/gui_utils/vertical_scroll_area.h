#ifndef VERTICALSCROLLAREA_H
#define VERTICALSCROLLAREA_H

#include <QScrollArea>

/**
 * @brief The VerticalScrollArea class
 *
 * This class is necessary because Qt does not provide a pure Vertical Scroll Area,
 * that behaves as a normal qwidget horizontally.
 *
 * @see https://forum.qt.io/topic/13374/solved-qscrollarea-vertical-scroll-only
 */
class VerticalScrollArea : public QScrollArea
{
	Q_OBJECT
public:
	explicit VerticalScrollArea(QWidget* parent = 0);

	virtual bool eventFilter(QObject* o, QEvent* e);

private:
	QWidget* m_scrollAreaWidgetContents;
};

#endif // VERTICALSCROLLAREA_H
