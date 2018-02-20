#ifndef NUMBERPUSHBUTTON_H
#define NUMBERPUSHBUTTON_H

#include <QPushButton>

class NumberPushButton : public QPushButton
{
	Q_OBJECT

	QString additionalText;
public:
	NumberPushButton(int number, QWidget *parent = nullptr)
	: QPushButton(parent)
	, additionalText(QString::number(number))
	{}

	NumberPushButton(const QString& number, QWidget *parent = nullptr)
	: QPushButton(parent)
	, additionalText(number)
	{}

	void paintEvent(QPaintEvent* p) override;
};


#endif // NUMBERPUSHBUTTON_H
