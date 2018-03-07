#include "wgscanclassifier.h"

#include <QVBoxLayout>
#include <QToolBox>
#include <QPushButton>




WGScanClassifier::WGScanClassifier(ScanClassifier* parent)
: parent(parent)
, toolboxCollections(new QToolBox(this))
{
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(toolboxCollections);
	setLayout(layout);


	toolboxCollections->addItem(new QPushButton("test"), "bla");
	toolboxCollections->addItem(new QPushButton("test2"), "bla2");
}

WGScanClassifier::~WGScanClassifier()
{
}
