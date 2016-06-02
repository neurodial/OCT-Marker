#ifndef MARKERMANAGER_H
#define MARKERMANAGER_H

#include <QObject>

class CScan;

class MarkerManager : public QObject
{
	Q_OBJECT

	int actBScan = 0;


	CScan* cscan = nullptr;


public:
	MarkerManager();
    virtual ~MarkerManager();

	int getActBScan() const                                     { return actBScan; }

	const CScan& getCScan()                                     { return *cscan;   }


public slots:
	virtual void chooseBScan(int bscan);
	virtual void inkrementBScan(int inkrement)                  { chooseBScan(actBScan + inkrement); }

	virtual void nextBScan()                                    { inkrementBScan(+1); }
	virtual void previousBScan()                                { inkrementBScan(-1); }


	virtual void loadOCTXml(QString filename);

signals:
	void bscanChanged(int bscan);
	void newCScanLoaded();
};

#endif // MARKERMANAGER_H
