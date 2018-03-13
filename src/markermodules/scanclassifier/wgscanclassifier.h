#ifndef WGSCANCLASSIFIER_H
#define WGSCANCLASSIFIER_H

#include<QWidget>

#include<vector>

class QAction;
class QAbstractButton;

class ScanClassifier;

class QLayout;
class ClassifierMarkerProxy;

class WGScanClassifier : public QWidget
{
	void addProxys2Layout(std::vector<ClassifierMarkerProxy*> proxyList, QLayout& layout);

public:
	WGScanClassifier(ScanClassifier* parent);
	~WGScanClassifier();


	QAbstractButton* genButton(QAction* action);
};

#endif // WGSCANCLASSIFIER_H
