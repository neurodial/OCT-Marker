#include "wgscanclassifier.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QToolBox>
#include <QPushButton>
#include <QToolButton>

#include"scanclassifier.h"
#include"classifiermarkerproxy.h"


WGScanClassifier::WGScanClassifier(ScanClassifier* parent)
: parent(parent)
{
	QVBoxLayout* layout = new QVBoxLayout();
	for(ClassifierMarkerProxy* proxy : parent->getScanClassifierProxys())
	{
		if(!proxy)
			continue;

		QGroupBox  * classifierGroup       = new QGroupBox(QString::fromStdString(proxy->getClassifierMarker().getViewName()), this);
		QVBoxLayout* classifierGroupLayout = new QVBoxLayout();

		for(QAction* action : proxy->getMarkerActions())
			classifierGroupLayout->addWidget(genButton(action));

		classifierGroup->setLayout(classifierGroupLayout);
		layout->addWidget(classifierGroup);
	}

	layout->addStretch();
	setLayout(layout);
}

WGScanClassifier::~WGScanClassifier()
{
}

QAbstractButton * WGScanClassifier::genButton(QAction* action)
{
	QToolButton* button = new QToolButton(this);
	button->setDefaultAction(action);
	return button;
}
