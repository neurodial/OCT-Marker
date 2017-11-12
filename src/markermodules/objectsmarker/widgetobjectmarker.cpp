#include "widgetobjectmarker.h"

#include<QVBoxLayout>
#include<QPushButton>


WidgetObjectMarker::WidgetObjectMarker(QWidget* parent)
: QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	addObjectButton = new QPushButton(this);

	addObjectButton->setText(tr("add rect"));
	addObjectButton->setCheckable(true);

	connect(addObjectButton, &QAbstractButton::toggled, this, &WidgetObjectMarker::addObjectStatusChangedSlotPrivte);

	layout->addWidget(addObjectButton);

	layout->addStretch();

	setLayout(layout);
}

void WidgetObjectMarker::addObjectStatusChangedSlot(bool v)
{
	addObjectButton->setChecked(v);
}

void WidgetObjectMarker::addObjectStatusChangedSlotPrivte(bool v)
{
	addObjectStatusChangedSignal(v);
}

