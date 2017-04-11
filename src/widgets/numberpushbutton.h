#ifndef NUMBERPUSHBUTTON_H
#define NUMBERPUSHBUTTON_H

#include <QPushButton>

class NumberPushButton : public QPushButton
{
	Q_OBJECT

	int number;
public:
	NumberPushButton(int number, QWidget *parent = nullptr)
	: QPushButton(parent)
	, number(number)
	{}

	void paintEvent(QPaintEvent* p) override;
};


#endif // NUMBERPUSHBUTTON_H
