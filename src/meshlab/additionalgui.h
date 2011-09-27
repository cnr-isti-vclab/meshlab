#ifndef ADDITIONALGUI_H
#define ADDITIONALGUI_H

#include <QtGui>
#include <QString>

class CheckBoxList: public QComboBox
{
    Q_OBJECT;

public:
    CheckBoxList(const QString& defaultValue,QWidget *widget = 0);
    ~CheckBoxList();
    void paintEvent(QPaintEvent *);
	bool eventFilter(QObject *object, QEvent *event);
    QStringList getSelected() const;
	void insertCheckableItem(const int pos,const QString& lab,const bool checked);
	void insertCheckableItem(const QString& lab,const bool checked );
	void updateSelected(int ind);
	QString selectedItemsString(const QString& sep) const;
	QStringList selectedItemsNames() const;
	
private slots:
	void currentHighlighted(int high);
private:
	QStringList sel;
	int highli;
	QString default;
};

#endif // CHECKBOXLIST_H