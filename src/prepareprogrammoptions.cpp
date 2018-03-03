#include"prepareprogrammoptions.h"

#include<data_structure/programoptions.h>


void PrepareProgrammOptions::prepareProgrammOptions()
{

	QAction* layerSegBlendThicknessmapAction = ProgramOptions::layerSegThicknessmapBlend.getAction();
	layerSegBlendThicknessmapAction->setText(tr("Blend Thicknessmap Color"));
	layerSegBlendThicknessmapAction->setIcon(QIcon(":/icons/typicons/waves-outline.svg"));


	QAction* actionShowOverlay = ProgramOptions::sloShowOverlay.getAction();
	actionShowOverlay->setText(tr("show overlay"));
	actionShowOverlay->setIcon(QIcon(":/icons/typicons/brush.svg"));


	QAction* autoSaveOctMarkers       = ProgramOptions::autoSaveOctMarkers.getAction();
	autoSaveOctMarkers->setText(tr("Autosave markers"));


	QAction* fillEpmtyPixelWhite = ProgramOptions::fillEmptyPixelWhite.getAction();
	QAction* registerBScans      = ProgramOptions::registerBScans     .getAction();
	QAction* loadRotateSlo       = ProgramOptions::loadRotateSlo      .getAction();
	QAction* holdOCTRawData      = ProgramOptions::holdOCTRawData     .getAction();
	QAction* readBScans          = ProgramOptions::readBScans         .getAction();
	QAction* saveOctBinFlat      = ProgramOptions::saveOctBinFlat     .getAction();
	fillEpmtyPixelWhite->setText(tr("Fill empty pixels white"));
	registerBScans     ->setText(tr("register BScans"));
	loadRotateSlo      ->setText(tr("rotate SLO"));
	holdOCTRawData     ->setText(tr("hold OCT raw data"));
	readBScans         ->setText(tr("read BScans from OCT data"));
	saveOctBinFlat     ->setText(tr("save in octbin flat format"));

	QAction* extraSegLine = ProgramOptions::bscanShowExtraSegmentationslines.getAction();
	extraSegLine->setText(tr("show extra segmentationslines"));
	extraSegLine->setIcon(QIcon(":/icons/chart_curve.png"));

	QAction* showSeglines = ProgramOptions::bscansShowSegmentationslines.getAction();
	showSeglines->setText(tr("show segmentationslines"));
	showSeglines->setIcon(QIcon(":/icons/chart_curve.png"));

	QAction* showWithAspectRatio = ProgramOptions::bscanRespectAspectRatio.getAction();
	showWithAspectRatio->setText(tr("Aspect ratio"));
	showWithAspectRatio->setIcon(QIcon(":/icons/typicons/equals-outline.svg"));


	QAction* showGrid = ProgramOptions::sloShowGrid.getAction();
	showGrid->setText(tr("show grid"));
	showGrid->setIcon(QIcon(":/icons/grid.png"));

	QAction* showBScanMousePos = ProgramOptions::sloShowBScanMousePos.getAction();
	showBScanMousePos->setText(tr("show mouse pos"));
	showBScanMousePos->setIcon(QIcon(":/icons/typicons/times-outline.svg"));

	QAction* sloClipScanArea = ProgramOptions::sloClipScanArea.getAction();
	sloClipScanArea->setText(tr("clip to scan area"));
	sloClipScanArea->setIcon(QIcon(":/icons/chart_curve.png")); // TODO: icon wählen
}
