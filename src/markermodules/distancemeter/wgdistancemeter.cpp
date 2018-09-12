/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "wgdistancemeter.h"

#include<cmath>

#include<QFormLayout>
#include<QVBoxLayout>
#include<QGroupBox>
#include<QLabel>
#include<QPushButton>


namespace
{
	QGroupBox* createBox(QWidget* parent, const QString& name, QWidget* widget1, QWidget* widget2 = nullptr)
	{
		QGroupBox* startPosBox = new QGroupBox(parent);
		startPosBox->setTitle(name);
		QVBoxLayout* boxLayout = new QVBoxLayout(startPosBox);
		boxLayout->addWidget(widget1);
		if(widget2)
			boxLayout->addWidget(widget2);
		startPosBox->setLayout(boxLayout);
		return startPosBox;
	}

	void setNumText(QLabel* label, double num)
	{
		if(std::isnan(num))
			label->setText("");
		else
			label->setNum(num);
	}
}


WGDistanceMeter::WGDistanceMeter(DistanceMeter* parent)
: startPos (new DistanceMeterPositionWidget(this))
, actualPos(new DistanceMeterPositionWidget(this))
, clickPos (new DistanceMeterPositionWidget(this))
, actualDistance(new DistanceMeterDistanceWidget(this))
, clickDistance (new DistanceMeterDistanceWidget(this))
{
	QPushButton* setNewPosButton = new QPushButton(this);
	setNewPosButton->setText(tr("set start pos"));
	setNewPosButton->setCheckable(true);
	setNewPosButton->setChecked(parent->isSetNewStartPosition());
	connect(parent         , &DistanceMeter::changeSetStartPosition, setNewPosButton, &QPushButton::setChecked           );
	connect(setNewPosButton, &QPushButton::toggled                 , parent         , &DistanceMeter::setNewStartPosition);

	QVBoxLayout* layout = new QVBoxLayout(this);

	layout->addWidget(createBox(this, tr("start pos" ), startPos , setNewPosButton));
	layout->addWidget(createBox(this, tr("click pos" ), clickPos , clickDistance ));
	layout->addWidget(createBox(this, tr("actual pos"), actualPos, actualDistance));

	layout->addStretch();

	setLayout(layout);

	connect(parent, &DistanceMeter::updatedStartPos , startPos , &DistanceMeterPositionWidget::showPosition);
	connect(parent, &DistanceMeter::updatedActualPos, actualPos, &DistanceMeterPositionWidget::showPosition);
	connect(parent, &DistanceMeter::updatedClickPos , clickPos , &DistanceMeterPositionWidget::showPosition);

	connect(parent, &DistanceMeter::updateMousePosDistance, actualDistance, &DistanceMeterDistanceWidget::showDistance);
	connect(parent, &DistanceMeter::updateClickDistance   , clickDistance , &DistanceMeterDistanceWidget::showDistance);

}

WGDistanceMeter::~WGDistanceMeter()
{
}


DistanceMeterPositionWidget::DistanceMeterPositionWidget(QWidget* parent)
: QWidget(parent)
, xPosLabel(new QLabel(this))
, yPosLabel(new QLabel(this))
, zPosLabel(new QLabel(this))
{
	QFormLayout* layout = new QFormLayout(this);

	layout->addRow("x", xPosLabel);
	layout->addRow("y", yPosLabel);
	layout->addRow("z", zPosLabel);

	setLayout(layout);

}

void DistanceMeterPositionWidget::showPosition(const Position& p)
{
	if(p.valid)
	{
		xPosLabel->setNum(p.x_milliMeter);
		yPosLabel->setNum(p.y_milliMeter);
		zPosLabel->setText(QString("%1").arg(p.z_milliMeter));
	}
	else
	{
		xPosLabel->setText("-");
		yPosLabel->setText("-");
		zPosLabel->setText("-");
	}
}

DistanceMeterDistanceWidget::DistanceMeterDistanceWidget(QWidget* parent)
: QWidget(parent)
, labelMillimeter     (new QLabel(this))
, labelBscanMillimeter(new QLabel(this))
, labelBscanPx        (new QLabel(this))
{
	QFormLayout* layout = new QFormLayout(this);

	labelMillimeter     ->setTextInteractionFlags(Qt::TextSelectableByMouse);
	labelBscanMillimeter->setTextInteractionFlags(Qt::TextSelectableByMouse);
	labelBscanPx        ->setTextInteractionFlags(Qt::TextSelectableByMouse);

	layout->addRow(tr("Millimeter"         ), labelMillimeter     );
	layout->addRow(tr("Millimeter (B-Scan)"), labelBscanMillimeter);
	layout->addRow(tr("Pixel (B-Scan)"     ), labelBscanPx        );
	setLayout(layout);
}

void DistanceMeterDistanceWidget::showDistance(const Distance& d)
{
	setNumText(labelMillimeter     , d.millimeter     );
	setNumText(labelBscanMillimeter, d.bscanMillimeter);
	setNumText(labelBscanPx        , d.bscanPx        );
}
