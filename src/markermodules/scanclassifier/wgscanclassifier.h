#ifndef WGSCANCLASSIFIER_H
#define WGSCANCLASSIFIER_H

#include <QWidget>

class QAction;
class QAbstractButton;

class ScanClassifier;

class WGScanClassifier : public QWidget
{
	ScanClassifier* parent;

public:
	WGScanClassifier(ScanClassifier* parent);
	~WGScanClassifier();


	QAbstractButton* genButton(QAction* action);
};

#endif // WGSCANCLASSIFIER_H
