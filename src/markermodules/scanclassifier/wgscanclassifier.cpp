#include "wgscanclassifier.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QToolBox>
#include <QPushButton>
#include <QToolButton>

#include"scanclassifier.h"
#include"classifiermarkerproxy.h"

#include<qt/flowlayout.h>

WGScanClassifier::WGScanClassifier(ScanClassifier* parent)
{
	QVBoxLayout* layout = new QVBoxLayout();
	addProxys2Layout(parent->getScanClassifierProxys(), *layout);

	addProxys2Layout(parent->getBScanClassifierProxys(), *layout);

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

void WGScanClassifier::addProxys2Layout(std::vector<ClassifierMarkerProxy*> proxyList, QLayout& layout)
{
	for(ClassifierMarkerProxy* proxy : proxyList)
	{
		if(!proxy)
			continue;

		QGroupBox  * classifierGroup       = new QGroupBox(QString::fromStdString(proxy->getClassifierMarker().getViewName()), this);
// 		QVBoxLayout* classifierGroupLayout = new QVBoxLayout();
		FlowLayout* classifierGroupLayout = new FlowLayout();

		for(QAction* action : proxy->getMarkerActions())
			classifierGroupLayout->addWidget(genButton(action));

		classifierGroup->setLayout(classifierGroupLayout);
		layout.addWidget(classifierGroup);
	}
}
