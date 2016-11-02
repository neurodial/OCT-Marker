#ifndef BSCANSEGTOOLBAR_H
#define BSCANSEGTOOLBAR_H

#include <QToolBar>

class BScanSegmentation;

class BScanSegToolBar : public QToolBar
{
	Q_OBJECT

public:
	BScanSegToolBar(BScanSegmentation* seg, QObject* parent);
};

#endif // BSCANSEGTOOLBAR_H
