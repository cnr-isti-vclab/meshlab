#ifndef ANGLE_EDIT_WIDGET_H
#define ANGLE_EDIT_WIDGET_H

#include <QLineEdit>
#include <QValidator>
#include <QString>

class AngleValidator : public QValidator
{
public:
	AngleValidator(QObject* parent)
		:QValidator(parent)
	{

	}

	State validate( QString & input, int & pos ) const
	{
		bool ok = true;
		int angle = input.toInt(&ok);
		if (!ok) 
			return QValidator::Invalid;
		if ((angle >= -360) && (angle <= 360)) return QValidator::Acceptable;
		else 
			return QValidator::Invalid; 
	}

	void fixup ( QString & input ) const
	{
		int angle = input.toInt();
		if (angle > 360) input = QString(360);
		else if (angle < -360) input = QString(-360);
	}
};

class AngleEditWidget : public QLineEdit
{
public:
	AngleEditWidget(QWidget* parent)
		:QLineEdit(parent),_val(parent)
	{
		setValidator(&_val);
	}

	const AngleValidator& validator() const
	{
		return _val;
	}

private:
	const AngleValidator _val;
};

#endif