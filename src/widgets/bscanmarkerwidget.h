#ifndef BSCANMARKERWIDGET_H
#define BSCANMARKERWIDGET_H

#include "cvimagewidget.h"

#include <QPoint>

namespace OctData { class BScan; }

class QWheelEvent;
class QMouseEvent;
class BScanMarkerManager;

class BScanMarkerWidget : public CVImageWidget
{
	Q_OBJECT
/*
	QPoint clickPos;
	QPoint mousePos;

	bool markerActiv = false;
	bool mouseInWidget = false;*/

	BScanMarkerManager& markerManger;
	
	QAction*                                saveRawImageAction = nullptr;
	QAction*                                saveRawMatAction   = nullptr;
	QAction*                                saveRawBinAction   = nullptr;
	QAction*                                saveImageBinAction = nullptr;
	const OctData::BScan*                   actBscan           = nullptr;
	
	int fdSaveRaw(QString& filename);
	void updateRawExport();
	bool existsRaw() const;
	bool rawSaveableAsImage() const;
public:
	BScanMarkerWidget(BScanMarkerManager& markerManger);

	virtual ~BScanMarkerWidget();

protected:
	void paintEvent(QPaintEvent* event);

	virtual void mouseMoveEvent   (QMouseEvent*);
	virtual void mousePressEvent  (QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	
	virtual void keyPressEvent    (QKeyEvent*  );
	virtual void leaveEvent       (QEvent*     );
	virtual void wheelEvent       (QWheelEvent*);

private slots:
	void bscanChanged(int);
	void cscanLoaded();

	void viewOptionsChangedSlot();

	// void markersMethodChanged();
	

public slots:
	virtual void saveRawImage();
	virtual void saveRawMat  ();
	virtual void saveRawBin  ();
	virtual void saveImageBin();
	
signals:
	void bscanChangeInkrement(int delta);
};

#endif // BSCANMARKERWIDGET_H
