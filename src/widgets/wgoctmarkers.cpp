#include "wgoctmarkers.h"

#include<QTableView>
#include<QVBoxLayout>

#include <model/paintmarkermodel.h>

WGOctMarkers::WGOctMarkers(PaintMarkerModel* model)
: view(new QTableView)
, model(model)
{

	view->setModel(model);


	view->setSelectionBehavior(QAbstractItemView::SelectRows);
	view->setSelectionMode(QAbstractItemView::SingleSelection);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(view);

}
