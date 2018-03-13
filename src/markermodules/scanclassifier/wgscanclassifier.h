#ifndef WGSCANCLASSIFIER_H
#define WGSCANCLASSIFIER_H

#include<vector>

#include<QWidget>

#include"scanclassifier.h"

class QAction;
class QAbstractButton;
class QLayout;

class ScanClassifier;

class ClassifierMarkerProxy;

class WGScanClassifier : public QWidget
{
	void addProxys2Layout(ScanClassifier::ClassifierProxys& proxyList, QLayout& layout);

public:
	WGScanClassifier(ScanClassifier* parent);
	~WGScanClassifier();


	QAbstractButton* genButton(QAction* action);
};

#endif // WGSCANCLASSIFIER_H
