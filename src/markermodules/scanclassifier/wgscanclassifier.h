#ifndef WGSCANCLASSIFIER_H
#define WGSCANCLASSIFIER_H

#include <QWidget>

class QToolBox;

class ScanClassifier;

class WGScanClassifier : public QWidget
{
	ScanClassifier* parent;
	QToolBox* toolboxCollections = nullptr;

public:
	WGScanClassifier(ScanClassifier* parent);
	~WGScanClassifier();
};

#endif // WGSCANCLASSIFIER_H
