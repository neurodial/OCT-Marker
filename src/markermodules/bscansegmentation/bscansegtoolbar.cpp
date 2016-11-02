#include "bscansegtoolbar.h"

#include <QSpinBox>

#include "bscansegmentation.h"


namespace
{
	QIcon createColorIcon(const QColor& color)
	{
		QPixmap pixmap(15, 15);
		pixmap.fill(color);
		return QIcon(pixmap);
	}

	QIcon createMonocromeIcon(uint8_t grayvalue)
	{
		return createColorIcon(QColor::fromRgb(grayvalue, grayvalue, grayvalue));
	}

}




BScanSegToolBar::BScanSegToolBar(BScanSegmentation* seg, QObject* parent)
{

	QActionGroup*  actionGroupMethod  = new QActionGroup(parent);
	QActionGroup*  actionPaintMethod  = new QActionGroup(parent);
	// QToolBar*      toolBar            = new QToolBar(tr("Segmentation"));

	setObjectName("ToolBarSegmentationMarker");

	QWidget* parentWidget = dynamic_cast<QWidget*>(parent);

	/*
	QSpinBox* paintSizeSpinBox = new QSpinBox(parentWidget);
	paintSizeSpinBox->setMinimum(1);
	paintSizeSpinBox->setValue(seg->getLocalOperatorSize());
	connect(paintSizeSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), seg             , &BScanSegmentation::setLocalOperatorSize);
	connect(seg             , &BScanSegmentation::localOperatorSizeChanged                , paintSizeSpinBox, &QSpinBox::setValue                     );
	addWidget(paintSizeSpinBox);
	*/

	/*
	QAction* actionPaintMethodDisc = new QAction(parent);
	actionPaintMethodDisc->setCheckable(true);
	actionPaintMethodDisc->setChecked(paintMethod == PaintMethod::Disc);
	actionPaintMethodDisc->setText(tr("Disc"));
	actionPaintMethodDisc->setIcon(QIcon(":/icons/paint_disc.png"));
	connect(actionPaintMethodDisc, &QAction::triggered, this, &BScanSegmentation::setPaintMethodDisc);
	toolBar->addAction(actionPaintMethodDisc);
	actionPaintMethod->addAction(actionPaintMethodDisc);


	QAction* actionPaintMethodQuadrat = new QAction(parent);
	actionPaintMethodQuadrat->setCheckable(true);
	actionPaintMethodQuadrat->setText(tr("Quadrat"));
	actionPaintMethodQuadrat->setChecked(paintMethod == PaintMethod::Quadrat);
	actionPaintMethodQuadrat->setIcon(QIcon(":/icons/paint_quadrat.png"));
	connect(actionPaintMethodQuadrat, &QAction::triggered, this, &BScanSegmentation::setPaintMethodQuadrat);
	toolBar->addAction(actionPaintMethodQuadrat);
	actionPaintMethod->addAction(actionPaintMethodQuadrat);

	toolBar->addSeparator();

	QAction* addAreaAction = new QAction(parent);
	addAreaAction->setCheckable(true);
	addAreaAction->setText(tr("Add"));
	addAreaAction->setIcon(createMonocromeIcon(paintArea0Value*255));
	connect(addAreaAction, &QAction::triggered, this, &BScanSegmentation::paintArea0Slot);
	connect(this, &BScanSegmentation::paintArea0Selected, addAreaAction, &QAction::setChecked);
	toolBar->addAction(addAreaAction);
	actionGroupMethod->addAction(addAreaAction);

	QAction* autoRemoveAddAreaAction = new QAction(parent);
	autoRemoveAddAreaAction->setCheckable(true);
	autoRemoveAddAreaAction->setChecked(autoPaintValue);
	autoRemoveAddAreaAction->setText(tr("Auto"));
	autoRemoveAddAreaAction->setIcon(createMonocromeIcon(124));
	connect(autoRemoveAddAreaAction, &QAction::triggered, this, &BScanSegmentation::autoAddRemoveArea);
	connect(this, &BScanSegmentation::paintAutoAreaSelected, autoRemoveAddAreaAction, &QAction::setChecked);
	toolBar->addAction(autoRemoveAddAreaAction);
	actionGroupMethod->addAction(autoRemoveAddAreaAction);

	QAction* removeAreaAction = new QAction(parent);
	removeAreaAction->setCheckable(true);
	removeAreaAction->setText(tr("Add"));
	removeAreaAction->setIcon(createMonocromeIcon(paintArea1Value*255));
	connect(removeAreaAction, &QAction::triggered, this, &BScanSegmentation::paintArea1Slot);
	connect(this, &BScanSegmentation::paintArea1Selected, removeAreaAction, &QAction::setChecked);
	toolBar->addAction(removeAreaAction);
	actionGroupMethod->addAction(removeAreaAction);
*/

	addSeparator();

	/*
	QAction* actionInitFromIlm = new QAction(parent);
	actionInitFromIlm->setText(tr("Init from ILM"));
	actionInitFromIlm->setIcon(QIcon(":/icons/wand.png"));
	connect(actionInitFromIlm, &QAction::triggered, this, &BScanSegmentation::initFromSegmentline);
	connect(this, &BScanSegmentation::paintArea1Selected, actionInitFromIlm, &QAction::setChecked);
	toolBar->addAction(actionInitFromIlm);


	QAction* actionInitFromTrashold = new QAction(parent);
	actionInitFromTrashold->setText(tr("Init from threshold"));
	actionInitFromTrashold->setIcon(QIcon(":/icons/wand.png"));
	connect(actionInitFromTrashold, &QAction::triggered, this, &BScanSegmentation::initFromThreshold);
	connect(this, &BScanSegmentation::paintArea1Selected, actionInitFromTrashold, &QAction::setChecked);
	toolBar->addAction(actionInitFromTrashold);

	toolBar->addSeparator();
	*/

	QAction* actionErodeBScan = new QAction(parent);
	actionErodeBScan->setText(tr("Erode"));
	actionErodeBScan->setIcon(QIcon(":/icons/arrow_in.png"));
	connect(actionErodeBScan, &QAction::triggered, seg, &BScanSegmentation::erodeBScan);
	connect(seg, &BScanSegmentation::paintArea1Selected, actionErodeBScan, &QAction::setChecked);
	addAction(actionErodeBScan);


	QAction* actionDilateBScan = new QAction(parent);
	actionDilateBScan->setText(tr("Dilate"));
	actionDilateBScan->setIcon(QIcon(":/icons/arrow_out.png"));
	connect(actionDilateBScan, &QAction::triggered, seg, &BScanSegmentation::dilateBScan);
	connect(seg, &BScanSegmentation::paintArea1Selected, actionDilateBScan, &QAction::setChecked);
	addAction(actionDilateBScan);

	QAction* actionOpenCloseBScan = new QAction(parent);
	actionOpenCloseBScan->setText(tr("Open/Close"));
	actionOpenCloseBScan->setIcon(QIcon(":/icons/arrow_inout.png"));
	connect(actionOpenCloseBScan, &QAction::triggered, seg, &BScanSegmentation::opencloseBScan);
	connect(seg, &BScanSegmentation::paintArea1Selected, actionOpenCloseBScan, &QAction::setChecked);
	addAction(actionOpenCloseBScan);

	QAction* actionMedianBScan = new QAction(parent);
	actionMedianBScan->setText(tr("Median"));
	actionMedianBScan->setIcon(QIcon(":/icons/arrow_inout.png"));
	connect(actionMedianBScan, &QAction::triggered, seg, &BScanSegmentation::medianBScan);
	connect(seg, &BScanSegmentation::paintArea1Selected, actionMedianBScan, &QAction::setChecked);
	addAction(actionMedianBScan);

}
