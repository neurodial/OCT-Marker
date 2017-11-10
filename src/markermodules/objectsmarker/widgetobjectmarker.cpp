#include "widgetobjectmarker.h"

#include<QVBoxLayout>
#include<QPushButton>


WidgetObjectMarker::WidgetObjectMarker(QWidget* parent)
: QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	QPushButton* button = new QPushButton(this);
	button->setText(tr("add rect"));

	layout->addWidget(button);

	layout->addStretch();

	setLayout(layout);
}
