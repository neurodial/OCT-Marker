#ifndef MARKERMANAGER_H
#define MARKERMANAGER_H

#include <QObject>

class MarkerManager : public QObject
{
	int actBScan = 0;

public:
	int getActBScan() const                                     { return actBScan; }


public slots:
	virtual void chooseBScan(int bscan);
	virtual void inkrementBScan(int inkrement);


signals:
	void bscanChanged(int bscan);
};

#endif // MARKERMANAGER_H
