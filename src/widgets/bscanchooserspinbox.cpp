#include "bscanchooserspinbox.h"


#include <octdata/datastruct/series.h>


#include <manager/octmarkermanager.h>
#include <manager/octdatamanager.h>

BScanChooserSpinBox::BScanChooserSpinBox(QWidget* parent)
: QSpinBox(parent)
{
	setAlignment(Qt::AlignRight);

	OctMarkerManager& markerManager = OctMarkerManager::getInstance();

	connect(&OctDataManager::getInstance(), &OctDataManager::seriesChanged, this, &BScanChooserSpinBox::configBscanChooser);

	connect(this, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), &markerManager, &OctMarkerManager::chooseBScan);
	connect(&markerManager, &OctMarkerManager::bscanChanged, this, &QSpinBox::setValue);

	configBscanChooser();
}

void BScanChooserSpinBox::configBscanChooser()
{
	const OctData::Series* series = OctDataManager::getInstance().getSeries();

	std::size_t maxBscan = 0;
	if(series)
	{
		maxBscan = series->bscanCount();
		if(maxBscan > 0)
			--maxBscan;
	}

	setMaximum(static_cast<int>(maxBscan));
	setValue(OctMarkerManager::getInstance().getActBScanNum());

	setSuffix(QString(" / %1").arg(maxBscan));
}

