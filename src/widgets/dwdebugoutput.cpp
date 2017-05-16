#include "dwdebugoutput.h"

#include<iostream>

#include<QTextEdit>

#include<helper/qdebugstream.h>

DWDebugOutput::DWDebugOutput(QWidget* parent)
: QDockWidget(parent)
, textOutput(new QTextEdit(this))
, debugStreamCout(new Q_DebugStream(std::cout, textOutput, "cout"))
, debugStreamCerr(new Q_DebugStream(std::cerr, textOutput, "cerr"))
{
	setWindowTitle(tr("Debug output"));
	setWidget(textOutput);

	textOutput->setReadOnly(true);
	textOutput->setUndoRedoEnabled(false);
	textOutput->setAcceptRichText(false);

}


DWDebugOutput::~DWDebugOutput()
{
	delete debugStreamCout;
	delete debugStreamCerr;
}


